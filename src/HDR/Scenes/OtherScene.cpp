#include "OtherScene.h"
#include "../../utils/Mesh3DReader.h"

#define PI 3.14159268

OtherScene::OtherScene() {
}

OtherScene::~OtherScene() {
}

void OtherScene::loadShaders() {
	mDiffuseShader.create("Shaders/diffuse.vs", "Shaders/diffuse.fs");
	mSimpleShader.create("Shaders/texture.vs", "Shaders/texture.fs");
}

void OtherScene::initCamera(Camera3D * camera) {
	Matrix4 startMatrix(0.942188, -0.00602069, 0.335031,  99.698,
						0.0169787, 0.999412,  -0.0297883, 37.7852,
						-0.334655, 0.0337545,  0.941736,  341.313,
						0,       0,       0,       1);
	camera->setTransformation(startMatrix);
}

void OtherScene::buildScene(Vector3 * pos, float * radius) {
	//mCubeMap.create("..\\..\\data\\stars\\cubeMap");

	const char * cubeName = "..\\..\\data\\Cube\\cube2.obj";
//	const char * windowName = "..\\..\\data\\Cube\\window.obj";
	const char * pillarName = "..\\..\\data\\Cube\\pillar.obj";
	const char * sunName = "..\\..\\data\\sun\\sun.obj";
	const char * starsName = "..\\..\\data\\stars\\stars.obj";

	double sunScale = 40.0;
	double sunTranslate = 1800.0;
	
	double starsScale = 2000.0;

	double cubeScale = 100.0;

//	Vector3 windowScale = Vector3(20.0, 0.0, 15.0);
	//Vector3 windowTranslate = Vector3(0.0, 97.0, -100.0);
	//Vector3 lightDelta = Vector3(0.0, 10.0, 0.0);
	//Vector3 windowTranslate = Vector3(-97.0, 30.0, -100.0);
	//Vector3 lightDelta = Vector3(-10.0, 0.0, 0.0);
	

	//double xDelta = bridgeTranslateX * BRIDGE_WIDTH / 2;
	

	Mesh3DReader::read(cubeName, mCube);
	if(!mCube.hasNormals()) {
			mCube.calculateVertexNormals();
	}
	mCube.scaleObject(Vector3(cubeScale, cubeScale, cubeScale));


	Mesh3DReader::read(pillarName, mPillar);
	if (!mPillar.hasNormals()) {
		mPillar.calculateVertexNormals();
	}
	
	mPillarMatrices[0] = buildPillarMatrix(false, 0.0);
	mPillarMatrices[1] = buildPillarMatrix(true, 0.0);	
	mPillarMatrices[2] = buildPillarMatrix(true, -200.0);

	
	/*
	Mesh3DReader::read(windowName, mWindow);
	if (!mWindow.hasNormals()) {
		mWindow.calculateVertexNormals();
	}	
	mWindow.translateObject(windowTranslate);
	mWindow.rotateObject(Vector3(0.0, 0.0, 1.0), PI/2);
	mWindow.scaleObject(windowScale);//*/

	Mesh3DReader::read(sunName, mSun);

	if(mSun.hasNormals()) {
		mSun.calculateVertexNormals();
	}

	mSun.translateObject(Vector3(sunTranslate, 0.0, 0.0));
	mSun.scaleObject(Vector3(sunScale, sunScale, sunScale));
	
	mLight.translateObject(Vector3(sunTranslate, 0.0, 0.0));
	mLightColor = Vector3(10.0, 10.0, 10.0);
	
	Vector3 bbmin, bbmax;
	mCube.calculateBoundingBox(bbmin, bbmax);
	
	double r = 0.5*(bbmin - bbmax).length();
	Vector3 center = 0.5*(bbmin + bbmax);
	
	//r = planetTranslate * 1.0;
	r = 1000.0;
	
	*pos = center;
	*radius = r;
	//*/

	Mesh3DReader::read(starsName, mStars);
	if(!mStars.hasNormals()) {
			mStars.calculateVertexNormals();
	}
	mStars.scaleObject(Vector3(starsScale, starsScale, starsScale));
}

Matrix4 OtherScene::buildPillarMatrix(bool left, double depth) {
	mPillar.setIdentity();

	double pillarScale = 6.8;
	Vector3 translateVector = Vector3(left ? -80 : 89.0, 32.0, depth);

	mPillar.translateObject(translateVector);
	mPillar.rotateObject(Vector3(1.0, 0.0, 0.0), PI/2);
	mPillar.rotateObject(Vector3(0.0, 0.0, 1.0), left ? -PI/2 : PI/2);
	mPillar.scaleObject(Vector3(pillarScale, pillarScale, pillarScale));
	return mPillar.getTransformation();
}

void OtherScene::animate(float timeDelta) {
	double satAngle = 2 * PI * timeDelta / 100000.0;
	double sunAngle = timeDelta / 1000000.0;

	mStars.rotateObject(Vector3(0.0, 1.0, 0.0), satAngle);	
	mSun.rotateAroundAxisWorld(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), sunAngle + satAngle);
}

void OtherScene::renderScene(Camera3D * camera) {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4 cameraInverse = camera->getTransformation().Inverse();
	
	Vector3 lightPosInCamera = cameraInverse * mSun.origin();
	
	mDiffuseShader.bind();
			
	mDiffuseShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	
	mDiffuseShader.setMatrix4x4Uniform("projection", camera->getProjectionMatrix());
	mDiffuseShader.setMatrix3x3Uniform("worldcameraNormal", camera->getTransformation().Transpose());

	mDiffuseShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z );
	mDiffuseShader.setVector4Uniform("lightcolor", mLightColor.x, mLightColor.y, mLightColor.z, 1.0);

	// Draw mesh
	drawObject(mDiffuseShader, mCube, true, true);

	for (int i = 0; i < PILLAR_COUNT; i++) {
		mPillar.setTransformation(mPillarMatrices[i]);
		drawObject(mDiffuseShader, mPillar, false, true);
	}
	
	mDiffuseShader.unbind();

	mSimpleShader.bind();

	mSimpleShader.setMatrix4x4Uniform("worldcamera", cameraInverse);
	mSimpleShader.setMatrix4x4Uniform("projection", camera->getProjectionMatrix());

	drawObject(mSimpleShader, mSun, true, false);
	drawObject(mSimpleShader, mStars, true, false);

	mSimpleShader.unbind();
}

void OtherScene::renderLight(Camera3D * camera, Shader * shader) {
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
	
	for (int i = 0; i < PILLAR_COUNT; i++) {
		mPillar.setTransformation(mPillarMatrices[i]);
		drawObjectSimple(*shader, mPillar, false);
	}
	
	drawObjectSimple(*shader, mCube, false);
	
	
	shader->unbind();
}

Vector3 OtherScene::getLightPosition() {
	return mSun.origin();
}

void OtherScene::setLightIntensity(Vector3 color) {
	mLightColor = color;
}

Vector3 OtherScene::getLightIntensity() {
	return mLightColor;
}