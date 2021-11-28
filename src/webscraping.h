#ifndef WEBSCRAPING_H
#define WEBSCRAPING_H

#include <stdint.h>
#include <stdbool.h>

#define ECB_RATES_URL "https://www.ecb.europa.eu/stats/policy_and_exchange_rates/euro_reference_exchange_rates/html/index.en.html"

/**
 * @brief Load desired web page to character array
 * @param pageURL A valid null-terminated URL path to desired web page
 * @param lenInBytes Pointer to variable that would hold returned array size in bytes;
 * Reading this value upon failure of the function (return value of NULL) is undefined behaviour
 * @returns Null-terminated character array of loaded webpage, NULL, if operation failed
 */
char * loadWebPageToStr(const char * pageURL, size_t * lenInBytes);


#endif
