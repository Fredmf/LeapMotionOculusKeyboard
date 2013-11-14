
#include "LMOC.h"
#include <SFML/System.hpp>
#include <iostream>

//program start init and threading
int main(int, char const**)
{
    LMOC lmoc;
    
    sf::Thread tThread(&LMOC::textThread,&lmoc);
    tThread.launch();
	
	#ifdef __APPLE__
	sf::Thread rThread(&LMOC::renderThread,&lmoc);
	rThread.launch();
    lmoc.run();
	#endif

	#ifdef WIN32
	sf::Thread rThread(&LMOC::run,&lmoc);
	rThread.launch();
	lmoc.renderThread();
	#endif

	return EXIT_SUCCESS;
}
