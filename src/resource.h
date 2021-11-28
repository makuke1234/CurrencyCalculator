#ifndef RESOURCE_H
#define RESOURCE_H

#include "winapi.h"

#define IDS_APPCLASSNAME	101
#define IDS_APPNAME			102

#define IDS_INFOLABEL1	103
#define IDS_INFOLABEL2	104
#define IDS_INFOLABEL3	105
#define IDS_OUTLABEL	106

#define IDF_CACERT_PEM	107

LPCWSTR rsc_load(UINT resourcename, LPCWSTR backup);
void rsc_loadSelf(wchar_t arr[], int maxSz, UINT resourcename, LPCWSTR backup);

const void * rsc_loadFile(UINT resourcename, DWORD * restrict pSize);

#endif