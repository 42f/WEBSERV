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
    void init(ReqResult const& requestResult, int receivedPort);
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
    int sendHeaders(int fdDest, int flags);
    int sendErrorBuffer(int fdDest, int flags);
    int sendFromPipe(int fdDest, int flags);
    int sendFromFile(int fdDest, int flags);
    int doSendFromFD(int fdSrc, int fdDest, int flags);

    ResponseHandler(ResponseHandler const& src);
    ResponseHandler& operator=(ResponseHandler const& rhs);

    /*
     * GET POST and DELETE Methods
     */

    class A_Method {
       public:
        A_Method(){};
        virtual ~A_Method(){};

        virtual void handler(config::Server const& serv,
                             LocationConfig const& loc, Request const& req,
                             Response& resp) = 0;

        virtual std::string resolveFilePath(LocationConfig const& loc,
                                    Request const& req) {
            std::string targetFile(loc.get_root());

            if (files::File::isFileFromPath(req.target.decoded_path)) {
                // if the request aims to a subdir of the location path,
                // we remove the location path part
                if (req.target.decoded_path.find(loc.get_path()) == 0) {
                    targetFile +=
                        req.target.decoded_path.substr(loc.get_path().length());
                } else {
                    targetFile += req.target.decoded_path;
                }
            } else if (loc.get_index().empty() == false) {
                targetFile += loc.get_index();
            } else
                return std::string();

            return targetFile;
        }

        static void makeErrorResponse(Response& resp, status::StatusCode code,
                                      config::Server const& serv) {
            resp.reset(Version(), code);

            std::map<int, std::string>::const_iterator errIt =
                serv.get_error_pages().find(code);
            if (errIt != serv.get_error_pages().end()) {
                std::string errorPagePath =
                    serv.get_error_pages().find(code)->second;
                resp.setFile(errorPagePath);
            }

            if (resp.getFileInst().isGood()) {
                setRespForFile(resp, resp.getFileInst());
            } else {
                setRespForErrorBuff(resp);
            }
        }

        static void setRespForErrorBuff(Response& resp) {
            resp.loadErrorHtmlBuffer(resp.getStatusCode());
            resp.setHeader(headerTitle::Content_Length,
                           resp.getErrorBuffer().length());
            resp.setHeader(headerTitle::Content_Type, "html");
            resp.getState() = respState::buffResp;
        }

        static void setRespForPipe(Response& resp, files::File const& file) {
            // TODO implement
            resp.setHeader(headerTitle::Content_Type, file.getType());  // debug
            resp.setHeader(headerTitle::Last_Modified,
                           file.getLastModified());  // debug
            resp.setHeader(headerTitle::Content_Length,
                           file.getSize());         // debug
            resp.getState() = respState::fileResp;  // debug
        }

        static void setRespForFile(Response& resp, files::File const& file) {
            resp.setHeader(headerTitle::Content_Type, file.getType());
            resp.setHeader(headerTitle::Last_Modified, file.getLastModified());

            if (resp.getFileInst().getSize() > DEFAULT_SEND_SIZE) {
                resp.setHeader(headerTitle::Transfer_Encoding, "chunked");
                resp.getState() = respState::fileResp | respState::chunkedResp;
            } else {
                resp.setHeader(headerTitle::Content_Length, file.getSize());
                resp.getState() = respState::fileResp;
            }
        }
    };

    class GetMethod : public A_Method {
       public:
        GetMethod(){};
        ~GetMethod(){};

        void handler(config::Server const& serv, LocationConfig const& loc,
                     Request const& req, Response& resp) {
            // Resolve the file to be read, if none, return a 404 Not Found
            std::string targetFile = resolveFilePath(loc, req);
            resp.setFile(targetFile);
            LogStream s; s << "File targeted: " << targetFile;

            files::File const& file = resp.getFileInst();

            if (file.isGood()) {
                if (isCGI(serv, file)) {
                    // TODO instanciate PipedCGI obj and check if isGood(). If
                    // not return right error
                    setRespForFile(resp, file);  // debug
                    resp.setStatus(status::Ok);  // debug
                } else {
                    setRespForFile(resp, file);
                    resp.setStatus(status::Ok);
                }
            } else
                makeErrorResponse(resp, status::NotFound, serv);
            resp.setHeader(headerTitle::Server, serv.get_name());
        }

        bool isCGI(config::Server const& serv, files::File const& file) {
            std::string fileExt = file.getExt();
            std::map<std::string, std::string>::const_iterator it =
                serv.get_cgis().begin();

            for (; it != serv.get_cgis().end(); it++) {
                if (fileExt == it->first) return true;
            }
            return false;
        }
    };

    class PostMethod : public A_Method {
       public:
        PostMethod(){};
        ~PostMethod(){};

        void handler(config::Server const& serv, LocationConfig const& loc,
                     Request const& req, Response& resp) {
            (void)serv;
            (void)resp;
            (void)loc;
            (void)req;
            std::cout << __func__ << " of POST." << std::endl;
        }
    };

    class DeleteMethod : public A_Method {
       public:
        DeleteMethod(){};
        ~DeleteMethod(){};

        void handler(config::Server const& serv, LocationConfig const& loc,
                     Request const& req, Response& resp) {

            std::string targetFile = resolveFilePath(loc, req);
            LogStream s; s << "File targeted: " << targetFile;

            struct stat st;
            if (targetFile.empty() == false
                                        && stat(targetFile.c_str(), &st) == 0) {
                resp.getState() = respState::noBodyResp;
                if (unlink(targetFile.c_str()) == 0)
                    resp.setStatus(status::NoContent);
                else if (errno == EBUSY)
                    resp.setStatus(status::Accepted);
                else
                    makeErrorResponse(resp, status::Unauthorized, serv);
            } else
                makeErrorResponse(resp, status::NotFound, serv);
        resp.setHeader(headerTitle::Server, serv.get_name());
        }
    };

    class UnsupportedMethod : public A_Method {
       public:
        UnsupportedMethod(){};
        ~UnsupportedMethod(){};

        void handler(config::Server const&, LocationConfig const&,
                     Request const&, Response&) {
            std::cout << __func__ << " of UNSUPPORTED." << std::endl;
        }
    };

};  // end reponseHandler
