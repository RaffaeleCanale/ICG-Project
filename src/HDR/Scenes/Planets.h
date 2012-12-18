#ifndef __PLANETS_H__
#define __PLANETS_H__

#include "Scene.h"
#include "../../gl/Mesh3D.h"
#include "../../gl/Light3D.h"
#include "../../math/Vector3.h"
#include "../../gl/shader.h"

class Planets : public Scene {

public:
	
	Planets();
	~Planets();

	virtual void loadShaders();
	virtual void buildScene(Vector3 * pos, float * radius);
	virtual void renderScene(Camera3D * camera);
	virtual void renderLight(Camera3D * camera, Shader * shader);
	virtual Vector3 getLightPosition();

private:

	Mesh3D mSun;
	Mesh3D mPlanet;
	Mesh3D mPlanet2;

	Mesh3D mStars;

	Light3D mLight;
	Vector3 mLightColor;


	Shader mDiffuseShader;
	Shader mSimpleShader;
	
};

#endif