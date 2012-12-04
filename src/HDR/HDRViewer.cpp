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
	m_DiffuseShader.create("Shaders/diffuse.vs", "Shaders/diffuse.fs");	
}



//-----------------------------------------------------------------------------


void HDRViewer::reshape(int _w, int _h) {
	TrackballViewer::reshape(_w,_h);
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

	// PLANET -----
	Mesh3DReader::read(planetName, m_planet);

	if (m_planet.hasNormals()) {
		m_planet.calculateVertexNormals();
	}
	
	//m_planet.setIdentity();
	
	m_planet.scaleObject(Vector3(planetScale, planetScale, planetScale));


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
	switch (key) {
		case 'h':
			printf("Help:\n");
			printf("'h'\t-\thelp\n");
			break;
		default:
			TrackballViewer::keyboard(key, x, y);
			break;
	}
	
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------


void HDRViewer::draw_scene(DrawMode _draw_mode) {

	// clear screen
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	

	m_DiffuseShader.bind();
	
	m_DiffuseShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_DiffuseShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_DiffuseShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());
	
	
	Vector3 lightPosInCamera = m_camera.getTransformation().Inverse() * m_light.origin();
	//Vector3 lightPosInCamera = m_camera.getTransformation().Inverse() * m_camera.origin();

	m_DiffuseShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z );
	// TODO Again, temp light intensity
	m_DiffuseShader.setVector3Uniform("lightcolor", 1.0, 1.0, 0.0);

	// Draw mesh		
	draw_object(m_DiffuseShader, m_planet, true);
	draw_object(m_DiffuseShader, m_sun, true);

	m_DiffuseShader.unbind();
}


//-----------------------------------------------------------------------------

/*
void HDRViewer::draw_object(Shader& sh, Mesh3D& mesh) {
	
	sh.setMatrix4x4Uniform("modelworld", mesh.getTransformation() );
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, mesh.getNormalPointer() );
	glTexCoordPointer( 2, GL_DOUBLE, 0, mesh.getUvTextureCoordPointer() );
	
	for(unsigned int i = 0; i < mesh.getNumberOfParts(); i++)
	{
		glDrawElements( GL_TRIANGLES, mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, mesh.getVertexIndicesPointer(i) );
	}
	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
}//*/

void HDRViewer::draw_object(Shader& sh, Mesh3D& mesh, bool showTexture) {
		
	sh.setMatrix4x4Uniform("modelworld", mesh.getTransformation() );
	sh.setMatrix3x3Uniform("modelworldNormal", mesh.getTransformation().Inverse().Transpose());
			
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

		sh.setVector3Uniform("diffuseColor", 
							 mesh.getMaterial(i).m_diffuseColor.x, 
							 mesh.getMaterial(i).m_diffuseColor.y, 
							 mesh.getMaterial(i).m_diffuseColor.z );

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

/*
void HDRViewer::drawHDR() {
	
	// clear screen
	glEnable(GL_DEPTH_TEST);
	glClearColor(1,1,1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_DiffuseShader.bind(); 
	
	// set parameters
	m_DiffuseShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_DiffuseShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_DiffuseShader.setMatrix4x4Uniform("modelworld", m_mesh.getTransformation() );
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if(m_mesh.hasUvTextureCoord()) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	//m_HDRShadingTexture.setLayer(0);
	//m_HDRShadingTexture.bind();
	//m_HDRShader.setIntUniform("texture", m_HDRShadingTexture.getLayer());
	
	glVertexPointer( 3, GL_DOUBLE, 0, m_mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_mesh.getNormalPointer() );
	if(m_mesh.hasUvTextureCoord()) {
		glTexCoordPointer( 2, GL_DOUBLE, 0, m_mesh.getUvTextureCoordPointer() );
	}
	for(unsigned int i = 0; i < m_mesh.getNumberOfParts(); i++) {
		bool hasTexture = m_mesh.hasUvTextureCoord() && m_mesh.getMaterial(i).hasDiffuseTexture();
		hasTexture = false;

		m_DiffuseShader.setIntUniform("useTexture", hasTexture);
		m_DiffuseShader.setVector3Uniform("diffuseColor", 
										  m_mesh.getMaterial(i).m_diffuseColor.x, 
										  m_mesh.getMaterial(i).m_diffuseColor.y, 
										  m_mesh.getMaterial(i).m_diffuseColor.z );
		if(hasTexture) {
			m_mesh.getMaterial(i).m_diffuseTexture.bind();
			m_DiffuseShader.setIntUniform("textureDiffuse", m_mesh.getMaterial(i).m_diffuseTexture.getLayer());
		}
		glDrawElements( GL_TRIANGLES, m_mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, m_mesh.getVertexIndicesPointer(i) );
		if(hasTexture) {
			m_mesh.getMaterial(i).m_diffuseTexture.unbind();
		}
	}
	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	if(m_mesh.hasUvTextureCoord())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	//m_HDRShadingTexture.unbind();
	m_DiffuseShader.unbind();
}//*/


//-----------------------------------------------------------------------------
/*
void HDRViewer::drawDepth() {
	
	// clear screen
	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//m_depthShader.bind(); 
	
	// set parameters
	m_depthShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_depthShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_depthShader.setMatrix4x4Uniform("modelworld", m_mesh.getTransformation() );
	m_depthShader.setFloatUniform("near",m_camera.getNearPlane());
	m_depthShader.setFloatUniform("far",m_camera.getFarPlane());
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, m_mesh.getVertexPointer() );
	
	for(unsigned int i = 0; i < m_mesh.getNumberOfParts(); i++)
	{
		glDrawElements( GL_TRIANGLES, m_mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, m_mesh.getVertexIndicesPointer(i) );
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	
	m_depthShader.unbind();
	
}

//*/
//-----------------------------------------------------------------------------

void 
HDRViewer::
renderFullScreenQuad() {
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


//-----------------------------------------------------------------------------
/*
void 
HDRViewer::
drawEdge() {
	
	// clear screen
	glDisable(GL_DEPTH_TEST);
	
	m_edgeShader.bind(); 
	m_depthTexture.setLayer(0);
	m_depthTexture.bind();
	m_edgeShader.setIntUniform("texture",m_depthTexture.getLayer());
	m_edgeShader.setFloatUniform("dx",1.0/width_);
	m_edgeShader.setFloatUniform("dy",1.0/height_);
	
	// render a quad over full image
	renderFullScreenQuad();
	
	m_edgeShader.unbind();
	
}
//*/
/*
//-----------------------------------------------------------------------------
void 
HDRViewer::
blendHDRAndEdge() {
	
	// clear screen
	glDisable(GL_DEPTH_TEST);
	
	m_blendingShader.bind();
	m_HDROutputTexture.setLayer(0);
	m_HDROutputTexture.bind();
	m_blendingShader.setIntUniform("texture1",m_HDROutputTexture.getLayer());
	m_edgeTexture.setLayer(1);
	m_edgeTexture.bind();
	m_blendingShader.setIntUniform("texture2",m_edgeTexture.getLayer());
	
	
	// render a quad over full image
	renderFullScreenQuad();
	
	m_blendingShader.unbind();
}
//*/

//=============================================================================
