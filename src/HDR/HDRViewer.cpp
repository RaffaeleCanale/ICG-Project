//=============================================================================
//                                                                            
//   Exercise code for the lecture
//                                                                            
//=============================================================================
//=============================================================================
//
//  CLASS HDRViewer - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "HDRViewer.h"
#include "../utils/Mesh3DReader.h"

//== IMPLEMENTATION ========================================================== 


HDRViewer::HDRViewer(const char* _title, int _width, int _height): TrackballViewer(_title, _width, _height){
  init();
}


//-----------------------------------------------------------------------------


void HDRViewer::init(){
	// initialize super
	TrackballViewer::init();

	// set camera to look at world coordinate center
	set_scene_pos(Vector3(0.0, 0.0, 0.0), 2.0);

	// load shaders
	mDiffuseShader.create("Shaders/diffuse.vs", "Shaders/diffuse.fs");
	mSimpleShader.create("Shaders/texture.vs", "Shaders/texture.fs");
	mExtractBloomShader.create("Shaders/bloom.vs", "Shaders/bloom.fs");
	mBlendShader.create("Shaders/ToneMappingAndBlend.vs", "Shaders/ToneMappingAndBlend.fs");
	mBlurShader.create("Shaders/blur.vs", "Shaders/blur.fs");
	mNullShader.create("Shaders/null.vs", "Shaders/null.fs");

	mBlurShader1.create("Shaders/blur1.vs", "Shaders/blur1.fs");
	mBlurShader2.create("Shaders/blur2.vs", "Shaders/blur2.fs");


	mExposure = 1.0;

	mDownSampleFactor = 3.125; // 256x256
	//mDownSampleFactor = 1.5625;
	mDownSampleFactor = 4;
	//mDownSampleFactor = 1; //tmp
	mBlurScale = 1.0;
	mBlurStrength = 1.0;
	mBlurSize = 10.0;

	mInUseBlurShader = &mBlurShader1;
	mBlurAlgorithmChoice = 1.0;
}



//-----------------------------------------------------------------------------


void HDRViewer::reshape(int _w, int _h) {
	TrackballViewer::reshape(_w,_h);

	mHdrBuffer.create(_w, _h, true);
	mBloomBuffer.create(_w / mDownSampleFactor, _h/mDownSampleFactor, false);

	mHdrTexture.create(_w, _h, GL_RGBA16F_ARB, GL_RGB, GL_FLOAT);
	mBloomTexture.create(_w/mDownSampleFactor, _h/mDownSampleFactor, GL_RGBA16F_ARB, GL_RGB, GL_FLOAT);
	//mDownSampledTexture.create(_w/mDownSamplePasses, _h/mDownSamplePasses, GL_RGB16F_ARB, GL_RGB, GL_FLOAT);

	mHdrBuffer.attachTexture(GL_COLOR_ATTACHMENT0_EXT, mHdrTexture.getID());
	mBloomBuffer.attachTexture(GL_COLOR_ATTACHMENT0_EXT, mBloomTexture.getID());
	//mDownSampleBuffer.attachTexture(GL_COLOR_ATTACHMENT0_EXT, mDownSampledTexture.getID());
}


//-----------------------------------------------------------------------------
// Unused
void HDRViewer::loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL) {
	// load mesh from obj into m_mesh
	Mesh3DReader::read( filenameOBJ, m_planet, filenameMTL);

	// calculate normals
	m_planet.calculateVertexNormals();
	
	// get bounding box & reset scene camera accordingly
	Vector3 bbmin, bbmax;
	m_planet.calculateBoundingBox(bbmin, bbmax);
	
	double radius = 0.5*(bbmin - bbmax).length();
	Vector3 center = 0.5*(bbmin + bbmax);
	
	set_scene_pos(center, radius);
}

void HDRViewer::buildSolarSystem() {

	const char * planetName = "..\\..\\data\\planets\\mars.obj";
	const char * planetName2 = "..\\..\\data\\planets\\jupiter.obj";
	const char * sunName = "..\\..\\data\\sun\\sun.obj";

	double sunScale = 200.0;
	double planetScale = 50.0;
	double planetTranslate = 2000.0;

	// SUN -------
	Mesh3DReader::read(sunName, m_sun);

	if(m_sun.hasNormals()) {
		m_sun.calculateVertexNormals();
	}

	//m_sun.setIdentity();
	m_sun.translateObject(Vector3(planetTranslate, 0.0, 0.0));
	
	//m_light.translateObject(m_light.origin() - m_sun.origin());

	m_sun.scaleObject(Vector3(sunScale, sunScale, sunScale));
	
	m_light.translateObject(Vector3(planetTranslate, 0.0, 0.0));
	m_lightColor = Vector3(1.0, 1.0, 1.0);

	// PLANET -----
	Mesh3DReader::read(planetName, m_planet);

	if (m_planet.hasNormals()) {
		m_planet.calculateVertexNormals();
	}
		
	m_planet.scaleObject(Vector3(planetScale, planetScale, planetScale));


	// PLANET2 -----
	Mesh3DReader::read(planetName2, m_planet2);

	if (m_planet2.hasNormals()) {
		m_planet2.calculateVertexNormals();
	}
		
	double planetScale2 = planetScale * 1.5;
	double planetTranslate2 = planetTranslate * .5;
	m_planet2.translateObject(Vector3(planetTranslate2, 0.0, planetTranslate2));
	m_planet2.scaleObject(Vector3(planetScale2, planetScale2, planetScale2));
	

	Vector3 bbmin, bbmax;
	m_sun.calculateBoundingBox(bbmin, bbmax);
	
	double radius = 0.5*(bbmin - bbmax).length();
	Vector3 center = 0.5*(bbmin + bbmax);
	
	radius = planetTranslate * 1.0;
	set_scene_pos(center, radius);
	//set_scene_pos(Vector3(0.0, 0.0, -1000.0), planetTranslate * 3.3);
	//set_scene_pos(center, planetTranslate * 3.3);

	
}



//-----------------------------------------------------------------------------


void HDRViewer::keyboard(int key, int x, int y) {
	int modifiers = glutGetModifiers();

	float delta = .1;
	
	int realKey = key;
	bool shift = false;
	bool ctrl = false;

	if (modifiers == GLUT_ACTIVE_CTRL) {
		realKey += 96;
		ctrl = true;
	} else if (modifiers == GLUT_ACTIVE_SHIFT) {
		realKey += 32;
		shift = true;
	} else if (modifiers == GLUT_ACTIVE_SHIFT + GLUT_ACTIVE_CTRL) {
		realKey += 96;
		shift = true;
		ctrl = true;
	}

	switch (realKey) {
		case 'h':
			printf("Help:\n");
			printf("'h'\t-\tHelp\n");
			printf("'e'\t-\tChange exposure\n");
			printf("'s'\t-\tChange blur scale\n");
			printf("'b'\t-\tChange blur strength\n");
			printf("'k'\t-\tChange blur size\n");
			printf("'a'\t-\tChange blur algorithm\n");
			break;

		case 'e':
			interpolateParameter(&mExposure, 0.0, 2.0, 0.1, ctrl, shift, false);
			printf("Exposure; %f\n", mExposure);
			break;

		case 'd':
			delta = ctrl ? mDownSampleFactor/2.0 : mDownSampleFactor;
			//interpolateParameter(&mDownSampleFactor, 1.0, 12.5, delta, ctrl, shift);

			//mBloomTexture.create(width_/mDownSampleFactor, height_/mDownSampleFactor, GL_RGBA16F_ARB, GL_RGB, GL_FLOAT);
			//printf("Down sample factor: %f\n\t%fx%f\n", mDownSampleFactor, height_/mDownSampleFactor, width_/mDownSampleFactor);
			printf("Not implemented yet");
			break;

		case 's':
			interpolateParameter(&mBlurScale, 1.0, 200.0, mBlurAlgorithmChoice == 1 ? 1.0 : 10.0, ctrl, shift, false);
			printf("Blur scale: %f\n", mBlurScale);
			break;

		case 'b':
			interpolateParameter(&mBlurStrength, 0.0, 1.0, 0.1, ctrl, shift, false);
			printf("Blur strength: %f\n", mBlurStrength);
			break;

		case 'k':
			interpolateParameter(&mBlurSize, 1.0, 30.0, 1.0, ctrl, false, true);			
			
			if (mBlurAlgorithmChoice == 1.0 && mBlurSize >= 20.0) {
				printf("Blur kernel size: %f (!)\n", mBlurSize);
			} else {
				printf("Blur kernel size: %f\n", mBlurSize);
			}
			if (!ctrl && mBlurSize == 20.0 && mBlurAlgorithmChoice == 1.0) {
				printf("WARNING: Blur kernel size too high, performance may drop!\n");
			}
			
			break;

		case 'a':
			interpolateParameter(&mBlurAlgorithmChoice, 1.0, 2.0, 1.0, ctrl, false, true);
			switch ((int) mBlurAlgorithmChoice) {
				case 1:
					mInUseBlurShader = &mBlurShader1;
					break;
				case 2:
					mInUseBlurShader = &mBlurShader2;
					break;
				default:
					break;
			}
			printf("Using blur algorithm %f\n", mBlurAlgorithmChoice);
			break;

		case '2':
			mInUseBlurShader = &mBlurShader2;
			mBlurAlgorithmChoice = 2.0;
			mBlurScale = 20.0;
			mBlurStrength = 0.9;
			mBlurSize = 10.0;

			printf("Using blur algorithm 2 with:\n");
			printf("Blur kernel size:\t%f\n", mBlurSize);
			printf("Blur strength:\t%f\n", mBlurStrength);
			printf("Blur scale:\t%f\n", mBlurScale);
			break;
		default:
			TrackballViewer::keyboard(key, x, y);
			break;
	}
	
	glutPostRedisplay();
}

void HDRViewer::interpolateParameter(float * parameter, float min, float max, float delta, bool isCtrlPressed, bool isShiftPressed, bool cyclic) {
	if (isShiftPressed) {
		delta /= 10.0;
	}
	if (isCtrlPressed) {
		delta = -delta;
	}
	*parameter += delta;
	if (*parameter < min) {
		*parameter = cyclic ? max : min;
	} else if (*parameter > max) {
		*parameter = cyclic ? min : max;
	}
}

//-----------------------------------------------------------------------------
void HDRViewer::draw_scene(DrawMode _draw_mode) {

	//*
	mHdrBuffer.bind(GL_COLOR_ATTACHMENT0_EXT);
		draw_elements();
	mHdrBuffer.unbind();
	//*/

	//*
	mBloomBuffer.bind(GL_COLOR_ATTACHMENT0_EXT);
		extractBrightAreas();
	mBloomBuffer.unbind();
	//*/
	
	//*
	mBloomBuffer.bind(GL_COLOR_ATTACHMENT0_EXT);
		renderBlur(1.0, 0.0);
		renderBlur(0.0, 1.0);
	mBloomBuffer.unbind();
	//*/
		
	//*
	blend();
	//*/
}

void HDRViewer::draw_elements() {

	// clear screen
	//glEnable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4 cameraInverse = m_camera.getTransformation().Inverse();
	
	Vector3 lightPosInCamera = cameraInverse * m_light.origin();
	
	mDiffuseShader.bind();
	//mHdrTexture.setLayer(0);
	mHdrTexture.bind();
	
	
	mDiffuseShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	
	mDiffuseShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	mDiffuseShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());

	mDiffuseShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z );
	mDiffuseShader.setVector4Uniform("lightcolor", m_lightColor.x, m_lightColor.y, m_lightColor.z, 1.0);

	// Draw mesh		
	draw_object(mDiffuseShader, m_planet, true, true);
	draw_object(mDiffuseShader, m_planet2, false, true);
	mDiffuseShader.unbind();

	mSimpleShader.bind();
	
	mSimpleShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	
	mSimpleShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	//m_LightShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());
	draw_object(mSimpleShader, m_sun, true, false);
	
	

	mSimpleShader.unbind();
	mHdrTexture.unbind();
}


void HDRViewer::extractBrightAreas() {
	glDisable(GL_DEPTH_TEST);	

	
	mHdrTexture.bind();

	mExtractBloomShader.bind();
		
	mExtractBloomShader.setIntUniform("texture", mHdrTexture.getLayer());
	mExtractBloomShader.setFloatUniform("treshold", 7);
	
	
	renderCustomScreenQuad();

	
	mExtractBloomShader.unbind();
	mBloomTexture.bind();
}

void HDRViewer::renderBlur(float dx, float dy) {
	
	mBloomTexture.bind();
	

	mInUseBlurShader->bind();
	mInUseBlurShader->setIntUniform("texture", mBloomTexture.getLayer());	
	mInUseBlurShader->setVector2Uniform("orientation", dx*mBlurScale/width_, dy*mBlurScale/height_);
	mInUseBlurShader->setIntUniform("blurAmount", (int)mBlurSize);

	if (mBlurAlgorithmChoice == 1) {		
		mInUseBlurShader->setFloatUniform("blurStrength", mBlurStrength);

	} else if (mBlurAlgorithmChoice == 2) {
		//float samples[10] = {-0.08, -0.05, -0.03, -0.02, -0.01, 0.01, 0.02, 0.03, 0.05, 0.08};
		float * samples = new float[(int) mBlurSize];
		float blurStart = - mBlurStrength;
		float blurWidth = -2.0 * blurStart;
		for (int i = 0; i < mBlurSize; i++) {
			samples[i] = blurStart + blurWidth*(float(i)/(mBlurSize-1.0));
		}

		mInUseBlurShader->setFloatArray("samples", samples, 10);		
	}
	
	renderCustomScreenQuad();
	
	mInUseBlurShader->unbind();
}

void HDRViewer::blend() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	
	mBlendShader.bind();
	mBlendShader.setFloatUniform("exposure", mExposure);
	mBlendShader.setFloatUniform("brightThreshold", 1.2f);

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
	mDownSampledTexture.setLayer(1);
	mDownSampledTexture.bind();
	mBlendShader.setIntUniform("bloom", mDownSampledTexture.getLayer());//*/

	//mBloomTexture.write("Tests\\bloom.tga");

	renderFullScreenQuad();

	mBlendShader.unbind();
}


void HDRViewer::draw_object(Shader& sh, Mesh3D& mesh, bool showTexture, bool useModelWorlNormal) {
		
	sh.setMatrix4x4Uniform("modelworld", mesh.getTransformation() );
	if (useModelWorlNormal) {		
		sh.setMatrix3x3Uniform("modelworldNormal", mesh.getTransformation().Inverse().Transpose());
	}
			
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if(showTexture) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
			
	// NOTE: Use floats here!
	glVertexPointer( 3, GL_DOUBLE, 0, mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, mesh.getNormalPointer() );
	if(showTexture) {
		glTexCoordPointer( 2, GL_DOUBLE, 0, mesh.getUvTextureCoordPointer() );
	}	

	for(unsigned int i = 0; i < mesh.getNumberOfParts(); i++)	{
		bool hasTexture = showTexture && mesh.hasUvTextureCoord() && mesh.getMaterial(i).hasDiffuseTexture();

		sh.setIntUniform("useTexture", hasTexture);
		//sh.setFloatUniform("specularExp", mesh.getMaterial(i).m_specularExp);

		sh.setVector4Uniform("diffuseColor", 
							 mesh.getMaterial(i).m_diffuseColor.x, 
							 mesh.getMaterial(i).m_diffuseColor.y, 
							 mesh.getMaterial(i).m_diffuseColor.z, 1.0 );

		if(hasTexture) {
			mesh.getMaterial(i).m_diffuseTexture.bind();
			sh.setIntUniform("texture", mesh.getMaterial(i).m_diffuseTexture.getLayer());			
		}
		glDrawElements( GL_TRIANGLES, mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, mesh.getVertexIndicesPointer(i) );
		if(hasTexture) {
			mesh.getMaterial(i).m_diffuseTexture.unbind();
		}
	}
			
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	if(showTexture) {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
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

void HDRViewer::renderCustomScreenQuad() {
	// render full screen quad (note that vertex coordinates are already in opengl coordinates, so no transformation required!)
	float vertexA = 1.0f;
	float vertexB = -1.0f;
	float texCoordA = mDownSampleFactor;
	float texCoordB = 0.0f;
	glBegin(GL_QUADS); 
		glTexCoord2f(texCoordA, texCoordA);	glVertex2f(vertexA, vertexA);
		glTexCoord2f(texCoordA, texCoordB);	glVertex2f(vertexA, vertexB);
		glTexCoord2f(texCoordB ,texCoordB);	glVertex2f(vertexB, vertexB);
		glTexCoord2f(texCoordB, texCoordA);	glVertex2f(vertexB, vertexA);
	glEnd();
}