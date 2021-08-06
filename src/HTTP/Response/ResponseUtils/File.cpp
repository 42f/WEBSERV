#include "File.hpp"

namespace fileHandler {

/* ............................... CONSTRUCTOR ...............................*/

	File::File( std::string const & path ) : _fileStream(path)	{
	}

/* ..............................COPY CONSTRUCTOR.............................*/

	/*
	File::File( const File & src ) {
	}
	*/

/* ................................ DESTRUCTOR ...............................*/

	File::~File( void )	{
		_fileStream.close();
	}

/* ................................. METHODS .................................*/



/* ................................. ACCESSOR ................................*/

std::ifstream &		File::get_stream() {
	return _fileStream;
}

/* ................................. OVERLOAD ................................*/

	/*
	File &				File::operator=( File const & rhs )	{
		if ( this != &rhs )	{
			this->_value = rhs.getValue();
		}
		return *this;
	}
	*/

	std::ostream &			operator<<( std::ostream & o, File & i )	{

		char c;
		while (i.get_stream().good()) {
			i.get_stream().get(c);
			o << c;
		}
		return o;
	}

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
} // end namespace fileHandler
