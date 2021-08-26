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

#include "CGI.hpp"
#include "Config/Directives/Redirect.hpp"
#include "Config/Server.hpp"
#include "HTTP/Request/Request.hpp"
#include "Headers/Headers.hpp"
#include "Logger.hpp"
#include "RequestHandler.hpp"
#include "RequestLine.hpp"
#include "Response.hpp"
#include "Status.hpp"
#include "Timer.hpp"
#include "Autoindex.hpp"

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
  Response const& getResponse(void) const;
  Request const& getRequest(void) const;

  ResponseHandler(void);
  ResponseHandler(ReqResult requestResult, int receivedPort);
  ~ResponseHandler(void);

 private:
  int _port;
  ReqResult _requestRes;
  Request _req;
  Response _resp;
  A_Method* _method;
  config::Server _serv;
  LocationConfig _loc;

  std::string getReqHeader(const std::string& target);
  void sendHeaders(int fdDest, int flags);
  void sendCgiHeaders(int fdSrc, int fdDest, int flags);
  void sendFromBuffer(int fdDest, int flags);
  void sendFromCgi(int fdDest, int flags);
  void sendFromFile(int fdDest, int flags);
  void doSendFromFD(int fdSrc, int fdDest, int flags);
  void manageRedirect(redirect const& red);

  ResponseHandler(ResponseHandler const& src);
  ResponseHandler& operator=(ResponseHandler const& rhs);

  /*
   * GET POST and DELETE Methods
   */

  class A_Method {
   protected:
    ResponseHandler& _inst;

   public:
    A_Method(ResponseHandler& inst) : _inst(inst){};
    virtual ~A_Method(){};

    virtual void handler() = 0;

    virtual std::string resolveTargetPath() {
      std::string resolved(_inst._loc.get_root());
      std::string target(_inst._req.target.decoded_path);

      if (files::File::isFileFromPath(target)) {
        resolved += removeLocPath(target);
      } else if (_inst._loc.get_auto_index() == true) {
        // if (target[target.length() - 1] != '/') {                            // TODO remove if completely fixed
        //   target += '/';
        // }
        resolved += removeLocPath(target);
      } else if (_inst._loc.get_index().empty() == false) {
        resolved += (target[target.length() - 1] == '/' ? "" : "/") + _inst._loc.get_index();
      } else {
        return std::string();
      }
      return resolved;
    }

   private:
    virtual std::string removeLocPath(std::string const& target) {
      if (target.find(_inst._loc.get_path()) == 0)
        return target.substr(_inst._loc.get_path().length());
      return target;
    }

   public:
    void handleCgiFile(std::string const& cgiBin) {
      CGI& cgiInst = _inst._resp.getCgiInst();
      cgiInst.execute_cgi(cgiBin, _inst._resp.getFileInst(), _inst._req,
                          _inst._serv);

      if (cgiInst.status() == cgi_status::SYSTEM_ERROR) {
        return makeStandardResponse(status::InternalServerError);
      } else {
        setRespForCgi();                        // TODO REMOVE debug
        _inst._resp.setStatus(status::Ok);  // TODO REMOVE debug
      }
    }

    std::string getCgiBinPath() {
      std::string fileExt = _inst._resp.getFileInst().getExt();
      std::map<std::string, std::string>::const_iterator it =
          _inst._serv.get_cgis().begin();

      for (; it != _inst._serv.get_cgis().end(); it++) {
        if (fileExt == it->first) return it->second;
      }
      return std::string();
    }

    void makeStandardResponse(status::StatusCode code,
                              const std::string& optionalMessage = "") {
      _inst._resp.reset(Version(), code);
      std::map<int, std::string> const& err_pages =
          _inst._serv.get_error_pages();

      std::map<int, std::string>::const_iterator errIt = err_pages.find(code);
      if (errIt != err_pages.end()) {
        std::string errorPagePath = err_pages.find(code)->second;
        _inst._resp.setFile(errorPagePath);
        if (_inst._resp.getFileInst().isGood()) {
          return setRespForFile();
        }
      }
      setRespForErrorBuff(optionalMessage);
    }

    void setRespNoBody(status::StatusCode code) {
      _inst._resp.getState() = respState::noBodyResp;
      _inst._resp.setStatus(code);
    }

    void setRespForAutoIndexBuff(std::string const& path) {
      Autoindex::make(_inst._req.target.path, path, _inst._resp);
      _inst._resp.setHeader(headerTitle::Content_Length,
                                _inst._resp.getBuffer().length());
      _inst._resp.setHeader(headerTitle::Content_Type, "html");
      _inst._resp.getState() = respState::buffResp;
    }

    void setRespForErrorBuff(const std::string& optionalMessage = "") {
      _inst._resp.loadErrorHtmlBuffer(_inst._resp.getStatusCode(),
                                          optionalMessage);
      _inst._resp.setHeader(headerTitle::Content_Length,
                                _inst._resp.getBuffer().length());
      if (optionalMessage.empty())
        _inst._resp.setHeader(headerTitle::Content_Type, "html");
      _inst._resp.getState() = respState::buffResp;
    }

    void setRespForCgi() {
      files::File const& file = _inst._resp.getFileInst();
      _inst._resp.setHeader(headerTitle::Last_Modified,
                                file.getLastModified());
      _inst._resp.setHeader(headerTitle::Transfer_Encoding, "chunked");
      _inst._resp.getState() = respState::cgiResp | respState::chunkedResp;
    }

    void setRespForFile() {
      files::File const& file = _inst._resp.getFileInst();
      _inst._resp.setHeader(headerTitle::Content_Type, file.getType());
      _inst._resp.setHeader(headerTitle::Last_Modified,
                                file.getLastModified());
      _inst._resp.setHeader(headerTitle::Content_Length, file.getSize());
      _inst._resp.getState() = respState::fileResp;
    }

    void manageRedirect(redirect const& red) {
      if (red.status >= 300 && red.status <= 308) {
        makeStandardResponse(red.status);
        _inst._resp.setHeader("Location",
                                  red.resolveRedirect(_inst._req.target));
      } else {
        makeStandardResponse(red.status, red.uri);
      }
    }

  };  // -- end of A_METHODE

  class GetMethod : public A_Method {
   public:
    GetMethod(ResponseHandler& inst) : A_Method(inst){};
    ~GetMethod(){};

    void handler() {
      std::string targetPath = resolveTargetPath();
      LogStream s;
      s << "File targeted in GET: " << targetPath;

      if (targetPath.empty()) {
        return makeStandardResponse(status::Unauthorized);
      }

      struct stat st;
      if (files::File::isFileFromPath(targetPath)) {
        _inst._resp.setFile(targetPath);
        files::File const& file = _inst._resp.getFileInst();
        if (file.isGood()) {
          std::string cgiBin = getCgiBinPath();
          if (cgiBin.empty() == false) {
            return handleCgiFile(cgiBin);
          } else {
            _inst._resp.setStatus(status::Ok);
            return setRespForFile();
          }
        } else {
          return makeStandardResponse(status::NotFound);
        }
      } else if (_inst._loc.get_auto_index() == true &&
                 stat(targetPath.c_str(), &st) == 0) {
        if (targetPath[targetPath.length() - 1] != '/') {
          return manageRedirect(
              redirect(status::MovedPermanently, _inst._req.target.path + '/'));
        } else {
          _inst._resp.setStatus(status::Ok);
          return setRespForAutoIndexBuff(targetPath);
        }
      }
      // Default response to avoid empty response
      return makeStandardResponse(status::InternalServerError);
    }
  };  // --- end GET METHOD

  // *----------------------------------------------------------------------------------------------------
  // *----------------------------------------------------------------------------------------------------
  // *----------------------------------------------------------------------------------------------------
  // *----------------------------------------------------------------------------------------------------

  class PostMethod : public A_Method {
   public:
    PostMethod(ResponseHandler& inst) : A_Method(inst){};
    ~PostMethod(){};

    void handler() {
      if (_inst._req.get_body().empty() &&
          _inst._req.target.decoded_query.empty()) {
        // TODO implement if empty body ? What does nginx do ?
        std::cout << "Empty body in post request..." << std::endl;
        return makeStandardResponse(status::BadRequest);
      }

      std::string targetPath = resolveTargetPath();
      LogStream s;
      s << "File targeted in POST: " << targetPath;

      _inst._resp.setFile(targetPath);
      files::File const& file = _inst._resp.getFileInst();

      if (file.isGood()) {
        std::string cgiBin = getCgiBinPath();
        if (cgiBin.empty()) {
          // TODO what if post to html ?...
          return makeStandardResponse(status::Unauthorized);
        } else {
          return handleCgiFile(cgiBin);
        }
      } else if (_inst._req.get_body().empty() == false) {
        handleUpload();
        // TODO what if post to html ?...
        return setRespNoBody(status::NoContent);  // ? is it
      }
    }

    void handleUpload() {
      std::cout << "PERFORM UPLOAD OF FILE IN BODY" << std::endl;
    }
  };  // --- end POST METHOD

  // *----------------------------------------------------------------------------------------------------
  // *----------------------------------------------------------------------------------------------------
  // *----------------------------------------------------------------------------------------------------
  // *----------------------------------------------------------------------------------------------------

  class DeleteMethod : public A_Method {
   public:
    DeleteMethod(ResponseHandler& inst) : A_Method(inst){};
    ~DeleteMethod(){};

    void handler() {
      std::string target = resolveTargetPath();
      LogStream s;
      s << "Target in DELETE: " << target;
      struct stat st;

      if (stat(target.c_str(), &st) == 0) {
        errno = 0;
        if (files::File::isDirFromPath(target) && rmdir(target.c_str()) == 0) {
          return setRespNoBody(status::NoContent);
        } else if (errno == ENOTEMPTY) {
          return makeStandardResponse(status::Conflict, strerror(errno));
        } else if (files::File::isFileFromPath(target) &&
                   unlink(target.c_str()) == 0) {
          return setRespNoBody(status::NoContent);
        } else {
          return makeStandardResponse(status::Unauthorized);
        }

      } else
        return makeStandardResponse(status::NotFound);
    }

    std::string resolveTargetPath() {
      std::string resolved(_inst._loc.get_root());
      std::string target(_inst._req.target.decoded_path);

      // if the request aims to a subdir of the location path,
      // we remove the location path part
      if (target.find(_inst._loc.get_path()) == 0) {
        resolved += target.substr(_inst._loc.get_path().length());
      } else {
        resolved += target;
      }
      return resolved;
    }
  };  // --- end DELETE METHOD

  class UnsupportedMethod : public A_Method {
   public:
    UnsupportedMethod(ResponseHandler& inst) : A_Method(inst){};
    ~UnsupportedMethod(){};

    void handler() { std::cout << __func__ << " of UNSUPPORTED." << std::endl; }
  };

};  // end reponseHandler
