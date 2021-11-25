#include "ui.h"
#include "webscraping.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR lpCmdArgs, int nCmdShow)
{
	if (initWndClass(hInst, lpCmdArgs, nCmdShow) == false)
	{
		dispErr(UIErr_wndClass);
		return 1;
	}

	if (initWindow((RECT){ 0 }) == false)
	{
		dispErr(UIErr_window);
		return 1;
	}

	int ret = msgLoop();
	cleanupWindow();

	return ret;
}
