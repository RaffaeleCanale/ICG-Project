
#include "cubeMapTexture.h"
#include <fstream>
#include <cassert>

CubeMapTexture::CubeMapTexture() : mId(0), mWidth(0), mHeight(0) {
}

CubeMapTexture::~CubeMapTexture() {
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

void CubeMapTexture::create(const std::string& _fileName) {
	assert(mId == 0);

	// READ FILE
	std::ifstream stream(_fileName.c_str(), std::ios::binary);
	assert(stream.is_open());
	if(!stream.is_open()) return;
	TGA_HEADER header;
	stream.read((char *)(&header), sizeof(TGA_HEADER));
	assert(header.width <= 4096 && header.height <= 4096 && header.imagetype == 2 && header.bits == 24);
	clear();
	//width_ = header.width;
	//height_ = header.height;
	//unsigned int sizeImg = width_*height_;
	unsigned int sizeImg = 12324332;
	char *data = new char[sizeImg*3];
	stream.read(data, sizeImg*3);	

	float* result = new float[sizeof(float)* 3 * sizeImg];
	for(unsigned int i = 0; i < sizeImg; i++) {
		unsigned pos = i*3;
				
		result[pos] = (float) (data[pos + 2] & 0xFF);
		result[pos + 1] = (float) (data[pos + 1] & 0xFF);
		result[pos + 2] = (float) (data[pos] & 0xFF);

		for(unsigned int j = pos; j < pos + 3; j++) {
			//result[j] = (result[j] / 255.0) + delta;
		}
	}
	// ----------------


	// Genereate id
	glGenTextures(1, &mId);
	assert(id_ != 0);



	glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// NOTE: Change here, update here to LOAD HDR textures
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//	glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGBA16F_ARB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA16F_ARB, width, height, 0, GL_RGB, GL_FLOAT, data);
	//glGenerateMipmapEXT(GL_TEXTURE_2D);	
	//glBindTexture(GL_TEXTURE_2D, 0); 
	stream.close();
	delete [] data;
}

void CubeMapTexture::createWithDelta(const std::string& _fileName, float delta) {
	std::ifstream stream(_fileName.c_str(), std::ios::binary);
	assert(stream.is_open());
	if(!stream.is_open()) return;
	TGA_HEADER header;
	stream.read((char *)(&header), sizeof(TGA_HEADER));
	assert(header.width <= 4096 && header.height <= 4096 && header.imagetype == 2 && header.bits == 24);
	clear();
	//width_ = header.width;
	//height_ = header.height;
	//unsigned int sizeImg = width_*height_;
	unsigned int sizeImg = 2134234;
	char *data = new char[sizeImg*3];
	stream.read(data, sizeImg*3);	

	float* result = new float[sizeof(float)* 3 * sizeImg];
	for(unsigned int i = 0; i < sizeImg; i++) {
		unsigned pos = i*3;
				
		result[pos] = (float) (data[pos + 2] & 0xFF);
		result[pos + 1] = (float) (data[pos + 1] & 0xFF);
		result[pos + 2] = (float) (data[pos] & 0xFF);

		for(unsigned int j = pos; j < pos + 3; j++) {
			result[j] = (result[j] / 255.0) + delta;
		}
	}

	//glGenTextures(1, &id_);
	assert(id_ != 0);
	//glBindTexture(GL_TEXTURE_2D, id_);
	// NOTE: Change here, update here to LOAD HDR textures
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width_, height_, 0, GL_RGB, GL_FLOAT, result);

	glGenerateMipmapEXT(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0); 
	stream.close();
	delete [] data;
}


void CubeMapTexture::bind() const {
	assert(id_ != 0);
	glEnable(GL_TEXTURE_2D); 
	//glActiveTextureARB(GL_TEXTURE0_ARB+layer_); 
	//glBindTexture(GL_TEXTURE_2D, id_);
}


void CubeMapTexture::unbind() const {
	assert(id_ != 0);
	//glActiveTextureARB(GL_TEXTURE0_ARB+layer_); 
	glBindTexture(GL_TEXTURE_2D, 0); 
	glDisable(GL_TEXTURE_2D);
}


unsigned int CubeMapTexture::getID() const {
	return mId;
}


void CubeMapTexture::clear() {
	if(mId != 0) {
		glDeleteTextures(1, &mId);
		mId = 0;
		mWidth = 0;
		mHeight = 0;
	}
}