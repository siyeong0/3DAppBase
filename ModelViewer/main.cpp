#include <iostream>

#include "ModelViewer.h"
#include "EDevice.h"

int main()
{
	ModelViewer App(eDevice::GL);

	if (App.Initialize())
	{
		std::cout << "Initialize Failed." << std::endl;
	}

	App.Run();

	return 0;
}
