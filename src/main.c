#include "ui.h"
#include "webscraping.h"
#include "currencies.h"

#include <stdio.h>

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE __attribute__((unused)) hPrev, PWSTR lpCmdArgs, int nCmdShow)
{
	if (initCurrencies(ECB_RATES_URL, CURRENCY_FILE) == false)
	{
		dispErr(CCErr_currencies);
		return 1;
	}

	if (initWndClass(hInst, lpCmdArgs, nCmdShow) == false)
	{
		dispErr(CCErr_wndClass);
		return 1;
	}

	if (initWindow((RECT){ 0 }) == false)
	{
		dispErr(CCErr_window);
		return 1;
	}

	int ret = msgLoop();
	cleanupWindow();

	cleanupCurrencies();

	return ret;
}
