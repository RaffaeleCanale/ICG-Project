
#include "Image.h"
#include <fstream>
#include <cassert>

Image::Image() : mWidth(0), mHeight(0) {
}


Image::~Image() {
	clear();
}

#pragma pack(push, 1)
typedef struct
{
	unsigned char  identsize;          // size of ID field that follows 18 byte header (0 usually)
	unsigned char  colourmaptype;      // type of colour map 0=none, 1=has palette
	unsigned char  imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	short colourmapstart;			   // first colour map entry in palette
	short colourmaplength;			   // number of colours in palette
	unsigned char  colourmapbits;      // number of bits per palette entry 15,16,24,32

	short xstart;             // image x origin
	short ystart;             // image y origin
	short width;              // image width in pixels
	short height;             // image height in pixels
	unsigned char  bits;               // image bits per pixel 8,16,24,32
	unsigned char  descriptor;         // image descriptor bits (vh flip bits)

	// pixel data follows header
} TGA_HEADER;
#pragma pack(pop)

void Image::read(const std::string& fileName) {
	if (endsWith(fileName, ".tga")) {
		readTga(fileName);
	} else if (endsWith(fileName, ".hdr")) {
		readHdr(fileName);
	}
}


void Image::readHdr(const std::string& fileName) {
	std::string name = fileName;	
	std::string ext = ".hdr";

	if (!endsWith(name, ext)) {
		name += ext;
	}

	std::ifstream stream(name.c_str(), std::ios::binary);
	assert(stream.is_open());
	if(!stream.is_open()) return;
	TGA_HEADER header;
	stream.read((char *)(&header), sizeof(TGA_HEADER));
	assert(header.width <= 4096 && header.height <= 4096 && header.imagetype == 2 && header.bits == 72);
	clear();
	mWidth = header.width;
	mHeight = header.height;
	
	
	int sizeImg = mWidth*mHeight;
	char *rawData = new char[sizeImg * 12];
	stream.read(rawData, sizeImg * 12);	
	
	mData = new float[sizeof(float)* 3 * sizeImg];
	for(int i = 0; i < sizeImg; i++) {
		
		int pixelPos = i*3;
				

		for (int j = 0; j < 3; j++) {

			int bytePos = (pixelPos+j)*4;
			char bytes[4] = {rawData[bytePos], 
							 rawData[bytePos+1],
							 rawData[bytePos+2],
							 rawData[bytePos+3]};

			memcpy(&mData[pixelPos+2-j], bytes, sizeof(float));
			mData[pixelPos+2-j] /= 255.0;
		}
	}

	stream.close();
	delete [] rawData;
}

void Image::readTga(const std::string& fileName) {
	std::string name = fileName;	
	std::string ext = ".tga";

	if (!endsWith(name, ext)) {
		name += ext;
	}

	std::ifstream stream(name.c_str(), std::ios::binary);
	assert(stream.is_open());
	if(!stream.is_open()) return;
	TGA_HEADER header;
	stream.read((char *)(&header), sizeof(TGA_HEADER));
	assert(header.width <= 4096 && header.height <= 4096 && header.imagetype == 2 && header.bits == 24);
	clear();
	mWidth = header.width;
	mHeight = header.height;
	
	
	int sizeImg = mWidth*mHeight;
	char *rawData = new char[sizeImg*3];
	stream.read(rawData, sizeImg*3);	

	mData = new float[sizeof(float)* 3 * sizeImg];

	for(int i = 0; i < sizeImg; i++) {
		int pixelPos = i*3;		

		mData[pixelPos] = (float) (rawData[pixelPos + 2] & 0xFF) / 255.0f;
		mData[pixelPos + 1] = (float) (rawData[pixelPos + 1] & 0xFF) / 255.0f;
		mData[pixelPos + 2] = (float) (rawData[pixelPos] & 0xFF) / 255.0f;
				
	}

	stream.close();
	delete [] rawData;
}



void Image::writeHdr(const std::string& fileName, void (*pixelFunction)(float*, float*, float*)) const {
	
	int sizeImg = mWidth*mHeight;
	
	std::string name = fileName;	
	std::string ext = ".hdr";

	if (!endsWith(name, ext)) {
		name += ext;
	}

	std::ofstream stream(name.c_str(), std::ios::binary);
	assert(stream.is_open());
	if(!stream.is_open()) return;
	TGA_HEADER header;
	header.identsize = 0;          // size of ID field that follows 18 byte header (0 usually)
	header.colourmaptype = 0;      // type of colour map 0=none, 1=has palette
	header.imagetype = 2;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
	header.colourmapstart = 0;     // first colour map entry in palette
	header.colourmaplength = 0;    // number of colours in palette
	header.colourmapbits=0;        // number of bits per palette entry 15,16,24,32
	header.xstart = 0;             // image x origin
	header.ystart = 0;             // image y origin
	header.width = (short)mWidth;         // image width in pixels
	header.height = (short)mHeight;       // image height in pixels
	header.bits = 72;              // image bits per pixel 8,16,24,32
	header.descriptor = 0;         // image descriptor bits (vh flip bits)
	stream.write((char*)(&header), sizeof(TGA_HEADER));
		
	char * rawData = new char[sizeImg * 12];
	for(int i = 0; i < sizeImg; i++) {
		int pixelPos = i*3;		

		//float red = mData[pos];
		//mData[pos] = mData[pos + 2];
		//mData[pos + 2] = red;

		float pixel[3] = {mData[pixelPos], mData[pixelPos + 1], mData[pixelPos + 2]};

		pixelFunction(&pixel[0], &pixel[1], &pixel[2]);

		for (int j = 0; j < 3; j++) {
			char bytes[4];

			pixel[2-j] *= 255.0f;
			memcpy(bytes, &pixel[2-j], sizeof(float));

			unsigned bytePos = (pixelPos+j) * 4;
			rawData[bytePos] = bytes[0];
			rawData[bytePos+1] = bytes[1];
			rawData[bytePos+2] = bytes[2];
			rawData[bytePos+3] = bytes[3];
		}
	}
	
	stream.write(rawData, sizeImg * 12);
	stream.close();
	delete[] rawData;
}

void Image::writeTga(const std::string& fileName) const {
	
	int sizeImg = mWidth*mHeight;
	
	std::string name = fileName;	
	std::string ext = ".tga";

	if (!endsWith(name, ext)) {
		name += ext;
	}
		
	std::ofstream stream(name.c_str(), std::ios::binary);
	assert(stream.is_open());
	if(!stream.is_open()) return;
	TGA_HEADER header;
	header.identsize = 0;          // size of ID field that follows 18 byte header (0 usually)
	header.colourmaptype = 0;      // type of colour map 0=none, 1=has palette
	header.imagetype = 2;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
	header.colourmapstart = 0;     // first colour map entry in palette
	header.colourmaplength = 0;    // number of colours in palette
	header.colourmapbits=0;        // number of bits per palette entry 15,16,24,32
	header.xstart = 0;             // image x origin
	header.ystart = 0;             // image y origin
	header.width = (short)mWidth;         // image width in pixels
	header.height = (short)mHeight;       // image height in pixels
	header.bits = 24;              // image bits per pixel 8,16,24,32
	header.descriptor = 0;         // image descriptor bits (vh flip bits)
	stream.write((char*)(&header), sizeof(TGA_HEADER));
	
	char * rawData = new char[sizeImg*3];

	for(int i = 0; i < sizeImg; i++) 	{
		int pixelPos = i*3;
		
		rawData[pixelPos] = (char)(mData[pixelPos + 2] * 255.0f);
		rawData[pixelPos + 1] = (char)(mData[pixelPos + 1] * 255.0f);
		rawData[pixelPos+ 2] = (char)(mData[pixelPos] * 255.0f);
	}
	stream.write(rawData, sizeImg*3);
	stream.close();
	delete [] rawData;
}


bool Image::endsWith(const std::string& fullString, const std::string& ending) const {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void Image::clear() {
	if (mWidth != 0 && mHeight != 0) {
		delete [] mData;
	}
	mWidth = 0;
	mHeight= 0;	
}
