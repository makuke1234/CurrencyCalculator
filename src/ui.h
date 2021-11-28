#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "winapi.h"
#include "currencies.h"

typedef enum CCErr
{
	CCErr_unknown,
	CCErr_wndClass,
	CCErr_window,
	CCErr_handle,
	CCErr_currencies,

	CCErr_num_of_items
} CCErr_e;

void dispErr(CCErr_e err);
void dispErrTitle(CCErr_e err, LPCWSTR title);
void dispErrWnd(CCErr_e err, LPCWSTR title, HWND hwnd);

typedef struct DPI
{
	int x, y;
} DPI_t;

#define MAX_HANDLES 5
#define MAX_RSCSTRING 32

typedef wchar_t RscString_t[MAX_RSCSTRING];

enum WndDropDown
{
	WndDropDown_from = 1,
	WndDropDown_to
};

typedef struct Window
{
	HINSTANCE hInst;
	PWSTR lpCmdArgs;
	int nCmdShow;

	RscString_t className, appName;

	DPI_t dpi;

	RECT sizeRect;
	POINT minSizeN;
	POINT border;

	POINT elemSpacingN, elemSizeN;

	RECT infoLabel1N, infoLabel2N, infoLabel3N;
	RECT curBox1N, curLabelN, curBox2N, curTextN;
	RECT outLabelN;

	RscString_t iLabel1Str, iLabel2Str, iLabel3Str, oLabelStr;

	HWND hwnd;

	HWND handles[MAX_HANDLES];

	int fontHeight;
	HFONT font;

} Window_t;

extern Window_t wnd;

void initResources();
bool initWndClass(HINSTANCE hInst, PWSTR lpCmdArgs, int nCmdShow);
bool initWindow(RECT sizeRect);
int msgLoop();
void cleanupWindow();
void calcWndElems();
void calcWndBorder();

// wndProc wrapper functions
void onPaint();
void onCommand(WPARAM wp);
void onSize(int newx, int newy);
void onSizing(WPARAM wp, RECT * wndRect);
void onDpiChange(int newdx, int newdy, RECT * wndRect);
LRESULT onCreate();

#endif
