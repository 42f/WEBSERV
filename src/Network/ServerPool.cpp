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

config::Server const&		ServerPool::getMatch( std::string hostHeader ) {

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
