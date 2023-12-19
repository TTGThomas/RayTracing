#pragma once

#include "Application.h"

int main()
{
	TTG::Application* app = createApplication();
	app->init();
	app->run();
	app->cleanup();
	delete app;
}
