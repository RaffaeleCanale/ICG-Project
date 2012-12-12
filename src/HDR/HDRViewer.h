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
#include "../gl/cubeMapTexture.h"
//== CLASS DEFINITION =========================================================

#define DOWN_SAMPLE_PASSES 1
	      
/*
 HDRViewer.
*/
class HDRViewer : public TrackballViewer {
public:
   
	typedef struct {
		float downSampleFactor;

		float blurScale;
		float blurStrength;
		int blurSize;

		float brightThreshold;

		char algorithmChoice;
	} BLOOM_PARAMETERS;


  HDRViewer(const char* _title, int _width, int _height);
  
	
  void loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL = std::string());

  void buildSolarSystem();
	

protected:

	// overloaded GUI function
	virtual void init();
	virtual void keyboard(int key, int x, int y);
	virtual void special(int key, int x, int y);
	virtual void reshape(int w, int h); 
	
	virtual void draw_scene(DrawMode _draw_mode);

	

private:	
	
	void blurEffectKey(int key);

	void updateAndPrint(int increment, bool isCtrlPressed, bool isShiftPressed);
	//float update(int line, float parameter, float min, float max, float delta, bool cyclic);
	
	float interpolateParameter(float parameter, float min, float max, float delta, bool isCtrlPressed, bool isShiftPressed, bool cyclic);

	void renderCubeMap();
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
	CubeMapTexture mCubeMap;

	Mesh3D m_sun;
	Mesh3D m_planet;
	Mesh3D m_planet2;

	Mesh3D m_stars;

	Light3D m_light;
	Vector3 m_lightColor;

	// HDR shader
	Shader mDiffuseShader;
	Shader mExtractBloomShader;
	Shader mBlendShader;
	Shader mSimpleShader;
	
	Shader mBlurShader[2];

	Shader mNullShader;
	
	typedef struct {
		short currentPage;
		short parametersCount;
		char cursor;
	} MENU_HELPER;
		

	float mExposure;

	

	MENU_HELPER mMenu;
	
	BLOOM_PARAMETERS mBloomEffect[3];
	char mBloomConfig;
		
	
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
