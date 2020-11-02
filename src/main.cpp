#include <iostream>
#include <string>
#include <vector>
#include "helpers.h"

int main()
{
	//Image image;
	//image.width = 500;
	//image.height = 300;
	//image.channels = 3;
	//image.buffer = std::vector<unsigned char>(image.width*image.height*image.channels);
	//populateImage(image);

	Image image = readBmp("1");
	writePng("test", image);
	return 0;
}
