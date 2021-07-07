#pragma once
#ifndef GALSPANIC_H_
#define GALSPANIC_H_
#include "resource.h"
#include "framework.h"

class GP
{
private:
	POINTF pointer;
	POINTF preP;
	int **arr;
	std::vector<POINTF> preV;
	ULONG_PTR g_GdiToken;
	RECT rectview;
	HBITMAP pImage, cImage;
	BITMAP pBit, cBit;
public:
	BOOL gtime = true;
	BOOL SPACE = false;
	BOOL flag = false;
	GP() {}
	~GP();
	std::vector<POINTF> &getV() { return preV; }
	POINTF getpoint() { return pointer; }
	POINTF getpreP() { return preP; }
	int **getarr() { return arr; }
	void setpoint(POINTF p) { pointer = p; }
	void set();
	void make_space();
	void GDI_Init(); 
	void Double_buffer(HWND hWnd, HDC hdc, int x, int y, int width, int height);
	void set_image(Bitmap& myBitmap);
	void changebitpixel(Bitmap& myBitmap);
	void DrawPointer(HWND hWnd, HDC hdc);
	void movePointer();
	void make_Surface();
	void get_space();
	RECT getrect();
	void BFS();
};
#endif // !GALSPANIC_H_
