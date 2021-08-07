#pragma once

# include <iostream>
# include <string>
# include <vector>
# include <set>
# include "../Config/Server.hpp"

namespace network {

	class ServerPool	{

		public:

			static void		init(const std::string & configFilePath);

			static std::vector<config::Server> const &	getPool( void );
			static std::set<int>						getPorts( void );
			static config::Server const&	getServerMatch( std::string hostHeader );
			// static Result<Target> const&	getLocationMatch( config::Server const & serv,
			// 												Target const & target );

			~ServerPool( void );


		private:

			static std::vector<config::Server>		_serverPool;

			ServerPool( void );
			ServerPool( ServerPool const & src );
			ServerPool &		operator=( ServerPool const & rhs );



		};

		//std::ostream &			operator<<( std::ostream & o, ServerPool const & i );

} // --- end of namespace network
