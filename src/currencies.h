#ifndef CURRENCIES_H
#define CURRENCIES_H

#include <stdint.h>
#include <stdbool.h>

#define CURRENCY_FILE "currencies.txt"

typedef struct Currency
{
	wchar_t * wName;
	double etalonValue;

} Currency_t;

bool makeCurrency_str(Currency_t * restrict currency, const char * name, const char * value);
bool makeCurrency_str_s(Currency_t * restrict currency, const char * name, size_t maxName, const char * value);
bool makeCurrency_double(Currency_t * restrict currency, const char * name, double value);
void destroyCurrency(Currency_t * restrict currency);

typedef struct CurrenciesData
{
	Currency_t * currencies;
	uint32_t n_currencies;

	struct
	{
		uint16_t day, mon, year;

	} date;

} CurrenciesData_t;

void destroyCurrenciesData(CurrenciesData_t * restrict currencies);

extern CurrenciesData_t currencyData;

/**
 * @brief Try loading currencies from web, if fails, load from file
 * @param pageURL A null-terminated URL to currencies' exchange info webpage
 * @param currencyFilePath A null-terminated file path where the currency data would be stored
 * @returns true on success
 */
bool initCurrencies(const char * pageURL, const char * currencyFilePath);
/**
 * @brief Try to save currency info to the file, if anything has changed
 */
void cleanupCurrencies();

#endif
