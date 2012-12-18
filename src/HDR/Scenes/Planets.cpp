#include "Planets.h"
#include "../../utils/Mesh3DReader.h"

Planets::Planets() {
}

Planets::~Planets() {
}

void Planets::loadShaders() {
	mDiffuseShader.create("Shaders/diffuse.vs", "Shaders/diffuse.fs");
	mSimpleShader.create("Shaders/texture.vs", "Shaders/texture.fs");
}

void Planets::buildScene(Vector3 * pos, float * radius) {
	//mCubeMap.create("..\\..\\data\\stars\\cubeMap");

	const char * planetName = "..\\..\\data\\planets\\mars.obj";
	const char * planetName2 = "..\\..\\data\\planets\\jupiter.obj";
	//const char * planetName2 = "..\\..\\data\\Satellite\\sat.obj";
	const char * sunName = "..\\..\\data\\sun\\sun.obj";
	const char * starsName = "..\\..\\data\\stars\\stars.obj";

	double sunScale = 200.0;
	double planetScale = 50.0;
	double planetTranslate = 2000.0;
	double starsScale = 9000.0;

	// SUN -------
	Mesh3DReader::read(sunName, mSun);

	if(mSun.hasNormals()) {
		mSun.calculateVertexNormals();
	}

	//m_sun.setIdentity();
	mSun.translateObject(Vector3(planetTranslate, 0.0, 0.0));
	
	//m_light.translateObject(m_light.origin() - m_sun.origin());

	mSun.scaleObject(Vector3(sunScale, sunScale, sunScale));
	
	mLight.translateObject(Vector3(planetTranslate, 0.0, 0.0));
	mLightColor = Vector3(10.0, 10.0, 10.0);

	// PLANET -----
	Mesh3DReader::read(planetName, mPlanet);

	if (mPlanet.hasNormals()) {
		mPlanet.calculateVertexNormals();
	}
		
	mPlanet.scaleObject(Vector3(planetScale, planetScale, planetScale));


	// PLANET2 -----
	Mesh3DReader::read(planetName2, mPlanet2);

	if (mPlanet2.hasNormals()) {
		mPlanet2.calculateVertexNormals();
	}
		
	//double planetScale2 = planetScale * 1.5;
	double planetScale2 = planetScale * 0.2;
	double planetTranslate2 = planetTranslate * .5;
	mPlanet2.translateObject(Vector3(planetTranslate2, 0.0, planetTranslate2));
	mPlanet2.scaleObject(Vector3(planetScale2, planetScale2, planetScale2));
	

	Vector3 bbmin, bbmax;
	mSun.calculateBoundingBox(bbmin, bbmax);
	
	double r = 0.5*(bbmin - bbmax).length();
	Vector3 center = 0.5*(bbmin + bbmax);
	
	r = planetTranslate * 1.0;
	
	*pos = center;
	*radius = r;
	//set_scene_pos(Vector3(0.0, 0.0, -1000.0), planetTranslate * 3.3);
	//set_scene_pos(center, planetTranslate * 3.3);

	
	Mesh3DReader::read(starsName, mStars);
	if(!mStars.hasNormals()) {
			mStars.calculateVertexNormals();
	}
	mStars.scaleObject(Vector3(starsScale, starsScale, starsScale));
}

void Planets::renderScene(Camera3D * camera) {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4 cameraInverse = camera->getTransformation().Inverse();
	
	Vector3 lightPosInCamera = cameraInverse * mLight.origin();
	
	mDiffuseShader.bind();
	//mHdrTexture.setLayer(0);
	//mHdrTexture.bind();
			
	mDiffuseShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	
	mDiffuseShader.setMatrix4x4Uniform("projection", camera->getProjectionMatrix());
	mDiffuseShader.setMatrix3x3Uniform("worldcameraNormal", camera->getTransformation().Transpose());

	mDiffuseShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z );
	mDiffuseShader.setVector4Uniform("lightcolor", mLightColor.x, mLightColor.y, mLightColor.z, 1.0);

	// Draw mesh		
	drawObject(mDiffuseShader, mPlanet, true, true);
	drawObject(mDiffuseShader, mPlanet2, false, true);	
	
	mDiffuseShader.unbind();
		
	mSimpleShader.bind();
	
	mSimpleShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	
	mSimpleShader.setMatrix4x4Uniform("projection", camera->getProjectionMatrix());
	//m_LightShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());
	drawObject(mSimpleShader, mSun, true, false);
	
	drawObject(mSimpleShader, mStars, true, false);

	mSimpleShader.unbind();//*/	
}

void Planets::renderLight(Camera3D * camera, Shader * shader) {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//m_camera.setSize(width_/scatter.downSampleFactor, height_/scatter.downSampleFactor);
	Matrix4 cameraInverse = camera->getTransformation().Inverse();	
	
	Vector3 lightPosInCamera = cameraInverse * mLight.origin();
	
	shader->bind();
	
	shader->setMatrix4x4Uniform("worldcamera", cameraInverse);	
	shader->setMatrix4x4Uniform("projection", camera->getProjectionMatrix());
	shader->setVector4Uniform("color", mLightColor.x, mLightColor.y, mLightColor.z, 1.0);
	//m_camera.setSize(width_, height_);

	drawObjectSimple(*shader, mSun, false);

	shader->setVector4Uniform("color", 0.0, 0.0, 0.0, 1.0);
	
	drawObjectSimple(*shader, mPlanet, false);
	drawObjectSimple(*shader, mPlanet2, false);
	
	
	shader->unbind();
}

Vector3 Planets::getLightPosition() {
	return mLight.origin();
}