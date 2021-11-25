#include "ui_dpi.h"

#include <ShellScalingApi.h>

typedef BOOL (WINAPI * STDAC_W10_1703)(DPI_AWARENESS_CONTEXT dpiContext);
typedef HRESULT (WINAPI * GDFM_W8_1)(HMONITOR monitor, MONITOR_DPI_TYPE dpiType, UINT * dpiX, UINT * dpiY);

static STDAC_W10_1703 SetProcessDpiAwarenessContext_W10_1703 = NULL;
static GDFM_W8_1 GetDpiForMonitor_W8_1 = NULL;

static HMODULE user32dll = NULL, shcoredll = NULL;

void dpi_loadFunctions()
{
	shcoredll = LoadLibraryW(L"Shcore.dll");
	if (shcoredll == NULL)
	{
		return;
	}
	GetDpiForMonitor_W8_1 = (GDFM_W8_1)GetProcAddress(
		shcoredll,
		"GetDpiForMonitor"
	);

	// Try loading 'fancy' dpi-awareness functions
	user32dll = LoadLibraryW(L"User32.dll");
	if (user32dll == NULL)
	{
		return;
	}
	SetProcessDpiAwarenessContext_W10_1703 = (STDAC_W10_1703)GetProcAddress(
		user32dll,
		"SetProcessDpiAwarenessContext"
	);
}
void dpi_unloadFunctions()
{
	SetProcessDpiAwarenessContext_W10_1703 = NULL;
	GetDpiForMonitor_W8_1 = NULL;
	if (user32dll != NULL)
	{
		FreeLibrary(user32dll);
		user32dll = NULL;
	}
	if (shcoredll != NULL)
	{
		FreeLibrary(shcoredll);
		shcoredll = NULL;
	}
}

void dpi_setAware()
{
	if (SetProcessDpiAwarenessContext_W10_1703 != NULL)
	{
		SetProcessDpiAwarenessContext_W10_1703(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
	}
	else
	{
		SetProcessDPIAware();
	}
}

static int s_prevDx = 0, s_prevDy = 0;

static void s_dpi_memorise(int * restrict dx, int * restrict dy)
{
	if (*dx == 0 || *dy == 0)
	{
		*dx = s_prevDx;
		*dy = s_prevDy;
	}
	else
	{
		s_prevDx = *dx;
		s_prevDy = *dy;
	}
}
static void s_dpi_memoriseX(int * restrict dx)
{
	if (*dx == 0)
	{
		*dx = s_prevDx;
	}
	else
	{
		s_prevDx = *dx;
	}
}
static void s_dpi_memoriseY(int * restrict dy)
{
	if (*dy == 0)
	{
		*dy = s_prevDx;
	}
	else
	{
		s_prevDx = *dy;
	}
}

void dpi_setPrev(int dx, int dy)
{
	s_prevDx = dx;
	s_prevDy = dy;
}

bool dpi_get(HWND hwnd, int * restrict dx, int * restrict dy)
{
	if (GetDpiForMonitor_W8_1 != NULL)
	{
		UINT x, y;
		HRESULT hr = GetDpiForMonitor_W8_1(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), MDT_EFFECTIVE_DPI, &x, &y);
		if (SUCCEEDED(hr))
		{
			*dx = (int)x;
			*dy = (int)y;
			s_prevDx = *dx;
			s_prevDy = *dy;
			return true;
		}
		else
		{
			*dx = 96;
			*dy = 96;
			s_prevDx = *dx;
			s_prevDy = *dy;
			return false;
		}
	}
	else
	{
		HDC screendc = GetDC(NULL);
		*dx = GetDeviceCaps(screendc, LOGPIXELSX);
		*dy = GetDeviceCaps(screendc, LOGPIXELSY);
		s_prevDx = *dx;
		s_prevDy = *dy;
		ReleaseDC(NULL, screendc);
		return true;
	}
}

void dpi_adjustDip(int * restrict x, int * restrict y, int dx, int dy)
{
	s_dpi_memorise(&dx, &dy);

	*x = MulDiv(*x, dx, 96);
	*y = MulDiv(*y, dy, 96);
}
int dpi_adjustDipx(int x, int dx)
{
	s_dpi_memoriseX(&dx);
	return MulDiv(x, dx, 96);
}
int dpi_adjustDipy(int y, int dy)
{
	s_dpi_memoriseY(&dy);
	return MulDiv(y, dy, 96);
}
int dpi_adjustFontDip(int sizePt, int dy)
{
	s_dpi_memoriseY(&dy);
	return -MulDiv(sizePt, dy, 72);
}
void dpi_adjustRectDip(RECT * restrict rect, int dx, int dy)
{
	rect->left   = (LONG)dpi_adjustDipx((int)rect->left,   dx);
	rect->top    = (LONG)dpi_adjustDipy((int)rect->top,    dy);
	rect->right  = (LONG)dpi_adjustDipx((int)rect->right,  dx);
	rect->bottom = (LONG)dpi_adjustDipy((int)rect->bottom, dy);
}



void dpi_adjustDpi(int * restrict x, int * restrict y, int dx, int dy)
{
	s_dpi_memorise(&dx, &dy);

	*x = MulDiv(*x, 96, dx);
	*y = MulDiv(*y, 96, dy);
}
int dpi_adjustDpix(int x, int dx)
{
	s_dpi_memoriseX(&dx);
	return MulDiv(x, 96, dx);
}
int dpi_adjustDpiy(int y, int dy)
{
	s_dpi_memoriseX(&dy);
	return MulDiv(y, 96, dy);
}
int dpi_adjustFontDpi(int sizePix, int dy)
{
	s_dpi_memoriseY(&dy);
	return -MulDiv(sizePix, 72, dy);
}
void dpi_adjustRectDpi(RECT * restrict rect, int dx, int dy)
{
	rect->left   = (LONG)dpi_adjustDpix((int)rect->left,   dx);
	rect->top    = (LONG)dpi_adjustDpiy((int)rect->top,    dy);
	rect->right  = (LONG)dpi_adjustDpix((int)rect->right,  dx);
	rect->bottom = (LONG)dpi_adjustDpiy((int)rect->bottom, dy);
}


