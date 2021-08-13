#include "File.hpp"
#include "Logger.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FD_UNSET -1

namespace  files {

/* ............................... CONSTRUCTOR ...............................*/

	File::File( void ) : _fd(FD_UNSET), _error(0) {
	}

	File::File( std::string const & path, int flags) : _fd(FD_UNSET), _path(path), _error(0) {

		_fd = open(path.c_str(), flags);
		if (_fd < 0)
			_error = errno;
	}

/* ..............................COPY CONSTRUCTOR.............................*/

	File::File( const File & src ) {
		*this = src;
	}

/* ................................ DESTRUCTOR ...............................*/

	File::~File( void )	{
		close(_fd);
	}

/* ................................. METHODS .................................*/



/* ................................. ACCESSOR ................................*/

size_t				File::getSize() const {

	if (_fd > FD_UNSET)	{
		struct stat st;
		if(stat(_path.c_str(), &st) != 0) {
			return 0;
		}
		return st.st_size;
	}
	return 0;
}

int 		File::getFD() const {
	return _fd;
}

bool		File::isGood() const {
	// struct stat st;
	// return _fd > FD_UNSET && fstat(_fd, &st) == 0;
	return _fd > FD_UNSET;
}

/* ................................. OVERLOAD ................................*/

	File &				File::operator=( File const & rhs )	{
		if ( this != &rhs )	{
			_fd = open(rhs._path.c_str(), O_RDONLY);
			_path = rhs._path;
			_error = rhs._error;
		}

		// if ( this != &rhs )	{
		// 	_fd = rhs._fd;
		// 	_path = rhs._path;
		// 	_error = rhs._error;
		// }
		return *this;
	}

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
} // end namespace fileHandler
