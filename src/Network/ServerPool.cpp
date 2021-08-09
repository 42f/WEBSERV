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

void	ServerPool::init(const std::string & configFilePath) {
	_serverPool = config::parse(configFilePath);
}

// ! WIP ------------------------------------------------
void	ServerPool::locationsInit(config::Server &serv) {

	std::vector<LocationConfig> locations = serv.get_locations();

	if (locations.empty() == false) {

		std::vector<LocationConfig>::iterator it = locations.begin();
		std::vector<LocationConfig>::iterator ite = locations.end();
		for (; it != ite; it++) {
			it->_root.assign((it->_root.empty()) ? serv.get_root() : it->_root);
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

config::Server const&		ServerPool::getServerMatch( std::string hostHeader ) {

	// clean the host header to get the IP part only
	std::cout << "header is:    " << hostHeader << std::endl;
	size_t	portPos = hostHeader.find(':');
	if (portPos != std::string::npos)
		hostHeader.resize(portPos);
	std::cout << "trying to match with: " << hostHeader << std::endl;

	std::vector<config::Server>::iterator	it = _serverPool.begin();
	for (;it != _serverPool.end(); it++)	{
		if (it->get_address() == hostHeader || it->get_name() == hostHeader)
			return *it;
	}
	return *_serverPool.begin();
}

// config::Server const&	ServerPool::getLocationMatch( config::Server const & serv,
// 															Target const & target ) {
//  (void)serv;
//  (void)target;

// 	// std::vector<LocationConfig>::const_iterator it = serv.get_locations().begin();



// 	// for (; it != serv.get_locations().end(); it++ )	{
// 	// 	if ( it->path )
// 	// }
// }


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

/*
void ServerPool::debugPrint( void ) const {

}
*/

/* ................................. END CLASS................................*/
} // end namespace network
