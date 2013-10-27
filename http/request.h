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

	typedef uint8_t Flags;

	static Flags const DISABLE_REDIRECTS = 0x01;

	Request(std::string const& url, Flags flags = 0);
	~Request(void);

	void open(std::string const& url, Flags flags = 0);

	inline std::string getResponseText(void) const { return std::string((char*)&response_data[0], response_data.size()); }
	inline uint16_t getHttpCode(void) const { return response_http_code; }

private:

	// Curl stuff
	CURL* curl;
	char curl_error[CURL_ERROR_SIZE];

	// Options
	bool disable_redirects;

	// Response
	ByteV response_data;
	long response_http_code;

	void extractFlags(Flags flags);

	static size_t responseReader(char* ptr, size_t size, size_t nmemb, void* userdata);

};

}

}

#endif
