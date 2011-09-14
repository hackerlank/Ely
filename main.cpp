/**
 * \file main.cpp
 *
 * \date Jul 26, 2011
 * \author Marco Paone
 */

//|||||||||||||||||||||||||||||||||||||||||||||||
#include "main.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * \fn
 *
 * All that is done is to create an instance of our application class Ely
 * and call its go() function. That's it.
 */
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
	game::Ely demo;
	try
	{
		demo.go();
	} catch (std::exception& e)
	{
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBoxA(NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
#endif
	}

	return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||
