//
//  LMOC.h
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 09.11.13.
//  Copyright (c) 2013 Fachbereich Informatik. All rights reserved.
//

#ifndef __LeapMotionOculusKeyboard__LMOC__
#define __LeapMotionOculusKeyboard__LMOC__

#define TEXTCNT 20

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <string>
#include <iostream>
#include <string>

//leap
#include "LeapListener.h"
//#include <Leap.h>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#elif WIN32
std::string resourcePath(void){
	return "";
}
#endif

class LMOC {
public:
    LMOC();
public:
    ~LMOC();
    
public:
    int run();
public:
    void renderingThread();

private:
    bool loadResources();
    
    //leap
private:
    LeapListener listener;
private:
    Controller controller;
    
    //sfml
private:
    sf::ContextSettings settings;
private:
    sf::Mutex settingsMu;
private:
    sf::RenderWindow window;
private:
    sf::Mutex windowMu;
    
    //resources
private:
    sf::Image icon;
//private:
//    sf::Texture texture;
//private:
//    sf::Sprite sprite;
private:
    sf::Font font;
private:
    sf::Music music;
//private:
//    sf::Text text;
    
private:
    bool running;
    bool rendering;
private:
    sf::Text texts[TEXTCNT];
    
    
private:
    Leap::Frame myFrame;
};
#endif /* defined(__LeapMotionOculusKeyboard__LMOC__) */

