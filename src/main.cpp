#include<iostream>
#include"miniVulkanRenderer.h"
#include"Common/miniLog.h"

using namespace std;
using namespace mini;

MiniVulkanRenderer miniRenderer;

int main()
{

	miniRenderer.init();

	miniRenderer.loop();

	return 0;
}