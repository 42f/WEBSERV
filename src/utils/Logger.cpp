#include <sstream>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <time.h>
#include <ctime>
#include "Logger.hpp"

const int Logger::toConsole = 0;
const int Logger::toFile = 1;

bool	Logger::_verbose = true;
int		Logger::_defaultDest = Logger::toFile;
std::string 	Logger::_fileName = "";

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Logger::Logger( std::string const destFile, int defaultDest )
{
	Logger::_fileName = destFile;
	Logger::_verbose = true;
	Logger::_defaultDest = defaultDest;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Logger::~Logger()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void			Logger::setVerbose(bool setting) {
	Logger::_verbose = setting;
}

std::string		Logger::_makeLogEntry(std::string rawEntry)	{
	std::stringstream	logEntry;
	std::stringstream	timestamp;
	time_t				theTime = time(NULL);
	struct tm			*aTime = localtime(&theTime);

 	timestamp << aTime->tm_year + 1900;
	timestamp << "." << std::setw(2) << std::setfill('0') << aTime->tm_mon + 1;
	timestamp << "." << std::setw(2) << std::setfill('0') << aTime->tm_mday;
	timestamp << '_';
	timestamp << std::setw(2) << std::setfill('0') << aTime->tm_hour << '.';
	timestamp << std::setw(2) << std::setfill('0') << aTime->tm_min << '.';
	timestamp << std::setw(2) << std::setfill('0') << aTime->tm_sec;
	logEntry << "--" << getpid() << "-- " << "[" << timestamp.str() << "] - " << rawEntry;
	return (logEntry.str());
}

bool		Logger::log( std::string const & message, int dest)  {

	int destination = (dest != Logger::_defaultDest) ? dest : Logger::_defaultDest;

	if (destination == Logger::toConsole && Logger::_verbose == true) {
		std::clog << _makeLogEntry(message) << std::endl;
	}
	else if (destination == Logger::toFile && Logger::_verbose == true) {
		std::ofstream	outputFS(Logger::_fileName.c_str(), std::ofstream::app);
		outputFS << _makeLogEntry(message) << std::endl;
		outputFS.close();
	}
	else	{
		return false;
	}
	return true;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

Logger&			Logger::getInstance(std::string const destFile, const int defaultDest) {

	static	Logger l(destFile, defaultDest);
	return l;
}

/* ************************************************************************** */
