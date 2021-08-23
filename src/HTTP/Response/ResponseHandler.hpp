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
      std::string targetFile(loc.get_root());

      if (files::File::isFileFromPath(req.target.decoded_path)) {
        // if the request aims to a subdir of the location path,
        // we remove the location path part
        if (req.target.decoded_path.find(loc.get_path()) == 0) {
          targetFile += req.target.decoded_path.substr(loc.get_path().length());
        } else {
          targetFile += req.target.decoded_path;
        }
      } else if (loc.get_index().empty() == false) {
        targetFile += loc.get_index();
      } else
        return std::string();
      return targetFile;
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
          setRespForFile(resp, resp.getFileInst());
          return;
        }
      }
      setRespForErrorBuff(resp, optionalMessage);
    }

    static void handleCgiFile(
        Response& resp, std::string& cgiBin, config::Server const& serv,
        LocationConfig const& loc,
        Request const& req) {  // TODO remove param once execute_cig is simplier
      resp.getCgiInst().execute_cgi(
          cgiBin, resp.getFileInst(), req, loc,
          serv);  // TODO send responseHandler const& instead ! So it can get
                  // everything itself
      if (resp.getCgiInst().status() == cgi_status::SYSTEM_ERROR) {
        return makeStandardResponse(resp, status::InternalServerError, serv);
      } else {
        setRespForCgi(resp, resp.getFileInst());  // debug
        resp.setStatus(status::Ok);               // debug
      }
    }

    static void setRespForErrorBuff(Response& resp,
                                    const std::string& optionalMessage = "") {
      resp.loadErrorHtmlBuffer(resp.getStatusCode(), optionalMessage);
      resp.setHeader(headerTitle::Content_Length,
                     resp.getErrorBuffer().length());
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
  };  // -- end of A_METHODE

  class GetMethod : public A_Method {
   public:
    GetMethod(){};
    ~GetMethod(){};

    void handler(config::Server const& serv, LocationConfig const& loc,
                 Request const& req, Response& resp) {
      // Resolve the file to be read, if none, return a 404 Not Found
      std::string targetFile = resolveTargetPath(loc, req);
      LogStream s;
      s << "File targeted in GET: " << targetFile;

      resp.setFile(targetFile);
      files::File const& file = resp.getFileInst();

      if (file.isGood()) {
        std::string cgiBin = getCgiBinPath(serv, file);
        if (cgiBin.empty() == false) {
          return handleCgiFile(resp, cgiBin, serv, loc, req);
        } else {
          setRespForFile(resp, file);
          resp.setStatus(status::Ok);
        }
      } else
        return makeStandardResponse(resp, status::NotFound, serv);
      // TODO ADD else if autoindex : send autodindex
      // set rep for autoindex -> respState::(to create)
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

      std::string targetFile = resolveTargetPath(loc, req);
      LogStream s;
      s << "File targeted in POST: " << targetFile;

      resp.setFile(targetFile);
      files::File const& file = resp.getFileInst();

      if (file.isGood()) {
        std::string cgiBin = getCgiBinPath(serv, file);
        if (cgiBin.empty() == false) {
          return handleCgiFile(resp, cgiBin, serv, loc, req);
        } else {
          return makeStandardResponse(resp, status::Unauthorized,
                                      serv);  // TODO what if post to html ?...
        }
      } else if (req.get_body().empty() == false) {
        handleUpload(loc, req, resp);
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
        if (files::File::isDirFromPath(target) && rmdir(target.c_str()) == 0)
          resp.setStatus(status::NoContent);
        else if (errno == ENOTEMPTY)
          return makeStandardResponse(resp, status::Conflict, serv,
                                      strerror(errno));
        else if (files::File::isFileFromPath(target) &&
                 unlink(target.c_str()) == 0)
          resp.setStatus(status::NoContent);
        else
          return makeStandardResponse(resp, status::Unauthorized, serv);
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
