//=============================================================================
//                                                                            
//   Exercise code for the lecture
//                                                                            
//=============================================================================

#include "HDRViewer.h"
#include "Scenes/HDRPlanets.h"


/**
 *	Starting method
 */
int main(int argc, char **argv) {
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
	}
	//*/
	glutMainLoop();
	
}
