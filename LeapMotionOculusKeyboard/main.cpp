#include "LMOC.h"
#include <SFML/System.hpp>
#include <iostream>

//program start init and threading
int main(int, char const**)
{
    LMOC lmoc;
    sf::Thread rThread(&LMOC::renderThread,&lmoc);
    rThread.launch();
    return lmoc.run();
}
