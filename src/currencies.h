#ifndef CURRENCIES_H
#define CURRENCIES_H

#include <stdint.h>

typedef struct Currency
{
	char * name;
	double etalonValue;
} Currency_t;

typedef struct CurrenciesData
{
	Currency_t * currencies;
	uint32_t n_currencies;
} CurrenciesData_t;

#endif
