//
// Created by alena on 30/07/2021.
//

#ifndef WEBSERV_STATUS_HPP
#define WEBSERV_STATUS_HPP

#include <map>
#include <string>

//TODO list des http status code
namespace status
{
	enum StatusCode
	{
		None = 0,

	/*
	 * Errors starting by 1xx: informational
	*/

		Continue = 100,
		SwitchingProtocols,
		Processing,
		EarlyHints,
	/*
	 * Errors starting by 2xx: successful_status_code
	*/

		Ok = 200,
		Created,
		Accepted,
		NonAuthoritativeInformation,
		NoContent,
		ResetContent,
		PartialContent,
		MultiStatus,
		AlreadyReported,
		IMUsed = 226,
	/*
	 * Errors starting by 3xx: redirectionstatuscode
	*/
		MultipleChoices = 300,
		MovedPermanently,
		Found,
		SeeOther,
		NotModified,
		UseProxy,
		TemporaryRedirect = 307,
		PermanentRedirect,
	/*
	 * Errors starting by 4xx: clienterrorstatuscode
	*/

		BadRequest = 400,
		Unauthorized,
		PaymentRequired,
		Forbidden,
		NotFound,
		MethodNotAllowed,
		NotAcceptable,
		ProxyAuthenticationRequired,
		RequestTimeout,
		Conflict,
		Gone,
		LengthRequired,
		PreconditionFailed,
		PayloadTooLarge,
		URITooLong,
		UnsupportedMediaType,
		RangeNotSatisfiable,
		ExpectationFailed, //417
		MisdirectedRequest = 421,
		UnprocessableEntity,
		Locked,
		FailedDependency,
		TooEarly,
		UpgradeRequired,
		PreconditionRequired = 428,
		TooManyRequests,
		RequestHeaderFieldsTooLarge = 431,
		UnavailableForLegalReasons = 451,

	/*
	 * Errors starting by 5xx: servererrorstatuscode
	*/

		InternalServerError = 500,
		NotImplemented,
		BadGateway,
		ServiceUnavailable,
		GatewayTimeout,
		HTTPVersionNotSupported,
		VariantAlsoNegotiates,
		InsufficientStorage,
		LoopDetected,
		NotExtended = 510,
		NetworkAuthenticationRequired
	};

	class StatusMessage
	{
	private:
		std::map<StatusCode, std::string>	_msgs;
		static StatusMessage				_instance;

		StatusMessage();

	public:
		static std::string&	get(StatusCode status);
	};

	std::string&	message(StatusCode status);
}

#endif //WEBSERV_STATUS_HPP
