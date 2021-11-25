#include "resource.h"

LPCWSTR rsc_load(HINSTANCE hInst, UINT resourcename, LPCWSTR backup)
{
	LPCWSTR ptr = NULL;
	if (LoadStringW(hInst, resourcename, (LPWSTR)&ptr, 0) == 0)
	{
		return backup;
	}
	else
	{
		return ptr;
	}
}
void rsc_loadSelf(HINSTANCE hInst, wchar_t arr[], int maxSz, UINT resourcename, LPCWSTR backup)
{
	if (LoadStringW(hInst, resourcename, arr, maxSz) == 0)
	{
		wcscpy_s(arr, maxSz, backup);
	}
}