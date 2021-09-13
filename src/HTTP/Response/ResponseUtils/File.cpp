#include "File.hpp"

#define FD_UNSET -1

namespace files {

std::map<std::string, std::string> files::File::_types;

/* ............................... CONSTRUCTOR ...............................*/

File::File(void) : _fd(FD_UNSET), _inode(0), _error(0), _flags(0), _mode(644) {}

File::File(std::string const& path, int flags, int mode)
    : _fd(FD_UNSET), _inode(0), _path(path), _error(0), _flags(flags), _mode(mode) {
  openFile();
}

/* ..............................COPY CONSTRUCTOR.............................*/

File::File(const File& src) { *this = src; }

/* ................................ DESTRUCTOR ...............................*/

File::~File(void) {
  if (_fd > FD_UNSET) close(_fd);
}

/* ................................. METHODS .................................*/

/* ................................. ACCESSOR ................................*/

void File::init(std::string const& path, int flags, int mode) {
  if (_fd != FD_UNSET) {
    close(_fd);
  }
  _path = path;
  _flags = flags;
  _mode = mode;
  openFile();
}

void File::openFile() {
  _error = 0;
  errno = 0;
  if (_flags == O_RDONLY)
    _fd = open(_path.c_str(), _flags);
  else
    _fd = open(_path.c_str(), _flags, _mode);
  if (_fd < 0)
    _error = errno;
  else {
    struct stat st;
    fstat(_fd, &st);
    _inode = st.st_ino;
  }
}

bool        File::isFile( void ) const {
  struct stat st;
  if (stat(_path.c_str(), &st) != 0) {
    return false;
  }
  return !S_ISDIR(st.st_mode);
}

bool        File::isDir( void ) const {
  struct stat st;
  if (stat(_path.c_str(), &st) != 0) {
    return false;
  }
  return S_ISDIR(st.st_mode);
}

std::string File::getLastModified() const {
  if (isGood()) {
    struct stat st;
    if (stat(_path.c_str(), &st) != 0) {
      return std::string();
    }
    return Timer::getTimeStr(gmtime(&(st.st_mtime)));
  }
  return std::string();
}

size_t File::getSize() const {
  if (_path.empty()) {
    return 0;
  }
  struct stat st;
  if (stat(_path.c_str(), &st) != 0) {
    return 0;
  }
  return st.st_size;
}

int File::getFD() const { return _fd; }

int File::getError() const { return _error; }

bool File::isGood() const {
  struct stat st;
  return _fd != FD_UNSET && fstat(_fd, &st) == 0 && st.st_ino == _inode;
}

// returns the content-type header field for the file instance
std::string File::getType(void) const {
  if (isFileFromPath(_path)) {
    return getTypeFromExt(getExtFromPath(_path));
  }
  return std::string(DEFAULT_CONTENT_TYPE);
}

std::string File::getFileName(void) const { return  getFileFromPath(_path); }
std::string File::getDirPart(void) const {  return getDirFromPath(_path); }

// returns the extension for the file isntance
std::string File::getExt(void) const { return getExtFromPath(_path); }

std::string File::getPath(void) const { return _path; }

bool File::isFileFromPath(std::string const& path) {
  if (path.empty()) return false;
  size_t lastPartHead = path.find_last_of('/');
  return path.find('.', lastPartHead) != std::string::npos;
}

bool File::isDirFromPath(std::string const& path) {
  if (path.empty()) return false;
  return !isFileFromPath(path);
}

// returns the filename + ext
std::string File::getFileFromPath(std::string const& path) {
  if (isFileFromPath(path)) {
    std::string output = path;
    size_t lastPartHead = output.find_last_of('/');
    return output.substr(lastPartHead);
  } else
    return std::string();
}

std::string File::getDirFromPath(std::string const& path) {
  if (isFileFromPath(path)) {
    std::string output = path;
    size_t lastPartHead = output.find_last_of('/');
    if (lastPartHead != std::string::npos) output.resize(lastPartHead);
    output += "/";
    return output;
  } else
    return path;
}

// returns the extension of a given path, if it is a file path
std::string File::getExtFromPath(std::string const& path) {
  if (isFileFromPath(path)) {
    std::string output = getFileFromPath(path);
    size_t extPos = output.find_last_of('.');
    extPos += extPos != std::string::npos ? 1 : 0;
    return output.substr(extPos);
  }
  return std::string();
}

// returns the content-type header from a given extension
std::string File::getTypeFromExt(std::string const& extSrc) const {
  typesMap_t::iterator ext = _types.find(extSrc);
  if (ext != _types.end()) return ext->second;
  return std::string(DEFAULT_CONTENT_TYPE);
}

void File::initContentTypes(char const* pathTypesConf) {
  std::ifstream file(pathTypesConf);

  if (file.good() == false) {
    std::cout << RED << "Error: Could not read Types MIME config files in "
              << pathTypesConf << NC << std::endl;
    exit(1);
  }

  size_t const sizeBuff = 1024;
  char buff[sizeBuff];
  std::string value;
  std::string format;

  while (file.good()) {
    value.clear();
    bzero(buff, sizeBuff);
    file.getline(buff, sizeBuff - 1, ';');
    size_t i = 0;
    while (buff[i] != '\0') {
      if (isblank(buff[i]) == false && buff[i] != '\n') {
        if (value.empty()) {
          while (buff[i] != '\0' && isblank(buff[i]) == false) {
            value += buff[i++];
          }
        } else {
          while (buff[i] != '\0' && isblank(buff[i]) == false) {
            format += buff[i++];
          }
          _types[format] = value;
          format.clear();
        }
      } else
        i++;
    }
  }
}

/* ................................. OVERLOAD ................................*/

/* ................................... DEBUG .................................*/

/* ................................. END CLASS................................*/
}  // namespace files
