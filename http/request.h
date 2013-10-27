#ifndef HPP_HTTP_REQUEST_H
#define HPP_HTTP_REQUEST_H

#include "../bytev.h"
#include "../noncopyable.h"

#include <curl/curl.h>
#include <string>
#include <stdint.h>

namespace Hpp
{

namespace Http
{

class Request : public NonCopyable
{

public:

	Request(std::string const& url);
	~Request(void);

	void open(std::string const& url);

	inline std::string getResponseText(void) const { return std::string((char*)&response_data[0], response_data.size()); }
	inline uint16_t getHttpCode(void) const { return response_http_code; }

private:

	CURL* curl;

	char curl_error[CURL_ERROR_SIZE];

	ByteV response_data;
	long response_http_code;

	static size_t responseReader(char* ptr, size_t size, size_t nmemb, void* userdata);

};

}

}

#endif
