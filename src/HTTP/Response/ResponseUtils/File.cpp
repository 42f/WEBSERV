#include "File.hpp"

#define FD_UNSET -1

namespace  files {

	std::map<std::string, std::string>   files::File::_types;


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

bool	File::isFileFromPath( std::string const & path) {

	size_t lastPartHead = path.find_last_of('/');
	return path.find('.', lastPartHead) != std::string::npos;
}

std::string	File::getFileFromPath(std::string const & path) {

	if (isFileFromPath(path)) {
		std::string output = path;
		size_t lastPartHead = output.find_last_of('/');
		return output.substr(lastPartHead);
	}
	else
		return std::string();
}

std::string	File::getType( void ) const {

	if (isFile(_path)) {
		size_t extPos = _path.find_last_of('.');
		extPos += (extPos != std::string::npos) ? 1 : 0;
		typesMap_t::iterator ext = _types.find(_path.substr(extPos));
		if (ext != _types.end())
			return ext->second;
	}
	return std::string(DEFAULT_CONTENT_TYPE);
}

void		File::initContentTypes( char const * pathTypesConf ) {

	std::ifstream	file(pathTypesConf);

	if (file.good() == false) {
		std::cout << RED << "Error: Could not read Types MIME config files in " << pathTypesConf << NC << std::endl;
		exit(1);
	}

	size_t const sizeBuff = 1024;
	char buff[sizeBuff];
	std::string value;
	std::string format;

	while (file.good()) {
		value.clear();
		bzero(buff, sizeBuff);
		file.getline(buff, sizeBuff - 1, ';' );
		size_t i = 0;
		while (buff[i] != '\0') {

			if (isblank(buff[i]) == false && buff[i] != '\n') {
				if (value.empty()) {
					while (buff[i] != '\0' && isblank(buff[i]) == false) {
						value += buff[i++];
					}
				}
				else {
					while (buff[i] != '\0' && isblank(buff[i]) == false) {
						format += buff[i++];
					}
					_types[format] = value;
					format.clear();
				}
			}
			else
				i++;
		}
	}
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
