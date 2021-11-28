#include "safelib.h"

#define __STDC_WANT_LIB_EXT1__ 1

#include <string.h>


const char * strstr_s(const char * restrict str, const size_t maxCount, const char * const restrict substr)
{
	const size_t strLen = strnlen_s(str, maxCount), substrLen = strlen(substr);
	if (strLen < substrLen)
	{
		return NULL;
	}

	for (const char * restrict endp = str + strLen - substrLen; str != endp; ++str)
	{
		if (memcmp(str, substr, sizeof(char) * substrLen) == 0)
		{
			return str;
		}
	}

	return NULL;
}
const char * strpbrk_s(const char * restrict dest, const size_t maxCount, const char * const restrict breakset)
{
	// Create LUT
	int8_t occurrLUT[256] = { 0 };
	for (size_t i = 0; breakset[i] != '\0'; ++i)
	{
		occurrLUT[(uint8_t)breakset[i]] = 1;
	}

	for (const char * restrict endp = dest + strnlen_s(dest, maxCount); dest != endp; ++dest)
	{
		if (occurrLUT[(uint8_t)*dest])
		{
			return dest;
		}
	}

	return NULL;
}
const char * strchr_s(const char * restrict str, const size_t maxCount, const char ch)
{
	for (const char * restrict endp = str + strnlen_s(str, maxCount); str != endp; ++str)
	{
		if (*str == ch)
		{
			return str;
		}
	}

	return NULL;
}
const char * strrchr_s(const char * restrict str, const size_t maxCount, const char ch)
{
	const char * restrict lastChp = NULL;
	for (const char * restrict endp = str + strnlen_s(str, maxCount); str != endp; ++str)
	{
		if (*str == ch)
		{
			lastChp = str;
		}
	}

	return lastChp;
}
