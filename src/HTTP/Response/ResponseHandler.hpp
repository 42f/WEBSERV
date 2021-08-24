#pragma once

#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "../Request/RequestLine.hpp"
#include "Autoindex.hpp"
#include "CGI.hpp"
#include "Config/Directives/Redirect.hpp"
#include "Config/Server.hpp"
#include "HTTP/Request/Request.hpp"
#include "Headers/Headers.hpp"
#include "RequestHandler.hpp"
#include "Response.hpp"
#include "Status.hpp"
#include "Timer.hpp"
#include "utils/Logger.hpp"

class ResponseHandler {
  class A_Method;
  typedef RequestHandler::result_type ReqResult;

 public:
  void init(ReqResult const requestResult, int receivedPort);
  void processRequest(void);

#if __APPLE__
  int doSend(int fdDest, int flags = 0);
#else
  int doSend(int fdDest, int flags = MSG_NOSIGNAL);
#endif

  bool isReady(void);
  Response const& getResponse(void);

  ResponseHandler(void);
  ResponseHandler(ReqResult requestResult, int receivedPort);
  ~ResponseHandler(void);

 private:
  int _port;
  ReqResult _request;
  Response _response;
  A_Method* _method;

  std::string getHeader(const Request& req, const std::string& target);
  void sendHeaders(int fdDest, int flags);
  void sendCgiHeaders(int fdSrc, int fdDest, int flags);
  void sendFromBuffer(int fdDest, int flags);
  void sendFromCgi(int fdDest, int flags);
  void sendFromFile(int fdDest, int flags);
  void doSendFromFD(int fdSrc, int fdDest, int flags);
  void manageRedirect(redirect red);

  ResponseHandler(ResponseHandler const& src);
  ResponseHandler& operator=(ResponseHandler const& rhs);

  /*
   * GET POST and DELETE Methods
   */

  class A_Method {
   public:
    A_Method(){};
    virtual ~A_Method(){};

    virtual void handler(config::Server const& serv, LocationConfig const& loc,
                         Request const& req, Response& resp) = 0;

    virtual std::string resolveTargetPath(LocationConfig const& loc,
                                          Request const& req) {
      std::string resolved(loc.get_root());
      std::string target(req.target.decoded_path);

      if (files::File::isFileFromPath(target)) {
        resolved += removeLocPath(loc, target);
      } else if (loc.get_auto_index() == true) {
        if (target[target.length() - 1] != '/') {
          target += '/';
        }
        resolved += removeLocPath(loc, target);
      } else if (loc.get_index().empty() == false) {
        resolved += loc.get_index();
      } else {
        return std::string();
      }
      return resolved;
    }

   private:
    virtual std::string removeLocPath(LocationConfig const& loc,
                                      std::string const& target) {
      if (target.find(loc.get_path()) == 0)
        return target.substr(loc.get_path().length());
      return target;
    }
   public:

    // TODO remove param once execute_cig is simplier
    static void handleCgiFile(Response& resp, std::string& cgiBin,
                              config::Server const& serv,
                              LocationConfig const& loc, Request const& req) {
      resp.getCgiInst().execute_cgi(cgiBin, resp.getFileInst(), req, loc, serv);
      // TODO send responseHandler const& instead ! So it can get
      // everything itself
      if (resp.getCgiInst().status() == cgi_status::SYSTEM_ERROR) {
        return makeStandardResponse(resp, status::InternalServerError, serv);
      } else {
        setRespForCgi(resp, resp.getFileInst());  // debug
        resp.setStatus(status::Ok);               // debug
      }
    }

    std::string getCgiBinPath(config::Server const& serv,
                              files::File const& file) {
      std::string fileExt = file.getExt();
      std::map<std::string, std::string>::const_iterator it =
          serv.get_cgis().begin();

      for (; it != serv.get_cgis().end(); it++) {
        if (fileExt == it->first) return it->second;
      }
      return std::string();
    }

    static void makeStandardResponse(Response& resp, status::StatusCode code,
                                     config::Server const& serv,
                                     const std::string& optionalMessage = "") {
      resp.reset(Version(), code);

      std::map<int, std::string>::const_iterator errIt =
          serv.get_error_pages().find(code);
      if (errIt != serv.get_error_pages().end()) {
        std::string errorPagePath = serv.get_error_pages().find(code)->second;
        resp.setFile(errorPagePath);
        if (resp.getFileInst().isGood()) {
          return setRespForFile(resp, resp.getFileInst());
        }
      }
      setRespForErrorBuff(resp, optionalMessage);
    }

    static void setRespNoBody(Response& resp, status::StatusCode code) {
      resp.getState() = respState::noBodyResp;
      resp.setStatus(code);
    }

    // static void setRespForAutoIndexBuff(Response& resp, std::string const&
    // path) {
    static void setRespForAutoIndexBuff(LocationConfig const & loc, Response& resp, std::string const& path) {
      // TODO call autoindex maker
      // loadAutoIndexBuffer();
      // std::cout << "MAKING AUTO INDEX" << std::endl;
      Autoindex::make(loc, path, resp);
      resp.setHeader(headerTitle::Content_Length, resp.getBuffer().length());
      resp.setHeader(headerTitle::Content_Type, "html");
      resp.getState() = respState::buffResp;
    }

    static void setRespForErrorBuff(Response& resp,
                                    const std::string& optionalMessage = "") {
      resp.loadErrorHtmlBuffer(resp.getStatusCode(), optionalMessage);
      resp.setHeader(headerTitle::Content_Length, resp.getBuffer().length());
      if (optionalMessage.empty())
        resp.setHeader(headerTitle::Content_Type, "html");
      resp.getState() = respState::buffResp;
    }

    static void setRespForCgi(Response& resp, files::File const& file) {
      resp.setHeader(headerTitle::Last_Modified, file.getLastModified());
      resp.setHeader(headerTitle::Transfer_Encoding, "chunked");
      resp.getState() = respState::cgiResp | respState::chunkedResp;
    }

    static void setRespForFile(Response& resp, files::File const& file) {
      resp.setHeader(headerTitle::Content_Type, file.getType());
      resp.setHeader(headerTitle::Last_Modified, file.getLastModified());
      resp.setHeader(headerTitle::Content_Length, file.getSize());
      resp.getState() = respState::fileResp;
    }

  };  // -- end of A_METHODE

  class GetMethod : public A_Method {
   public:
    GetMethod(){};
    ~GetMethod(){};

    void handler(config::Server const& serv, LocationConfig const& loc,
                 Request const& req, Response& resp) {
      std::string targetPath = resolveTargetPath(loc, req);
      LogStream s;
      s << "File targeted in GET: " << targetPath;

      if (targetPath.empty()) {
        return makeStandardResponse(resp, status::Unauthorized, serv);
      }

      struct stat st;
      if (files::File::isFileFromPath(targetPath)) {
        resp.setFile(targetPath);
        files::File const& file = resp.getFileInst();
        if (file.isGood()) {
          std::string cgiBin = getCgiBinPath(serv, file);
          if (cgiBin.empty() == false) {
            return handleCgiFile(resp, cgiBin, serv, loc, req);
          } else {
            resp.setStatus(status::Ok);
            return setRespForFile(resp, file);
          }
        } else {
          return makeStandardResponse(resp, status::NotFound, serv);
        }
      } else if (loc.get_auto_index() == true &&
                 stat(targetPath.c_str(), &st) == 0) {
        resp.setStatus(status::Ok);
        return setRespForAutoIndexBuff(loc, resp, targetPath);
      }
      }
    };  // --- end GET METHOD

    // *----------------------------------------------------------------------------------------------------
    // *----------------------------------------------------------------------------------------------------
    // *----------------------------------------------------------------------------------------------------
    // *----------------------------------------------------------------------------------------------------

    class PostMethod : public A_Method {
     public:
      PostMethod(){};
      ~PostMethod(){};

      void handler(config::Server const& serv, LocationConfig const& loc,
                   Request const& req, Response& resp) {
        if (req.get_body().empty() && req.target.decoded_query.empty()) {
          // TODO implement if empty body ? What does nginx do ?
          std::cout << "Empty body in post request..." << std::endl;
          return makeStandardResponse(resp, status::BadRequest, serv);
        }

        std::string targetPath = resolveTargetPath(loc, req);
        LogStream s;
        s << "File targeted in POST: " << targetPath;

        resp.setFile(targetPath);
        files::File const& file = resp.getFileInst();

        if (file.isGood()) {
          std::string cgiBin = getCgiBinPath(serv, file);
          if (cgiBin.empty()) {
            // TODO what if post to html ?...
            return makeStandardResponse(resp, status::Unauthorized, serv);
          } else {
            return handleCgiFile(resp, cgiBin, serv, loc, req);
          }
        } else if (req.get_body().empty() == false) {
          handleUpload(loc, req, resp);
          // TODO what if post to html ?...
          return makeStandardResponse(resp, status::Ok, serv);
        }
      }

      void handleUpload(LocationConfig const& loc, Request const& req,
                        Response& resp) {
        (void)resp;
        (void)loc;
        std::cout << "sep = [" << req.get_header("Content-Type").unwrap_or("")
                  << "]" << std::endl;
        std::cout << "IN POST, GOT BODY: "
                  << std::string(req.get_body().begin(), req.get_body().end());
      }
    };  // --- end POST METHOD

    // *----------------------------------------------------------------------------------------------------
    // *----------------------------------------------------------------------------------------------------
    // *----------------------------------------------------------------------------------------------------
    // *----------------------------------------------------------------------------------------------------

    class DeleteMethod : public A_Method {
     public:
      DeleteMethod(){};
      ~DeleteMethod(){};

      void handler(config::Server const& serv, LocationConfig const& loc,
                   Request const& req, Response& resp) {
        std::string target = resolveTargetPath(loc, req);
        LogStream s;
        s << "Target in DELETE: " << target;
        struct stat st;

        if (stat(target.c_str(), &st) == 0) {
          resp.getState() = respState::noBodyResp;
          errno = 0;
          if (files::File::isDirFromPath(target) &&
              rmdir(target.c_str()) == 0) {
            return setRespNoBody(resp, status::NoContent);
          } else if (errno == ENOTEMPTY) {
            return makeStandardResponse(resp, status::Conflict, serv,
                                        strerror(errno));
          } else if (files::File::isFileFromPath(target) &&
                     unlink(target.c_str()) == 0) {
            return setRespNoBody(resp, status::NoContent);
          } else {
            return makeStandardResponse(resp, status::Unauthorized, serv);
          }

        } else
          return makeStandardResponse(resp, status::NotFound, serv);
      }

      std::string resolveTargetPath(LocationConfig const& loc,
                                    Request const& req) {
        std::string target(loc.get_root());

        // if the request aims to a subdir of the location path,
        // we remove the location path part
        if (req.target.decoded_path.find(loc.get_path()) == 0) {
          target += req.target.decoded_path.substr(loc.get_path().length());
        } else {
          target += req.target.decoded_path;
        }
        return target;
      }
    };  // --- end DELETE METHOD

    class UnsupportedMethod : public A_Method {
     public:
      UnsupportedMethod(){};
      ~UnsupportedMethod(){};

      void handler(config::Server const&, LocationConfig const&, Request const&,
                   Response&) {
        std::cout << __func__ << " of UNSUPPORTED." << std::endl;
      }
    };

  };  // end reponseHandler
