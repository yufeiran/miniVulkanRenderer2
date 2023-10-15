#include<iostream>
#include"miniVulkanRenderer.h"
#include"Common/miniLog.h"

using namespace std;
using namespace mini;

MiniVulkanRenderer miniRenderer;

int main()
{
	try {
		miniRenderer.init();

		miniRenderer.loop();

	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		Log(e.what(),LOG_TYPE::ERROR_TYPE);
		return EXIT_FAILURE;
	}


	return 0;
}