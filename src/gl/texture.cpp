//=============================================================================
//                                                                            
//   Exercise code for Introduction to Computer Graphics
//   LGG - EPFL
//                                                                            
//=============================================================================
//=============================================================================
//
//   CLASS Texture - IMPLEMENTATION
//
//=============================================================================
#include "texture.h"
#include "../utils/Image.h"

#include <fstream>
#include <cassert>

Texture::Texture() : id_(0), layer_(0), width_(0), height_(0) {
}

Texture::Texture(unsigned int _width, 
				 unsigned int _height, 
				 unsigned int _internalFormat, 
				 unsigned int _format,
				 unsigned int _type) : id_(0), layer_(0), width_(0), height_(0) {
	create(_width, _height, _internalFormat, _format, _type, NULL);
}

Texture::~Texture() {
	clear();
}

void Texture::create(unsigned int _width, 
					 unsigned int _height, 
					 unsigned int _internalFormat, 
					 unsigned int _format,
					 unsigned int _type,
					 void * _data,
					 int _param) {
	assert(_width <= 4096 && _height <= 4096);
	clear();
	width_ = _width;
	height_ = _height;
	glGenTextures(1, &id_);
	assert(id_ != 0);
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, width_, height_, 0, _format, _type, _data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0); 
}


void Texture::create(const std::string& _fileName) {
	Image image;
	image.read(_fileName);

	width_ = image.getWidth();
	height_ = image.getHeight();

	glGenTextures(1, &id_);
	assert(id_ != 0);
	glBindTexture(GL_TEXTURE_2D, id_);
	// NOTE: Change here, update here to LOAD HDR textures
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width_, height_, 0, GL_RGB, GL_FLOAT, image.getData());
	glGenerateMipmapEXT(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0); 	
}

///////////////////////////////////////////////////////////////////////////
void Texture::write(const std::string& _fileName) const {
	assert(id_ != 0);
	glBindTexture(GL_TEXTURE_2D, id_);
	
	int sizeImg = width_*height_;
	float *data = new float[sizeImg*3];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, data);

	Image image(data, width_, height_);
	image.writeTga(_fileName);
	
	glBindTexture(GL_TEXTURE_2D, 0); 
}
///////////////////////////////////////////////////////////////////////////
void Texture::bind() const
{
	assert(id_ != 0);
	glEnable(GL_TEXTURE_2D); 
	glActiveTextureARB(GL_TEXTURE0_ARB+layer_); 
	glBindTexture(GL_TEXTURE_2D, id_);
}
///////////////////////////////////////////////////////////////////////////
void Texture::unbind() const
{
	assert(id_ != 0);
	glActiveTextureARB(GL_TEXTURE0_ARB+layer_); 
	glBindTexture(GL_TEXTURE_2D, 0); 
	glDisable(GL_TEXTURE_2D);
}
///////////////////////////////////////////////////////////////////////////
void Texture::setLayer(unsigned int _layer)
{
	layer_ = _layer;
}
///////////////////////////////////////////////////////////////////////////
unsigned int Texture::getLayer() const
{
	return layer_;
}
///////////////////////////////////////////////////////////////////////////
unsigned int Texture::getID() const
{
	return id_;
}
///////////////////////////////////////////////////////////////////////////
void Texture::clear()
{
	if(id_ != 0)
	{
		glDeleteTextures(1, &id_);
		id_ = 0;
		layer_ = 0;
		width_ = 0;
		height_ = 0;
	}
}
///////////////////////////////////////////////////////////////////////////