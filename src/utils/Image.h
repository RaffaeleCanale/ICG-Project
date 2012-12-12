
#include <string>
//#include "../gl/gl.h"
class Image {
public:
	Image();
	Image(float * data, unsigned int width, unsigned int height) {
		mData = data;
		mWidth = width;
		mHeight = height;
	}
	~Image();
		
	void readTga(const std::string& fileName);
	void readHdr(const std::string& fileName);
	
	void writeHdr(const std::string& fileName, void (*pixelFunction)(float*, float*, float*)) const;
	void writeTga(const std::string& fileName) const;
	
	void read(const std::string& fileName);

	unsigned int getWidth() { return mWidth; }
	unsigned int getHeight() { return mHeight; }

	float * getData() { return mData; }

	

private:
	void clear();
	
	bool endsWith(const std::string& fullString, const std::string& ending) const;
	

	unsigned int mWidth;
	unsigned int mHeight;

	float * mData;
};
