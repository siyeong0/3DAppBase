#include <iostream>

#include "ModelViewer.h"

int main()
{
	ModelViewer App;
	if (App.Initialize())
	{
		std::cout << "Initialize Failed." << std::endl;
	}

	App.Run();

	return 0;
}
