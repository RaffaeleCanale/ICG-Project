//=============================================================================
//                                                                            
//   Exercise code for the lecture
//                                                                            
//=============================================================================
//=============================================================================
//
//  CLASS MeshViewer
//
//=============================================================================


#ifndef MESH_VIEWER_HH
#define MESH_VIEWER_HH


//== INCLUDES =================================================================
#include <string>
#include <vector>

#include "../../gl/TrackballViewer.h"
#include "../../gl/shader.h"
#include "../../gl/Mesh3D.h"
#include "../../gl/Light3D.h"
#include "../../gl/texture.h"
#include "../../gl/fbo.h"

//== CLASS DEFINITION =========================================================


	      
/*
 * Class that will display the result
*/
class HDRViewer : public TrackballViewer
{
public:
   
  HDRViewer(const char* _title, int _width, int _height);
  
	
  void loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL = std::string());
	
protected:

	// overloaded GUI function
  virtual void init();
	virtual void keyboard(int key, int x, int y);
virtual void reshape(int w, int h); 
	
  virtual void draw_scene(DrawMode _draw_mode);

private:
	void drawScene();
	//void drawDepth();
	//void drawEdge();
	//void blendCartoonAndEdge();

	void renderFullScreenQuad();
	
	
protected:
	
	// frame buffer object for render2texture
	FrameBufferObject m_fbo;
	
	// mesh object
	Mesh3D m_mesh;
	
	// cartoon shader
	Shader m_mainShader;
	
	// depth shader
	//Shader m_depthShader;
	
	// edge shader
	//Shader m_edgeShader;

	// blending shader
	//Shader m_blendingShader;
	
	// cartoon shading texture
	Texture m_mainShadingTexture;
	
	// cartoon output texture
	Texture m_mainOutputTexture;
	
	// depth texture
	//Texture m_depthTexture;
	
	// edge texture
	//Texture m_edgeTexture;
	
	
};


//=============================================================================
#endif // MESH_VIEWER_HH defined
//=============================================================================
