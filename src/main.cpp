//--------------------------------------------------
// Robot Simulator
// main.cpp
// Date: 01/10/2020
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include "helpers.h"
#include "imgProc.h"


void detectARtags(Image& image);

int main()
{
	for(int i=0;i<=4;i++)
	{
		std::string name = "x";
		name[0] = '0'+i;
		Image image = readBmp(name);
		detectARtags(image);
		writePng(name, image);
	}

	return 0;
}

void detectARtags(Image& image)
{
	Image original = image;
	// Convert to grayscale
	image = grayscaleMax(image);
	
	// Smoothing the image with gaussian filter
	std::vector<float> gaussianKernel = {	1, 4, 7, 4,1,
											4,16,26,16,4,
											7,26,41,26,7,
											4,16,26,16,4,
											1, 4, 7, 4,1};
	for(unsigned int i=0;i<gaussianKernel.size();i++)
		gaussianKernel[i]/=273;
	image = convolution(image, gaussianKernel);
	image = computeEdgels(image, 20);
	image = grayscaleToColor(image);
}

