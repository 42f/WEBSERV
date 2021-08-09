#pragma once

# include <iostream>
# include <string>
# include <map>
# include <utility>

namespace headerTitle {

	enum Title {
		Allow,
		Content_Language,
		Content_Length,
		Content_Location,
		Content_Type,
		Date,
		Last_Modified,
		Location,
		Retry_After,
		Server,
		Transfer_Encoding,
		WWW_Authenticate
	};
}

// TODO ------------ REMOVE ? --------------------------------------------------
/*	class HeaderTitleField {

		public:
			static std::string& get(Title title);

		private:
			std::map<Title, std::string>	_fields;
			static HeaderTitleField			_instance;

			HeaderTitleField();


	class DefaultValues {

		public:
			static std::string& get(Title title);

		private:
			std::map<Title, std::string>	_fields;
			static DefaultValues			_instance;

			DefaultValues();

	};
}

class ResponseHeader	{

	friend std::ostream &	operator<<( std::ostream & o, ResponseHeader const & i );

	public:

		typedef std::pair<const std::string, std::string>  			header_t;

		ResponseHeader( std::string const customHeader, std::string value = "");
		ResponseHeader( headerTitle::Title title, std::string value = "" );
		ResponseHeader( ResponseHeader const & src );

		header_t&			getData( void );
		const std::string&	field( void ) const;
		const std::string&	value( void ) const;

		~ResponseHeader( void );

	private:

		header_t			_header;

		ResponseHeader( void );
		ResponseHeader &		operator=( ResponseHeader const & rhs );
};
*/
