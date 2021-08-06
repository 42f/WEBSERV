#include "ResponseHeader.hpp"

namespace headerTitle {

	/*
	 *	HEADER FIELDS: part before `:` in the header
	 */
	HeaderTitleField	HeaderTitleField::_instance;

	HeaderTitleField::HeaderTitleField() {


		_fields[headerTitle::Allow] = "Allow";
		_fields[headerTitle::Content_Language] = "Content_Language";
		_fields[headerTitle::Content_Length] = "Content_Length";
		_fields[headerTitle::Content_Location] = "Content_Location";
		_fields[headerTitle::Content_Type] = "Content_Type";
		_fields[headerTitle::Date] = "Date";
		_fields[headerTitle::Last_Modified] = "Last_Modified";
		_fields[headerTitle::Location] = "Location";
		_fields[headerTitle::Retry_After] = "Retry_After";
		_fields[headerTitle::Server] = "Server";
		_fields[headerTitle::Transfer_Encoding] = "Transfer_Encoding";
		_fields[headerTitle::WWW_Authenticate] = "WWW_Authenticate";

	}

	std::string& 	HeaderTitleField::get(Title title) {
		return HeaderTitleField::_instance._fields[title];
	}

	/*
	 * DEFAULT VALUE: part after `:` in the header, only part of them are defined
	 */

	DefaultValues	DefaultValues::_instance;

	DefaultValues::DefaultValues() {

		_fields[headerTitle::Allow] = "";
		_fields[headerTitle::Content_Language] = "";
		_fields[headerTitle::Content_Length] = "";
		_fields[headerTitle::Content_Location] = "";
		_fields[headerTitle::Content_Type] = "";
		_fields[headerTitle::Date] = "";
		_fields[headerTitle::Last_Modified] = "";
		_fields[headerTitle::Location] = "";
		_fields[headerTitle::Retry_After] = "";
		_fields[headerTitle::Server] = "Webserv_v.0.1";
		_fields[headerTitle::Transfer_Encoding] = "";
		_fields[headerTitle::WWW_Authenticate] = "";
	}

	std::string& 	DefaultValues::get(Title title) {
		return DefaultValues::_instance._fields[title];
	}
};

/******************************************************************************/
/******************************************************************************/


/* ............................... CONSTRUCTOR ...............................*/

ResponseHeader::ResponseHeader( std::string customHeader, std::string value) :
									_header(customHeader, value) {

}

ResponseHeader::ResponseHeader( headerTitle::Title title, std::string value ) :
				_header(headerTitle::HeaderTitleField::get(title) , value) {


}

ResponseHeader::ResponseHeader( void )	{ }

/* ..............................COPY CONSTRUCTOR.............................*/

ResponseHeader::ResponseHeader( const ResponseHeader & src ) :
											_header(src.field(), src.value()) {
}

/* ................................ DESTRUCTOR ...............................*/

ResponseHeader::~ResponseHeader( void )	{
}

/* ................................. METHODS .................................*/



/* ................................. ACCESSOR ................................*/

ResponseHeader::header_t&			ResponseHeader::getData( void ) {
	return _header;
}

const std::string&	ResponseHeader::field( void ) const {
	return _header.first;
}

const std::string&		ResponseHeader::value( void ) const {
	return _header.second;
}

/* ................................. OVERLOAD ................................*/

/*
ResponseHeader &				ResponseHeader::operator=( ResponseHeader const & rhs )	{
	if ( this != &rhs )	{
		this->_value = rhs.getValue();
	}
	return *this;
}
*/

std::ostream &			operator<<( std::ostream & o, ResponseHeader const & i )	{

	o << i.field() << ": " << i.value();
	return o;
}

/* ................................... DEBUG .................................*/

/*
void ResponseHeader::debugPrint( void ) const {

}
*/

/* ................................. END CLASS................................*/
