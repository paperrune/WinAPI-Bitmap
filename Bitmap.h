#include <Windows.h>

class Bitmap {
private:
	int pad;

	BITMAPINFO bmInfo;

	HBITMAP hBitmap;

	HDC hScrDC;
	HDC hMemDC;
public:
	unsigned char *pixel;

	int width;
	int height;

	BITMAPINFOHEADER bmInfoHeader;

	Bitmap();
	Bitmap(const char path[]);
	Bitmap(int width, int height);
	~Bitmap();

	void create(int width, int height);
	void fromFile(const char path[]);
	void fromScreen();
	void fromScreen(int reference_x, int reference_y, int screen_width, int screen_height);
	void rotate(double angle);
	void setPixel(int x, int y, int BGR, unsigned char value);
	void toFile(const char path[]);

	unsigned char getPixel(int x, int y, int BGR);

	Bitmap* copy();
	Bitmap* resize(int width, int height);
};
