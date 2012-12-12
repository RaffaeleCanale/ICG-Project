
#include <string>
#include "gl.h"
#include "../utils/Image.h"

class CubeMapTexture {
public:
	CubeMapTexture();	
	~CubeMapTexture();
		
	// Read RGB TGA file
	void create(const std::string& _dirName);
	
	void bind() const;
	void unbind() const;
			
	//Id of this texture
	unsigned int getID() const;

private:
	void readFile(std::string& fileName, float * data, float * width, float * height);

	void clear();
	
	unsigned int facesId[6];
	unsigned int mId;	
};
