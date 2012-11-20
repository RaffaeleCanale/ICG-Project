#include "HdrApp.h"
#include "AllocOverLoaderOn.h"

int main(int, char**)
{
	try
	{
		HdrApp *app = new HdrApp();
		app->run(1024, 768);
		delete app;
	}
	catch (const std::exception &e)
	{
		std::cerr << "-- Exception--\n" << e.what() << std::endl;
	}

	return 0;
}
