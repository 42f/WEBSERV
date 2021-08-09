#include "File.hpp"

namespace  files {

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

std::ifstream &		File::getStream() {
	return _fileStream;
}

bool				File::isGood() {
	return _fileStream.good();
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
		while (i.getStream().good()) {
			i.getStream().get(c);
			o << c;
		}
		return o;
	}

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
} // end namespace fileHandler
