#include "request.h"

#include "../exception.h"

namespace Hpp
{

namespace Http
{

Request::Request(std::string const& url, Flags flags) :
curl(curl_easy_init())
{
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseReader);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	open(url, flags);
}

Request::~Request(void)
{
	curl_easy_cleanup(curl);
}

void Request::open(std::string const& url, Flags flags)
{
	// Reset response
	response_data.clear();
	response_http_code = 0;

	// Refresh options
	extractFlags(flags);

	// Url
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	// Redirects
	if (disable_redirects) curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
	else curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode perform_result = curl_easy_perform(curl);
	if (perform_result != 0) {
		throw Exception("Unable to connect to " + url + "! Reason: " + curl_error);
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_http_code);
}

void Request::extractFlags(Request::Flags flags)
{
	disable_redirects = flags & DISABLE_REDIRECTS;
}

size_t Request::responseReader(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	size_t totalsize = size * nmemb;
	Request* r = reinterpret_cast< Request* >(userdata);

	r->response_data.insert(r->response_data.end(), ptr, ptr + totalsize);

	return totalsize;
}

}

}
