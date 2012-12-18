#include "Satellite.h"
#include "../../utils/Mesh3DReader.h"
#define PI 3.1415926

Satellite::Satellite(bool fixCamera) : mFixCamera(fixCamera) {
}

Satellite::~Satellite() {
}

void Satellite::loadShaders() {
	mDiffuseShader.create("Shaders/diffuse.vs", "Shaders/diffuse.fs");
	mSimpleShader.create("Shaders/texture.vs", "Shaders/texture.fs");
}

void Satellite::initCamera(Camera3D * camera) {
	if (mFixCamera) {
		camera->setClippingPlanes(camera->getNearPlane(), 100000.0);
	}
}

void Satellite::buildScene(Vector3 * pos, float * radius) {

	const char * satName = "..\\..\\data\\Satellite\\sat.obj";
	const char * sunName = "..\\..\\data\\sun\\sun.obj";
	const char * starsName = "..\\..\\data\\stars\\stars.obj";

	double sunScale = 200.0;
	double sunTranslate = 8000.0;

	double satScale = 0.4;	
	double satTranslate = 200;

	double starsScale = 9000.0;

	// SUN -------
	Mesh3DReader::read(sunName, mSun);

	if(mSun.hasNormals()) {
		mSun.calculateVertexNormals();
	}

	mSun.translateObject(Vector3(sunTranslate, 0.0, 0.0));
	mSun.scaleObject(Vector3(sunScale, sunScale, sunScale));
	
	mLight.translateObject(Vector3(sunTranslate, 0.0, 0.0));
	mLightColor = Vector3(10.0, 10.0, 10.0);

	// SATELLITE -----
	Mesh3DReader::read(satName, mSatellite);

	if (mSatellite.hasNormals()) {
		mSatellite.calculateVertexNormals();
	}
	
	mSatellite.translateObject(Vector3(0.0, 0.0, satTranslate));
	mSatellite.rotateObject(Vector3(0.0, 1.0, 0.0), PI/2);	
	mSatellite.scaleObject(Vector3(satScale, satScale, satScale));


	// STARS ------
	Vector3 bbmin, bbmax;
	mSun.calculateBoundingBox(bbmin, bbmax);
	
	double r = 0.5*(bbmin - bbmax).length();
	Vector3 center = 0.5*(bbmin + bbmax);
	
	r = sunTranslate * 1.0;
	
	*pos = center;
	if (mFixCamera) {
		*radius = 10.0;
	} else {
		*radius = r;
	}
	
	//set_scene_pos(Vector3(0.0, 0.0, -1000.0), planetTranslate * 3.3);
	//set_scene_pos(center, planetTranslate * 3.3);

	
	Mesh3DReader::read(starsName, mStars);
	if(!mStars.hasNormals()) {
			mStars.calculateVertexNormals();
	}
	mStars.scaleObject(Vector3(starsScale, starsScale, starsScale));
}

void Satellite::animate(float timeDelta) {

	double satAngle = 2 * PI * timeDelta / 100000.0;
	double sunAngle = timeDelta / 1000000.0;

	
	//mSatellite.rotateObject(Vector3(1.0, 0.0, 0.0), satAngle);
	if (mFixCamera) {
		mStars.rotateObject(Vector3(0.0, 0.0, 1.0), satAngle);	
		mSun.rotateAroundAxisWorld(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), sunAngle + satAngle);
	} else {
		mSatellite.rotateObject(Vector3(1.0, 0.0, 0.0), satAngle);
		mSun.rotateAroundAxisWorld(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), sunAngle);
	}

}

void Satellite::renderScene(Camera3D * camera) {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4 cameraInverse = camera->getTransformation().Inverse();
	
	Vector3 lightPosInCamera = cameraInverse * getLightPosition();
	
	mDiffuseShader.bind();
	//mHdrTexture.setLayer(0);
	//mHdrTexture.bind();
			
	mDiffuseShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	
	mDiffuseShader.setMatrix4x4Uniform("projection", camera->getProjectionMatrix());
	mDiffuseShader.setMatrix3x3Uniform("worldcameraNormal", camera->getTransformation().Transpose());

	mDiffuseShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z );
	mDiffuseShader.setVector4Uniform("lightcolor", mLightColor.x, mLightColor.y, mLightColor.z, 1.0);

	// Draw mesh		
	drawObject(mDiffuseShader, mSatellite, true, true);
	
	mDiffuseShader.unbind();
		
	mSimpleShader.bind();
	
	mSimpleShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	
	mSimpleShader.setMatrix4x4Uniform("projection", camera->getProjectionMatrix());
	//m_LightShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());
	drawObject(mSimpleShader, mSun, true, false);
	
	drawObject(mSimpleShader, mStars, true, false);

	mSimpleShader.unbind();//*/	
}

void Satellite::renderLight(Camera3D * camera, Shader * shader) {
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
	
	drawObjectSimple(*shader, mSatellite, false);
	
	
	shader->unbind();
}

void Satellite::setLightIntensity(Vector3 color) {
	mLightColor = color;
}

Vector3 Satellite::getLightIntensity() {
	return mLightColor;
}

Vector3 Satellite::getLightPosition() {
	//return mLight.origin();
	return mSun.origin();
}