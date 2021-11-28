#include "webscraping.h"
#include "resource.h"

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

static struct
{
	CURL * curl;

} s_curlObj = { 0 };

static inline bool s_autoInitCurl()
{
	if (s_curlObj.curl != NULL)
	{
		return true;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	s_curlObj.curl = curl_easy_init();

	if (s_curlObj.curl == NULL)
	{
		return false;
	}
	// Load cacert resource

	const void * fileMem;
	DWORD fileSize;

	fileMem = rsc_loadFile(IDF_CACERT_PEM, &fileSize);

	struct curl_blob blob = {
		.data = (void *)fileMem,
		.len = fileSize,
		.flags = CURL_BLOB_COPY
	};
	curl_easy_setopt(s_curlObj.curl, CURLOPT_CAINFO_BLOB, &blob);

	return true;
}
static inline void s_autoDestroyCurl()
{
	if (s_curlObj.curl == NULL)
	{
		return;
	}
	curl_easy_cleanup(s_curlObj.curl);
	curl_global_cleanup();
}


struct UserMemStruct
{
	char * mem;
	size_t size;
};

static size_t pageLoaderCallback(void * contents, size_t size, size_t nmemb, void * userPtr)
{
	struct UserMemStruct * restrict userMem = userPtr;
	const size_t realSize = size * nmemb;

	char * ptr = realloc(userMem->mem, userMem->size + realSize + 1);

	if (ptr == NULL)
	{
		return 0;
	}

	userMem->mem = ptr;
	memcpy(&userMem->mem[userMem->size], contents, realSize);
	userMem->size += realSize;
	userMem->mem[userMem->size] = '\0';

	return realSize;
}

char * loadWebPageToStr(const char * pageURL, size_t * lenInBytes)
{
	if (s_autoInitCurl() == false)
	{
		return NULL;
	}

	struct UserMemStruct userMem = { 0 };

	curl_easy_setopt(s_curlObj.curl, CURLOPT_URL, pageURL);
	curl_easy_setopt(s_curlObj.curl, CURLOPT_WRITEFUNCTION, &pageLoaderCallback);
	curl_easy_setopt(s_curlObj.curl, CURLOPT_WRITEDATA, &userMem);
	curl_easy_setopt(s_curlObj.curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	CURLcode result = curl_easy_perform(s_curlObj.curl);

	curl_easy_cleanup(s_curlObj.curl);

	if (result != CURLE_OK)
	{
		if (userMem.mem != NULL)
		{
			free(userMem.mem);
		}
		return NULL;
	}
	else
	{
		(*lenInBytes) = userMem.size;
		return userMem.mem;
	}
}
