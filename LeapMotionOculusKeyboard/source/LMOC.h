//
//  LMOC.h
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 09.11.13.
//  Copyright (c) 2013 Fachbereich Informatik. All rights reserved.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.

#ifndef __LeapMotionOculusKeyboard__LMOC__
#define __LeapMotionOculusKeyboard__LMOC__

#define TEXTCNT 20
#define NUM_VBO 6
#define BUFFER_OFFSET(i) ((void*)(i))

#ifdef __APPLE__
#include <SFML/OpenGL.hpp>
#elif WIN32
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <LeapMath.h>

#include <string>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Vectors.h"

//leap
#include "LeapListener.h"
//#include <Leap.h>

//oculus
#include <OVR.h>

#include "Cam.h"
#include "CamOVR.h"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#elif WIN32
#include "ResourcePath.h"
#endif

class LMOC {
    //GLuint textures
///////////////////////////////////DATA TYPES
    struct Vertex{
        glm::vec3 v;
        glm::vec3 n;
        glm::vec2 t;
        float objId;
    };
    struct Face{
        glm::ivec3 vertInd;
        float objId;
    };

	struct GraphObj{
        glm::vec3 min;
		glm::vec3 max;
		float objId;
		std::string name;
	};
    
///////////////////////////////////KON//DEST
public:
    LMOC();
public:
    ~LMOC();

    
///////////////////////////////////MAIN LOOP
public:
    void runLoop();

    
///////////////////////////////////MODULES
public:
    void checkInput();
public:
    void render();
public:
    void renderDebugText();
public:
    void renderInit();
public:
    void textThread();
public:
    void leapMatrix();
private:
	void checkEvents();
//PHYSICS
private:
	void touchedObjects(Leap::Vector tipP);
    glm::mat4 zeroMat;
    glm::mat4 touchedOMat;
    void calcColMat(void);
    std::vector<float> toucedFingers;
//LOAD UTILS
private:
    bool loadResources();
private:
    bool loadModel(sf::String path,std::vector<Vertex> *vert_data, std::vector<unsigned int> *ind_data, bool withBounds);
//////////////////////////////////OCULUS
private:
    bool renderLeftEye;
    
    
//////////////////////////////////LEAP MOTION
private:
    LeapListener listener;
private:
    Controller controller;
    
    
    
///////////////////////////////////ATRIBUTES
private:
	bool firstRun;
    bool fullscreen;
    //sfml
private:
    sf::ContextSettings settings;
private:
    sf::RenderWindow window;
private:
    GLuint lmocShader;
    
private:
    std::vector<Leap::Matrix> matrixVectorHands;
    std::vector<Leap::Matrix> matrixVectorFingers;
    sf::Mutex MatrixMu;
    
    sf::Shader myshader;
    //resources
    //TEXTURES
private:
    sf::Image icon;
private:
    sf::Texture keyboardT;
    sf::Texture keyboardTCaps;
    sf::Texture palmT;
    sf::Texture fingerT;
    bool keyCaps;
    bool stab;
    bool fast;
private:
    sf::Font font;
private:
    sf::Music music;
    
private:
    bool running;
    bool rendering;
private:
    sf::Text texts[TEXTCNT];
    sf::Mutex textsMu;
    
    
private:
    Leap::Frame myFrame;
    sf::Mutex myFrameMu;
    
private:
    GLuint VBO[NUM_VBO];
    Cam Eyes;
    CamOVR EyesOVR;
    
private:
    std::vector<Vertex> keyboardVert_data;
    std::vector<unsigned int> keyboardInd_data;
    std::vector<Vertex> palmVert_data;
    std::vector<unsigned int> palmInd_data;
    std::vector<Vertex> fingerVert_data;
    std::vector<unsigned int> fingerInd_data;
    GLint attribute_u_keyId;
    
private:
    glm::vec3 playerPos;
    float viewanchor;
private:
    int objectCount;
	std::vector<GraphObj> objBounds;
	unsigned int objDraw;
//oculus
private:
    OVR::System pOVRSystem;
	OVR::Ptr<OVR::DeviceManager> pManager;
	OVR::Ptr<OVR::HMDDevice> pHMD;
	OVR::Ptr<OVR::SensorDevice> pSensor;
	OVR::HMDInfo hmd;
    bool oculusConnected;
};
#endif /* defined(__LeapMotionOculusKeyboard__LMOC__) */

