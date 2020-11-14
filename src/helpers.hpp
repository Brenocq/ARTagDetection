//--------------------------------------------------
// Robot Simulator
// helpers.h
// Date: 01/10/2020
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef HELPERS_H
#define HELPERS_H

#include "svpng/svpng.h"
#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <iterator>

//--------------------//
//------ Image -------//
//--------------------//
struct Image
{
	std::vector<unsigned char> buffer;
	uint32_t width = 0;
	uint32_t height = 0;
	uint8_t channels = 3;

	unsigned char getPixel(int x, int y, int c=0)
	{
		return buffer[y*width*channels + x*channels + c];
	}
};

struct Point
{
	float x;
	float y;
};

struct Line
{
	Point p0;
	Point p1;
};

struct Quadrangle
{
	Point p0;
	Point p1;
	Point p2;
	Point p3;
};

void populateImage(Image& image)
{
	// Populate buffer
	for(int i=0;i<image.height; i++)
	{
		for(int j=0;j<image.width; j++)
		{
			image.buffer[i*image.width*image.channels+j*image.channels] = float(j)/image.width*127+float(i)/image.height*127;
			image.buffer[i*image.width*image.channels+j*image.channels+1] = float(image.width-j)/image.width*127+float(i)/image.height*127;
			image.buffer[i*image.width*image.channels+j*image.channels+2] = float(j)/image.width*127+float(image.height-i)/image.height*127;
		}
	}
}

//--------------------//
//------- PNG --------//
//--------------------//
void writePng(std::string fileName, Image image)
{
	Image output;
	output.width = image.width;
	output.height = image.height;
	if(image.channels == 1)
	{
		output.channels = 3;
		output.buffer = std::vector<unsigned char>(output.width*output.height*output.channels);
		for(int y=0;y<image.height;y++)
			for(int x=0;x<image.width;x++)
			{
				unsigned char val = image.buffer[y*image.width + x];
				output.buffer[y*output.width*output.channels + x*output.channels + 0] = val;
				output.buffer[y*output.width*output.channels + x*output.channels + 1] = val;
				output.buffer[y*output.width*output.channels + x*output.channels + 2] = val;
			}
	}
	else if(image.channels==3 || image.channels==4)
		output = image;
	else
		return;

	// Save png
	FILE* fp = fopen(("../../output/"+fileName+".png").c_str(), "wb");
	svpng(fp, output.width, output.height, output.buffer.data(), output.channels==3?0:1);
	fclose(fp);
}

//--------------------//
//-------- BMP -------//
//--------------------//
Image readBmp(std::string fileName)
{
	Image image;

	static constexpr size_t HEADER_SIZE = 54;
	std::string path = std::string("../../gallery/")+fileName+std::string(".bmp");
	std::ifstream bmp(path.c_str(), std::ios::binary);
    std::array<char, HEADER_SIZE> header;
    bmp.read(header.data(), header.size());

    auto fileSize = *reinterpret_cast<uint32_t *>(&header[2]);
    auto dataOffset = *reinterpret_cast<uint32_t *>(&header[10]);
    auto width = *reinterpret_cast<uint32_t *>(&header[18]);
    auto height = *reinterpret_cast<uint32_t *>(&header[22]);
    auto depth = *reinterpret_cast<uint16_t *>(&header[28]);

	image.width = width;
	image.height = height;
	image.channels = depth/8;

    //std::cout << "fileSize: " << fileSize << std::endl;
    //std::cout << "dataOffset: " << dataOffset << std::endl;
    //std::cout << "width: " << width << std::endl;
    //std::cout << "height: " << height << std::endl;
    //std::cout << "depth: " << depth << "-bit" << std::endl;
    //std::cout << "channels: " << (int)image.channels << std::endl;
    //std::vector<char> img(dataOffset - HEADER_SIZE);
    //bmp.read(img.data(), img.size());

    auto dataSize = ((width * 3 + 3) & (~3)) * height;
    std::vector<char> img(dataSize);
    img.resize(dataSize);
    bmp.read(img.data(), img.size());
    std::vector<unsigned char> buffer(dataSize);

	for(int y=0;y<height;y++)
		for(int x=0;x<width*3;x++)
			buffer[(height-y-1)*width*3+x] = (unsigned char)(img[y*width*3+x]);//float(i)/dataSize*255;//

	image.buffer = buffer;

	return image;
}

#endif// HELPERS_H
