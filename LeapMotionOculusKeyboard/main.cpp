#include "LMOC.h"
#include <SFML/System.hpp>
#include <iostream>

//program start init and threading
int main(int, char const**)
{
    LMOC lmoc;
    sf::Thread rThread(&LMOC::renderThread,&lmoc);
    sf::Thread tThread(&LMOC::textThread,&lmoc);
    rThread.launch();
    tThread.launch();
    return lmoc.run();
}
