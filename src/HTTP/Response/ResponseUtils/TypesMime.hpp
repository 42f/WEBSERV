#pragma once

# include "File"
# include <iostream>
# include <string>

namespace files {

	class TypesMime	{

			typdef	std::map<std::string, std::string> 	typesMap;
		public:

			static bool	init( File & conf_mime );
			~TypesMime( void );

			static std::string & const	getMime(std::string & const extension) const;

		private:
			static std::map<std::string, std::string>	types;

			TypesMime( void );
			TypesMime( TypesMime const & src );
			TypesMime &		operator=( TypesMime const & rhs );

		};

		std::ostream &			operator<<( std::ostream & o, TypesMime const & i );

} // --- end of namespace files
