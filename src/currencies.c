#include "currencies.h"
#include "winapi.h"
#include "webscraping.h"

#include <stdlib.h>
#include <stdio.h>
#include "safelib.h"

bool makeCurrency_str(Currency_t * restrict currency, const char * name, const char * value)
{
	// Convert normal string to UTF-16
	int bufLen = MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		name,
		-1,
		NULL,
		0
	);
	currency->wName = malloc(sizeof(wchar_t) * (size_t)bufLen);

	if (currency->wName == NULL)
	{
		return false;
	}
	MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		name,
		-1,
		currency->wName,
		bufLen
	);

	// Convert normal string to double
	currency->etalonValue = atof(value);

	return true;
}
bool makeCurrency_str_s(Currency_t * restrict currency, const char * name, size_t maxName, const char * value)
{
	// Convert normal string to UTF-16
	int bufLen = MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		name,
		(int)maxName,
		NULL,
		0
	);
	if (name[maxName] != '\0')
	{
		++bufLen;
	}

	currency->wName = malloc(sizeof(wchar_t) * (size_t)bufLen);

	if (currency->wName == NULL)
	{
		return false;
	}
	MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		name,
		(int)maxName,
		currency->wName,
		bufLen
	);
	currency->wName[bufLen - 1] = L'\0';

	// Convert normal string to double
	currency->etalonValue = atof(value);

	return true;
}
bool makeCurrency_double(Currency_t * restrict currency, const char * name, double value)
{
	// Convert normal string to UTF-16
	int bufLen = MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		name,
		-1,
		NULL,
		0
	);
	currency->wName = malloc(sizeof(wchar_t) * (size_t)bufLen);

	if (currency->wName == NULL)
	{
		return false;
	}
	MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		name,
		-1,
		currency->wName,
		bufLen
	);

	// Convert normal string to double
	currency->etalonValue = value;

	return true;
}
void destroyCurrency(Currency_t * restrict currency)
{
	if (currency->wName != NULL)
	{
		free(currency->wName);
	}
}

void destroyCurrenciesData(CurrenciesData_t * restrict currencies)
{
	if (currencies->currencies == NULL)
	{
		return;
	}
	for (uint32_t i = 0; i < currencies->n_currencies; ++i)
	{
		destroyCurrency(&currencies->currencies[i]);
	}
	free(currencies->currencies);
}

CurrenciesData_t currencyData = { 0 };

static struct
{
	const char * filePath;

} s_currencyData_impl;


static inline bool s_readCurrencyFile(CurrenciesData_t * restrict currencies)
{
	// Open file
	FILE * file = fopen(s_currencyData_impl.filePath, "r");
	if (file == NULL)
	{
		return false;
	}

	// Try to load number of currencies and date
	uint32_t numCur;
	
	if (fscanf(
			file,
			"%u %2hu.%2hu.%hu",
			&numCur,
			&currencies->date.day,
			&currencies->date.mon,
			&currencies->date.year
		) < 4
	)
	{
		fclose(file);
		return false;
	}

	// Date date loaded

	currencies->currencies = realloc(currencies->currencies, sizeof(Currency_t) * numCur);
	if (currencies->currencies == NULL)
	{
		fclose(file);
		return false;
	}

	for (uint32_t i = 0; i < numCur; ++i)
	{
		// Read all other lines
		char buf[32];
		double value;

		if (fscanf(file, "%32s %lg", buf, &value) < 2 ||
			makeCurrency_double(&currencies->currencies[i], buf, value) == false
		)
		{
			free(currencies->currencies);
			fclose(file);
			return false;
		}
	}

	fclose(file);

	currencies->n_currencies = numCur;

	return true;
}
static inline bool s_writeCurrencyFile(const CurrenciesData_t * const restrict currencies)
{
	if (currencies->currencies == NULL || currencies->n_currencies == 0)
	{
		return true;
	}
	FILE * file = fopen(s_currencyData_impl.filePath, "w");
	if (file == NULL)
	{
		return false;
	}

	// Write date
	fprintf(
		file,
		"%u %.2hu.%.2hu.%hu\n",
		currencies->n_currencies,
		currencies->date.day,
		currencies->date.mon,
		currencies->date.year
	);

	for (uint32_t i = 0; i < currencies->n_currencies; ++i)
	{
		const Currency_t * const restrict currency = &currencies->currencies[i];
		char buf[32] = { 0 };
		// Convert wide currency to small
		WideCharToMultiByte(
			CP_UTF8,
			0,
			currency->wName,
			-1,
			buf,
			32,
			NULL,
			NULL
		);

		fprintf(file, "%s %.3f\n", buf, currency->etalonValue);
	}

	fclose(file);
	return true;
}

static inline bool s_parseOneCurrency(
	Currency_t * restrict currency,
	const char * restrict * restrict pageData,
	size_t * restrict pageLen
)
{
	const char * restrict ptr = strstr_s(*pageData, *pageLen, "class=\"currency\"");
	if (ptr == NULL)
	{
		return false;
	}
	ptr = strrchr_s(*pageData, (size_t)(ptr - *pageData), '\n');
	if (ptr == NULL)
	{
		false;
	}
	*pageLen -= (size_t)(ptr - *pageData);
	*pageData = ptr;

	ptr = strstr_s(*pageData, *pageLen, "<td id=\"");
	if (ptr == NULL)
	{
		return false;
	}
	// Skip HTML td
	*pageLen -= (size_t)(ptr - *pageData);
	*pageData = ptr;

	ptr = strchr_s(*pageData, *pageLen, '"');
	if (ptr == NULL)
	{
		return false;
	}
	++ptr;

	*pageLen -= (size_t)(ptr - *pageData);
	*pageData = ptr;

	const char * restrict curNamePtr = *pageData;

	ptr = strchr_s(*pageData, *pageLen, '"');
	if (ptr == NULL)
	{
		return false;
	}
	size_t curNameLen = (size_t)(ptr - curNamePtr);

	*pageLen -= (size_t)(ptr - *pageData);
	*pageData = ptr;

	ptr = strstr_s(*pageData, *pageLen, "<span class=\"rate\">");
	if (ptr == NULL)
	{
		return false;
	}
	// Skip HTML span
	ptr += 19;

	const char * restrict curValuePtr = ptr;
	
	*pageLen -= (size_t)(ptr - *pageData);
	*pageData = ptr;

	ptr = strstr_s(*pageData, *pageLen, "</span>");
	if (ptr == NULL)
	{
		return false;
	}
	*pageLen -= (size_t)(ptr - *pageData);
	*pageData = ptr;

	return makeCurrency_str_s(currency, curNamePtr, curNameLen, curValuePtr);
}
static inline bool s_parseCurWebpage(
	CurrenciesData_t * restrict currencies,
	const char * restrict pageData,
	size_t pageLen
)
{
	/*
	 * General plan of parsing
	 * Data is located in <div*class="jumbo-box">
	 */
	const char * restrict ptr = strstr_s(pageData, pageLen, "<div class=\"jumbo-box\">");
	if (ptr == NULL)
	{
		return false;
	}
	pageLen -= (size_t)(ptr - pageData);
	pageData = ptr;

	ptr = strstr_s(pageData, pageLen, "<h3>");
	if (ptr == NULL)
	{
		return false;
	}
	// Skip HTML <h3>
	ptr += 4;

	pageLen -= (size_t)(ptr - pageData);
	pageData = ptr;

	// scan to first number
	ptr = strpbrk_s(pageData, pageLen, "0123456789");
	if (ptr == NULL)
	{
		return false;
	}
	pageLen -= (size_t)(ptr - pageData);
	pageData = ptr;

	// parse to date
	{
		char monthStr[16];
		// If less than 3 items were scanned
		if (sscanf_s(
				pageData,
				"%hu %s %hu",
				&currencies->date.day,
				monthStr,
				16,
				&currencies->date.year
			) < 3
		)
		{
			return false;
		}

		// Convert month string to number
		static const char months[12][10] = {
			"January",
			"February",
			"March",
			"April",
			"May",
			"June",
			"July",
			"August",
			"September",
			"October",
			"November",
			"December"
		};
		for (uint16_t i = 0; i < 12; ++i)
		{
			if (strcmp(monthStr, months[i]) == 0)
			{
				currencies->date.mon = i + 1;
				break;
			}
		}
	}
	
	// Get end of line
	ptr = strchr_s(pageData, pageLen, '\n');
	if (ptr == NULL)
	{
		return false;
	}
	pageLen -= (size_t)(ptr - pageData);
	pageData = ptr;

	// Get currency data
	
	// Get to the line that says class="currency"
	for (uint32_t i = 0; ; ++i)
	{
		Currency_t cur = { 0 };
		if (s_parseOneCurrency(&cur, &pageData, &pageLen) == false)
		{
			break;
		}

		// Add cur to currencies
		Currency_t * mem = realloc(currencies->currencies, sizeof(Currency_t) * (i + 1));

		if (mem == NULL)
		{
			return false;
		}

		currencies->currencies = mem;
		currencies->n_currencies = i + 1;

		currencies->currencies[i] = cur;
	}


	return true;
}

bool initCurrencies(const char * pageURL, const char * currencyFilePath)
{
	s_currencyData_impl.filePath = currencyFilePath;

	size_t webPageLen;
	char * webPage = loadWebPageToStr(pageURL, &webPageLen);

	// If loading page was unsuccessful
	if (webPage == NULL)
	{
		// Try to load data from file
		return s_readCurrencyFile(&currencyData);
	}
	else
	{
		bool success = s_parseCurWebpage(&currencyData, webPage, webPageLen);

		free(webPage);

		return success;
	}
}

void cleanupCurrencies()
{
	CurrenciesData_t tempCurData = { 0 };
	bool readResult = s_readCurrencyFile(&tempCurData);

	if ((readResult == true && memcmp(&tempCurData.date, &currencyData.date, sizeof currencyData.date) != 0) ||
		(readResult == false)
	)
	{
		destroyCurrenciesData(&tempCurData);

		// Write new currency info to file
		s_writeCurrencyFile(&currencyData);
	}

	destroyCurrenciesData(&currencyData);
}
