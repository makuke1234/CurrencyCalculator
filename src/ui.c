#include "ui.h"
#include "ui_dpi.h"

#include "resource.h"

#include <CommCtrl.h>
#include <stdio.h>

static const wchar_t * s_errMsgs[CCErr_num_of_items] = {
	L"Unknown error occurred!",
	L"Error initialising window class!",
	L"Error creating window!",
	L"Invalid handle!",
	L"Error loading currencies' exchange rate data!"
};

void dispErr(CCErr_e err)
{
	dispErrWnd(err, NULL, NULL);
}
void dispErrTitle(CCErr_e err, LPCWSTR title)
{
	dispErrWnd(err, title, NULL);
}
void dispErrWnd(CCErr_e err, LPCWSTR title, HWND hwnd)
{
	if (err >= CCErr_num_of_items)
	{
		err = CCErr_unknown;
	}
	if (title == NULL)
	{
		title = wnd.appName;
	}
	MessageBoxW(hwnd, s_errMsgs[err], title, MB_ICONERROR | MB_OK);
}

Window_t wnd = {
	.dpi       = { 96, 96 },
	.sizeRect  = { 0, 0, 340, 280 },
	.minSizeN     = { 340, 280 },
	.elemSpacingN = { 8, 8 },
	.elemSizeN    = { 80, 22 },
	
	.infoLabel1N = { 0, 0, 130, 22 },
	.infoLabel2N = { 0, 0, 130, 22 },
	.infoLabel3N = { 0, 0, 130, 22 },

	.curBox1N  = { 0, 0, 50, 22 },
	.curLabelN = { 0, 0, 30, 22 },
	.curBox2N  = { 0, 0, 50, 22 },

	.curTextN  = { 0, 0, 80, 22 },
	.outLabelN = { 0, 0, 150, 22 },

	.fontHeight = 10
};

void initResources()
{
	rsc_loadSelf(wnd.className, MAX_RSCSTRING, IDS_APPCLASSNAME, L"CurrencyCalcClass");
	rsc_loadSelf(wnd.appName,   MAX_RSCSTRING, IDS_APPNAME,      L"CurrencyCalculator");
	
	rsc_loadSelf(wnd.iLabel1Str, MAX_RSCSTRING, IDS_INFOLABEL1, L"Rates' timeliness:");
	rsc_loadSelf(wnd.iLabel2Str, MAX_RSCSTRING, IDS_INFOLABEL2, L"ECB rate:");
	rsc_loadSelf(wnd.iLabel3Str, MAX_RSCSTRING, IDS_INFOLABEL3, L"Enter amount:");
	
	rsc_loadSelf(wnd.oLabelStr, MAX_RSCSTRING, IDS_OUTLABEL, L"Value:");
}

static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

bool initWndClass(HINSTANCE hInst, PWSTR lpCmdArgs, int nCmdShow)
{
	dpi_loadFunctions();
	calcWndElems();

	wnd.hInst     = hInst;
	wnd.lpCmdArgs = lpCmdArgs;
	wnd.nCmdShow  = nCmdShow;
	
	initResources();

	WNDCLASSEXW wc = { 0 };
	wc.cbSize        = sizeof wc;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon         = LoadIconW(hInst, IDI_APPLICATION);
	wc.hIconSm       = wc.hIcon;
	wc.hInstance     = hInst;
	wc.lpfnWndProc   = &wndProc;
	wc.lpszClassName = wnd.className;

	return RegisterClassExW(&wc) != 0;
}
bool initWindow(RECT sizeRect)
{
	static const RECT nullRect = { 0 };
	if (memcmp(&sizeRect, &nullRect, sizeof(RECT)) != 0)
	{
		wnd.sizeRect = sizeRect;
	}

	CreateWindowExW(
		0,
		wnd.className,
		wnd.appName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wnd.sizeRect.right,
		wnd.sizeRect.bottom,
		NULL,
		NULL,
		wnd.hInst,
		NULL
	);

	if (wnd.hwnd == NULL)
	{
		return false;
	}

	calcWndBorder();

	dpi_adjustRectDip(&wnd.sizeRect, 0, 0);

	wnd.sizeRect.right  += wnd.border.x;
	wnd.sizeRect.bottom += wnd.border.y;

	SetWindowPos(
		wnd.hwnd,
		NULL,
		0,
		0,
		wnd.sizeRect.right,
		wnd.sizeRect.bottom,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER
	);

	ShowWindow(wnd.hwnd, wnd.nCmdShow);
	UpdateWindow(wnd.hwnd);

	return true;
}
int msgLoop()
{
	BOOL ret;
	MSG msg;
	while ((ret = GetMessageW(&msg, NULL, 0, 0)))
	{
		if (ret == -1)
		{
			dispErr(CCErr_handle);
			return -1;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return (int)msg.wParam;
}
void cleanupWindow()
{
	dpi_unloadFunctions();
}

static inline void calcElemSameLine(RECT * restrict elem, const RECT * restrict prevElem)
{
	static const RECT nullRect = { 0 };
	if (prevElem == NULL)
	{
		prevElem = &nullRect;
		elem->top += wnd.elemSpacingN.y;
	}

	elem->left += prevElem->right + wnd.elemSpacingN.x;
	elem->top  += prevElem->top;

	elem->right  += elem->left;
	elem->bottom += elem->top;
}
static inline void calcElemNewLine(RECT * restrict elem, const RECT * restrict prevElem)
{
	static const RECT nullRect = { 0 };
	if (prevElem == NULL)
	{
		prevElem = &nullRect;
		elem->left += wnd.elemSpacingN.x;
	}

	elem->left += prevElem->left;
	elem->top  += prevElem->bottom + wnd.elemSpacingN.y;

	elem->right  += elem->left;
	elem->bottom += elem->top;
}

void calcWndElems()
{
	calcElemSameLine(&wnd.infoLabel1N, NULL);
	calcElemNewLine(&wnd.infoLabel2N, &wnd.infoLabel1N);

	calcElemNewLine(&wnd.curBox1N, &wnd.infoLabel2N);
	calcElemSameLine(&wnd.curLabelN, &wnd.curBox1N);
	calcElemSameLine(&wnd.curBox2N, &wnd.curLabelN);

	calcElemNewLine(&wnd.infoLabel3N, &wnd.curBox1N);
	calcElemNewLine(&wnd.curTextN, &wnd.infoLabel3N);
	calcElemSameLine(&wnd.outLabelN, &wnd.curTextN);
}
void calcWndBorder()
{
	RECT r;
	if (!GetClientRect(wnd.hwnd, &r))
	{
		return;
	}

	wnd.border.x = wnd.sizeRect.right  - r.right;
	wnd.border.y = wnd.sizeRect.bottom - r.bottom;
}

static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (wnd.hwnd == NULL && msg != WM_CREATE)
	{
		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	switch (msg)
	{
	case WM_PAINT:
		onPaint(hwnd);
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_COMMAND:
		onCommand(wp, lp);
		break;
	case WM_SIZING:
		onSizing(wp, (RECT *)lp);
		return TRUE;
	case WM_SIZE:
		onSize((int)LOWORD(lp), (int)HIWORD(wp));
		break;
	case WM_DPICHANGED:
		onDpiChange((int)LOWORD(wp), (int)HIWORD(wp), (RECT *)lp);
		break;
	case WM_CLOSE:
		for (int i = 0; i < MAX_HANDLES; ++i)
		{
			DestroyWindow(wnd.handles[i]);
		}
		DeleteObject(wnd.font);
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		wnd.hwnd = hwnd;
		return onCreate();
	default:
		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	return 0;
}

#define MAX_PAINT_STR 256

void onPaint(HWND hwnd)
{
	RECT r;
	GetClientRect(hwnd, &r);
	int win_width  = r.right  - r.left;
	int win_height = r.bottom - r.top;

	PAINTSTRUCT ps;
	HDC realhdc = BeginPaint(hwnd, &ps);
	HDC hdc = CreateCompatibleDC(realhdc);
	HBITMAP hMemBm = CreateCompatibleBitmap(realhdc, win_width, win_height);
	SelectObject(hdc, hMemBm);

	// Fill in background color
	FillRect(hdc, &r, (HBRUSH)COLOR_WINDOW);

	// Start painting

	// Select font
	SelectObject(hdc, wnd.font);
	// Set transparent background mode
	SetBkMode(hdc, TRANSPARENT);



	wchar_t tempStr[MAX_PAINT_STR];
	int len = swprintf_s(tempStr, MAX_PAINT_STR, L"%s %hu.%hu.%hu", wnd.iLabel1Str, currencyData.date.day, currencyData.date.mon, currencyData.date.year);
	RECT tempRect = wnd.infoLabel1N;
	dpi_adjustRectDip(&tempRect, 0, 0);
	DrawTextW(hdc, tempStr, len, &tempRect, DT_LEFT | DT_NOCLIP);



	double rate = currencyData.currencies[wnd.selectCur2].etalonValue /
	              currencyData.currencies[wnd.selectCur1].etalonValue;
	len = swprintf_s(
		tempStr,
		MAX_PAINT_STR,
		L"%s %.4f",
		wnd.iLabel2Str,
		rate
	);
	tempRect = wnd.infoLabel2N;
	dpi_adjustRectDip(&tempRect, 0, 0);
	DrawTextW(hdc, tempStr, len, &tempRect, DT_LEFT | DT_NOCLIP);


	if (wnd.hasValidInputValue)
	{
		len = swprintf_s(
			tempStr,
			MAX_PAINT_STR,
			L"%s %.2f %s",
			wnd.oLabelStr,
			rate * wnd.textInputValue,
			currencyData.currencies[wnd.selectCur2].wName
		);
	}
	else
	{
		len = swprintf_s(tempStr, MAX_PAINT_STR, L"%s NaN", wnd.oLabelStr);
	}
	tempRect = wnd.outLabelN;
	dpi_adjustRectDip(&tempRect, 0, 0);
	DrawTextW(hdc, tempStr, len, &tempRect, DT_LEFT | DT_NOCLIP);

	// End painting
	BitBlt(realhdc, 0, 0, win_width, win_height, hdc, 0, 0, SRCCOPY);
	DeleteObject(hMemBm);
	DeleteDC(hdc);
	EndPaint(hwnd, &ps);
}
static inline void updateRateDisp()
{
	// Redraw
	RECT tempRect = wnd.infoLabel2N;
	dpi_adjustRectDip(&tempRect, 0, 0);
	RECT client;
	GetClientRect(wnd.hwnd, &client);
	tempRect.right = client.right;
	InvalidateRect(wnd.hwnd, &tempRect, TRUE);
}
static inline void updateConvDisp()
{
	RECT tempRect = wnd.outLabelN;
	dpi_adjustRectDip(&tempRect, 0, 0);
	RECT client;
	GetClientRect(wnd.hwnd, &client);
	tempRect.right = client.right;
	InvalidateRect(wnd.hwnd, &tempRect, TRUE);
}

void onCommand(WPARAM wp, LPARAM lp)
{
	switch (HIWORD(wp))
	{
	case CBN_SELCHANGE:
	{
		const int index = (int)SendMessageW((HWND)lp, CB_GETCURSEL, 0, 0);
		if (index == CB_ERR)
		{
			return;
		}
		// Get control menu
		switch (LOWORD(wp))
		{
		case WndMenuDropDown_from:
			wnd.selectCur1 = index;
			break;
		case WndMenuDropDown_to:
			wnd.selectCur2 = index;
			break;
		}
		updateRateDisp();
		updateConvDisp();
		break;
	}
	case EN_CHANGE:
		if (LOWORD(wp) == WndMenuTextInput)
		{
			// try to convert text to number
			int lineLen = (int)SendMessageW(wnd.handles[4], EM_LINELENGTH, 0, 0) + 1;
			wchar_t line[lineLen];
			GetWindowTextW(wnd.handles[4], line, lineLen);

			wchar_t * nextp;
			double val = wcstod(line, &nextp);

			if (nextp == line)
			{
				wnd.hasValidInputValue = false;
			}
			else
			{
				wnd.textInputValue = val;
				wnd.hasValidInputValue = true;
			}
			updateConvDisp();
		}
		break;
	}
}
void onSize(int newx, int newy)
{
	wnd.sizeRect.right = newx;
	wnd.sizeRect.bottom = newy;
	InvalidateRect(wnd.hwnd, NULL, FALSE);
}
void onSizing(WPARAM wp, RECT * wndRect)
{
	int minx = dpi_adjustDipx(wnd.minSizeN.x, 0) + wnd.border.x;
	int miny = dpi_adjustDipx(wnd.minSizeN.y, 0) + wnd.border.y;
	// X-coordinate
	switch (wp)
	{
	// From right
	case WMSZ_BOTTOMRIGHT:
	case WMSZ_RIGHT:
	case WMSZ_TOPRIGHT:
		if ((wndRect->right - wndRect->left) < minx)
		{
			wndRect->right = wndRect->left + minx;
		}
		break;
	// From left
	case WMSZ_BOTTOMLEFT:
	case WMSZ_LEFT:
	case WMSZ_TOPLEFT:
		if ((wndRect->right - wndRect->left) < minx)
		{
			wndRect->left = wndRect->right - minx;
		}
		break;
	}

	// Y-coordinate
	switch (wp)
	{
	// From bottom
	case WMSZ_BOTTOM:
	case WMSZ_BOTTOMLEFT:
	case WMSZ_BOTTOMRIGHT:
		if ((wndRect->bottom - wndRect->top) < miny)
		{
			wndRect->bottom = wndRect->top + miny;
		}
		break;
	// From top
	case WMSZ_TOP:
	case WMSZ_TOPLEFT:
	case WMSZ_TOPRIGHT:
		if ((wndRect->bottom - wndRect->top) < miny)
		{
			wndRect->top = wndRect->bottom - miny;
		}
		break;
	}
}
void onDpiChange(int newdx, int newdy, RECT * wndRect)
{
	wnd.dpi = (DPI_t){
		.x = newdx,
		.y = newdy
	};
	dpi_setPrev(newdx, newdy);

	wnd.sizeRect.right  = wndRect->right  - wndRect->left;
	wnd.sizeRect.bottom = wndRect->bottom - wndRect->top;
	MoveWindow(
		wnd.hwnd,
		wndRect->left,
		wndRect->top,
		wnd.sizeRect.right,
		wnd.sizeRect.bottom,
		TRUE
	);
	wnd.sizeRect = *wndRect;

	calcWndBorder();
}

LRESULT onCreate()
{
	dpi_get(wnd.hwnd, &wnd.dpi.x, &wnd.dpi.y);

	// Create buttons
	RECT tr;
	
	// First info label
	tr = wnd.infoLabel3N;
	dpi_adjustRectDip(&tr, 0, 0);
	wnd.handles[0] = CreateWindowExW(
		0,
		L"static",
		wnd.iLabel3Str,
		WS_CHILD | WS_VISIBLE,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		wnd.hwnd,
		NULL,
		wnd.hInst,
		NULL
	);

	// currency listbox 1
	tr = wnd.curBox1N;
	dpi_adjustRectDip(&tr, 0, 0);
	wnd.handles[1] = CreateWindowExW(
		0,
		L"combobox",
		L"",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		wnd.hwnd,
		(HMENU)WndMenuDropDown_from,
		wnd.hInst,
		NULL
	);

	tr = wnd.curLabelN;
	dpi_adjustRectDip(&tr, 0, 0);
	wnd.handles[2] = CreateWindowExW(
		0,
		L"static",
		L"➤",
		WS_CHILD | WS_VISIBLE | DT_CENTER,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		wnd.hwnd,
		NULL,
		wnd.hInst,
		NULL
	);

	tr = wnd.curBox2N;
	dpi_adjustRectDip(&tr, 0, 0);
	wnd.handles[3] = CreateWindowExW(
		0,
		L"combobox",
		L"",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		wnd.hwnd,
		(HMENU)WndMenuDropDown_to,
		wnd.hInst,
		NULL
	);

	tr = wnd.curTextN;
	dpi_adjustRectDip(&tr, 0, 0);
	wnd.handles[4] = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"edit",
		L"",
		WS_CHILD | WS_VISIBLE,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		wnd.hwnd,
		(HMENU)WndMenuTextInput,
		wnd.hInst,
		NULL
	);

	// Check-up
	for (int i = 0; i < MAX_HANDLES; ++i)
	{
		if (wnd.handles[i] == NULL)
		{
			return -1;
		}
	}

	// Create font
	wnd.font = CreateFontW(
		dpi_adjustFontDip(wnd.fontHeight, 0),
		0,
		0,
		0,
		FW_MEDIUM,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		L"Segoe UI"
	);

	if (wnd.font == NULL)
	{
		return -1;
	}

	// Set font
	for (int i = 0; i < MAX_HANDLES; ++i)
	{
		SendMessageW(wnd.handles[i], WM_SETFONT, (WPARAM)wnd.font, TRUE);
	}

	// Set test strings
	for (size_t i = 0; i < currencyData.n_currencies; ++i)
	{
		LPCWSTR currencyName = currencyData.currencies[i].wName;
		SendMessageW(wnd.handles[1], CB_ADDSTRING, 0, (LPARAM)currencyName);
		SendMessageW(wnd.handles[3], CB_ADDSTRING, 0, (LPARAM)currencyName);
	}

	// Set index to 0
	SendMessageW(wnd.handles[1], CB_SETCURSEL, 0, 0);
	SendMessageW(wnd.handles[3], CB_SETCURSEL, 0, 0);

	return 0;
}
