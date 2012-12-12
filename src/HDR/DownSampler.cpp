
#include "DownSampler.h"
#include "math.h"

DownSampler::DownSampler() {
}

DownSampler::~DownSampler(){
}
                        
void DownSampler::init(int w, int h) {
			
	// Create FrameBuffer Objects
	
	for (size_t i = 0 ; i < DOWNSAMPLE_PASSES; i++) {
		w = ceil(w/2.0f);
		h = ceil(h/2.0f);
		mFbos[i] = new FrameBufferObject();				
		mFbos[i]->create(w, h);
		//mFbos[i].attachTexture(GL_COLOR_ATTACHMENT0_EXT, mRenderTextures[i].getLayer());
	}
}

void DownSampler::release() {
	for (size_t i=0 ; i < DOWNSAMPLE_PASSES ; ++i) {
		delete mFbos[i];
	}
}


void DownSampler::downsample(FrameBufferObject * fbo, Shader * shader) {
	//assert(Math::ceil(mainRt->getW()/2.0f) == renderTextures[0]->getW());
	//assert(Math::ceil(mainRt->getH()/2.0f) == renderTextures[0]->getH());

	
	// Set global state
	glEnable(GL_TEXTURE_2D);

	// Generate all down-sampled render texture
	for (int i=0 ; i < DOWNSAMPLE_PASSES ; i++)	{
		FrameBufferObject *curRt = mFbos[i];
		FrameBufferObject *prevRt = (i==0 ? fbo : mFbos[i-1]);
		//FrameBufferObject *prevRt = &mFbos[i-1];

		float w = curRt->getHeight();
		float h = curRt->getWidth();

		// Render on current fbo
		//curRt->bind();
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

		// Set viewport and ortho projection
		
		glViewport(0, 0, w, h);
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0, 1.0);
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();//*/

		// Use previous render texture as input texture
		//prevRt->bind(GL_COLOR_ATTACHMENT0_EXT);

		// User shader to render
		if (shader)	{
			shader->bind();
			shader->setFloatUniform("dx", 1.0/w);
			shader->setFloatUniform("dy", 1.0/h);
		}

		// Draw quad on the screen
		//glColor3fv(Color::WHITE);
		glBegin(GL_QUADS); 
			glTexCoord2i(0, 0);	glVertex2i(0, 0);
			glTexCoord2i(0, 1);	glVertex2i(0, 1);
			glTexCoord2i(1 ,1);	glVertex2i(1, 1);
			glTexCoord2i(1, 0);	glVertex2i(1, 0);
		glEnd();

		// Restore shader
		if (shader)
			shader->unbind();
	}

	// Restore state
	//OpenGLFBO::endCapture();
}//*/
