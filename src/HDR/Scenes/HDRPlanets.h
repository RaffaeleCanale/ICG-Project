
#include <string>
#include "../HDRViewer.h"

class HDRPlanets : public HDRViewer {

public:
	HDRPlanets(const char* _title, int _width, int _height);

	void buildScene(const std::string& filenameOBJ, const std::string& filenameMTL = std::string());
};

