#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "Bitmap.h"

Bitmap::Bitmap(){
	hBitmap = 0;
	hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

	hMemDC = CreateCompatibleDC(hScrDC);
	pixel = new unsigned char[0];
	width = 0;
	height = 0;

	setlocale(LC_ALL, "korean");
}
Bitmap::Bitmap(char path[]){
	hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);
	pixel = new unsigned char[0];

	setlocale(LC_ALL, "korean");
	fromFile(path);
}
Bitmap::Bitmap(int width, int height){
	hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);
	pixel = new unsigned char[0];

	setlocale(LC_ALL, "korean");
	create(width, height);
}
Bitmap::~Bitmap(){
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	DeleteDC(hScrDC);
	delete[] pixel;
}

void Bitmap::create(int width, int height){
	hBitmap = CreateCompatibleBitmap(hScrDC, (this->width = width), (this->height = height));

	pad = (3 * width % 4) ? (4 - 3 * width % 4) : (0);

	::ZeroMemory(&bmInfoHeader, sizeof(BITMAPINFOHEADER));
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoHeader.biWidth = width;
	bmInfoHeader.biHeight = -height;
	bmInfoHeader.biPlanes = 1;
	bmInfoHeader.biBitCount = 24;
	bmInfoHeader.biCompression = BI_RGB;
	bmInfoHeader.biSizeImage = (3 * width + pad) * height;

	pixel = (unsigned char*)realloc(pixel, (3 * width + pad) * height);
	GetDIBits(hScrDC, hBitmap, 0, height, pixel, (BITMAPINFO*)&bmInfoHeader, DIB_RGB_COLORS);
}
void Bitmap::fromFile(const char path[]){
	BITMAP bitmap;

	BITMAPFILEHEADER *bmFileHeader;

	DWORD fileSize;
	DWORD dwRead;

	HANDLE hFile;

	LPVOID lpDIBits;

#ifdef UNICODE
	wchar_t *_path = new wchar_t[strlen(path) + 1];
	mbstowcs(_path, path, strlen(path) + 1);
	hFile = CreateFile(_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	delete[] _path;
#else
	hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif

	fileSize = GetFileSize(hFile, NULL);
	ReadFile(hFile, (bmFileHeader = (BITMAPFILEHEADER*)(new unsigned char[fileSize])), fileSize, &dwRead, NULL);

	lpDIBits = ((PBYTE)bmFileHeader + sizeof(BITMAPFILEHEADER));
	if (hBitmap){
		DeleteObject(hBitmap);
	}
	hBitmap = CreateDIBitmap(hScrDC, (BITMAPINFOHEADER*)lpDIBits, 0, NULL, NULL, 0);
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	SetDIBits(NULL, hBitmap, 0, bitmap.bmHeight, (PBYTE)bmFileHeader + bmFileHeader->bfOffBits, (BITMAPINFO*)lpDIBits, DIB_RGB_COLORS);

	pad = (3 * bitmap.bmWidth % 4) ? (4 - 3 * bitmap.bmWidth % 4) : (0);

	::ZeroMemory(&bmInfoHeader, sizeof(BITMAPINFOHEADER));
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoHeader.biWidth = (width = bitmap.bmWidth);
	bmInfoHeader.biHeight = -(height = bitmap.bmHeight);
	bmInfoHeader.biPlanes = 1;
	bmInfoHeader.biBitCount = 24;
	bmInfoHeader.biCompression = BI_RGB;
	bmInfoHeader.biSizeImage = (3 * width + pad) * height;

	pixel = (unsigned char*)realloc(pixel, (3 * width + pad) * height);
	GetDIBits(hScrDC, hBitmap, 0, height, pixel, (BITMAPINFO*)&bmInfoHeader, DIB_RGB_COLORS);

	CloseHandle(hFile);
	delete[] bmFileHeader;
}
void Bitmap::fromScreen(){
	fromScreen(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}
void Bitmap::fromScreen(int reference_x, int reference_y, int screen_width, int screen_height){
	HBITMAP hOldBitmap;

	if (width != screen_width || height != screen_height){
		if (hBitmap){
			DeleteObject(hBitmap);
		}
		hBitmap = CreateCompatibleBitmap(hScrDC, width = screen_width, height = screen_height);
		pixel = (unsigned char*)realloc(pixel, (3 * width + pad) * height);
	}
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	BitBlt(hMemDC, 0, 0, screen_width, screen_height, hScrDC, reference_x, reference_y, CAPTUREBLT | SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);

	pad = (3 * width % 4) ? (4 - 3 * width % 4) : (0);

	::ZeroMemory(&bmInfoHeader, sizeof(BITMAPINFOHEADER));
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoHeader.biWidth = width;
	bmInfoHeader.biHeight = -height;
	bmInfoHeader.biPlanes = 1;
	bmInfoHeader.biBitCount = 24;
	bmInfoHeader.biCompression = BI_RGB;
	bmInfoHeader.biSizeImage = (3 * width + pad) * height;

	GetDIBits(hScrDC, hBitmap, 0, height, pixel, (BITMAPINFO*)&bmInfoHeader, DIB_RGB_COLORS);

	DeleteObject(hOldBitmap);
}
void Bitmap::rotate(double angle){
	unsigned char *temp = new unsigned char[(3 * width + pad) * height];

	double x0 = (width - 1) / 2.0;
	double y0 = (height - 1) / 2.0;

	while (angle < 0)	angle += 360;
	while (angle >= 360) angle -= 360;

	double degree = 3.1415926535897931 * ((360 - angle) / 180);
	double cosine = cos(degree);
	double sine = sin(degree);

	for (int i = 0; i < width * height; i++){
		int x1 = i % width;
		int y1 = i / width;

		int x2 = cosine * (x1 - x0) - sine	 * (y1 - y0) + x0 + 0.5;
		int y2 = sine	* (x1 - x0) + cosine * (y1 - y0) + y0 + 0.5;

		for (int j = 0; j < 3; j++){
			temp[(3 * width + pad) * y1 + 3 * x1 + j] = (0 <= x2 && x2 < width && 0 <= y2 && y2 < height) ? (pixel[(3 * width + pad) * y2 + 3 * x2 + j]) : (0);
		}
	}
	for (int i = 0; i < width * height * 3; i++){
		pixel[i] = temp[i];
	}
	delete[] temp;
}
void Bitmap::toFile(const char path[]){
	BITMAP bitmap;
	BITMAPINFO *bmInfo;

	BITMAPFILEHEADER bmFileHeader;

	DWORD dwWritten;

	HANDLE hFile;

#ifdef UNICODE
	wchar_t *_path = new wchar_t[strlen(path) + 1];
	mbstowcs(_path, path, strlen(path) + 1);
	hFile = CreateFile(_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	delete[] _path;
#else
	hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif

	SetDIBits(hScrDC, hBitmap, 0, height, pixel, (BITMAPINFO*)&bmInfoHeader, DIB_RGB_COLORS);
	bmInfo = new BITMAPINFO[bmInfoHeader.biSize + bmInfoHeader.biSizeImage];
	bmInfo->bmiHeader = bmInfoHeader;

	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	GetDIBits(hMemDC, hBitmap, 0, height, (PBYTE)bmInfo + bmInfoHeader.biSize, bmInfo, DIB_RGB_COLORS);

	bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmFileHeader.bfReserved1 = 0;
	bmFileHeader.bfReserved2 = 0;
	bmFileHeader.bfSize = bmInfoHeader.biSize + bmInfoHeader.biSizeImage + sizeof(BITMAPFILEHEADER);
	bmFileHeader.bfType = 0x4d42;

	WriteFile(hFile, &bmFileHeader, sizeof(bmFileHeader), &dwWritten, NULL);
	WriteFile(hFile, bmInfo, bmInfoHeader.biSize + bmInfoHeader.biSizeImage, &dwWritten, NULL);

	CloseHandle(hFile);
	delete[] bmInfo;
}

void Bitmap::setPixel(int x, int y, int BGR, unsigned char value){
	if (x < width && y < height && BGR < 3){
		pixel[(3 * width + pad) * y + x * 3 + BGR] = value;
	}
}
unsigned char Bitmap::getPixel(int x, int y, int BGR){
	unsigned char value = 0;

	if (x < width && y < height && BGR < 3){
		value = pixel[(3 * width + pad) * y + x * 3 + BGR];
	}
	return value;
}
