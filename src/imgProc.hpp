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
#include "helpers.hpp"

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

Image computeEdgels(Image image, int thresh)
{
	Image result;
	result.width = image.width;
	result.height = image.height;
	result.channels = 1;
	result.buffer = std::vector<unsigned char>(result.width*result.height);

	// θ = arctan(gy/gx)
	// gy: y-component of the gradient
	// gx: x-component of the gradient
	for(int y=1;y<image.height;y++)
	{
		for(int x=1;x<image.width;x++)
		{
			int prevX = image.buffer[y*image.width + (x-1)];
			int prevY = image.buffer[(y-1)*image.width + x];
			int now = image.buffer[y*image.width + x];
			int dx = now-prevX;
			int dy = now-prevY;
			if(dx>thresh || dy>thresh || dx<-thresh || dy<-thresh)
			{
				// TODO Use a lookup table (faster)
				result.buffer[y*result.width + x] = std::atan2(dy, dx)*255./(M_PI*2);
			}
		}
	}
	return result;
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

Image grayscaleToColor(Image image)
{
	// TODO
	if(image.channels!=1)
	{
		std::cout << "[grayscaleToColor] Image should be in gray scale" << std::endl;
		return image;
	}

	Image result;
	result.width = image.width;
	result.height = image.height;
	result.channels = 3;
	result.buffer = std::vector<unsigned char>(result.width*result.height*result.channels);

	for(int y=0;y<image.height;y++)
	{
		for(int x=0;x<image.width;x++)
		{
			unsigned char val = image.buffer[y*image.width + x];
			if(val==0)
				continue;

			if(val<255/4)
				result.buffer[y*result.width*result.channels + x*result.channels] = 255;
			else if(val<255/2)
				result.buffer[y*result.width*result.channels + x*result.channels+1] = 255;
			else if(val<3*255/4)
				result.buffer[y*result.width*result.channels + x*result.channels+2] = 255;
			else
			{
				result.buffer[y*result.width*result.channels + x*result.channels] = 255;
				result.buffer[y*result.width*result.channels + x*result.channels+2] = 255;
			}
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

Image grayscaleIgnoreColor(Image image)
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
			int minimum=255;
			for(int c=0;c<image.channels;c++)
			{
				maximum = std::max(maximum,(int)image.buffer[y*image.width*image.channels + x*image.channels + c]);
				minimum = std::min(minimum,(int)image.buffer[y*image.width*image.channels + x*image.channels + c]);
			}
			int diff = maximum-minimum;
			if(diff<20)
				result.buffer[y*image.width + x] = maximum;
			else
				result.buffer[y*image.width + x] = 255;
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

Image mergeRGB(Image imageR, Image imageG, Image imageB)
{
	// TODO error handling
	Image result;
	result.width = imageR.width;
	result.height = imageR.height;
	result.channels = 3;
	result.buffer = std::vector<unsigned char>(result.width*result.height*result.channels);

	for(int y=0;y<result.height;y++)
	{
		for(int x=0;x<result.width;x++)
		{
			int index = y*result.width*result.channels + x*result.channels;
			result.buffer[index+0] = imageR.buffer[y*result.width+x];
			result.buffer[index+1] = imageG.buffer[y*result.width+x];
			result.buffer[index+2] = imageB.buffer[y*result.width+x];
		}
	}
	return result;
}

Image mergeOrientation(Image image1, Image image2)
{
	// TODO
	if(image1.width!=image2.width || image1.height!=image2.height || image1.channels!=image2.channels)
	{
		std::cout << "[mergeOrientation] Incompatible images. Nothing done" << std::endl;
		return image1;
	}

	for(int y=0;y<image1.height;y++)
	{
		for(int x=0;x<image1.width;x++)
		{
			int index = y*image1.width*image1.channels + x*image1.channels;
			float dx = image1.buffer[index];
			float dy = image2.buffer[index];
			if(dx>10 || dy>10)
				image1.buffer[index]=atan2(dy,dx)/(2*M_PI)*255.f;
		}
	}
	return image1;
}

Image zhangSuen(Image image)
{
	// Reference https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm
	if(image.channels != 1)
	{
		std::cout << "[zhangSuen] Image should have only one channels. Nothing done." << std::endl;
		return image;
	}

	int width = image.width;
	std::vector<int> sequence = {-width, -width+1, +1, width+1, width, width-1, -1, -width-1, -width};

	std::vector<int> pixelsToRemove;
	bool repeat;
	do{
		repeat = false;
		// Step 1 and 2
		for(int step = 1; step<=2;step++)
		{
			pixelsToRemove.clear();
			for(int y=1;y<image.height-1;y++)
			{
				for(int x=1;x<image.width-1;x++)
				{
					int index = y*image.width + x;
					if(image.buffer[index]!=255)// Only check white pixels
						continue;

					// Calculate B
					int B=0;
					for(auto offset : sequence)
						if(image.buffer[index+offset] == 0)
							B++;

					// Calculate A
					int A=0;
					unsigned char lastVal = image.buffer[index+sequence[0]];
					for(auto offset : sequence)
						if(image.buffer[index+offset] == 0 && lastVal == 255)
						{
							A++;
							lastVal = image.buffer[index+offset];
						}

					// Check if should set as black
					if(B>=2 && B<=6 && A==1 &&
							(image.buffer[index+sequence[0]]==255 || image.buffer[index+sequence[2]]==255 || image.buffer[index+sequence[(step==1?4:6)]]==255) &&
							(image.buffer[index+sequence[(step==1?2:0)]]==255 || image.buffer[index+sequence[4]]==255 || image.buffer[index+sequence[6]]==255))
					{
						pixelsToRemove.push_back(index);
						repeat = true;
					}

				}
			}
			// Set pixels as black
			for(auto index : pixelsToRemove)
			{
				image.buffer[index] = 0;
			}
		}
	}while(repeat);

	return image;
}

std::vector<bool> visited;
Image imageToCompute;

std::vector<Point> getRegion(Point point, unsigned char value)
{
	if(point.x<0 || point.y<0 || point.x>=imageToCompute.width || point.y>=imageToCompute.height)
		return {};

	unsigned char currVal = imageToCompute.buffer[point.y*imageToCompute.width + point.x];
	int error = std::abs(currVal-value);
	if(error>127)
		error = 255-error;
	if(currVal==0 || error > 25 || visited[point.y*imageToCompute.width+point.x])
		return {};

	// 4 or 8 neighbors?
	visited[point.y*imageToCompute.width+point.x] = true;
	std::vector<Point> reg1 = getRegion({point.x+1, point.y}, value);
	std::vector<Point> reg2 = getRegion({point.x-1, point.y}, value);
	std::vector<Point> reg3 = getRegion({point.x, point.y+1}, value);
	std::vector<Point> reg4 = getRegion({point.x, point.y-1}, value);

	std::vector<Point> result = {point};
	result.insert(result.end(), reg1.begin(), reg1.end());
	result.insert(result.end(), reg2.begin(), reg2.end());
	result.insert(result.end(), reg3.begin(), reg3.end());
	result.insert(result.end(), reg4.begin(), reg4.end());

	return result;
}

std::vector<Line> computeLines(Image image)
{
	visited = std::vector<bool>(image.width*image.height);
	imageToCompute = image;

	std::cout << "Width " << imageToCompute.width << " height" << imageToCompute.height << std::endl;
	std::vector<Line> lines;

	for(int y=0;y<imageToCompute.height;y++)
	{
		for(int x=0;x<imageToCompute.width;x++)
		{
			if(!visited[y*imageToCompute.width+x])
			{
				unsigned char value = imageToCompute.getPixel(x,y);
				if(value>0)
				{
					//std::cout << "Calculate region" << std::endl;
					std::vector<Point> region = getRegion({x,y}, value);

					// Ignore small regions
					if((int)region.size()<20)
						continue;

					// Calculate principal axis
					float w = 1.f;
					float sumW = w*region.size();
					float sumXsquare = 0;
					float sumX = 0;
					float sumYsquare = 0;
					float sumY = 0;
					float sumXY = 0;

					int maxX = 0;
					int maxY = 0;
					int minX = 999999;
					int minY = 999999;

					for(auto point : region)
					{
						sumXsquare += w*point.x*point.x;
						sumX += w*point.x;
						sumYsquare += w*point.y*point.y;
						sumY += w*point.y;
						sumXY += w*point.x*point.y;
						maxX = std::max(float(maxX), point.x);
						maxY = std::max(float(maxY), point.y);
						minX = std::min(float(minX), point.x);
						minY = std::min(float(minY), point.y);
					}

					// Compute center
					Point center = {sumX/sumW, sumY/sumW};

					// Compute eigen values
					float a = sumXsquare-sumX*sumX/sumW;
					float b = sumXY-sumX*sumY/sumW;
					float c = sumYsquare-sumY*sumY/sumW;

					float delta = sqrt((a-c)*(a-c)/4+b*b);
					float Vs = (a+c)/2-delta;// Small eigen value
					float Vl = (a+c)/2+delta;// Large eigen value

					// Compute line angle
					float lineAngle = atan2((Vl-a),b);

					// Straightness of the line (small is better)
					float straightness = sqrt(Vs)/sqrt(Vl);

					// Wrong orientation if not using lineAngle
					bool tiltedRight = (lineAngle<M_PI/2 && lineAngle>0) || (lineAngle<-M_PI/2);
					//Point extreme0 = {minX, tiltedRight ? minY : maxY};
					//Point extreme1 = {maxX, !tiltedRight ? minY : maxY};
					Point extreme0;
					Point extreme1;
					if(tiltedRight)
					{
						// Compute segment size (Approximation)
						float dx = maxX-center.x;
						float dy = maxY-center.y;
						float sizeUp = sqrt(dx*dx + dy*dy);
						dx = center.x-minX;
						dy = center.y-minY;
						float sizeDown = sqrt(dx*dx + dy*dy);

						extreme0 = {center.x+cos(lineAngle)*sizeUp, center.y+sin(lineAngle)*sizeUp};
						extreme1 = {center.x+cos(M_PI+lineAngle)*sizeDown, center.y+sin(M_PI+lineAngle)*sizeDown};
					}
					else
					{
						// Compute segment size (Approximation)
						float dx = center.x-minX;
						float dy = maxY-center.y;
						float sizeUp = sqrt(dx*dx + dy*dy);
						dx = maxX-center.x;
						dy = center.y-minY;
						float sizeDown = sqrt(dx*dx + dy*dy);

						extreme0 = {center.x+cos(lineAngle)*sizeUp, center.y+sin(lineAngle)*sizeUp};
						extreme1 = {center.x+cos(M_PI+lineAngle)*sizeDown, center.y+sin(M_PI+lineAngle)*sizeDown};
					}

					// Add line
					lines.push_back({extreme0,extreme1});
				}
			}
		}
	}

	visited.clear();

	return lines;
}

float minLineDistance(Line l0, Line l1)
{
	float dx[4] = {l1.p0.x-l0.p0.x, l1.p1.x-l0.p0.x, l1.p0.x-l0.p1.x, l1.p1.x-l0.p1.x};
	float dy[4] = {l1.p0.y-l0.p0.y, l1.p1.y-l0.p0.y, l1.p0.y-l0.p1.y, l1.p1.y-l0.p1.y};
	float minDist = 999999;

	for(int i=0;i<4;i++)
	{
		minDist = std::min(minDist, sqrt(dx[i]*dx[i]+dy[i]*dy[i]));
	}
	return minDist;
}

Quadrangle getQuadFromLines(std::vector<Line> lines)
{
	Quadrangle quad;
	for(int i=0;i<4;i++)
	{
		Line l0 = lines[i];
		Line l1 = lines[(i+1)%4];
		
		float l0a = l0.p1.y-l0.p0.y;
		float l0b = l0.p0.x-l0.p1.x;
		float l0c = l0a*l0.p0.x + l0b*l0.p0.y;

		float l1a = l1.p1.y-l1.p0.y;
		float l1b = l1.p0.x-l1.p1.x;
		float l1c = l1a*l1.p0.x + l1b*l1.p0.y;

		float det = l0a*l1b-l1a*l0b;
		if(det==0)
		{
			std::cout << "PARALLEL LINES" << std::endl;
			exit(1);
		}else
		{
			float x = (l1b*l0c-l0b*l1c)/det;
			float y = (l0a*l1c-l1a*l0c)/det;
			switch(i)
			{
				case 0:
					quad.p0 = {x,y};
					break;
				case 1:
					quad.p1 = {x,y};
					break;
				case 2:
					quad.p2 = {x,y};
					break;
				case 3:
					quad.p3 = {x,y};
					break;
			}
		}
	}
	return quad;
}

std::vector<Quadrangle> findQuadrangles(std::vector<Line> lines, std::vector<std::vector<int>> connections, int depth=0, std::vector<int> currList={})
{
	std::vector<Quadrangle> result;

	if(depth == 0)
	{
		// First call
		for(int i=0; i<(int)lines.size(); i++)
		{
			std::vector<int> lineList = {i};
			std::vector<Quadrangle> res = findQuadrangles(lines, connections, depth+1, lineList);
			result.insert(result.end(), res.begin(), res.end());
		}
	}
	else if(depth < 4)
	{
		// For each connection until 5...
		for(auto lineIndex : connections[currList.back()])
		{
			bool skip=false;
			for(auto curr : currList)
				if(lineIndex==curr)
				{
					skip=true;
					break;
				}
			if(skip)continue;

			std::vector<int> lineList = currList;
			lineList.push_back(lineIndex);
			std::vector<Quadrangle> res = findQuadrangles(lines, connections, depth+1, lineList);
			result.insert(result.end(), res.begin(), res.end());
		}
	}
	else if(depth == 4)
	{
		// Check if last one connects with first
		bool closed=false;
		for(auto connection : connections[currList.back()])
		{
			if(connection==currList[0])
				closed = true;
		}
		if(!closed)
			return {};

		//std::cout << "Index: " << 
		//	currList[0] << " " << 
		//	currList[1] << " " << 
		//	currList[2] << " " << 
		//	currList[3] << " " << 
		//	std::endl;
	
		// Intersect lines to find points
		std::vector<Point> interserctionPoints;

		//std::cout << "Quad: " << 
		//	"(" << lines[currList[0]].p0.x << "," << lines[currList[0]].p0.y << ")-(" << lines[currList[0]].p1.x << "," << lines[currList[0]].p1.y << ") " << 
		//	"(" << lines[currList[1]].p0.x << "," << lines[currList[1]].p0.y << ")-(" << lines[currList[1]].p1.x << "," << lines[currList[1]].p1.y << ") " << 
		//	"(" << lines[currList[2]].p0.x << "," << lines[currList[2]].p0.y << ")-(" << lines[currList[2]].p1.x << "," << lines[currList[2]].p1.y << ") " << 
		//	"(" << lines[currList[3]].p0.x << "," << lines[currList[3]].p0.y << ")-(" << lines[currList[3]].p1.x << "," << lines[currList[3]].p1.y << ") " << 
		//	std::endl;

		std::vector<Line> quadLines = {lines[currList[0]], lines[currList[1]], lines[currList[2]], lines[currList[3]]};

		// Check if it intersects with first and has no repeted line
		result.push_back(getQuadFromLines(quadLines));
	}

	// TODO filter repeated quadrangles
	
	return result;
}

std::vector<Quadrangle> computeQuadrangles(std::vector<Line> lines)
{
	std::vector<std::vector<int>> connectedLines(lines.size());
	float maxDist = 5;
	float minDiffA = 0.5;

	// Find connected lines
	for(int i=0; i<(int)lines.size(); i++)
	{
		Line l0 = lines[i];
		for(int j=0; j<(int)lines.size(); j++)
		{
			if(i==j)
				continue;
			bool skip=false;
			for(int k=0;k<connectedLines[i].size();k++)
				if(j==connectedLines[i][k])
					skip = true;
			if(skip)continue;

			Line l1 = lines[j];
			// Check distance
			if(minLineDistance(l0, l1)>maxDist)
				continue;

			// (TODO deal with vertical lines)
			if(l0.p1.x==l0.p0.x || l1.p1.x==l1.p0.x)
				continue;

			// Angular coeficient 
			float l0a = (l0.p1.y-l0.p0.y)/(l0.p1.x-l0.p0.x);
			float l1a = (l1.p1.y-l1.p0.y)/(l1.p1.x-l1.p0.x);

			if(std::abs(l0a-l1a)<minDiffA)
				continue;

			connectedLines[i].push_back(j);
			connectedLines[j].push_back(i);
		}
	}

	//for(int i=0; i<connectedLines.size();i++)
	//{
	//	std::cout << i << "-> ";
	//	for(int j=0; j<connectedLines[i].size();j++)
	//	{
	//		std::cout << connectedLines[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}

	std::vector<Quadrangle> result = findQuadrangles(lines, connectedLines);

	//for(auto line : lines)
	//	result.push_back({line.p0, line.p1, line.p0, line.p1});

	return result;
}

void drawLines(Image& image, std::vector<Line> lines)
{
	// https://stackoverflow.com/questions/10060046/drawing-lines-with-bresenhams-line-algorithm
	for(auto line : lines)
	{
		Point p0 = {int(line.p0.x), int(line.p0.y)};
		Point p1 = {int(line.p1.x), int(line.p1.y)};
		int dx = p1.x - p0.x;
		int dy = p1.y - p0.y;

		int dLong = abs(dx);
		int dShort = abs(dy);

		int offsetLong = dx > 0 ? 1 : -1;
		int offsetShort = dy > 0 ? image.width : -image.width;

		if(dLong < dShort)
		{
			std::swap(dShort, dLong);
			std::swap(offsetShort, offsetLong);
		}

		int error = dLong/2;
		int index = p0.y*image.width*image.channels + p0.x*image.channels;
		const int offset[] = {offsetLong, offsetLong + offsetShort};
		const int abs_d[]  = {dShort, dShort - dLong};
		for(int i = 0; i <= dLong; ++i)
		{
			for(int c=0;c<image.channels;c++)
				if(index+c<image.buffer.size())
					image.buffer[index+c] = 255;

			const int errorIsTooBig = error >= dLong;
			index += offset[errorIsTooBig]*image.channels;
			error += abs_d[errorIsTooBig];
		}
	}
}

Image drawQuadrangles(Image image, std::vector<Quadrangle> quadrangles)
{
	Image result = image;
	for(auto q : quadrangles)
	{
		drawLines(result, {{q.p0, q.p1}, {q.p1, q.p2}, {q.p2, q.p3}, {q.p3, q.p0}});
	}
	return result;
}

#endif// IMG_PROC_H
