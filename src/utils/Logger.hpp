#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <iostream>
# include <string>

class Logger
{
	public:

		// log a new line
		int		log(std::string const &dest, std::string const & message);

		// set Verbose, true by default
		void			setVerbose(bool setting = true);

		static Logger&	getInstance(std::string const destFile = "");
		static const	std::string	toConsole;
		static const	std::string	toFile;

		~Logger();

	private:

		Logger( std::string const destFile);

		bool				_verbose;
		std::string			_makeLogEntry(std::string rawEntry);
		int					_logToConsole(std::string entry);
		int					_logToFile(std::string entry);
		std::string			_destFile;
};

#endif /* ********************************************************** LOGGER_H */
