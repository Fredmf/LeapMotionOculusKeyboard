// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.

#include "LMOC.h"
#include <SFML/System.hpp>
#include <iostream>

//program start init and threading
int main(int, char const**)
{
    LMOC lmoc;

    #ifdef __APPLE__
    //sf::Thread tThread(&LMOC::textThread,&lmoc);
    //tThread.launch();
//	sf::Thread rThread(&LMOC::renderThread,&lmoc);
//	rThread.launch();
//    lmoc.checkInput();
	lmoc.runLoop();
	#endif

	#ifdef WIN32
	//sf::Thread rThread(&LMOC::run,&lmoc);
	//rThread.launch();
	lmoc.renderThread();
	#endif

	return EXIT_SUCCESS;
}
