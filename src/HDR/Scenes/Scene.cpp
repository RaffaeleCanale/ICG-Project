#include "Scene.h"

Scene::Scene() {
}

Scene::~Scene() {
}

//*
void Scene::setLightIntensity(Vector3 color) {}
Vector3 Scene::getLightIntensity() {return Vector3(0.0, 0.0, 0.0);}
void Scene::initCamera(Camera3D * camera) {}
void Scene::loadShaders() {}
void Scene::buildScene(Vector3 * pos, float * radius) {}
void Scene::renderScene(Camera3D * camera) {}
void Scene::renderLight(Camera3D * camera, Shader * shader) {}
Vector3 Scene::getLightPosition() {
	return Vector3(0.0, 0.0, 0.0);
}//*/

void Scene::drawObject(Shader& sh, Mesh3D& mesh, bool showTexture, bool useModelWorlNormal) {		
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
		
		
		//*
		sh.setVector4Uniform("diffuseColor", 
			mesh.getMaterial(i).m_diffuseColor.x, 
			mesh.getMaterial(i).m_diffuseColor.y, 
			mesh.getMaterial(i).m_diffuseColor.z, 1.0 );//*/
		//sh.setVector4Uniform("diffuseColor", 1.0, 1.0, 1.0, 1.0);

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

void Scene::drawObjectSimple(Shader& sh, Mesh3D& mesh, bool useModelWorlNormal) {

	sh.setMatrix4x4Uniform("modelworld", mesh.getTransformation() );
	if (useModelWorlNormal) {		
		sh.setMatrix3x3Uniform("modelworldNormal", mesh.getTransformation().Inverse().Transpose());
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer( 3, GL_DOUBLE, 0, mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, mesh.getNormalPointer() );

	for(unsigned int i = 0; i < mesh.getNumberOfParts(); i++)	{
		glDrawElements( GL_TRIANGLES, mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, mesh.getVertexIndicesPointer(i) );		
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Scene::animate(float timeDelta) {}