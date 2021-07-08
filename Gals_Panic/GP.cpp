#include "GP.h"
#include "framework.h"

bool operator!=(const POINTF a,const POINTF b);
bool operator==(const POINTF a,const POINTF b);

GP::~GP()
{
	GdiplusShutdown(g_GdiToken);
	for (int i = 0; i < rectview.right; i++)
		delete[] arr[i];

	delete[] arr;
}

void GP::set()
{
	rectview.right = width;
	rectview.left = 0;
	rectview.bottom = height;
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
	getrect();
	makeBOSS();
}


void GP::make_space()
{
	srand((unsigned int)time(NULL));

	int x = rand() % (width - 350)+200;
	int y = rand() % (height - 350)+200;
	

	for (int i = x; i < x + 100; i++)
	{
		for (int j = y; j < y + 100; j++)
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
	hImage = CreateCompatibleBitmap(hdc,width, height);
	hImage2 = CreateCompatibleBitmap(hdc, width, height);
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

	drawBOSS(memDC);

	BitBlt(hdc, x, y, bx, by, memDC, x, y, SRCCOPY);

	SelectObject(memDC, old);
	DeleteDC(memDC);

}

void GP::set_image(Bitmap &myBitmap)
{
	Gdiplus::Graphics mgraphics(&myBitmap);

	mgraphics.DrawImage(&myBitmap, 0, 0, width, height);

}

void GP::changebitpixel(Bitmap &myBitmap)
{
	int bx = width, by = height;

	Gdiplus::Graphics mgraphics(&myBitmap);

	for (UINT row = 0; row < bx; row++)
	{
		for (UINT col = 0; col < by; col++)
		{
			if(arr[row][col] == 1 || arr[row][col] == 2)
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
//	Double_buffer(hWnd, hdc, x-10, y-10, bx * 2, by * 2);

	Double_buffer(hWnd, hdc, 0, 0, 800, 800);

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

	const int velocity = 5;
//	preP = pointer;
	bool click = true;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
  		SPACE = true;
	}
	else
		SPACE = false;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		click = false;
		for (int i = 0; i < velocity; i++)
		{
//			pointer.x += 1;
			if (!movePointerrecur(1))
				break;
		}
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		click = false;
		for (int i = 0; i < velocity; i++)
		{
//			pointer.x -= 1;
			if (!movePointerrecur(2))
				break;
		}
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		click = false;
		for (int i = 0; i < velocity; i++)
		{
//			pointer.y -= 1;
			if (!movePointerrecur(3))
				break;
		}
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		click = false;
		for (int i = 0; i < velocity; i++)
		{
//			pointer.y += 1;
			if (!movePointerrecur(4))
				break;
		}
	}



	if (click && preV.size() > 0)
	{
		for (int i = 0; i < velocity; i++)
		{
			if (!movePointerrecur(5))
				break;
		}
	}


	//if (arr[int(pointer.x)][int(pointer.y)] != 2 || SPACE)
	//{
	//	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && (pointer != preP)) // 스페이스
	//	{
	//		if (arr[int(pointer.x)][int(pointer.y)] != 0 && preV.size() == 0)
	//		{
	//			pointer = preP;
	//			return;
	//		}
	//		SPACE = true;

	//		if (arr[int(pointer.x)][int(pointer.y)] == 2)
	//		{
	//			get_space();
	//			make_Surface();
	//			flag = true;
	//			SPACE = false;
	//			return;
	//		}




	//		if(arr[int(pointer.x)][int(pointer.y)] != 0)
	//			pointer = preP;
	//		else
	//		{
	//			if (preV.size() == 0)
	//			{
	//				arr[int(preP.x)][int(preP.y)] = 3;
	//				preV.push_back(preP);
	//			}
	//			arr[int(pointer.x)][int(pointer.y)] = 3;
	//			preV.push_back(pointer);
	//		}
	//	}
	//	else
	//	{
	//		if (preV.size() > 0)
	//		{
	//			arr[int(pointer.x)][int(pointer.y)] = 0;
	//			pointer = preV.back();
	//			preV.erase(preV.begin() + preV.size() - 1, preV.end());
	//		}
	//		else
	//		{
	//			pointer = preP;
	//			SPACE = false;
	//		}

	//		preP = pointer;
	//	}

	//}
	//else if(arr[int(pointer.x)][int(pointer.y)] != 2)
	//{
	//	pointer = preP;
	//}

}

bool GP::movePointerrecur(int n)
{
	preP = pointer;

	// 1 right 2 left 3 up 4 down
	if (n == 1) {
		pointer.x += 1;
	}
	if (n == 2) {
		pointer.x -= 1;
	}
	if (n == 3) {
		pointer.y -= 1;
	}
	if (n == 4) {
		pointer.y += 1;
	}

	if (pointer.x < 0 || pointer.x >= width || pointer.y < 0 || pointer.y >= height)
	{
		pointer = preP;
		return false;
	}


	if (SPACE)
	{
		if (arr[int(pointer.x)][int(pointer.y)] == 0)
		{
//			if (preV.size() == 0)
//			{
//				preV.push_back(preP);
//			}
			arr[int(pointer.x)][int(pointer.y)] = 3;
			preV.push_back(preP);
//			preV.push_back(pointer);
			return true;

		}
		else if (arr[int(pointer.x)][int(pointer.y)] == 1)
		{
			pointer = preP;
			return false;

		}
		else if (arr[int(pointer.x)][int(pointer.y)] == 2)
		{
			if (preV.size() > 0)
			{
				get_space();
				make_Surface();
				flag = true;
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			if (preP != pointer)
			{
				pointer = preP;
				return false;
			}
			else
				return true;
//			else if (preV.size() > 0)
//			{
//				arr[int(preP.x)][int(preP.y)] = 0;
////				arr[int(pointer.x)][int(pointer.y)] = 0;
//				pointer = preV.back();
//				preV.erase(preV.begin() + preV.size() - 1, preV.end());
//				return true;
//			}
//			else
//			{
//				pointer = preP;
//				return false;
//			}

		}
	}
	else
	{
		if (preV.size() > 0)
		{ // 현재 포인터만 벡터에 넣으면 두번째 preV에서 다시이동하면 벡터에 지워진상태에서 3이 유지되서 나중에 자국이 남음 
			arr[int(preP.x)][int(preP.y)] = 0;
//			arr[int(pointer.x)][int(pointer.y)] = 0;
			pointer = preV.back();
//			preV.erase(preV.begin() + preV.size() - 1, preV.end());
			preV.erase(preV.end() - 1);
			return true;
		}
		else
		{
			if (arr[int(pointer.x)][int(pointer.y)] == 2)
			{
				return true;
			}
			else
			{
				pointer = preP;
				return false;
			}

		}
	}
	/*//if (arr[int(pointer.x)][int(pointer.y)] != 2)
	//{
	//	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && (pointer != preP)) // 스페이스
	//	{
	//		if (arr[int(pointer.x)][int(pointer.y)] != 0 && preV.size() == 0)
	//		{
	//			pointer = preP;
	//			return false;
	//		}
	//		SPACE = true;

	//		if (arr[int(pointer.x)][int(pointer.y)] == 2)
	//		{
	//			get_space();
	//			make_Surface();
	//			flag = true;
	//			SPACE = false;
	//			return false;
	//		}




	//		if (arr[int(pointer.x)][int(pointer.y)] != 0)
	//		{
	//			pointer = preP;
	//			return false;
	//		}
	//		else
	//		{
	//			if (preV.size() == 0)
	//			{
	//				arr[int(preP.x)][int(preP.y)] = 3;
	//				preV.push_back(preP);
	//			}
	//			arr[int(pointer.x)][int(pointer.y)] = 3;
	//			preV.push_back(pointer);
	//			return true;
	//		}
	//	}
	//	else
	//	{
	//		if (preV.size() > 0)
	//		{
	//			arr[int(pointer.x)][int(pointer.y)] = 0;
	//			pointer = preV.back();
	//			preV.erase(preV.begin() + preV.size() - 1, preV.end());
	//			return true;
	//		}
	//		else
	//		{
	//			pointer = preP;
	//			SPACE = false;
	//			return false;
	//		}

	//		preP = pointer;
	//	}

	//}
	//else if (arr[int(pointer.x)][int(pointer.y)] != 2)
	//{
	//	pointer = preP;
	//	return false;
	//}*/

}

void GP::make_Surface()
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (arr[i][j] == 2 || arr[i][j] == 3)
				arr[i][j] = 1;
		}
	}
	bool flag = true;
	for (int i = 0; i < width; i++)
	{
		for (int j = 1; j < height; j++)
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
	for (int i = 0; i < width; i++)
	{
		for (int j = 1; j < height; j++)
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
	arr[int(pointer.x)][int(pointer.y)] = 2;

}

void GP::get_space()
{
	const int dir[8][2] = { {1,1},{1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1} };

	POINTF stand;

	//for (int i = 0; i < 8; i++)
	//{
	//	if ((arr[int(preV[0].x) + dir[i][0]][int(preV[0].y) + dir[i][1]] == 1 &&
	//		arr[int(preV[0].x) + dir[i][0]][int(preV[0].y) + dir[i][1]] == 2 &&
	//		arr[int(preV[0].x) + dir[i][0]][int(preV[0].y) + dir[i][1]] == 3))
	//	{
	//		stand.x = preV[0].x + dir[i][0];
	//		stand.y = preV[0].y + dir[i][1];
	//		break;
	//	}
	//}

	POINTF p = pointer;

	/*for (int i = 0; i < preV.size(); i++)
	{
		stand.x = (p.x + preV[i].x) / 2;
		stand.y = (p.y + preV[i].y) / 2;

		if (p.x == preV[i].x || p.y == preV[i].y)
		{
			bool ch = false;
			for (int j = 0; j < 8; j++)
			{
				stand.x += dir[j][0];
				stand.y += dir[j][1];
				if (check_inner_point(stand))
				{
					true;
					break;
				}
			}
			if (ch)
				break;

		}
		else
		{
			if (check_inner_point(stand))
				break;
		}
		
	}

	BFS(stand);*/

	for (int i = 0; i < rectview.right; i++)
	{
		for (int j = 0; j < rectview.bottom; j++)
		{
			if (arr[i][j] == 3 || arr[i][j] == 2)
				arr[i][j] = 1;
		}
	}

	change_maxrect();

	POINTF temp;
	for (int i = maxrect.left; i <= maxrect.right; i++)
	{
		for (int j = maxrect.top; j <= maxrect.bottom; j++)
		{
			temp.x = i;
			temp.y = j;
			if(check_inner_point(temp))
				arr[int(temp.x)][int(temp.y)] = 1;
		}
	}



	/*
	for (int i = 0; i < preV.size(); i++)
	{
		int j = preV[i].x;
		for (int k = preV[i].x + 1; k < rectview.right; k++)
		{
			if (arr[k][int(preV[i].y)] == 1)
			{
				j = k;
				break;
			}
		}

		for (int k = preV[i].x + 1; k < j; k++)
		{
			arr[k][int(preV[i].y)] = 1;
		}

		j = preV[i].x;
		for (int k = preV[i].x - 1; k > 0; k--)
		{
			if (arr[k][int(preV[i].y)] == 1)
			{
				j = k;
				break;
			}
		}

		for (int k = preV[i].x - 1; k > j; k--)
		{
			arr[k][int(preV[i].y)] = 1;
		}
	}*/



}

void GP::getrect()
{
	LONG MAXX = maxrect.right, MINX = maxrect.left, MAXY = maxrect.bottom, MINY = maxrect.top;

	for(int i = 1; i < rectview.right; i++)
	{
		for (int j = 1; j < rectview.bottom; j++)
		{
			if (arr[i][j] != 0)
			{
				if (i > MAXX)
					MAXX = i;

				if (i < MINX)
					MINX = i;

				if (j > MAXY)
					MAXY = j;

				if (j < MINY)
					MINY = j;

			}
		}

		
	}
	maxrect.left = MINX;
	maxrect.right = MAXX;
	maxrect.bottom = MAXY;
	maxrect.top = MINY;

}

void GP::change_maxrect()
{
	LONG MAXX = maxrect.right, MINX = maxrect.left, MAXY = maxrect.bottom, MINY = maxrect.top;

	for (int i = 0; i < preV.size(); i++)
	{

		if (arr[int(preV[i].x)][int(preV[i].y)] != 0)
		{
			if (int(preV[i].x > MAXX))
				MAXX = int(preV[i].x);

			if (int(preV[i].x < MINX))
				MINX = int(preV[i].x);

			if (int(preV[i].y > MAXY))
				MAXY = int(preV[i].y);

			if (int(preV[i].y < MINY))
				MINY = int(preV[i].y);

		}
	}

	maxrect.left = MINX;
	maxrect.right = MAXX;
	maxrect.bottom = MAXY;
	maxrect.top = MINY;
}


void GP::BFS(POINTF p)
{
	if (p.x < 0 || p.x >= rectview.right || p.y < 0 || p.y >= rectview.bottom)
		return;

	if (arr[int(p.x)][int(p.y)] != 0)
		return;

	arr[int(p.x)][int(p.y)] = 1;

	POINTF temp = p;
	temp.x++;
	BFS(temp); 
	temp = p;

	temp.x--;
	BFS(temp);
	temp = p;

	temp.y++;
	BFS(temp); 
	temp = p;

	temp.y--;
	BFS(temp);

}

bool GP::check_inner_point(POINTF stand)
{
	if (arr[int(stand.x)][int(stand.y)] != 0)
		return false;
	bool ch = false;

	for (int j = stand.x + 1; j < rectview.right; j++)
		if (arr[j][int(stand.y)] != 0)
		{
			ch = true;
			continue;
		}

	if (ch)
	{
		ch = false;
		for (int j = stand.x - 1; j > 0; j--)
			if (arr[j][int(stand.y)] != 0)
			{
				ch = true;
				continue;
			}
	}
	if (ch)
	{
		ch = false;
		for (int j = stand.y + 1; j < rectview.bottom; j++)
			if (arr[int(stand.x)][j] != 0)
			{
				ch = true;
				continue;
			}
	}
	if (ch)
	{
		ch = false;
		for (int j = stand.y - 1; j > 0; j--)
			if (arr[int(stand.x)][j] != 0)
			{
				ch = true;
				continue;
			}
	}
	if (ch)
	{
		return true;
	}
	
	return false;
}

void GP::makeBOSS()
{
	srand((unsigned int)time(NULL));

	b.R = 50;
	b.x = rand() & (width - 20) + 10;
	b.y = rand() & (height - 20) + 10;

	while (!CollisionWall())
	{
		b.x = rand() & (width - 20) + 10;
		b.y = rand() & (height - 20) + 10;
	}


}

void GP::drawBOSS(HDC hdc)
{/*
	HDC hMemDC;
	HBITMAP hOldBitmap;
	int bx, by;

	hMemDC = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, cImage);
*/
	Ellipse(hdc, b.x - b.R, b.y - b.R, b.x + b.R, b.y + b.R);
/*
	BitBlt(hdc, b.x - b.R, b.y - b.R, b.R * 2, b.R * 2, hMemDC, b.x - b.R, b.y - b.R, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);

	DeleteDC(hMemDC);

*/
}

bool GP::CollisionWall()
{
	bool ch = false;
	if (b.x <= b.R)
	{
		b.vx *= -1;
		b.x = b.R +1 ;
		ch = true;

	}

	if (b.x >= width - b.R)
	{
		b.vx *= -1;
		b.x = width - b.R - 1;
		ch = true;
	}

	if (b.y <= b.R)
	{
		b.vy *= -1;
		b.y = b.R + 1;
		ch = true;
	}

	if (b.y >= height - b.R)
	{
		b.vy *= -1;
		b.y = height - b.R - 1;
		ch = true;
	}

	if (ch)
		return true;
	//srand((unsigned int)time(NULL));

	//if (b.x <= b.R)
	//{
	//	b.vx *= -1;
	//	b.vy = rand() % 5 + 5;
	//	if (rand() % 2)
	//		b.vy *= -1;
	//	b.x = b.R + 1;
	//	ch = true;

	//}

	//if (b.x >= width - b.R)
	//{
	//	b.vx *= -1;
	//	b.vy = rand() % 5 + 5;
	//	if (rand() % 2)
	//		b.vy *= -1;
	//	b.x = width - b.R - 1;
	//	ch = true;
	//}

	//if (b.y <= b.R)
	//{
	//	b.vx = rand() % 5 + 5;
	//	if (rand() % 2)
	//		b.vx *= -1;
	//	b.vy *= -1;
	//	b.y = b.R + 1;
	//	ch = true;
	//}

	//if (b.y >= height - b.R)
	//{
	//	b.vx = rand() % 5 + 5;
	//	if (rand() % 2)
	//		b.vx *= -1;
	//	b.vy *= -1;
	//	b.y = height - b.R - 1;
	//	ch = true;
	//}

	//if (ch)
	//	return true;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (arr[i][j] == 1)
			{
				if (sqrt(pow((int)pointer.x - i, 2) + pow((int)pointer.y - j, 2) <= b.R))
				{
					FLOAT nx = pointer.x - i, ny = pointer.y - j;
					change_vector(nx, ny);

					return true;
				}
			}
		}
	}

}

void GP::CollisionPointer()
{

}

void GP::change_vector(FLOAT nx, FLOAT ny)
{
	nx /= b.R;
	ny /= b.R;

	FLOAT tx = -nx, ty = ny;

	FLOAT innern = b.vx*nx + b.vy*ny;
	FLOAT innert = b.vx*tx + b.vy*ty;

	b.vx = nx * -innern + tx * innert;
	b.vy = ny * -innern + ty * innert;

	FLOAT bp = sqrt(pow(b.vx, 2) + pow(b.vy, 2));
	b.vx /= bp;
	b.vy /= bp;
	b.vx *= b.velocity;
	b.vy *= b.velocity;


}

void GP::update()
{
	movePointer();
	CollisionWall();
	b.x += b.vx;
	b.y += b.vy; 

}

bool operator!=(const POINTF a, const POINTF b)
{
	if (a.x == b.x && a.y == b.y)
		return false;
	else
		return true;
}

bool operator==(const POINTF a, const POINTF b)
{
	if (a.x == b.x && a.y == b.y)
		return true;
	else
		return false;
}
