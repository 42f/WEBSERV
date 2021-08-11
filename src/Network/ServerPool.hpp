#pragma once

# include <iostream>
# include <string>
# include <vector>
# include <set>
# include <algorithm>

# include "Config/Directives/Location.hpp"

namespace network {

	class ServerPool	{

		public:

			static void		init(const std::string & configFilePath);

			static std::vector<config::Server> const &	getPool( void );
			static std::set<int>						getPorts( void );
			static config::Server const&	getServerMatch( std::string hostHeader,
																int receivedPort );
			static LocationConfig const		getLocationMatch( config::Server const & serv,
															Target const & target );

			~ServerPool( void );


		private:

			static std::vector<config::Server>		_serverPool;

			static void		locationsInit(config::Server &serv);
			static bool		isPathMatch( LocationConfig const & loc, Target const & target);
			static void		cleanLocationPath(std::string &locPath);

			ServerPool( void );
			ServerPool( ServerPool const & src );
			ServerPool &		operator=( ServerPool const & rhs );

			static void 		debugPrint( void ) ;

			// friend std::ostream &	operator<<( std::ostream & o, ServerPool const & i );

		};


} // --- end of namespace network
