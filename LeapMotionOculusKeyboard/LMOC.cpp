//
//  LMOC.cpp
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 09.11.13.
//  Copyright (c) 2013 Fachbereich Informatik. All rights reserved.
//

#include "LMOC.h"

#include <sstream>

LMOC::LMOC(){
    // Create the main window
    
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 0;
    
    window.create(sf::VideoMode(1280, 800), "Leap Motion (Oculus) Keyboard", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    
    // activate the window's context
    window.setActive(true);
    
    // get information about window (to check if everything is ok)
    settings = window.getSettings();
    std::cout << "depth bits:" << settings.depthBits << std::endl;
    std::cout << "stencil bits:" << settings.stencilBits << std::endl;
    std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
    std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;

    
    //LEAP***************************************
    controller.addListener(listener);
    //LEAP***************************************
    
    loadResources();
    
    // Play the music
    music.play();
    
    running=true;
    rendering=true;
}

LMOC::~LMOC(){
    std::cout << "~LMOC()" << std::endl;
    controller.removeListener(listener);
}

bool LMOC::loadResources(){
    // Set the Icon
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
		std::cerr << "icon not found" << std::endl;
        return false;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    // Load a sprite to display
//    if (!texture.loadFromFile(resourcePath() + "cute_image.jpg")) {
//		std::cerr << "image not found" << std::endl;
//        return false;
//    }
//    sprite.setTexture(texture);
//    sprite.setScale(((float)window.getSize().x/(float)texture.getSize().x), ((float)window.getSize().y/(float)texture.getSize().y));
    
    // Create a graphical text to display
//    if (!font.loadFromFile(resourcePath() + "sensation.ttf")) {
//		std::cerr << "font not found" << std::endl;
//        return false;
//    }
    if (!font.loadFromFile(resourcePath() + "stan0757.ttf")) {
		std::cerr << "font not found" << std::endl;
        return false;
    }
//    text.setColor(sf::Color::Black);
//    text.setFont(font);
//    text.setCharacterSize(8);
//    text.setString(s);
    
    sf::String s = "Hello SFML";
    for (int i=0; i<TEXTCNT; i++) {
        texts[i].setColor(sf::Color::White);
        texts[i].setFont(font);
        texts[i].setCharacterSize(8);
        texts[i].setString(s);
        texts[i].setPosition(1,(i*(texts[i].getCharacterSize()+1))+1);
    }
    
    // Load a music to play
    if (!music.openFromFile(resourcePath() + "nice_music.ogg")) {
		std::cerr << "music not found" << std::endl;
        return false;
    }
    
    return true;
}

void LMOC::renderingThread()
{
    std::cout << "hellow from renderingThread" << std::endl;
    // the rendering loop
    while (rendering)
    {
        
        
        
        
        //post
        window.pushGLStates();
        window.clear();
        for (int i=0; i<TEXTCNT; i++) {
            window.draw(texts[i]);
        }
        window.popGLStates();
        
        // Update the window
        window.display();
    }
    running=false;
    std::cout << "renderingThread done"<< std::endl;
}

int LMOC::run()
{
    std::cout << "hellow from run" << std::endl;
    while (running)
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed) {
                std::cout << "event sf::Event::Closed" << std::endl;
                rendering=false;
            }
            
            // Escape pressed : exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                std::cout << "event sf::Keyboard::Escape" << std::endl;
                rendering=false;
            }
        }
        myFrame = listener.frame;
        std::stringstream sstext;
        sstext << "Frame ID: " << myFrame.id();
        texts[0].setString(sstext.str());

        sstext.str("");sstext.clear();
        sstext << "Timestamp: " << myFrame.timestamp();
        texts[1].setString(sstext.str());
        
        sstext.str("");sstext.clear();
        sstext << "Hands Count: " << myFrame.hands().count();
        texts[2].setString(sstext.str());
        
        sstext.str("");sstext.clear();
        sstext << "Fingers Count: " << myFrame.fingers().count();
        texts[3].setString(sstext.str());
        
        sstext.str("");sstext.clear();
        sstext << "Tools Count: " << myFrame.tools().count();
        texts[4].setString(sstext.str());
        
        sstext.str("");sstext.clear();
        sstext << "Gestures Count: " << myFrame.gestures().count();
        texts[5].setString(sstext.str());
        
    }
    window.close();
    
    std::cout << "run done"<< std::endl;
    return EXIT_SUCCESS;
}