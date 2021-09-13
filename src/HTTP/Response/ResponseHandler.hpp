#pragma once

#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "CGI/CGI.hpp"
#include "Config/Directives/Redirect.hpp"
#include "Config/Server.hpp"
#include "HTTP/Headers/Headers.hpp"
#include "HTTP/Request/Request.hpp"
#include "Logger.hpp"
#include "RequestHandler.hpp"
#include "RequestUtils/RequestLine.hpp"
#include "Response.hpp"
#include "Status.hpp"
#include "Autoindex.hpp"

class ResponseHandler {
  class A_Method;

 public:
  void init(RequestHandler& reqHandler, int receivedPort);
  int processRequest(void);

#if __APPLE__
  int doSend(int fdDest, int flags = 0);
#else
  int doSend(int fdDest, int flags = MSG_NOSIGNAL);
#endif

  bool isReady(void);
  Response const& getResponse(void) const;
  Request const& getRequest(void) const;

  ResponseHandler(void);
  ResponseHandler(RequestHandler& reqHandler, int receivedPort);
  ~ResponseHandler(void);

  static void doSendCachedTooManyRequests(int fdDst);

 private:
  RequestHandler& _requestHandler;
  Request _req;
  int _port;
  config::Server _serv;
  LocationConfig _loc;
  A_Method* _method;
  Response _resp;

  std::string getReqHeader(const std::string& target);
  void sendHeaders(int fdDest, int flags);
  void sendCgiHeaders(int fdDest, int flags);
  void sendFromBuffer(int fdDest, int flags);
  void sendFromCgi(int fdDest, int flags);
  void sendFromFile(int fdDest, int flags);
  int doSendFromFD(int fdSrc, int fdDest, int flags);
  void manageRedirect(redirect const& red);
  int getOutputFd(void);
  status::StatusCode pickCgiError(cgi_status::status cgiStat) const;

  void logData( void );

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
      std::string output;
      std::string target(_inst._req.target.decoded_path);
      std::string file = removeLocPath(target);

      output = _inst._loc.get_root();
      if (file[0] != '/' && output[output.length() - 1] != '/') output += '/';
      output += file;
      return output;
    }

  //  private:
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
      setRespForCgi();
      _inst._resp.setStatus(status::Ok);
    }

    std::string getCgiBinPath(void) {
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
      struct stat st;
      if (errIt != err_pages.end() && stat(errIt->second.c_str(), &st) == 0 &&
          !S_ISDIR(st.st_mode)) {
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
      _inst._resp.setHeader(headerTitle::Last_Modified, file.getLastModified());
      _inst._resp.setHeader(headerTitle::Transfer_Encoding, "chunked");
      _inst._resp.getState() = respState::cgiResp | respState::chunkedResp;
    }

    void setRespForFile() {
      files::File const& file = _inst._resp.getFileInst();
      _inst._resp.setHeader(headerTitle::Content_Type, file.getType());
      _inst._resp.setHeader(headerTitle::Last_Modified, file.getLastModified());
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

  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------

  class GetMethod : public A_Method {
   public:
    GetMethod(ResponseHandler& inst) : A_Method(inst){};
    ~GetMethod(){};

    void handler() {
      std::string targetPath = resolveTargetPath();
#if LOG_LEVEL == LOG_LEVEL_TRACE
      LogStream s;
      s << "File targeted in GET: " << targetPath;
#endif
      _inst._resp.setFile(targetPath);
      files::File const& file = _inst._resp.getFileInst();

      /*
      **  Target exists and is a Directory
      */
      if (file.isGood() && file.isDir()) {
        if (endsWithSlash(_inst._req.target.path) == false) {
          return manageRedirect(
              redirect(status::MovedPermanently, _inst._req.target.path + '/'));
        } else if (_inst._loc.has_index()) {
          return handleIndexFile(targetPath);
        } else if (_inst._loc.has_auto_index()) {
          return handleAutoIndex(targetPath);
        }
        /*
        **  Target exists and is a File
        */
      } else if (file.isGood() && file.isFile()) {
        std::string cgiBin = getCgiBinPath();
        if (cgiBin.empty()) {
          return handleRegFile();
        } else {
          return handleCgiFile(cgiBin);
        }
        /*
        **  Target could not be opened / does not exist
        */
      } else if (file.getError() & EACCES) {
        return makeStandardResponse(status::TooManyRequests);
      } else if (file.getError() & ENOENT) {
        return makeStandardResponse(status::NotFound);
      }

      /*
      **  Default response
      */
      return makeStandardResponse(status::Forbidden);
    }

   private:
    bool endsWithSlash(std::string const& path) {
      return path.empty() == false && *(--path.end()) == '/';
    }

    void handleAutoIndex(std::string const& targetPath) {
      _inst._resp.setStatus(status::Ok);
      return setRespForAutoIndexBuff(targetPath);
    }

    void handleIndexFile(std::string const& targetPath) {

      std::string indexPath(_inst._resp.getFileInst().getPath());
      indexPath += _inst._loc.get_index();
      files::File & file = _inst._resp.setFile(indexPath);

      if (file.isGood() && file.isFile()) {
        _inst._resp.setStatus(status::Ok);
        return setRespForFile();
      } else if (file.isGood() && file.isDir()) {
        return makeStandardResponse(status::Forbidden);
      } else if (_inst._loc.has_auto_index()) {
        return handleAutoIndex(targetPath);
      } else {
        return makeStandardResponse(status::NotFound);
      }
    }

    void handleRegFile() {
      _inst._resp.setStatus(status::Ok);
      return setRespForFile();
    }

    void setRespForAutoIndexBuff(std::string const& path) {
      Autoindex::make(_inst._req.target.path, path, _inst._resp);
      _inst._resp.setHeader(headerTitle::Content_Length,
                            _inst._resp.getBuffer().length());
      _inst._resp.setHeader(headerTitle::Content_Type, "html");
      _inst._resp.getState() = respState::buffResp;
    }

  };  // --- end GET METHOD

  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------

  class PostMethod : public A_Method {
   public:
    PostMethod(ResponseHandler& inst) : A_Method(inst){};
    ~PostMethod(){};

    void handler() {
      if (_inst._req.get_body().empty())
        return makeStandardResponse(status::BadRequest);

      std::string targetPath = resolveTargetPath();

#if LOG_LEVEL == LOG_LEVEL_TRACE
      LogStream s;
      s << "File targeted in POST: " << targetPath;
#endif

      _inst._resp.setFile(targetPath);
      files::File const& file = _inst._resp.getFileInst();

      /*
      **  File targeted exists, and is a file
      */
      if (file.isGood() && file.isFile()) {
        std::string cgiBin = getCgiBinPath();
        if (cgiBin.empty()) {
          return makeStandardResponse(status::Conflict, "File exists already.");
        } else {
          return handleCgiFile(cgiBin);
        }
        /*
        **  File targeted could not opened
        */
      } else if (file.isGood() == false && _inst._loc.get_upload() == true &&
                 file.getError() & ENOENT) {
        return doUploadFile();
      } else if (file.isGood() == false && _inst._loc.get_upload() == true) {
        return makeStandardResponse(status::Conflict, strerror(file.getError()));
      }

      /*
      **  Default response
      */
      return makeStandardResponse(status::Forbidden);
    }

    void doUploadFile() {
      files::File const& requestedFile = _inst._resp.getFileInst();

      files::File uploadFile(requestedFile.getPath(),
                             O_CREAT | O_TRUNC | O_WRONLY, 0644);
      if (uploadFile.isGood()) {
        size_t len = _inst._req.get_body().size();
        if (len > 0) {
          char const* data = _inst._req.get_body().data();

          // TODO -> do select here ??

          size_t ret = write(uploadFile.getFD(), data, len);
          if (ret > 0)
            return makeStandardResponse(status::Accepted);
          else
            return makeStandardResponse(status::InternalServerError);
        } else {
          return makeStandardResponse(status::Accepted);
        }
      } else {
        return makeStandardResponse(status::Conflict,
                                    strerror(uploadFile.getError()));
      }
    }
  };  // --- end POST METHOD

  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------
  // *--------------------------------------------------------------------------

  class DeleteMethod : public A_Method {
   public:
    DeleteMethod(ResponseHandler& inst) : A_Method(inst){};
    ~DeleteMethod(){};

    void handler() {
      std::string target = resolveTargetPath();

#if LOG_LEVEL == LOG_LEVEL_TRACE
      LogStream s;
      s << "Target in DELETE: " << target;
#endif

      _inst._resp.setFile(target);
      files::File const& file = _inst._resp.getFileInst();

      if (file.isGood() && file.isFile()) {
        return doDeleteFile(target);
      } else if (file.isGood() && file.isDir()) {
        return makeStandardResponse(status::Conflict, "Target is a directory");
      } else {
        return makeStandardResponse(status::Conflict,
                                    strerror(file.getError()));
      }
    }

    void doDeleteFile(std::string const& target) {
      errno = 0;
      if (unlink(target.c_str()) == 0) {
        return setRespNoBody(status::NoContent);
      } else if (errno & EBUSY) {
        return makeStandardResponse(status::Accepted);
      } else {
        return makeStandardResponse(status::Conflict, strerror(errno));
      }
    }
  };  // --- end DELETE METHOD

  class UnsupportedMethod : public A_Method {
   public:
    UnsupportedMethod(ResponseHandler& inst) : A_Method(inst){};
    ~UnsupportedMethod(){};

    void handler() { return makeStandardResponse(status::MethodNotAllowed); }
  };

};  // end reponseHandler
