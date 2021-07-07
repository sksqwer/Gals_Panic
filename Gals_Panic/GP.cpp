#include "GP.h"
#include "framework.h"

bool operator!=(const POINTF a,const POINTF b);

GP::~GP()
{
	GdiplusShutdown(g_GdiToken);
}

void GP::set()
{
	rectview.right = 1920;
	rectview.left = 0;
	rectview.bottom = 1200;
	rectview.top = 0;
	arr = new int*[rectview.right];
	for (int i = 0; i < rectview.right; i++)
		arr[i] = new int[rectview.bottom];

	for (int i = 0; i < rectview.right; i++)
		for (int j = 0; j < rectview.bottom; j++)
			arr[i][j] = 0;
	pImage = (HBITMAP)LoadImage(NULL, TEXT("images/pointer.bmp"),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(pImage, sizeof(BITMAP), &pBit);

	cImage = (HBITMAP)LoadImage(NULL, TEXT("images/cover.bmp"),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(pImage, sizeof(BITMAP), &cBit);

	make_space();
}


void GP::make_space()
{

	int x = rand() % (1920 - 500)+500;
	int y = rand() % (1200 - 500)+500;
	

	for (int i = x; i < x + 100; i++)
	{
		for (int j = y; j < y + 150; j++)
			arr[i][j] = 1;
	}
	pointer.x = x;
	pointer.y = y;
	make_Surface();
}

void GP::GDI_Init()
{
	GdiplusStartupInput gpsi;
	GdiplusStartup(&g_GdiToken, &gpsi, NULL);
}

void GP::Double_buffer(HWND hWnd, HDC hdc, int x, int y, int width, int height)
{
	HBITMAP hImage, hImage2;
	hImage = CreateCompatibleBitmap(hdc,1920, 1200);
	hImage2 = CreateCompatibleBitmap(hdc, 1920, 1200);
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP old, old2;
	int bx = width, by = height;


	old = (HBITMAP)SelectObject(memDC, hImage);
	HDC memDC2 = CreateCompatibleDC(memDC);
	old2 = (HBITMAP)SelectObject(memDC2, hImage2);
	Gdiplus::Graphics graphics(memDC2);


	Bitmap myBitmap((WCHAR *)L"images/random.bmp");
	set_image(myBitmap);
	graphics.DrawImage(&myBitmap, 0, 0);

	BitBlt(memDC, x, y, bx, by, memDC2, x, y, SRCCOPY);
	SelectObject(memDC2, old2);
	DeleteDC(memDC2);

	memDC2 = CreateCompatibleDC(memDC);
	old2 = (HBITMAP)SelectObject(memDC2, hImage2);
	Gdiplus::Graphics graphics2(memDC2);
	Bitmap myBitmap2((WCHAR *)L"images/cover.bmp");
	changebitpixel(myBitmap2);
	graphics2.DrawImage(&myBitmap2, 0, 0);

	BitBlt(memDC, x, y, bx, by, memDC2, x, y, SRCCOPY);
	SelectObject(memDC2, old2);
	DeleteDC(memDC2);

	BitBlt(hdc, x, y, bx, by, memDC, x, y, SRCCOPY);

	SelectObject(memDC, old);
	DeleteDC(memDC);

}

void GP::set_image(Bitmap &myBitmap)
{
	Gdiplus::Graphics mgraphics(&myBitmap);

	mgraphics.DrawImage(&myBitmap, 0, 0, 1920, 1200);

}

void GP::changebitpixel(Bitmap &myBitmap)
{
	int bx = 1920, by = 1200;

	Gdiplus::Graphics mgraphics(&myBitmap);

	for (UINT row = 0; row < bx; row++)
	{
		for (UINT col = 0; col < by; col++)
		{
			if(arr[row][col] == 1)
				myBitmap.SetPixel(row, col, Color(0, 0, 0, 0));
			else if(arr[row][col] == 3)
				myBitmap.SetPixel(row, col, Color(255, 100, 100, 100));
		}
	}
}

void GP::DrawPointer(HWND hWnd, HDC hdc)
{

	HDC hMemDC;
	HBITMAP hOldBitmap;
	int bx, by;

	hMemDC = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, pImage);
	bx = pBit.bmWidth;
	by = pBit.bmHeight;


	INT x = preP.x, y = preP.y;
	Double_buffer(hWnd, hdc, x-10, y-10, bx * 2, by * 2);


	x = pointer.x, y = pointer.y;
	TransparentBlt(hdc, x-5, y-5 , bx, by, hMemDC, 0, 0, bx, by, RGB(255, 0, 255));

	SelectObject(hMemDC, hOldBitmap);

	DeleteDC(hMemDC);


}

void GP::movePointer()
{
	gtime = false;
	DWORD newTime = GetTickCount();
	static DWORD oldTime = newTime;
	if (newTime - oldTime < 10) return;
	oldTime = newTime;



	preP = pointer;


	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		pointer.x += 1;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		pointer.x -= 1;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		pointer.y -= 1;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		pointer.y += 1;
	}

	if (arr[int(pointer.x)][int(pointer.y)] != 2 || SPACE)
	{
		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && (pointer != preP)) // 스페이스
		{
			if (arr[int(pointer.x)][int(pointer.y)] != 0 && preV.size() == 0)
			{
				pointer = preP;
				return;
			}
			SPACE = true;

			if (arr[int(pointer.x)][int(pointer.y)] == 2)
			{
				get_space();
				make_Surface();
				flag = true;
				SPACE = false;
				return;
			}




			if(arr[int(pointer.x)][int(pointer.y)] != 0)
				pointer = preP;
			else
			{
				if (preV.size() == 0)
				{
					arr[int(preP.x)][int(preP.y)] = 3;
					preV.push_back(preP);
				}
				arr[int(pointer.x)][int(pointer.y)] = 3;
				preV.push_back(pointer);
			}
		}
		else
		{
			if (preV.size() > 0)
			{
				arr[int(pointer.x)][int(pointer.y)] = 0;
				pointer = preV.back();
				preV.erase(preV.begin() + preV.size() - 1, preV.end());
			}
			else
			{
				pointer = preP;
				SPACE = false;
			}

			preP = pointer;
		}

	}
	else if(arr[int(pointer.x)][int(pointer.y)] != 2)
	{
		pointer = preP;
	}

}

void GP::make_Surface()
{
	for (int i = 0; i < 1918; i++)
	{
		for (int j = 0; j < 1058; j++)
		{
			if (arr[i][j] == 2)
				arr[i][j] = 1;
		}
	}
	bool flag = true;
	for (int i = 0; i < 1918; i++)
	{
		for (int j = 1; j < 1058; j++)
		{
			if (flag)
			{
				if (arr[i][j] == 1)
				{
					arr[i][j] = 2;
					flag = false;
				}
			}
			else
			{
				if (arr[i][j] != 1)
				{
					arr[i][j - 1] = 2;
					flag = true;
				}
			}
		}
	}
	flag = true;
	for (int i = 0; i < 1058; i++)
	{
		for (int j = 1; j < 1918; j++)
		{
			if (flag)
			{
				if (arr[j][i] == 1)
				{
					arr[j][i] = 2;
					flag = false;
				}
			}
			else
			{
				if (arr[j][i] != 1)
				{
					arr[j - 1][i] = 2;
					flag = true;
				}
			}
		}
	}

}

void GP::get_space()
{
	const int dir[8][2] = { {1,1},{1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1} };

	POINTF stand;

	for (int i = 0; i < 8; i++)
	{
		if ((arr[int(preV[0].x) + dir[i][0]][int(preV[0].y) + dir[i][1]] == 1 &&
			arr[int(preV[0].x) + dir[i][0]][int(preV[0].y) + dir[i][1]] == 2 &&
			arr[int(preV[0].x) + dir[i][0]][int(preV[0].y) + dir[i][1]] == 3))
		{
			stand.x = preV[0].x + dir[i][0];
			stand.y = preV[0].y + dir[i][1];
			break;
		}
	}








	for (int i = 0; i < rectview.right; i++)
	{
		for (int j = 0; j < rectview.bottom; j++)
		{
			if (arr[i][j] == 3 || arr[i][j] == 2)
				arr[i][j] = 1;
		}
	}


	//
	//for (int i = 0; i < preV.size(); i++)
	//{
	//	int j = preV[i].x;
	//	for (int k = preV[i].x + 1; k < rectview.right; k++)
	//	{
	//		if (arr[k][int(preV[i].y)] == 1)
	//		{
	//			j = k;
	//			break;
	//		}
	//	}

	//	for (int k = preV[i].x + 1; k < j; k++)
	//	{
	//		arr[k][int(preV[i].y)] = 1;
	//	}

	//	j = preV[i].x;
	//	for (int k = preV[i].x - 1; k > 0; k--)
	//	{
	//		if (arr[k][int(preV[i].y)] == 1)
	//		{
	//			j = k;
	//			break;
	//		}
	//	}

	//	for (int k = preV[i].x - 1; k > j; k--)
	//	{
	//		arr[k][int(preV[i].y)] = 1;
	//	}
	//}



}

RECT GP::getrect()
{
	LONG MAXX = LONG(preV[0].x), MINX = LONG(preV[0].x), MAXY = LONG(preV[0].x), MINY = LONG(preV[0].x);

	for(int i = 1; i < preV.size(); i++)
	{
		if (preV[i].x > MAXX)
			MAXX = preV[i].x;

		if (preV[i].x < MINX)
			MAXX = preV[i].x;

		if (preV[i].y > MAXY)
			MAXX = preV[i].y;

		if (preV[i].y < MINY)
			MAXX = preV[i].y;

	}

	RECT r = { MINX, MINY, MAXX, MAXY };

	return r;
}

void GP::BFS()
{

}

bool operator!=(const POINTF a, const POINTF b)
{
	if (a.x == b.x && a.y == b.y)
		return false;
	else
		return true;
}
