#ifndef __SCENE_H__
#define __SCENE_H__

#include "../../math/Vector3.h"
#include "../../gl/Camera3D.h"
#include "../../gl/shader.h"
#include "../../gl/Mesh3D.h"
//#include "glew.h"

class Scene {

public:
	Scene();
	~Scene();

	virtual void loadShaders();
	virtual void buildScene(Vector3 * pos, float * radius);
	virtual void renderScene(Camera3D * camera);
	virtual void renderLight(Camera3D * camera, Shader * shader);
	virtual Vector3 getLightPosition();
	virtual void animate(float timeDelta);
	virtual void initCamera(Camera3D * camera);
	virtual void setLightIntensity(Vector3 color);
	virtual Vector3 getLightIntensity();

protected:
	void drawObject(Shader& sh, Mesh3D& mesh, bool showTexture, bool useModelWorlNormal);

	void drawObjectSimple(Shader& sh, Mesh3D& mesh, bool useModelWorlNormal);
};
#endif