#include <Windows.h>

class Bitmap{
private:
	BITMAPINFO bmInfo;

	BITMAPINFOHEADER bmInfoHeader;

	HBITMAP hBitmap;

	HDC hScrDC;
	HDC hMemDC;

	unsigned char *pixel;
public:
	int width;
	int height;	

	Bitmap();
	Bitmap(char path[]);
	Bitmap(int width, int height);
	~Bitmap();

	void create(int width, int height);
	void fromFile(char path[]);
	void fromScreen();
	void fromScreen(int reference_x, int reference_y, int screen_width, int screen_height);
	void rotate(double angle);
	void setPixel(int x, int y, int BGR, unsigned char value);
	void toFile(char path[]);

	unsigned char getPixel(int x, int y, int BGR);
};
