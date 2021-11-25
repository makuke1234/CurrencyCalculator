#include "ui.h"
#include "ui_dpi.h"

#include "resource.h"

#include <CommCtrl.h>

static const wchar_t * s_errMsgs[UIErr_num_of_items] = {
	L"Unknown error occurred!",
	L"Error initialising window class!",
	L"Error creating window!",
	L"Invalid handle!"
};

void dispErr(UIErr_e err)
{
	dispErrWnd(err, NULL, NULL);
}
void dispErrTitle(UIErr_e err, LPCWSTR title)
{
	dispErrWnd(err, title, NULL);
}
void dispErrWnd(UIErr_e err, LPCWSTR title, HWND hwnd)
{
	if (err >= UIErr_num_of_items)
	{
		err = UIErr_unknown;
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
	.minSizeNeutral      = { 340, 280 },
	.elemSpacingN  = { 8, 8 },
	.elemSizeN     = { 80, 22 },
	
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
	rsc_loadSelf(wnd.hInst, wnd.className, MAX_RSCSTRING, IDS_APPCLASSNAME, L"CurrencyCalcClass");
	rsc_loadSelf(wnd.hInst, wnd.appName,   MAX_RSCSTRING, IDS_APPNAME,      L"CurrencyCalculator");
	
	rsc_loadSelf(wnd.hInst, wnd.iLabel1Str, MAX_RSCSTRING, IDS_INFOLABEL1, L"Rates' timeliness:");
	rsc_loadSelf(wnd.hInst, wnd.iLabel2Str, MAX_RSCSTRING, IDS_INFOLABEL2, L"ECB rate:");
	rsc_loadSelf(wnd.hInst, wnd.iLabel3Str, MAX_RSCSTRING, IDS_INFOLABEL3, L"Enter amount:");
	
	rsc_loadSelf(wnd.hInst, wnd.oLabelStr, MAX_RSCSTRING, IDS_OUTLABEL, L"Value:");
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

	wnd.hwnd = CreateWindowExW(
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
			dispErr(UIErr_handle);
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
	switch (msg)
	{
	case WM_PAINT:
		onPaint(hwnd);
		break;
	case WM_COMMAND:
		onCommand(wp);
		break;
	case WM_SIZING:
		onSizing(wp, (RECT *)lp);
		return TRUE;
	case WM_SIZE:
		onSize(wp, (int)LOWORD(lp), (int)HIWORD(wp));
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
		return onCreate(hwnd);
	default:
		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	return 0;
}

void onPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);




	EndPaint(hwnd, &ps);
}
void onCommand(WPARAM wp)
{
	switch (HIWORD(wp))
	{
	case CBN_SELCHANGE:
	{
		const int index = SendMessageW(wnd.hwnd, CB_GETCURSEL, 0, 0);
		if (index == CB_ERR)
		{
			return;
		}
		// Get control menu
		switch (LOWORD(wp))
		{
		case WndDropDown_from:
			dispErr(UIErr_unknown);
			break;
		case WndDropDown_to:
			break;
		}
	}
	}
}
void onSize(WPARAM wp, int newx, int newy)
{
	wnd.sizeRect.right = newx;
	wnd.sizeRect.bottom = newy;
	InvalidateRect(wnd.hwnd, NULL, FALSE);
}
void onSizing(WPARAM wp, RECT * wndRect)
{
	int minx = dpi_adjustDipx(wnd.minSizeNeutral.x, 0) + wnd.border.x;
	int miny = dpi_adjustDipx(wnd.minSizeNeutral.y, 0) + wnd.border.y;
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

LRESULT onCreate(HWND hwnd)
{
	wnd.hwnd = hwnd;
	dpi_get(hwnd, &wnd.dpi.x, &wnd.dpi.y);

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
		hwnd,
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
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		hwnd,
		(HMENU)WndDropDown_from,
		wnd.hInst,
		NULL
	);

	tr = wnd.curLabelN;
	dpi_adjustRectDip(&tr, 0, 0);
	wnd.handles[2] = CreateWindowExW(
		0,
		L"static",
		L"->",
		WS_CHILD | WS_VISIBLE,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		hwnd,
		(HMENU)WndDropDown_to,
		wnd.hInst,
		NULL
	);

	tr = wnd.curBox2N;
	dpi_adjustRectDip(&tr, 0, 0);
	wnd.handles[3] = CreateWindowExW(
		0,
		L"combobox",
		L"",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
		tr.left,
		tr.top,
		tr.right  - tr.left,
		tr.bottom - tr.top,
		hwnd,
		NULL,
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
		hwnd,
		NULL,
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
	SendMessageW(wnd.handles[1], CB_ADDSTRING, 0, (LPARAM)L"test1");
	SendMessageW(wnd.handles[1], CB_ADDSTRING, 0, (LPARAM)L"test2");
	SendMessageW(wnd.handles[3], CB_ADDSTRING, 0, (LPARAM)L"test1");
	SendMessageW(wnd.handles[3], CB_ADDSTRING, 0, (LPARAM)L"test2");

	return 0;
}
