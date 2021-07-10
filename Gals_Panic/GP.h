#pragma once
#ifndef GALSPANIC_H_
#define GALSPANIC_H_
#include "resource.h"
#include "framework.h"


class GP
{
private:
	class Boss
	{
	public:
		FLOAT x, y;
		FLOAT velocity = 10;
		FLOAT vx = 10, vy = 10;
		int R = 10;
	};
	const int width = 800, height = 800;
	POINTF pointer;
	POINTF preP;
	int **arr;
	std::vector<POINTF> preV;
	ULONG_PTR g_GdiToken;
	RECT rectview;
	RECT maxrect = { width, height, 0, 0};
	HBITMAP pImage, cImage;
	BITMAP pBit, cBit;
	Bitmap *myBitmap, *myBitmap2;
public:
	Boss b;
	BOOL gtime = true;
	BOOL SPACE = false;
	BOOL flag = false;
	GP() {}
	~GP();
	std::vector<POINTF> &getV() { return preV; }
	POINTF getpoint() { return pointer; }
	POINTF getpreP() { return preP; }
	RECT getmaxrect() { return maxrect; }
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
	bool movePointerrecur(int n);
	void make_Surface();
	void get_space();
	void getrect();
	RECT getVrect();
	void change_maxrect();
	bool on_vector_line(FLOAT a, FLOAT b);
	void BFS(POINTF p);
	bool check_inner_point(POINTF p);

	void makeBOSS();
	void drawBOSS(HDC hdc);
	bool CollisionWall();
	void CollisionPointer();
	void change_vector(FLOAT vx, FLOAT vy);

	void update();
};


#endif // !GALSPANIC_H_
