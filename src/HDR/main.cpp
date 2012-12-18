//=============================================================================
//                                                                            
//   Exercise code for the lecture
//                                                                            
//=============================================================================

#include "HDRViewer.h"
#include "Scenes/Planets.h"
#include "Scenes/Satellite.h"
#include "Scenes/Scene.h"
#include "../utils/Image.h"


void pixelFunction(float * r, float * g, float * b) {
	/*
	*r += 2;
	*g += 2;
	*b += 2;//*/

	if (*r + *g + *b > 2.5) {
		*r = 10;
		*g = 10;
		*b = 10;
	}
}

OtherScene scene;
//Satellite scene(true);

/**
 *	Starting method
 */
int main(int argc, char **argv) {
	bool createImage = false;
	bool render = true;

	if (createImage) {
		Image image;

		image.readTga("..\\..\\data\\stars\\textures\\stars2");
		image.writeHdr("..\\..\\data\\stars\\textures\\stars2", pixelFunction);
	} 

	if(render) {
		
	
	glutInit(&argc, argv);
	
	HDRViewer window("HDR Viewer", 800, 800);
	
	// Load scene given in argument

	window.loadScene(scene);
	
	/*
	if(argc == 2) {
		window.loadMesh(argv[1]);
	} else if(argc == 3) {
		window.loadMesh(argv[1], argv[2]);
	}//*/

	glutMainLoop();//*/
	}
	
}