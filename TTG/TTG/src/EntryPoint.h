#pragma once

#include "Application.h"

int main()
{
	TTG::Application* app = createApplication();
	app->Init();
	app->Run();
	app->Cleanup();
	delete app;
}
