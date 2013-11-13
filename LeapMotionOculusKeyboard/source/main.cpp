#include "LMOC.h"
#include <SFML/System.hpp>
#include <iostream>

//program start init and threading
int main(int, char const**)
{
    LMOC lmoc;
    
    sf::Thread tThread(&LMOC::textThread,&lmoc);
    sf::Thread rThread(&LMOC::renderThread,&lmoc);
    //sf::Thread mThread(&LMOC::matrixThread,&lmoc);
    
    tThread.launch();
    rThread.launch();
    //mThread.launch();
    
    return lmoc.run();
}
