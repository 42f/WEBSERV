#pragma once

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "Config/Directives/Location.hpp"

namespace network {

class ServerPool {
 public:
  static void init(const std::string &configFilePath);

  static std::vector<config::Server> const &getPool(void);
  static std::set<int> getPorts(void);
  static config::Server const &getServerMatch(std::string hostHeader,
                                              int receivedPort);
  static LocationConfig const getLocationMatch(config::Server const &serv,
                                               Target const &target);

  ~ServerPool(void);

 private:
  static std::vector<config::Server> _serverPool;

  static void locationsInit(config::Server &serv);
  static bool isPathMatch(LocationConfig const &loc, Target const &target);
  static void cleanPath(std::string &locPath);
  static void cleanRoot(std::string &locRoot);

  ServerPool(void){};
  ServerPool(ServerPool const &src);
  ServerPool &operator=(ServerPool const &rhs);
};

}  // namespace network
