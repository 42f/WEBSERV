#pragma once

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "Constants.hpp"
#include "Logger.hpp"
#include "Timer.hpp"

namespace files {

class File {
  friend class GetMethod;

 public:
  File(void);
  File(std::string const& path, int flags = O_RDONLY, int mode = 0644);
  ~File(void);

  void init(std::string const& path, int flags = O_RDONLY, int mode = 0644);
  bool isGood(void) const;
  int getFD(void) const;
  size_t getSize(void) const;
  int getError(void) const;

  bool isFile(void) const;
  bool isDir(void) const;

  std::string getLastModified(void) const;
  std::string getType(void) const;
  std::string getPath(void) const;
  std::string getExt(void) const;
  std::string getFileName(void) const;
  std::string getTypeFromExt(std::string const& ext) const;

  static bool isFileFromPath(std::string const& path);
  static bool isDirFromPath(std::string const& path);
  static std::string getFileFromPath(std::string const& path);
  static std::string getDirFromPath(std::string const& path);
  static std::string getExtFromPath(std::string const& path);

  static void initContentTypes(char const* pathTypesConf);

 private:
  typedef std::map<std::string, std::string> typesMap_t;

  static typesMap_t _types;
  int _fd;
  ino_t _inode;
  std::string _path;
  int _error;
  int _flags;
  int _mode;

  void openFile(void);
  File(File const& src);
};

}  // namespace files
