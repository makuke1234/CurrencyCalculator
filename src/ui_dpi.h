#ifndef UI_DPI_H
#define UI_DPI_H

#include "ui.h"


void dpi_loadFunctions();
void dpi_unloadFunctions();
void dpi_setAware();
void dpi_setPrev(int dx, int dy);
bool dpi_get(HWND hwnd, int * restrict dx, int * restrict dy);


void dpi_adjustDip(int * restrict x, int * restrict y, int dx, int dy);
int dpi_adjustDipx(int x, int dx);
int dpi_adjustDipy(int y, int dy);
int dpi_adjustFontDip(int sizePt, int dy);
void dpi_adjustRectDip(RECT * restrict rect, int dx, int dy);


void dpi_adjustDpi(int * restrict x, int * restrict y, int dx, int dy);
int dpi_adjustDpix(int x, int dx);
int dpi_adjustDpiy(int y, int dy);
int dpi_adjustFontDpi(int sizePix, int dy);
void dpi_adjustRectDpi(RECT * restrict rect, int dx, int dy);

#endif
