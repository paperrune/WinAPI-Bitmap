#include "Bitmap.h"

int main(){
	Bitmap bitmap;

	// Capturing entire screen
	bitmap.fromScreen();
	bitmap.toFile("0.bmp");

	// Capturing partial screen
	bitmap.fromScreen(100, 100, 1024, 768);
	bitmap.toFile("1.bmp");

	// Access pixel
	bitmap.fromFile("0.bmp");
	for(int y = 0;y < bitmap.height;y++){
		for(int x = 0;x < bitmap.width;x++){
			unsigned char pixel_mean = (bitmap.getPixel(x, y, 0) + bitmap.getPixel(x, y, 1) + bitmap.getPixel(x, y, 2)) / 3;

			for(int BGR = 0;BGR < 3;BGR++){
				bitmap.setPixel(x, y, BGR, pixel_mean);
			}
		}
	}
	bitmap.toFile("2.bmp");
}