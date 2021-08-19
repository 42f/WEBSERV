#pragma once

# include <iostream>
# include <fstream>
# include <string>
# include <map>
# include <fcntl.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <stdlib.h>

# include "Logger.hpp"
# include "Timer.hpp"
# include "Constants.hpp"

namespace files {

	class File	{

        friend class GetMethod;

		public:


			File( void );
			File( std::string const & path, int flags = O_RDONLY );
			~File( void );

			File &		operator=( File const & rhs );

			bool		isGood(void) const;
            int         getFD( void ) const;
            size_t		getSize( void ) const;
            int         getError( void ) const;

            std::string         getLastModified( void ) const;
            std::string         getType( void ) const;
            std::string         getPath( void ) const;
            std::string         getExt( void ) const;
            std::string         getTypeFromExt( std::string const & ext ) const;

            static bool         isFileFromPath( std::string const & path );
            static bool         isDirFromPath( std::string const & path );
            static std::string	getFileFromPath( std::string const & path );
            static std::string	getExtFromPath( std::string const & path );

            static void initContentTypes( char const * pathTypesConf );

		private:

            typedef std::map<std::string, std::string>   typesMap_t;

            static typesMap_t   _types;
            int                 _fd;
            ino_t               _inode;
			std::string 		_path;
			int          		_error;
			int          		_flags;

            void        openFile( void );
			File( File const & src );
		};

} // --- end of namespace fileHandler
