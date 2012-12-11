
#include <string>
#include "gl.h"

class CubeMapTexture {
public:
	CubeMapTexture();	
	~CubeMapTexture();
		
	// Read RGB TGA file
	void create(const std::string& _fileName);
	void createWithDelta(const std::string& _fileName, float delta);
	
	void bind() const;
	void unbind() const;
			
	//Id of this texture
	unsigned int getID() const;

	unsigned int getWidth() { return mWidth; }
	unsigned int getHeight() { return mHeight; }
private:
	void clear();
	
	unsigned int mId;	
	unsigned int mWidth;
	unsigned int mHeight;
};
