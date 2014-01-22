//Fred Fluegge 2014
//Everyone is permitted to copy and distribute verbatim or modified
//copies of this document, and changing it is allowed as long
//as the name is changed.
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from the use of this software.

////////////////////////////////////////////////////////////////////////
//
//  ran.h
//  bietet die Templatefunktion T ran<T>(void)
//  welche einem eine zufällige Zahl gewünschten Datentyps zurückgibt.
//  Funktioniert nur auf UNIX Systemen die einem die Möglichkeit geben
//  auf Systementropien zurückzugreifen
//
//  für T ran<T>(void) wird /dev/random
//  Um im Bedarfsfall mehr Performance zu liefern wurde
//  T ranU<T>(void) implementiert welche /dev/urandom verwendet
//  /dev/urandom bietet weniger gute Zufallszahlen und wiederhohlt sich
//  ab n abfragen in schneller Zeitfolge blockiert jedoch nicht den
//  Leseprozess sofern keine neue Entropie verfügbar ist
//
//  Fred M. Flügge 15.11.2013
//
/////////////////////////////////////////////////////////////////////////

#ifndef CF_ran_h
#define CF_ran_h
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <iostream>

template <typename T>
T ran(void) {
    
    int randomData = open("/dev/random", O_RDONLY);
    T myRandomVal;
    size_t randomDataLen = 0;
    while (randomDataLen < sizeof myRandomVal)
    {
        ssize_t result = read(randomData, ((char*)&myRandomVal) + randomDataLen, (sizeof myRandomVal) - randomDataLen);
        if (result < 0)
        {
            perror("ERROR: cannot read /dev/random");
        }
        randomDataLen += result;
    }
    close(randomData);
    
    return myRandomVal;
}

template <typename T>
T ranU(void) {
    
    int randomData = open("/dev/urandom", O_RDONLY);
    T myRandomVal;
    size_t randomDataLen = 0;
    while (randomDataLen < sizeof myRandomVal)
    {
        ssize_t result = read(randomData, ((char*)&myRandomVal) + randomDataLen, (sizeof myRandomVal) - randomDataLen);
        if (result < 0)
        {
            perror("ERROR: cannot read /dev/urandom");
        }
        randomDataLen += result;
    }
    close(randomData);
    
    //std::cout << (float)(myRandomVal % 10000)*0.0001  << std::endl;
    return myRandomVal;
}
#endif
