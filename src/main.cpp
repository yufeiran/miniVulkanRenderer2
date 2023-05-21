#include<iostream>
#include"miniVulkanRenderer.h"
#include"Common/miniLog.h"

using namespace std;
using namespace mini;

int main()
{

	Log("Hello World!");
	MiniVulkanRenderer miniRenderer;
	try {
		miniRenderer.init();

		miniRenderer.loop();

	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	return 0;
}