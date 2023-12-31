#ifndef SPH_ENTRYPOINT_H
#define SPH_ENTRYPOINT_H

#include "Application.h"

extern sph::Application* sph::CreateApplication();

int main(int argc, char** argv)
{
	auto app = sph::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif