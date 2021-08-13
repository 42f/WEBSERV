#include "File.hpp"
#include "Logger.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FD_UNSET -1

namespace  files {

/* ............................... CONSTRUCTOR ...............................*/

	File::File( void ) :  _fd(FD_UNSET), _error(0), _flags(0) {
	}

	File::File( std::string const & path, int flags) : _fd(FD_UNSET), _path(path), _error(0), _flags(flags) {

		openFile();
	}

/* ..............................COPY CONSTRUCTOR.............................*/

	File::File( const File & src ) {
		*this = src;
	}

/* ................................ DESTRUCTOR ...............................*/

	File::~File( void )	{
		if (_fd > FD_UNSET)
			close(_fd);
	}

/* ................................. METHODS .................................*/



/* ................................. ACCESSOR ................................*/

void				File::openFile() {
		_error = 0;
		errno = 0;
		_fd = open(_path.c_str(), _flags);
		if (_fd < 0)
			_error = errno;

}

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

int 		File::getError() const {
	return _error;
}

bool		File::isGood() const {
	struct stat st;
	return _fd > FD_UNSET && fstat(_fd, &st) == 0;
}

/* ................................. OVERLOAD ................................*/

	File &				File::operator=( File const & rhs )	{
		if ( this != &rhs )	{
			_flags = rhs._flags;
			_path = rhs._path;
			_error = 0;

			openFile();
		}
		return *this;
	}

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
} // end namespace fileHandler
