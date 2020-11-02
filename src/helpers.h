#include "svpng/svpng.h"
#include "jpeglib.h"
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
	if(image.channels!=3 && image.channels!=4)
		return;
	// Save png
	FILE* fp = fopen(("../../output/"+fileName+".png").c_str(), "wb");
	svpng(fp, image.width, image.height, image.buffer.data(), image.channels==3?0:1);
	fclose(fp);
}

//--------------------//
//------- JPEG -------//
//--------------------//
Image readJpeg(std::string fileName)
{
	Image image;	
	int width, height;

	jpeg_decompress_struct decInfo;
	J_COLOR_SPACE colorSpace = JCS_RGB;

	std::string path = std::string("../../gallery/")+fileName+std::string(".jpg");
	std::cout << "PATH " << path << std::endl;
  	struct jpeg_decompress_struct cinfo;
  	FILE * infile;        /* source file */
  	JSAMPARRAY buffer;        /* Output row buffer */
  	int row_stride;       /* physical row width in output buffer */

  	if ((infile = fopen(path.c_str(), "rb")) == NULL) {
  	  fprintf(stderr, "can't open %s\n", fileName);
  	  return image;
  	}

  	/* Now we can initialize the JPEG decompression object. */
  	jpeg_create_decompress(&cinfo);

  	/* Step 2: specify data source (eg, a file) */
  	jpeg_stdio_src(&cinfo, infile);

  	(void) jpeg_read_header(&cinfo, TRUE);

  	// Here I want to only get raw bytes
  	(void) jpeg_start_decompress(&cinfo);

  	row_stride = cinfo.output_width * cinfo.output_components;
  	/* Make a one-row-high sample array that will go away when done with image */
  	buffer = (*cinfo.mem->alloc_sarray)
  	  	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  	while (cinfo.output_scanline < cinfo.output_height) {

  	  (void) jpeg_read_scanlines(&cinfo, buffer, 1);
  	  /* Assume put_scanline_someplace wants a pointer and sample count. */
  	  // put_scanline_someplace(buffer[0], row_stride);
  	}

  	/* Step 7: Finish decompression */

  	(void) jpeg_finish_decompress(&cinfo);

  	jpeg_destroy_decompress(&cinfo);

  	fclose(infile);

	image.width = width;
	image.height = height;
	image.channels = 3;
	image.buffer = std::vector<unsigned char>(image.width*image.height*image.channels);
	return image;
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

    std::vector<char> img(dataOffset - HEADER_SIZE);
    bmp.read(img.data(), img.size());

    auto dataSize = ((width * 3 + 3) & (~3)) * height;
    img.resize(dataSize);
    bmp.read(img.data(), img.size());
    std::vector<unsigned char> buffer(dataSize);

	for(int i=0;i<dataSize;i++)
	{
		buffer[i] = (unsigned char)(img[i]);//float(i)/dataSize*255;//
	}

	image.buffer = buffer;

	return image;
}
