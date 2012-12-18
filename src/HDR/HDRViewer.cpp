
#include "HDRViewer.h"

#define bloom mBloomEffect[mBloomConfig]
#define tone mTone[mToneConfig]
#define scatter mScatterEffect[mScatterConfig]


HDRViewer::HDRViewer(const char* _title, int _width, int _height): TrackballViewer(_title, _width, _height) {
  init();
}


void HDRViewer::init(){
	// initialize super
	TrackballViewer::init();

	// set camera to look at world coordinate center
	set_scene_pos(Vector3(0.0, 0.0, 0.0), 2.0);

	/********************
	 *** LOAD SHADERS ***
	 ********************/
	mExtractBloomShader.create("Shaders/bloom.vs", "Shaders/bloom.fs");
	mBlendShader.create("Shaders/ToneMappingAndBlend.vs", "Shaders/ToneMappingAndBlend.fs");
	mUniColorShader.create("Shaders/UniColor.vs", "Shaders/UniColor.fs");
	mScatterBlurShader.create("Shaders/scatterBlur.vs", "Shaders/scatterBlur.fs");

	mBlurShader[0].create("Shaders/blur1.vs", "Shaders/blur1.fs");
	mBlurShader[1].create("Shaders/blur2.vs", "Shaders/blur2.fs");
		
	/******************************
	 *** SET DEFAULT PARAMETERS ***
	 ******************************/
	// TONE MAPPING
	mTone[0].exposure = 1.0;	
	mTone[0].brightThreshold = 1.2f;
	mTone[0].textureRatio = 1.0;
	mTone[0].bloomRatio = 0.6;
	mTone[0].scatterRatio = 0.6;
	mToneConfig = 0;

	// BLOOM
	mBloomEffect[1].downSampleFactor = 4;
	mBloomEffect[1].blurScale = 1.0;
	mBloomEffect[1].blurStrength = 1.0;
	mBloomEffect[1].blurSize = 10.0;
	mBloomEffect[1].brightThreshold = 7.0;
	mBloomEffect[1].algorithmChoice = 0.0;

	mBloomEffect[2].downSampleFactor = 4;
	mBloomEffect[2].blurScale = 20.0;
	mBloomEffect[2].blurStrength = 0.9;
	mBloomEffect[2].blurSize = 10.0;
	mBloomEffect[2].brightThreshold = 7.0;
	mBloomEffect[2].algorithmChoice = 1.0;	

	mBloomConfig = 0;
	
	// LIGHT SCATTERING
	mScatterEffect[1].downSampleFactor = 4.0;
	mScatterEffect[1].decay = 0.89;
	mScatterEffect[1].density = 0.93;
	mScatterEffect[1].weight = 1.0;
	mScatterEffect[1].samples = 40;
	
	mScatterConfig = 0;

	// MENU
	mMenu.currentPage = -1;
	mMenu.cursor = 0;

	// ANIMATION
	mCurrentTime = -1.0;
	mAnimate = false;

	updateAndPrint(0);
}


void HDRViewer::reshape(int _w, int _h) {
	TrackballViewer::reshape(_w,_h);

	mHdrBuffer.create(_w, _h, true);
	mBloomBuffer.create(_w/bloom.downSampleFactor, _h/bloom.downSampleFactor, false);
	mScatterBuffer.create(_w/scatter.downSampleFactor, _h/scatter.downSampleFactor, false);

	mHdrTexture.create(_w, _h, GL_RGBA16F_ARB, GL_RGB, GL_FLOAT);
	mBloomTexture.create(_w/bloom.downSampleFactor, _h/bloom.downSampleFactor, GL_RGBA16F_ARB, GL_RGB, GL_FLOAT);
	mScatterTexture.create(_w, _h, GL_RGBA16F_ARB, GL_RGB, GL_FLOAT);
	mScatterDownsampledTexture.create(_w/scatter.downSampleFactor, _h/scatter.downSampleFactor, GL_RGBA16F_ARB, GL_RGB, GL_FLOAT);
	

	mHdrBuffer.attachTexture(GL_COLOR_ATTACHMENT0_EXT, mHdrTexture.getID());
	mHdrBuffer.attachTexture(GL_COLOR_ATTACHMENT1_EXT, mScatterTexture.getID());
	mBloomBuffer.attachTexture(GL_COLOR_ATTACHMENT0_EXT, mBloomTexture.getID());
	mScatterBuffer.attachTexture(GL_COLOR_ATTACHMENT0_EXT, mScatterDownsampledTexture.getID());
}


void HDRViewer::loadScene(SCENE_TYPE scene) {
	mScene = scene;

	mScene.loadShaders();

	Vector3 pos = Vector3(0.0, 0.0, 0.0);
	float radius = 0;
	mScene.buildScene(&pos, &radius);
	set_scene_pos(pos, radius);	

	mScene.initCamera(&m_camera);
}


void HDRViewer::idle() {
	if (mAnimate) {
		float prevTime = mCurrentTime;
		mCurrentTime = mWatch.stop();
		mScene.animate(mCurrentTime - prevTime);

		glutPostRedisplay();		
	}
}


void HDRViewer::keyboard(int key, int x, int y) {
	bool mainPage = mMenu.currentPage == -1;

	switch(key) {
		case 13:	//Enter
			if (mainPage) {
				mMenu.currentPage = mMenu.cursor;
				mMenu.cursor = 0;				
			}
			break;

		case 8:		//RETURN
		case 27:	//ESC
			if (mainPage) {
				exit(0);
			} else {
				mMenu.cursor = mMenu.currentPage;
				mMenu.currentPage = -1; //Return to main page				
			}
			break;

		case 32:	//SPACE
			if(mAnimate) {
				mWatch.stop();
				mCurrentTime = 0.0;
			} else {
				mWatch.start();
				if (mCurrentTime < 0.0) {
					// init mCurrentTime for the first time
					mCurrentTime = mWatch.stop();
				}
			}

			mAnimate = !mAnimate;
			break;

		default:
			TrackballViewer::keyboard(key, x, y);
			break;
	}

	updateAndPrint(0);
}

void HDRViewer::special(int key, int x, int y) {
	int modifiers = glutGetModifiers();
	
	float increment = 0.0;
	float delta = 1.0;

	if (modifiers == GLUT_ACTIVE_CTRL) {
		delta = 0.1;
	} else if (modifiers == GLUT_ACTIVE_SHIFT) {
		delta = 10.0;
	} else if (modifiers == GLUT_ACTIVE_SHIFT + GLUT_ACTIVE_CTRL) {
		delta = 100.0;
	}

	switch(key) {
		case GLUT_KEY_UP:
			mMenu.cursor = incrementParameter(mMenu.cursor, 0, mMenu.parametersCount-1, -1, true);
			break;

		case GLUT_KEY_DOWN:
			mMenu.cursor = incrementParameter(mMenu.cursor, 0, mMenu.parametersCount-1, 1, true);
			break;

		case GLUT_KEY_RIGHT:
			increment = delta;
			break;

		case GLUT_KEY_LEFT:
			increment = -delta;
			break;

		default:
			break;			
	}

	updateAndPrint(increment);
}

void HDRViewer::updateAndPrint(float increment) {
	for (int i = 0; i < 5; i++) {
		printf("\n\n\n\n\n");  //clear screen
	}

	mMenu.parametersCount = 0;
	bool needReshape = false; // Some parameters change will need that we call the reshape method in order to see changes

	switch(mMenu.currentPage) {
		case -1:  // Main page		
			mToneConfig = update(mToneConfig, 0.0, tone_param_count - 1.0, increment*1.0, true);
			printf(" Tone mapping\t\t[Setting %i]\n", mToneConfig);


			mBloomConfig = update(mBloomConfig, 0.0, bloom_param_count - 1.0, increment*1.0, true, &needReshape);
			if (mBloomConfig == 0) {
				printf(" Bloom effect\t\t[DISABLED]\n");
			} else {
				printf(" Bloom effect\t\t[Setting %i]\n", mBloomConfig);
			}


			mScatterConfig = update(mScatterConfig, 0.0, scatter_param_count - 1.0, increment*1.0, true, &needReshape);
			if (mScatterConfig == 0) {
				printf(" Light scattering\t[DISABLED]\n");
			} else {
				printf(" Light scattering\t[Setting %i]\n", mScatterConfig);
			}
			break;

		case 0:	// Tone mapping
			{
			printf("Tone mapping parameters:\n\n");

			tone.exposure = update(tone.exposure, 0.0, 2.0, increment*0.1, false);
			printf("\tExposure:       \t\t\t%f\n", tone.exposure);

			tone.brightThreshold = update(tone.brightThreshold, 0.0, 2.0, increment*0.1, false);
			printf("\tBright threshold: \t\t\t%f\n", tone.brightThreshold);

			tone.textureRatio = update(tone.textureRatio, 0.0, 10.0, increment*0.1, false);
			printf("\tTexture ratio in blending:\t\t%f\n", tone.textureRatio);

			tone.bloomRatio = update(tone.bloomRatio, 0.0, 10.0, increment*0.1, false);
			printf("\tBloom ratio in blending:\t\t%f\n", tone.bloomRatio);

			tone.scatterRatio = update(tone.scatterRatio, 0.0, 10.0, increment*0.1, false);
			printf("\tLight scattering ratio in blending:\t%f\n", tone.scatterRatio);

			Vector3 light = mScene.getLightIntensity();
			float min = min(light.x, min(light.y, light.z));
			float max = max(light.x, max(light.y, light.z));
			float delta = update(0.0, -min, 15.0 - max, 0.1*increment, false);
			mScene.setLightIntensity(light + Vector3(delta, delta, delta));
			light = mScene.getLightIntensity();
			printf("\tLight intensity: \t\t(%f, %f, %f)\n", light.x, light.y, light.z);
			break;
			}

		case 1:	// Bloom
			
			printf("Bloom parameters:\n\n");		
						
			bloom.algorithmChoice = update(bloom.algorithmChoice, 0.0, 1.0, increment*1.0, true);
			switch(bloom.algorithmChoice) {
				case 0:
					printf("\tBlur algorithm:    \t\tGaussian\n");
					break;
				case 1:
					printf("\tBlur algorithm:    \t\tLinear\n");
					break;
			}


			bloom.blurSize = update(bloom.blurSize, 1.0, 30.0, increment*1.0, false);
			printf("\tBlur kernel size: \t\t%i\n", bloom.blurSize);
			

			bloom.blurScale = update(bloom.blurScale, 1.0, 200.0, increment*1.0, false);
			printf("\tBlur range scale:  \t\t%f\n", bloom.blurScale);

			
			bloom.blurStrength = update(bloom.blurStrength, 0.0, 1.0, increment*0.1, false);
			printf("\tBlur strength:     \t\t%f\n", bloom.blurStrength);

			
			bloom.brightThreshold = update(bloom.brightThreshold, 0.0, 15.0, increment*1.0, false);			
			printf("\tBloom luminance threshold:\t%f\n", bloom.brightThreshold);

			bloom.downSampleFactor = update(bloom.downSampleFactor, 1.0, 16.0, increment*1.0, false, &needReshape);
			printf("\tBlur downsample factor:\t\t%f\n", bloom.downSampleFactor);			
			break;
			

		case 2:	// Light scattering
			printf("Light scattering parameters:\n\n");

			scatter.decay = update(scatter.decay, 0.0, 10.0, increment*0.1, false);
			printf("\tLight decay:    \t\t%f\n", scatter.decay);

			scatter.density = update(scatter.density, 0.0, 10.0, increment*0.1, false);
			printf("\tLight density:  \t\t%f\n", scatter.density);

			scatter.weight = update(scatter.weight, 0.0, 10.0, increment*0.1, false);
			printf("\tLight weight:    \t\t%f\n", scatter.weight);

			scatter.samples = update(scatter.samples, 0.0, 100.0, increment*1.0, false);
			printf("\tSamples:       \t\t%i\n", scatter.samples);

			break;
	}

	if (mMenu.currentPage == -1) {
		printf("\n\nLEFT/RIGHT arrows to modify preset\n");
		printf("ESC to go to exit\n");
	} else {
		printf("\n\nLEFT/RIGHT arrows to modify parameter\n");
		printf("CTRL/SHIFT to regulate the change delta\n");
		printf("ESC to return to parent\n");
	}
	if (mAnimate) {
		printf("SPACEBAR to stop animation\n");
	} else {
		printf("SPACEBAR to start animation\n");
	}
		

	if (needReshape) {
		reshape(width_, height_);
	}
	glutPostRedisplay();
}

float HDRViewer::update(float parameter, float min, float max, float delta, bool cyclic, bool * needReshape) {
	float result;
	
	if (mMenu.cursor == mMenu.parametersCount) {	// This is the parameter the user selected
		printf(">");
		result = incrementParameter(parameter, min, max, delta, cyclic);
		if (needReshape != NULL) {	//If a pointer has been passed, then it means that this parameter needs a reshape
			*needReshape = true;
		}
	} else {
		result = parameter;
	}

	mMenu.parametersCount++;	// Automatically counts the number of parameters to help display
	return result;
}

float HDRViewer::incrementParameter(float parameter, float min, float max, float delta, bool cyclic) {
	parameter += delta;

	if (parameter < min) {
		parameter = cyclic ? max : min;
	} else if (parameter > max) {
		parameter = cyclic ? min : max;
	}
	return parameter;
}

//-----------------------------------------------------------------------------
void HDRViewer::draw_scene(DrawMode _draw_mode) {
	mHdrBuffer.bind(GL_COLOR_ATTACHMENT0_EXT);
		//renderCubeMap();
		mScene.renderScene(&m_camera);
	mHdrBuffer.unbind();	
	
	mBloomBuffer.bind(GL_COLOR_ATTACHMENT0_EXT);
		if (mBloomConfig != 0) {
			extractBrightAreas();	
			renderBlur(1.0, 0.0);
			renderBlur(0.0, 1.0);	
		} else {		
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	mBloomBuffer.unbind();
	

	mHdrBuffer.bind(GL_COLOR_ATTACHMENT1_EXT);	
		if (mScatterConfig != 0) {	
			mScene.renderLight(&m_camera, &mUniColorShader);
		} else {
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	mHdrBuffer.unbind();
	
	mScatterBuffer.bind(GL_COLOR_ATTACHMENT0_EXT);
		if (mScatterConfig != 0) {
			renderScattering();	
		} else {
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	mScatterBuffer.unbind();		

	
	blend();	
}

void HDRViewer::generateShadowMap() {
	/*
	//First step: Render from the light POV to a FBO, story depth values only
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);	//Rendering offscreen
	
	//Using the fixed pipeline to render to the depthbuffer
	glUseProgramObjectARB(0);
	
	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0,0,width_ / shadow.downSampleFactor,height_ / shadow.downSampleFactor);
	
	// Clear previous frame values
	glClear( GL_DEPTH_BUFFER_BIT);
	
	//Disable color rendering, we only want to write to the Z-Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	mShadowCamera.setIdentity();
	mShadowCamera.setFOV(45.0);
	mShadowCamera.setSize(width_, height_);
	mShadowCamera.setClippingPlanes(10, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	double position_x = mScene.getLightPosition().x;
	double position_y = mScene.getLightPosition().y;
	double position_z = mScene.getLightPosition().z;
	double lookAt_x = 1.0;
	double lookAt_y = 0.0;
	double lookAt_z = 0.0;
	gluLookAt(position_x,position_y,position_z,lookAt_x,lookAt_y,lookAt_z,0,1,0);
	
	
	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	glCullFace(GL_FRONT);
	mScene.renderScene(mShadowCamera);
	
	//Save modelview/projection matrice into texture7, also add a biais
	static double modelView[16];
	static double projection[16];
	
	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5 
	// y = y* 0.5 + 0.5 
	// z = z* 0.5 + 0.5 
	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {	
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0};
	
	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	
	glLoadIdentity();	
	glLoadMatrixd(bias);
	
	// concatating all matrice into one.
	glMultMatrixd (projection);
	glMultMatrixd (modelView);
	
	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
	
	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	
	glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);
	
	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
	
	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the shadow shader
	glUseProgramObjectARB(shadowShaderId);
	glUniform1iARB(shadowMapUniform,7);
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D,depthTextureId);
	
	
	
		

	
	setupMatrices(p_camera[0],p_camera[1],p_camera[2],l_camera[0],l_camera[1],l_camera[2]);
	
	glCullFace(GL_BACK);
	drawObjects();
	//*/
}


void HDRViewer::renderCubeMap() {
	/*
	glBegin(GL_QUADS);
	float BOX_SIZE = height_;
	//Top face
	glColor3f(1.0f, 1.0f, 0.0f);
	glNormal3f(0.0, 1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
 
	//Bottom face
	glColor3f(1.0f, 0.0f, 1.0f);
	glNormal3f(0.0, -1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
 
	//Left face
	glNormal3f(-1.0, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
 
	glEnd();
 
 
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
 
	//Right face
	glNormal3f(1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
 
 
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
 
 
	//Back face
	glNormal3f(0.0, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
 
	glEnd();
	glDisable(GL_TEXTURE_2D);//*/

	// Activate environnment mapping
	//*
	glPushMatrix();
		glLoadIdentity();
		GLfloat s_plane[] = { 1.0, 0.0, 0.0, 0.0 };
		GLfloat t_plane[] = { 0.0, 1.0, 0.0, 0.0 };
		GLfloat r_plane[] = { 0.0, 0.0, 1.0, 0.0 };
		glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
		glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane);
		glTexGenfv(GL_R, GL_OBJECT_PLANE, r_plane);
    glPopMatrix();

	//*
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR); //*/
	
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	//mCubeMap.bind();

	// Draw a cube
	float s = 500.0f;

	glBegin(GL_QUADS); 
		glVertex3f(+s, -s, -s);
		glVertex3f(+s, -s, +s);
		glVertex3f(+s, +s, +s);
		glVertex3f(+s, +s, -s);

		glVertex3f(-s, -s, -s);
		glVertex3f(-s, +s, -s);
		glVertex3f(-s, +s, +s);
		glVertex3f(-s, -s, +s);

		glVertex3f(-s, +s, -s);
		glVertex3f(+s, +s, -s);
		glVertex3f(+s, +s, +s);
		glVertex3f(-s, +s, +s);

		glVertex3f(-s, -s, -s);
		glVertex3f(-s, -s, +s);
		glVertex3f(+s, -s, +s);
		glVertex3f(+s, -s, -s);

		glVertex3f(-s, -s, -s);
		glVertex3f(+s, -s, -s);
		glVertex3f(+s, +s, -s);
		glVertex3f(-s, +s, -s);

		glVertex3f(-s, -s, +s);
		glVertex3f(-s, +s, +s);
		glVertex3f(+s, +s, +s);
		glVertex3f(+s, -s, +s);
	glEnd();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP);//*/
}


void HDRViewer::extractBrightAreas() {
	glDisable(GL_DEPTH_TEST);	
		
	mHdrTexture.bind();
	mExtractBloomShader.bind();
		
	mExtractBloomShader.setIntUniform("texture", mHdrTexture.getLayer());
	mExtractBloomShader.setFloatUniform("treshold", bloom.brightThreshold);	
	
	renderCustomScreenQuad(bloom.downSampleFactor);
	
	mExtractBloomShader.unbind();
	mBloomTexture.bind();
}

void HDRViewer::renderBlur(float dx, float dy) {
	Shader * sh = &mBlurShader[bloom.algorithmChoice];

	mBloomTexture.bind();		
	sh->bind();

	sh->setIntUniform("texture", mBloomTexture.getLayer());	
	sh->setVector2Uniform("orientation", dx*bloom.blurScale/width_, 
											dy*bloom.blurScale/height_);
	sh->setIntUniform("blurAmount", bloom.blurSize);

	if (bloom.algorithmChoice == 0) {		
		sh->setFloatUniform("blurStrength", bloom.blurStrength);

	} else if (bloom.algorithmChoice == 1) {
		//float samples[10] = {-0.08, -0.05, -0.03, -0.02, -0.01, 0.01, 0.02, 0.03, 0.05, 0.08};
		float * samples = new float[bloom.blurSize];
		float blurStart = - bloom.blurStrength;
		float blurWidth = -2.0 * blurStart;
		for (int i = 0; i < bloom.blurSize; i++) {
			samples[i] = blurStart + blurWidth*(float(i)/(bloom.blurSize-1.0));
		}

		sh->setFloatArray("samples", samples, bloom.blurSize);		
	}
	
	renderCustomScreenQuad(bloom.downSampleFactor);
	
	sh->unbind();
}


void HDRViewer::renderScattering() {
	mScatterTexture.bind();
	
	Matrix4 cameraInverse = m_camera.getTransformation().Inverse();	
		
	Vector4 tmp = m_camera.getProjectionMatrix() * cameraInverse *
		Vector4(mScene.getLightPosition().x, mScene.getLightPosition().y, mScene.getLightPosition().z, 1.0);
	Vector3 lightPosInCamera = Vector3(tmp.x, tmp.y, tmp.z) / tmp.w;	
	Vector2 pos = Vector2(  ((lightPosInCamera.x + 1.0)/2.0),
									((lightPosInCamera.y + 1.0)/2.0));

	Shader * sh = &mScatterBlurShader;
	sh->bind();
	sh->setIntUniform("texture", mScatterTexture.getLayer());
	sh->setVector2Uniform("lightPos", pos.x, pos.y);
	sh->setFloatUniform("decay", scatter.decay);
	sh->setFloatUniform("density", scatter.density);
	sh->setFloatUniform("weight", scatter.weight);
	sh->setIntUniform("samples", scatter.samples);
	
	
	renderCustomScreenQuad(scatter.downSampleFactor);
	
	
	sh->unbind();
}

void HDRViewer::blend() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	
	mBlendShader.bind();
	mBlendShader.setFloatUniform("exposure", tone.exposure);
	mBlendShader.setFloatUniform("brightThreshold", tone.brightThreshold);
	mBlendShader.setFloatUniform("textureRatio", tone.textureRatio);
	mBlendShader.setFloatUniform("bloomRatio", tone.bloomRatio);
	mBlendShader.setFloatUniform("scatterRatio", tone.scatterRatio);

	//*
	mHdrTexture.setLayer(0);
	mHdrTexture.bind();
	mBlendShader.setIntUniform("texture", mHdrTexture.getLayer());//*/
	/*
	mBloomTexture.setLayer(0);
	mBloomTexture.bind();
	mBlendShader.setIntUniform("texture", mBloomTexture.getLayer());//*/

	//*
	mBloomTexture.setLayer(1);
	mBloomTexture.bind();
	mBlendShader.setIntUniform("bloom", mBloomTexture.getLayer());//*/
	/*
	mScatterTexture.setLayer(1);
	mScatterTexture.bind();
	mBlendShader.setIntUniform("bloom", mScatterTexture.getLayer());//*/

	//mBloomTexture.write("Tests\\bloom.tga");
	//*
	mScatterDownsampledTexture.setLayer(2);
	mScatterDownsampledTexture.bind();
	mBlendShader.setIntUniform("scatter", mScatterDownsampledTexture.getLayer());//*/

	renderFullScreenQuad();

	mBlendShader.unbind();
}





void HDRViewer::renderFullScreenQuad() {
	// render full screen quad (note that vertex coordinates are already in opengl coordinates, so no transformation required!)
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
}

void HDRViewer::renderCustomScreenQuad(float downSampleFactor) {
	// render full screen quad (note that vertex coordinates are already in opengl coordinates, so no transformation required!)
	float vertexA = 1.0f;
	float vertexB = -1.0f;
	float texCoordA = downSampleFactor;
	float texCoordB = 0.0f;
	glBegin(GL_QUADS); 
		glTexCoord2f(texCoordA, texCoordA);	glVertex2f(vertexA, vertexA);
		glTexCoord2f(texCoordA, texCoordB);	glVertex2f(vertexA, vertexB);
		glTexCoord2f(texCoordB ,texCoordB);	glVertex2f(vertexB, vertexB);
		glTexCoord2f(texCoordB, texCoordA);	glVertex2f(vertexB, vertexA);
	glEnd();
}