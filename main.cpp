/*
 * main.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

int main(void)
{
	MyApplication app;
	app.startup();

	while(app.keepRunning())
	{
		app.renderOneFrame();
	}

	return 0;
}
