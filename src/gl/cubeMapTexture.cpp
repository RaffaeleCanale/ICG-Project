
#include "cubeMapTexture.h"
#include <fstream>
#include <cassert>

CubeMapTexture::CubeMapTexture() : mId(0) {
}

CubeMapTexture::~CubeMapTexture() {
	clear();
}

void CubeMapTexture::create(const std::string& dirName) {
	assert(mId == 0);

	
	// ----------------
	std::string names[6] = {"/right.tga", "/left.tga", 
							"/up.tga", "/down.tga",
							"/front.tga", "/back.tga"};
	Image images[6];
	for (int i = 0; i <= 5; i++) {
		images[i].readTga(dirName + names[i]);
	}

	// Genereate id
	glGenTextures(1, &mId);
	assert(mId != 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	GLenum sides[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
					   GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT,
					   GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT};
	for (int i = 0; i < 6; i++) {
		glTexImage2D(sides[i], 0, GL_RGB16F_ARB, images[i].getWidth(), images[i].getHeight(), 0, GL_RGB, GL_FLOAT, images[i].getData());	
	}
	//images[0].write("tesadf.tga");
	//glGenerateMipmapEXT(GL_TEXTURE_2D);	
	//glBindTexture(GL_TEXTURE_2D, 0); 
}

void CubeMapTexture::bind() const {
	assert(mId != 0);
	//glEnable(GL_TEXTURE_2D); 
	//glActiveTextureARB(GL_TEXTURE0_ARB+layer_); 
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, mId);
}


void CubeMapTexture::unbind() const {
	assert(mId != 0);
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
	}
}