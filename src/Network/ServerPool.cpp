#include "ServerPool.hpp"

namespace network {

std::vector<config::Server>		ServerPool::_serverPool;

/* ............................... CONSTRUCTOR ...............................*/

ServerPool::ServerPool( void )	{
}

/* ..............................COPY CONSTRUCTOR.............................*/

/*
ServerPool::ServerPool( const ServerPool & src ) {
}
*/

/* ................................ DESTRUCTOR ...............................*/

ServerPool::~ServerPool( void )	{
}

/* ................................. METHODS .................................*/

/*
 * Parses configuration file and update locations blocks with server
 * inherited data where needed
 */
void	ServerPool::init(const std::string & configFilePath) {
	_serverPool = config::parse(configFilePath);
	std::for_each(_serverPool.begin(), _serverPool.end(), ServerPool::locationsInit);
}

void	ServerPool::locationsInit(config::Server &serv) {

	std::vector<LocationConfig>& locations = serv.get_locations();

	if (locations.empty() == false) {

		std::vector<LocationConfig>::iterator it = locations.begin();
		std::vector<LocationConfig>::iterator ite = locations.end();
		for (; it != ite; it++) {
			it->_root.assign((it->_root.empty())
										? serv.get_root() : it->_root);
			it->_index.assign((it->_index.empty())
										? serv.get_index() : it->_index);
			it->_body_size = (it->_body_size == LocationConfig::SIZE_UNSET)
										? serv.get_body_size() : it->_body_size;
		}
	}
}



/* ................................. ACCESSOR ................................*/


const std::vector<config::Server> &	ServerPool::getPool( void ) {

	return _serverPool;
}

std::set<int>				ServerPool::getPorts( void ) {

	std::set<int>	ports;
	std::vector<config::Server>::iterator it = _serverPool.begin();
	for (; it != _serverPool.end(); it++) {
		ports.insert(it->get_port());
	}
	return ports;
}

config::Server const&		ServerPool::getServerMatch( std::string hostHeader,
																int receivedPort ) {

	// clean the host header to get the IP/name part only
	size_t	portPos = hostHeader.find(':');
	if (portPos != std::string::npos)
		hostHeader.resize(portPos);

	//iterate in reverse order on vector to get the best match
	std::vector<config::Server>::reverse_iterator	it = _serverPool.rbegin();
	std::vector<config::Server>::reverse_iterator	bestCandidate = _serverPool.rend();
	for (;it != _serverPool.rend(); it++)	{
		if (it->get_port() == receivedPort)
			bestCandidate = it;

		if (it->get_port() == receivedPort
		&& (it->get_address() == hostHeader || it->get_name() == hostHeader))
			return *it;
	}
	return *bestCandidate;
}

LocationConfig const ServerPool::getLocationMatch( config::Server const & serv,
															Target const & target ) {

	std::vector<LocationConfig> const & locs = serv.get_locations();
	std::vector<LocationConfig>::const_iterator it = locs.begin();
	std::vector<LocationConfig>::const_iterator ite = locs.end();
	for (; it != ite; it++)	{
		if (it->_path == target.path)
			return *it;

		//TODO debug remove
		std::cout << "not this location......." << it->_path << std::endl;

	}
	return (LocationConfig(serv));
}

/* ................................. OVERLOAD ................................*/

/*
ServerPool &				ServerPool::operator=( ServerPool const & rhs )	{
	if ( this != &rhs )	{
		this->_value = rhs.getValue();
	}
	return *this;
}
*/

/*
std::ostream &			operator<<( std::ostream & o, ServerPool const & i )	{
	o << "Value = " << i.getValue();
	return o;
}
*/


/* ................................... DEBUG .................................*/

void ServerPool::debugPrint( void )  {

	std::cout << "___________DEBUG" << std::endl;
	std::vector<config::Server>::iterator it = _serverPool.begin();
	for (; it != _serverPool.end(); it++) {
		std::cout << *it;
	}
}

/* ................................. END CLASS................................*/
} // end namespace network
