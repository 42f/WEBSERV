#include "File.hpp"
#include "utils/Logger.hpp"

namespace  files {

/* ............................... CONSTRUCTOR ...............................*/

	File::File( void ) : _fileStream() {
	}

	File::File( std::string const & path ) : _fileStream(path)	{
	}

/* ..............................COPY CONSTRUCTOR.............................*/

	File::File( const File & src ) {
		*this = src;
	}

/* ................................ DESTRUCTOR ...............................*/

	File::~File( void )	{
		_fileStream.close();
	}

/* ................................. METHODS .................................*/



/* ................................. ACCESSOR ................................*/

size_t				File::getSize() {

	// ignore extacts characters from file and discard them, until the end of file
	_fileStream.ignore( std::numeric_limits<std::streamsize>::max() );
	// returns the amount of bytes of the last read (in this case, ignore() )
	std::streamsize size = _fileStream.gcount();

	// Reset the EOF flag and index to 0
	_fileStream.clear();
	_fileStream.seekg( 0, std::ios_base::beg );
	return size;
}

std::ifstream &		File::getStream() {
	return _fileStream;
}

bool				File::isGood() {
	return _fileStream.good();
}

/* ................................. OVERLOAD ................................*/

	File &				File::operator=( File const & rhs )	{
		if ( this != &rhs )	{
			// this->_fileStream = rhs._fileStream;
			this->_path = rhs._path;
			_fileStream.open(_path);
		}
		return *this;
	}

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
