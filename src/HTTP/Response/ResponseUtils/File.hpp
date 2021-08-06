#pragma once

# include <fstream>
# include <iostream>
# include <istream>
# include <string>

namespace fileHandler {

	enum Status
	{
		Waiting,
		Ready		// Ready to be sent to be streamed
	};

	class File	{

		friend std::ostream &	operator<<( std::ostream & o, File const & i );

		public:

			std::ifstream &			get_stream(void);
			std::ifstream const &	get_stream(void) const;

			File(  std::string const & path  );
			~File( void );



		private:

			std::ifstream		_fileStream;

			File( File const & src );
			File &		operator=( File const & rhs );

		};


} // --- end of namespace fileHandler
