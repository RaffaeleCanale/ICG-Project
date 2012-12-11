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

#include "../gl/TrackballViewer.h"
#include "../gl/shader.h"
#include "../gl/Mesh3D.h"
#include "../gl/Light3D.h"
#include "../gl/texture.h"
#include "../gl/fbo.h"

//== CLASS DEFINITION =========================================================

#define DOWN_SAMPLE_PASSES 1
	      
/*
 HDRViewer.
*/
class HDRViewer : public TrackballViewer {
public:
   
  HDRViewer(const char* _title, int _width, int _height);
  
	
  void loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL = std::string());

  void buildSolarSystem();
	
protected:

	// overloaded GUI function
	virtual void init();
	virtual void keyboard(int key, int x, int y);
	virtual void reshape(int w, int h); 
	
	virtual void draw_scene(DrawMode _draw_mode);

private:	
	
	void interpolateParameter(float * parameter, float min, float max, float delta, bool isCtrlPressed, bool isShiftPressed, bool cyclic);

	void draw_elements();
	void extractBrightAreas();
	void renderBlur(float dx, float dy);
	void blend();

	//void draw_object(Shader& sh, Mesh3D& mesh);
	void draw_object(Shader& sh, Mesh3D& mesh, bool showTexture, bool useModelWorlNormal);

	void renderFullScreenQuad();
	void renderCustomScreenQuad();

	//void renderHalfScreenQuad();
	
protected:
	
	// frame buffer object for render2texture
	FrameBufferObject mHdrBuffer;
	FrameBufferObject mBloomBuffer;	

	//FrameBufferObject mDownSampleBuffers[DOWN_SAMPLE_PASSES];
	
	// mesh object
	Mesh3D m_sun;
	Mesh3D m_planet;
	Mesh3D m_planet2;

	Light3D m_light;
	Vector3 m_lightColor;

	// HDR shader
	Shader mDiffuseShader;
	Shader mExtractBloomShader;
	Shader mBlendShader;
	Shader mSimpleShader;
	
	Shader mBlurShader;

	Shader mBlurShader1;
	Shader mBlurShader2;

	Shader mNullShader;
	

	float mExposure;
	float mDownSampleFactor;

	float mBlurScale;
	float mBlurStrength;
	float mBlurSize;

	float mBlurAlgorithmChoice;
	Shader * mInUseBlurShader;

	
	// depth shader
	//Shader m_depthShader;
	
	// edge shader
	//Shader m_edgeShader;

	// blending shader
	//Shader m_blendingShader;
	
	// HDR shading texture
	//Texture m_HDRShadingTexture;
	
	// HDR output texture
	Texture mHdrTexture;
	Texture mBloomTexture;
	
	// depth texture
	//Texture m_depthTexture;
	
	// edge texture
	//Texture m_edgeTexture;
	
	
};


//=============================================================================
#endif // MESH_VIEWER_HH defined
//=============================================================================
