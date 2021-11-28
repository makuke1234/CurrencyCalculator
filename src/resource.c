#include "resource.h"

LPCWSTR rsc_load(UINT resourcename, LPCWSTR backup)
{
	LPCWSTR ptr = NULL;
	if (LoadStringW(NULL, resourcename, (LPWSTR)&ptr, 0) == 0)
	{
		return backup;
	}
	else
	{
		return ptr;
	}
}
void rsc_loadSelf(wchar_t arr[], int maxSz, UINT resourcename, LPCWSTR backup)
{
	if (LoadStringW(NULL, resourcename, arr, maxSz) == 0)
	{
		wcscpy_s(arr, (rsize_t)maxSz, backup);
	}
}

const void * rsc_loadFile(UINT resourcename, DWORD * restrict pSize)
{
	HRSRC myRes = FindResourceW(NULL, MAKEINTRESOURCEW(resourcename), RT_RCDATA);
	(*pSize) = SizeofResource(NULL, myRes);

	HGLOBAL myResData = LoadResource(NULL, myRes);

	return LockResource(myResData);
}
