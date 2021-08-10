#pragma once

# include "File.hpp"
# include <iostream>
# include <string>
# include <map>

namespace files {

	class TypesMime	{

			typedef	std::map<std::string, std::string> 	typesMap;
		public:

			static bool	init( File & conf_mime );
			~TypesMime( void );

			// static std::string & 	getMime(std::string & const extension);

		private:
			static typesMap	types;

			TypesMime( void );
			TypesMime( TypesMime const & src );
			TypesMime &		operator=( TypesMime const & rhs );

		};

		std::ostream &			operator<<( std::ostream & o, TypesMime const & i );

} // --- end of namespace files
