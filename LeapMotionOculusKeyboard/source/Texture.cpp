//
//  Texture.cpp
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 20.12.13.
//  Copyright (c) 2013 Fachbereich Informatik. All rights reserved.
//

#include "Texture.h"
void checkGLErro(const char* prefix = "")
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << prefix << " GL_ERROR: " << gluErrorString(err) << " (0x" << std::hex << err << ")" << std::endl;
	}
}

Texture::Texture(){
    
}

Texture::~Texture(){
    
}

void Texture::gen(){
    glGenTextures(NUM_TEXTURES, textures);
    checkGLErro("Texture::gen");
}

void Texture::loadFromFile(std::string src,unsigned int tex){
    
    if(tex>=NUM_TEXTURES){
        std::cerr << "invalid texture type, define texture in Texture.h enum" << std::endl;
    }else{
        sf::Image img;
        if (!img.loadFromFile(src)) {
            std::cerr << "image not found: "<< src << std::endl;
        }
        img.flipVertically();
        
        glBindTexture(GL_TEXTURE_2D, textures[tex]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
}

void Texture::bind(unsigned int tex){
    if(tex>=NUM_TEXTURES){
        std::cerr << "invalid texture type, define texture in Texture.h enum" << std::endl;
    }else{
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0+tex);
        glBindTexture(GL_TEXTURE_2D, textures[tex]);
        glMatrixMode(GL_MODELVIEW);
    }
}