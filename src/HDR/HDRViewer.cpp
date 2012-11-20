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
	m_DiffuseShader.create("Diffuse.vs", "Diffuse.fs");
	m_TextureShader.create("tex.vs","tex.fs");
	//m_depthShader.create("depth.vs", "depth.fs");
	//m_edgeShader.create("edge.vs", "edge.fs");
	//m_blendingShader.create("blending.vs","blending.fs");

	// setup 1D color texture with 4 colors
	/*
	float  tex[12] =
	{
		0.2, 0.2, 0.2,
		0.4, 0.4, 0.4,
		0.6, 0.6, 0.6,
		0.8, 0.8, 0.8
	};

	m_HDRShadingTexture.create(4, 1, GL_RGB, GL_RGB, GL_FLOAT, tex, GL_NEAREST);
	//*/
}



//-----------------------------------------------------------------------------


void HDRViewer::reshape(int _w, int _h) {
	TrackballViewer::reshape(_w,_h);
	
	// resize framebuffer and textures
	m_fbo.create(_w,_h, true);
	
	m_HDROutputTexture.create(_w,_h,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE);
	// try GL_RGB4, GL_RGB8, GL_RGB10, GL_RGB16 to see effect of edge precision
	//m_depthTexture.create(_w,_h,GL_RGB16,GL_RGB,GL_UNSIGNED_BYTE);
	//m_edgeTexture.create(_w,_h,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE);
	
	// attach textures to frame buffer
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT0_EXT, m_HDROutputTexture.getID());
	//m_fbo.attachTexture(GL_COLOR_ATTACHMENT1_EXT, m_depthTexture.getID());
	//m_fbo.attachTexture(GL_COLOR_ATTACHMENT2_EXT, m_edgeTexture.getID());
	
}


//-----------------------------------------------------------------------------

void HDRViewer::loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL) {
	// load mesh from obj into m_mesh
	Mesh3DReader::read( filenameOBJ, m_mesh, filenameMTL);

	// calculate normals
	m_mesh.calculateVertexNormals();
	
	// get bounding box & reset scene camera accordingly
	Vector3 bbmin, bbmax;
	m_mesh.calculateBoundingBox(bbmin, bbmax);
	
	double radius = 0.5*(bbmin - bbmax).length();
	Vector3 center = 0.5*(bbmin + bbmax);
	
	set_scene_pos(center, radius);
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
	// draw HDR shading
	//m_fbo.bind(GL_COLOR_ATTACHMENT0_EXT);
	//drawHDR();
	//m_fbo.unbind();
	
	/*
	// draw depth image
	m_fbo.bind(GL_COLOR_ATTACHMENT1_EXT);
	drawDepth();
	m_fbo.unbind();
	
	// calculate edges on depth image
	m_fbo.bind(GL_COLOR_ATTACHMENT2_EXT);
	drawEdge();
	m_fbo.unbind();
	
	// blend edges and HDR shading
	//blendHDRAndEdge();
	//*/
	
	// clear screen
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	
	m_TextureShader.bind(); 
	
	// set parameters
	m_TextureShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_TextureShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	
	/*
	//stars
	glDisable(GL_DEPTH_TEST);
	m_Stars.setIdentity();
	m_Stars.scaleObject(Vector3(m_starsScale, m_starsScale, m_starsScale));
	m_Stars.translateWorld(Vector3(m_camera.origin()));
	m_meshShaderTexture.setMatrix4x4Uniform("modelworld", m_Stars.getTransformation() );
	m_Stars.getMaterial(0).m_diffuseTexture.bind();
	m_meshShaderTexture.setIntUniform("texture", m_Stars.getMaterial(0).m_diffuseTexture.getLayer());
	draw_object(m_meshShaderTexture, m_Stars);
	glEnable(GL_DEPTH_TEST); 
	
	//sun
	m_meshShaderTexture.setMatrix4x4Uniform("modelworld", m_Sun.getTransformation() );
	m_Sun.getMaterial(0).m_diffuseTexture.bind();
	m_meshShaderTexture.setIntUniform("texture", m_Sun.getMaterial(0).m_diffuseTexture.getLayer());
	draw_object(m_meshShaderTexture, m_Sun);
	
	m_meshShaderTexture.unbind(); //*/
	
	//-------------------------------
	
	m_DiffuseShader.bind();
	

	m_DiffuseShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_DiffuseShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_DiffuseShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());
	
	// TODO Temp code here
	//Vector3 lightPosInCamera = m_camera.getTransformation().Inverse() * m_light.origin();
	Vector3 lightPosInCamera = m_camera.getTransformation().Inverse() * m_camera.origin();

	m_DiffuseShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z );
	// TODO Again, temp light intensity
	m_DiffuseShader.setVector3Uniform("lightcolor", 1.0, 1.0, 1.0);

	// Draw mesh	
	draw_object(m_DiffuseShader, m_mesh, true);

}


//-----------------------------------------------------------------------------
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
}


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
	
}

void HDRViewer::draw_object(Shader& sh, Mesh3D& mesh, bool showTexture) {

	sh.setMatrix4x4Uniform("modelworld", mesh.getTransformation() );
	sh.setMatrix3x3Uniform("modelworldNormal", mesh.getTransformation().Inverse().Transpose());
			
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if(showTexture) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
			
	glVertexPointer( 3, GL_DOUBLE, 0, mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, mesh.getNormalPointer() );
	if(showTexture) {
		glTexCoordPointer( 2, GL_DOUBLE, 0, mesh.getUvTextureCoordPointer() );
	}	

	for(unsigned int i = 0; i < mesh.getNumberOfParts(); i++)	{
		sh.setIntUniform("useTexture", showTexture && mesh.getMaterial(i).hasDiffuseTexture());
		sh.setVector3Uniform("diffuseColor", 
							 mesh.getMaterial(i).m_diffuseColor.x, 
							 mesh.getMaterial(i).m_diffuseColor.y, 
							 mesh.getMaterial(i).m_diffuseColor.z );

		if(showTexture && mesh.getMaterial(i).hasDiffuseTexture()) {
			mesh.getMaterial(i).m_diffuseTexture.bind();
			sh.setIntUniform("texture", mesh.getMaterial(i).m_diffuseTexture.getLayer());
		}
		glDrawElements( GL_TRIANGLES, mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, mesh.getVertexIndicesPointer(i) );
		if(showTexture && mesh.getMaterial(i).hasDiffuseTexture()) {
			mesh.getMaterial(i).m_diffuseTexture.unbind();
		}
	}
			
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	if(showTexture) {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

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
