#include "ServerPool.hpp"

namespace network {

std::vector<config::Server> ServerPool::_serverPool;

/* ............................... CONSTRUCTOR ...............................*/

/* ..............................COPY CONSTRUCTOR.............................*/

/* ................................ DESTRUCTOR ...............................*/

ServerPool::~ServerPool(void) {}

/* ................................. METHODS .................................*/

/*
 * Parses configuration file and update locations blocks with server
 * inherited data where needed
 */
void ServerPool::init(const std::string &configFilePath) {
  _serverPool = config::parse(configFilePath);
  std::for_each(_serverPool.begin(), _serverPool.end(),
                ServerPool::locationsInit);
}

void ServerPool::locationsInit(config::Server &serv) {
  std::cout << RED << "______________________________________________" << NC << std::endl;
  // cleanRoot(serv.get_root());

  std::vector<LocationConfig> &locations = serv.get_locations();
  if (locations.empty() == false) {
    std::vector<LocationConfig>::iterator it = locations.begin();
    std::vector<LocationConfig>::iterator ite = locations.end();
    for (; it != ite; it++) {
      it->_auto_index = serv.get_auto_index() == true ? true: it->_auto_index;
      it->_root.assign((it->_root.empty()) ? serv.get_root() : it->_root);
      it->_index.assign((it->_index.empty()) ? serv.get_index() : it->_index);
      it->_body_size = (it->_body_size == LocationConfig::SIZE_UNSET)
                           ? serv.get_body_size()
                           : it->_body_size;
      // cleanPath(it->_path);
      // cleanRoot(it->_root);
    std::cout << *it << std::endl;
    }
  }
}

// void ServerPool::cleanPath(std::string &locPath) {                              // TODO remove if fixed completely with redirection
//   // if (locPath[0] != '/') locPath = '/' + locPath;
//   // if (locPath[locPath.length() - 1] != '/') locPath.push_back('/');
// }

// void ServerPool::cleanRoot(std::string &root) {
//   // if (root.size() > 1 && root[root.length() - 1] != '/') root.push_back('/');
// }

/* ................................. ACCESSOR ................................*/

const std::vector<config::Server> &ServerPool::getPool(void) {
  return _serverPool;
}

std::set<int> ServerPool::getPorts(void) {
  std::set<int> ports;
  std::vector<config::Server>::iterator it = _serverPool.begin();
  for (; it != _serverPool.end(); it++) {
    ports.insert(it->get_port());
  }
  return ports;
}

config::Server const &ServerPool::getServerMatch(std::string hostHeader,
                                                 int receivedPort) {
  // clean the host header to get the IP/name part only
  size_t portPos = hostHeader.find(':');
  if (portPos != std::string::npos) hostHeader.resize(portPos);

  // iterate in reverse order on vector to get the best match
  std::vector<config::Server>::reverse_iterator it = _serverPool.rbegin();
  std::vector<config::Server>::reverse_iterator bestCandidate =
      _serverPool.rend();
  for (; it != _serverPool.rend(); it++) {
    if (it->get_port() == receivedPort) bestCandidate = it;

    if (it->get_port() == receivedPort &&
        (it->get_address() == hostHeader || it->get_name() == hostHeader))
      return *it;
  }
  return *bestCandidate;
}

/*
 * Try to match the target with any location path, if none, reduce one element
 * of the path and try again, until the target is empty, then no location can
 * be resolved so a location is created with the parent server infos.
 */
LocationConfig const ServerPool::getLocationMatch(config::Server const &serv,
                                                  Target const &target) {
  std::vector<LocationConfig> const &locs = serv.get_locations();

  if (locs.empty() == false) {
    std::vector<LocationConfig>::const_iterator it;
    std::string targetPath = target.decoded_path;

    if (files::File::isFileFromPath(targetPath)) {
      targetPath.resize(targetPath.find_last_of('/') + 1);
    }
    while (targetPath.empty() == false) {
      for (it = locs.begin(); it != locs.end(); it++) {
        // LogStream s; s << "Trying "  << targetPath << " vs. " <<
        // it->get_path(); // TODO remove
        if (targetPath == it->get_path() || targetPath + '/' == it->get_path())
          return *it;
      }
      if (targetPath.find_last_of('/') == targetPath.find_first_of('/') &&
          targetPath.size() > 1)
        targetPath.resize(targetPath.find_last_of('/') + 1);
      else
        targetPath.resize(targetPath.find_last_of('/'));
    }
  }
  return (LocationConfig(serv));
}

/* ................................. OVERLOAD ................................*/

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/

}  // end namespace network
