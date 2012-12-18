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
//#include "../gl/cubeMapTexture.h"
#include "Scenes/Scene.h"
#include "Scenes/Planets.h"
#include "Scenes/OtherScene.h"
#include "Scenes/Satellite.h"
#include "../utils/StopWatch.h"

#define SCENE_TYPE Satellite

//== CLASS DEFINITION =========================================================

#define bloom_param_count 3
#define tone_param_count 1
#define scatter_param_count 2
	      
/*
 HDRViewer.
*/
class HDRViewer : public TrackballViewer {
public:
   	
  HDRViewer(const char* _title, int _width, int _height);
  
	
  void loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL = std::string());

  void loadScene(SCENE_TYPE scene);
	

protected:

	// overloaded GUI function
	virtual void init();
	virtual void keyboard(int key, int x, int y);
	virtual void special(int key, int x, int y);
	virtual void reshape(int w, int h); 

	virtual void idle();
	
	virtual void draw_scene(DrawMode _draw_mode);

	

private:	
	typedef struct {
		short currentPage;
		short parametersCount;
		char cursor;
	} MENU_HELPER;

	typedef struct {
		float exposure;		
		float brightThreshold;

		float textureRatio;
		float bloomRatio;
		float scatterRatio;
	} TONE_MAPPING_PARAMETERS;

	typedef struct {
		float downSampleFactor;

		float blurScale;
		float blurStrength;
		int blurSize;

		float brightThreshold;

		char algorithmChoice;
	} BLOOM_PARAMETERS;

	typedef struct {
		float downSampleFactor;

		float decay;
		float density;
		float weight;

		int samples;
	} SCATTER_PARAMETERS;


	void updateAndPrint(float increment);
	float update(float parameter, float min, float max, float delta, bool cyclic, bool * needReshape = NULL);

	float incrementParameter(float parameter, float min, float max, float delta, bool cyclic);

	void renderCubeMap();
	void generateShadowMap();
	//void renderScene();
	void extractBrightAreas();
	void renderBlur(float dx, float dy);
	//void renderLight();
	void renderScattering();
	void blend();

	//void drawObject(Shader& sh, Mesh3D& mesh, bool showTexture, bool useModelWorlNormal);
	//void drawObjectSimple(Shader& sh, Mesh3D& mesh, bool useModelWorlNormal);

	void renderFullScreenQuad();
	void renderCustomScreenQuad(float downSampleScale);

	// -------------------------------------
	// ---------- ATTRIBURES --------------
	// -------------------------------------

	StopWatch mWatch;
	float mCurrentTime;
	bool mAnimate;

	// frame buffer object for render2texture
	FrameBufferObject mHdrBuffer;
	FrameBufferObject mBloomBuffer;
	FrameBufferObject mScatterBuffer;
	

	// Textures
	Texture mHdrTexture;
	Texture mBloomTexture;
	Texture mScatterTexture;
	Texture mScatterDownsampledTexture;

	// mesh object	
	SCENE_TYPE mScene;

	// HDR shader	
	Shader mFlareShader;

	Shader mExtractBloomShader;		
	Shader mBlendShader;

	Shader mScatterBlurShader;
	Shader mUniColorShader;
	
	Shader mBlurShader[2];
	
	// Config variables		
	MENU_HELPER mMenu;	
	
	BLOOM_PARAMETERS mBloomEffect[bloom_param_count];
	char mBloomConfig;

	TONE_MAPPING_PARAMETERS mTone[tone_param_count];
	char mToneConfig;

	SCATTER_PARAMETERS mScatterEffect[scatter_param_count];
	char mScatterConfig;
		
};


//=============================================================================
#endif // MESH_VIEWER_HH defined
//=============================================================================
