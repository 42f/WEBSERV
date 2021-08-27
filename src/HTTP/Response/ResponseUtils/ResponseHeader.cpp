#include "ResponseHeader.hpp"

namespace headerTitle {

	/*
	 *	HEADER FIELDS: part before `:` in the header
	 */
	HeaderTitleField	HeaderTitleField::_instance;

	HeaderTitleField::HeaderTitleField() {


		_fields[headerTitle::Allow] = "Allow";
		_fields[headerTitle::Content_Language] = "Content-Language";
		_fields[headerTitle::Content_Length] = "Content-Length";
		_fields[headerTitle::Content_Location] = "Content-Location";
		_fields[headerTitle::Content_Type] = "Content-Type";
		_fields[headerTitle::Date] = "Date";
		_fields[headerTitle::Last_Modified] = "Last-Modified";
		_fields[headerTitle::Location] = "Location";
		_fields[headerTitle::Retry_After] = "Retry-After";
		_fields[headerTitle::Server] = "Server";
		_fields[headerTitle::Transfer_Encoding] = "Transfer-Encoding";
		_fields[headerTitle::WWW_Authenticate] = "WWW-Authenticate";

	}

	std::string& 	HeaderTitleField::get(Title title) {
		return HeaderTitleField::_instance._fields[title];
	}
}
