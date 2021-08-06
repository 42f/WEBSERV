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


		public:

			std::ifstream &			get_stream(void);

			File(  std::string const & path  );
			~File( void );



		private:

			std::ifstream		_fileStream;

			File( File const & src );
			File &		operator=( File const & rhs );

		};

		std::ostream &	operator<<( std::ostream & o, File  & i );

} // --- end of namespace fileHandler
