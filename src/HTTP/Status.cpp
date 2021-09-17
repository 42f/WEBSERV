//
// Created by alena on 30/07/2021.
//

#include "Status.hpp"

namespace status
{

	StatusMessage	StatusMessage::_instance;

	StatusMessage::StatusMessage()
	{
		_msgs[status::None] = "";
		_msgs[status::Continue] = "Continue";
		_msgs[status::SwitchingProtocols] = "Switching Protocols";
		_msgs[status::Processing] = "Processing";
		_msgs[status::EarlyHints] = "Early Hints";
		_msgs[status::Ok] = "OK";
		_msgs[status::Created] = "Created";
		_msgs[status::Accepted] = "Accepted";
		_msgs[status::NonAuthoritativeInformation] = "Non Authoritative Information";
		_msgs[status::NoContent] = "No Content";
		_msgs[status::ResetContent] = "Reset Content";
		_msgs[status::PartialContent] = "Partial Content";
		_msgs[status::MultiStatus] = "Multi Status";
		_msgs[status::AlreadyReported] = "Already Reported";
		_msgs[status::IMUsed] = "IM Used";
		_msgs[status::MultipleChoices] = "Multiple Choices";
		_msgs[status::MovedPermanently] = "Moved Permanently";
		_msgs[status::Found] = "Found";
		_msgs[status::SeeOther] = "See Other";
		_msgs[status::NotModified] = "Not Modified";
		_msgs[status::UseProxy] = "Use Proxy";
		_msgs[status::TemporaryRedirect] = "Temporary Redirect";
		_msgs[status::PermanentRedirect] = "Permanent Redirect";
		_msgs[status::BadRequest] = "Bad Request";
		_msgs[status::Unauthorized] = "Unauthorized";
		_msgs[status::PaymentRequired] = "Payment Required";
		_msgs[status::Forbidden] = "Forbidden";
		_msgs[status::NotFound] = "NotFound";
		_msgs[status::MethodNotAllowed] = "Method Not Allowed";
		_msgs[status::NotAcceptable] = "Not Acceptable";
		_msgs[status::ProxyAuthenticationRequired] = "Proxy Authentication Required";
		_msgs[status::RequestTimeout] = "Request Timeout";
		_msgs[status::Conflict] = "Conflict";
		_msgs[status::Gone] = "Gone";
		_msgs[status::LengthRequired] = "Length Required";
		_msgs[status::PreconditionFailed] = "Precondition Failed";
		_msgs[status::PayloadTooLarge] = "Payload Too Large";
		_msgs[status::URITooLong] = "URI Too Long";
		_msgs[status::UnsupportedMediaType] = "Unsupported Media Type";
		_msgs[status::RangeNotSatisfiable] = "Range Not Satisfiable";
		_msgs[status::ExpectationFailed] = "Expectation Failed";
		_msgs[status::Teapot] = "I'm a teapot";
		_msgs[status::MisdirectedRequest] = "Misdirected Request";
		_msgs[status::UnprocessableEntity] = "Unprocessable Entity";
		_msgs[status::Locked] = "Locked";
		_msgs[status::FailedDependency] = "Failed Dependency";
		_msgs[status::TooEarly] = "Too Early";
		_msgs[status::UpgradeRequired] = "Upgrade Required";
		_msgs[status::PreconditionRequired] = "Precondition Required";
		_msgs[status::TooManyRequests] = "Too Many Requests";
		_msgs[status::RequestHeaderFieldsTooLarge] = "Request Header Fields TooLarge";
		_msgs[status::UnavailableForLegalReasons] = "Unavailable For Legal Reasons";
		_msgs[status::InternalServerError] = "Internal ServerError";
		_msgs[status::NotImplemented] = "Not Implemented";
		_msgs[status::BadGateway] = "Bad Gateway";
		_msgs[status::ServiceUnavailable] = "Service Unavailable";
		_msgs[status::GatewayTimeout] = "Gateway Timeout";
		_msgs[status::HTTPVersionNotSupported] = "HTTP Version Not Supported";
		_msgs[status::VariantAlsoNegotiates] = "Variant Also Negotiates";
		_msgs[status::InsufficientStorage] = "Insufficient Storage";
		_msgs[status::LoopDetected] = "Loop Detected";
		_msgs[status::NotExtended] = "Not Extended";
		_msgs[status::NetworkAuthenticationRequired] = "Network Authentication Required";
	}

	std::string & StatusMessage::get(StatusCode status)
	{
		return StatusMessage::_instance._msgs[status];
	}

	std::string &message(StatusCode status)
	{
		return StatusMessage::get(status);
	}

}
