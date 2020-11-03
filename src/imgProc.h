//--------------------------------------------------
// Robot Simulator
// imgProc.h
// Date: 02/10/2020
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef IMG_PROC_H
#define IMG_PROC_H
#include <vector>
#include <iostream>
#include <math.h>
#include "helpers.h"

Image derivate(Image image)
{
	for(int y=0;y<image.height;y++)
	{
		for(int x=1;x<image.width;x++)
		{
			for(int c=0;c<image.channels;c++)
			{
				int prev = image.buffer[y*image.width*image.channels + (x-1)*image.channels +c];
				int now = image.buffer[y*image.width*image.channels + x*image.channels +c];
				image.buffer[y*image.width*image.channels + (x-1)*image.channels +c] = ((now-prev)+255)/2;
			}
			if(x==image.width-1)
				for(int c=0;c<image.channels;c++)
					image.buffer[y*image.width*image.channels + x*image.channels+c] = 0;
		}
	}
	return image;
}

Image derivateAbs(Image image, bool horizontal=true)
{
	for(int y=horizontal?0:1;y<image.height;y++)
	{
		for(int x=horizontal?1:0;x<image.width;x++)
		{
			for(int c=0;c<image.channels;c++)
			{
				int prevIndex = horizontal?	(y*image.width*image.channels     + (x-1)*image.channels +c):
										((y-1)*image.width*image.channels + x*image.channels +c);
				int prev = image.buffer[prevIndex];
				int now = image.buffer[y*image.width*image.channels + x*image.channels +c];
				image.buffer[prevIndex] = std::abs(now-prev);
			}
			if(x==image.width-1)
				for(int c=0;c<image.channels;c++)
					image.buffer[y*image.width*image.channels + x*image.channels+c] = 0;
		}
	}
	return image;
}

Image convolution(Image image, std::vector<float> kernel)
{
	int kernelSize = (sqrt(kernel.size())/2);// Kernel half side lenght

	Image result;
	result.width = image.width-kernelSize*2;
	result.height = image.height-kernelSize*2;
	result.channels = image.channels;
	result.buffer = std::vector<unsigned char>(result.width*result.height*result.channels);
	
	for(int y=kernelSize, yr=0; y<image.height-kernelSize; y++, yr++)
	{
		for(int x=kernelSize, xr=0; x<image.width-kernelSize; x++, xr++)
		{
			for(int c=0;c<image.channels;c++)
			{
				float sum=0;
				for(int ky=-kernelSize;ky<kernelSize;ky++)
					for(int kx=-kernelSize;kx<kernelSize;kx++)
					{
						sum += kernel[(ky+kernelSize)*(kernelSize*2+1) + (kx+kernelSize)] 
							* image.buffer[(y+ky)*image.width*image.channels + (x+kx)*image.channels + c];
					}

				result.buffer[yr*result.width*image.channels + xr*image.channels + c] = (unsigned char)sum;
			}
		}
	}

	return result;
}

Image grayscale(Image image)
{
	Image result;
	result.width = image.width;
	result.height = image.height;
	result.channels = 1;
	result.buffer = std::vector<unsigned char>(result.width*result.height);

	for(int y=0;y<image.height;y++)
	{
		for(int x=0;x<image.width;x++)
		{
			int mean=0;
			for(int c=0;c<image.channels;c++)
				mean += image.buffer[y*image.width*image.channels + x*image.channels + c];
			mean/=image.channels;

			result.buffer[y*image.width + x] = mean;
		}
	}
	return result;
}

Image grayscaleMax(Image image)
{
	Image result;
	result.width = image.width;
	result.height = image.height;
	result.channels = 1;
	result.buffer = std::vector<unsigned char>(result.width*result.height);

	for(int y=0;y<image.height;y++)
	{
		for(int x=0;x<image.width;x++)
		{
			int maximum=0;
			for(int c=0;c<image.channels;c++)
				maximum = std::max(maximum,(int)image.buffer[y*image.width*image.channels + x*image.channels + c]);

			result.buffer[y*image.width + x] = maximum;
		}
	}
	return result;
}

Image threshold(Image image, unsigned char thresh)
{
	for(int y=0;y<image.height;y++)
	{
		for(int x=0;x<image.width;x++)
		{
			int mean=0;
			for(int c=0;c<image.channels;c++)
				mean += image.buffer[y*image.width*image.channels + x*image.channels + c];
			mean/=image.channels;

			if(mean>thresh)
				for(int c=0;c<image.channels;c++)
					image.buffer[y*image.width*image.channels + x*image.channels + c] = 255;
			else
				for(int c=0;c<image.channels;c++)
					image.buffer[y*image.width*image.channels + x*image.channels + c] = 0;
		}
	}
	return image;
}

Image mergeMax(Image image1, Image image2)
{
	if(image1.width!=image2.width || image1.height!=image2.height || image1.channels!=image2.channels)
	{
		std::cout << "[mergeMax] Incompatible images. Nothing done" << std::endl;
		return image1;
	}

	for(int y=0;y<image1.height;y++)
	{
		for(int x=0;x<image1.width;x++)
		{
			for(int c=0;c<image1.channels;c++)
			{
				int index = y*image1.width*image1.channels + x*image1.channels + c;
				image1.buffer[index]=std::max(image1.buffer[index], image2.buffer[index]);
			}
		}
	}
	return image1;
}

#endif// IMG_PROC_H
