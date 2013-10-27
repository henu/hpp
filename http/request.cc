#include "request.h"

#include "../exception.h"

namespace Hpp
{

namespace Http
{

Request::Request(std::string const& url) :
curl(curl_easy_init())
{
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseReader);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	open(url);
}

Request::~Request(void)
{
	curl_easy_cleanup(curl);
}

void Request::open(std::string const& url)
{
	response_data.clear();
	response_http_code = 0;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	CURLcode perform_result = curl_easy_perform(curl);
	if (perform_result != 0) {
		throw Exception("Unable to connect to " + url + "! Reason: " + curl_error);
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_http_code);
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
