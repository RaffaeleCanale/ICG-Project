#ifndef __OTHERSC_H__
#define __OTHERSC_H__

#include "Scene.h"
#include "../../gl/Mesh3D.h"
#include "../../gl/Light3D.h"
#include "../../math/Vector3.h"
#include "../../gl/shader.h"

#define PILLAR_COUNT 3

class OtherScene : public Scene {

public:
	
	OtherScene();
	~OtherScene();

	virtual void loadShaders();
	virtual void buildScene(Vector3 * pos, float * radius);
	virtual void renderScene(Camera3D * camera);
	virtual void renderLight(Camera3D * camera, Shader * shader);
	virtual Vector3 getLightPosition();
	virtual void animate(float timeDelta);
	virtual void initCamera(Camera3D * camera);
	virtual void setLightIntensity(Vector3 color);
	virtual Vector3 getLightIntensity();

	
private:
	Matrix4 buildPillarMatrix(bool left, double depth);

	Mesh3D mCube;
	//Mesh3D mWindow;
	Mesh3D mPillar;
	Matrix4 mPillarMatrices[PILLAR_COUNT];

	Mesh3D mSun;
	Mesh3D mStars;

	Light3D mLight;
	Vector3 mLightColor;


	Shader mDiffuseShader;
	Shader mSimpleShader;
	
};

#endif