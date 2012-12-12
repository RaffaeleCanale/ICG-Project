//=============================================================================
//                                                                            
//   Exercise code for the lecture
//                                                                            
//=============================================================================

#include "HDRViewer.h"
#include "Scenes/HDRPlanets.h"


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

/**
 *	Starting method
 */
int main(int argc, char **argv) {
	bool createImage = true;
	bool render = true;

	if (createImage) {
		Image image;
		//image.readTga("..\\..\\data\\sun\\textures\\sun");
		//image.writeHdr("..\\..\\data\\sun\\textures\\sun2", pixelFunction);

		image.readTga("..\\..\\data\\stars\\textures\\stars2");
		image.writeHdr("..\\..\\data\\stars\\textures\\stars2", pixelFunction);
	} 

	if(render) {
		
	
	glutInit(&argc, argv);
	
	HDRViewer window("HDR Viewer", 800, 800);
	
	// Load scene given in argument

	argv[1] = "..\\..\\data\\bunny.obj";
	argv[1] = "..\\..\\data\\earth\\earth.obj";
	//argv[1] = "..\\..\\data\\planets\\mars.obj";

	window.buildSolarSystem();

	/*
	if(argc == 2) {
		window.loadMesh(argv[1]);
	} else if(argc == 3) {
		window.loadMesh(argv[1], argv[2]);
	}//*/

	glutMainLoop();//*/
	}
	
}