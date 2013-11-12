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
#define NUM_VBO 2
#define BUFFER_OFFSET(i) ((void*)(i))

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
    struct Vertex{
        sf::Vector3f v;
        sf::Vector3f n;
        sf::Vector2f t;
    };
    
public:
    LMOC();
public:
    ~LMOC();
    
public:
    int run();
public:
    void renderThread();

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
    sf::RenderWindow window;
private:
    sf::Shader shaders;
    
    //resources
private:
    sf::Image icon;
private:
    sf::Texture texture;
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
    sf::Mutex textsMu;
    
    
private:
    Leap::Frame myFrame;
    
private:
    GLuint VBO[NUM_VBO];
    
private:
    std::vector<sf::Vector3f> v_data;
    std::vector<sf::Vector2f> vt_data;
    std::vector<sf::Vector3f> vn_data;
    std::vector<sf::Vector3i> f_data;
    
    std::vector<Vertex> vert_data;
    std::vector<unsigned int> ind_data;
    
private:
    sf::Vector3f playerPos;
    float viewanchor;
};
#endif /* defined(__LeapMotionOculusKeyboard__LMOC__) */

