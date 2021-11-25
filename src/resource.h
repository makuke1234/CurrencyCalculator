#ifndef RESOURCE_H
#define RESOURCE_H

#include "ui.h"

#define IDS_APPCLASSNAME	101
#define IDS_APPNAME			102

#define IDS_INFOLABEL1	103
#define IDS_INFOLABEL2	104
#define IDS_INFOLABEL3	105
#define IDS_OUTLABEL	106

LPCWSTR rsc_load(HINSTANCE hInst, UINT resourcename, LPCWSTR backup);
void rsc_loadSelf(HINSTANCE hInst, wchar_t arr[], int maxSz, UINT resourcename, LPCWSTR backup);

#endif