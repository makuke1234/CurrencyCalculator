#ifndef UI_H
#define UI_H

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define NOMINMAX
#include <Windows.h>

#include <stdbool.h>

#include "currencies.h"

typedef enum UIErr
{
	UIErr_unknown,
	UIErr_wndClass,
	UIErr_window,
	UIErr_handle,

	UIErr_num_of_items
} UIErr_e;

void dispErr(UIErr_e err);
void dispErrTitle(UIErr_e err, LPCWSTR title);
void dispErrWnd(UIErr_e err, LPCWSTR title, HWND hwnd);

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
	POINT minSizeNeutral;
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

	CurrenciesData_t data;

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
void onPaint(HWND hwnd);
void onCommand(WPARAM wp);
void onSize(WPARAM wp, int newx, int newy);
void onSizing(WPARAM wp, RECT * wndRect);
void onDpiChange(int newdx, int newdy, RECT * wndRect);
LRESULT onCreate(HWND hwnd);

#endif
