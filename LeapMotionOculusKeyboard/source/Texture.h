//
//  Texture.h
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 20.12.13.
//  Copyright (c) 2013 Fachbereich Informatik. All rights reserved.
//

#ifndef __LeapMotionOculusKeyboard__Texture__
#define __LeapMotionOculusKeyboard__Texture__

#ifdef __APPLE__
#include <SFML/OpenGL.hpp>
#elif WIN32
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

class Texture{
public:
    enum {
        TEX_KEY,
        TEX_KEYS,
        TEX_FINGER,
        TEX_HAND,
        NUM_TEXTURES
    };
    
public:
    Texture();
public:
    ~Texture();
public:
    void loadFromFile(std::string src,unsigned int tex);
public:
    void bind(unsigned int tex);
public:
    void gen(void);
public:
    GLuint textures[NUM_TEXTURES+1];
};


#endif /* defined(__LeapMotionOculusKeyboard__Texture__) */
