#include "../gl/shader.h"
#include "../gl/texture.h"
#include "../gl/fbo.h"

#define DOWNSAMPLE_PASSES 1

class DownSampler {
public:
	 DownSampler();
	~DownSampler();

	// Create the downsampler
	void init(int w, int h);

	// Free memory
	void release();

	// Downsample mainRt using the shader
	void downsample(FrameBufferObject * fbo, Shader *shader);


	
	// Get a downsampled render texture
	FrameBufferObject * getRenderTexture(int n) const { return mFbos[n]; }


private:
	FrameBufferObject * mFbos[DOWNSAMPLE_PASSES];
};

//*/