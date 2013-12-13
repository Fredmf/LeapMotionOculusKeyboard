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

#include "Vectors.h"

//leap
#include "LeapListener.h"
//#include <Leap.h>

//oculus
#include <OVR.h>

#include "Cam.h"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#elif WIN32
#include "ResourcePath.h"
#endif


class LMOC {
    struct Vertex{
        sf::Vector3f v;
        sf::Vector3f n;
        sf::Vector2f t;
        int objId;
    };
    struct Face{
        sf::Vector3i vertInd;
        int objId;
    };

	struct GraphObj{
		sf::Vector3f min;
		sf::Vector3f max;
		int objId;
		std::string name;
	};
    
public:
    LMOC();
public:
    ~LMOC();
    
public:
    void checkInput();
public:
    void render();
public:
    void renderInit();
public:
    void textThread();
public:
    void matrixThread();
private:
	void checkEvents();
private:
	std::vector<std::string> touchedObjects(Leap::Vector tipP);
	void touchedObjectsV(Leap::Vector tipP);
	void touchedObjectsPil(Leap::Vector tipP);

private:
    bool loadResources();
private:
    bool loadModel(sf::String path,std::vector<Vertex> *vert_data, std::vector<unsigned int> *ind_data, bool withBounds);
    
    //leap
private:
    LeapListener listener;
private:
    Controller controller;
private:
	bool firstRun;
    //sfml
private:
    sf::ContextSettings settings;
private:
    sf::RenderWindow window;
private:
    sf::Shader keyboardS;
    sf::Shader handS;
    
private:
    std::vector<Leap::Matrix> matrixVectorHands;
    std::vector<Leap::Matrix> matrixVectorFingers;
    sf::Mutex MatrixMu;
    
    //resources
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
    sf::Mutex myFrameMu;
    
private:
    GLuint VBO[NUM_VBO];
    Cam Eyes;
    
private:
    
    std::vector<Vertex> keyboardVert_data;
    std::vector<unsigned int> keyboardInd_data;
    std::vector<Vertex> palmVert_data;
    std::vector<unsigned int> palmInd_data;
    std::vector<Vertex> fingerVert_data;
    std::vector<unsigned int> fingerInd_data;
    float edgefalloff;
    float intensity;
    float ambient;
    
private:
    Vector3 playerPos;
    float viewanchor;
private:
    int objectCount;
	std::vector<GraphObj> objBounds;
	unsigned int objDraw;
//oculus
private:
	OVR::Ptr<OVR::DeviceManager> pManager;
	OVR::Ptr<OVR::HMDDevice> pHMD;
	OVR::Ptr<OVR::SensorDevice> pSensor;
	OVR::HMDInfo hmd;
};
#endif /* defined(__LeapMotionOculusKeyboard__LMOC__) */

