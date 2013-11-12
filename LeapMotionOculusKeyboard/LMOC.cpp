//
//  LMOC.cpp
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 09.11.13.
//  Copyright (c) 2013 Fachbereich Informatik. All rights reserved.
//

#include "LMOC.h"

#include <sstream>
#include <fstream>
#include <sfml/OpenGL.hpp>

LMOC::LMOC(){
    // Create the main window
    Eyes.initCam(0,0,120);
    objectCount=-1;
    viewanchor = 45.0;
    playerPos.x=0.0;
    playerPos.y=0.0;
    playerPos.z=0.0;
    keyCaps=false;
    stab = false;
    
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
    
    sf::Image timg;
    if (!timg.loadFromFile(resourcePath() + "keyboardTex.png")) {
        std::cerr << "image not found" << std::endl;
        return false;
    }
    timg.flipVertically();
    keyboardT.loadFromImage(timg);
    if (!timg.loadFromFile(resourcePath() + "keyboardTexCaps.png")) {
        std::cerr << "image not found" << std::endl;
        return false;
    }
    timg.flipVertically();
    keyboardTCaps.loadFromImage(timg);

    if (!font.loadFromFile(resourcePath() + "stan0757.ttf")) {
		std::cerr << "font not found" << std::endl;
        return false;
    }
    
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
    
    if (!shaders.loadFromFile(resourcePath() + "minimal.vert", resourcePath() + "minimal.frag")){
		std::cerr << "shaders not found" << std::endl;
        return false;
    }
    loadModel(resourcePath() + "keyboard.obj");
//    loadModel(resourcePath() + "fingertip.obj");
//    loadModel(resourcePath() + "palm.obj");
    return true;
}

bool LMOC::loadModel(sf::String path){
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "MODEL NOT FOUND!" << std::endl;
    }
    char prefix[50];
    while (file.good()) {
        file >> prefix;
        switch (prefix[0]) {
            case 'v': //point normal or texcoord
            {
                switch (prefix[1]) {
                    case 'o':
                        objectCount++;
                        break;
                    case 't': //texcoord
                    {
                        sf::Vector2f vt;
                        file >> vt.x >> vt.y;
                        vt_data.push_back(vt);
                    }
                        break;
                    case 'n': //normal
                    {
                        sf::Vector3f vn;
                        file >> vn.x >> vn.y >> vn.z;
                        vn_data.push_back(vn);
                    }
                        break;
                    default: //points
                    {
                        sf::Vector3f v;
                        file >> v.x >> v.y >> v.z;
                        v_data.push_back(v);
                    }
                        break;
                }
            }
                break;
            case 'f':
            {
                Face face;
                face.objId=objectCount;
                file >> face.vertInd.x >> face.vertInd.y >> face.vertInd.z;
                f_data.push_back(face);
                file >> face.vertInd.x >> face.vertInd.y >> face.vertInd.z;
                f_data.push_back(face);
                file >> face.vertInd.x >> face.vertInd.y >> face.vertInd.z;
                f_data.push_back(face);
            }
                break;
            default:
                break;
        }
    }
    file.close();
    for (int i=0; i<f_data.size(); i++) {
        Vertex vert;
        vert.objId=f_data[i].objId;
        vert.v=v_data[f_data[i].vertInd.x-1];
        vert.t=vt_data[f_data[i].vertInd.y-1];
        vert.n=vn_data[f_data[i].vertInd.z-1];
        vert_data.push_back(vert);
    }
    for (unsigned int i=0;i<vert_data.size()*3;i++){
        ind_data.push_back(i);
    }
    return true;
}

void LMOC::textThread(){
    while(rendering){
        myFrameMu.lock();
    myFrame = listener.frame;
        myFrameMu.unlock();
    std::stringstream sstext[TEXTCNT];
    sstext[0] << "Frame ID: " << myFrame.id();
    sstext[1] << "Timestamp: " << myFrame.timestamp();
    sstext[2] << "Hands Count: " << myFrame.hands().count();
    sstext[3] << "Fingers Count: " << myFrame.fingers().count();
    sstext[4] << "Tools Count: " << myFrame.tools().count();
    sstext[5] << "Gestures Count: " << myFrame.gestures().count();
        sstext[8] << "stab: " << stab;
    
    if (myFrame.hands().count()>0) {
        sstext[6] << "Hand 0 sphereRadius: " << myFrame.hands()[0].sphereRadius() << "mm";
        sstext[7] << "Hand 0 palmPosition: " << myFrame.hands()[0].palmPosition().x << " " << myFrame.hands()[0].palmPosition().y << " " << myFrame.hands()[0].palmPosition().z;
    }
    textsMu.lock();
    for (int i = 0; i<TEXTCNT; i++) {
        texts[i].setString(sstext[i].str());
    }
    textsMu.unlock();
    }
    std::cout << "textThread done" << std::endl;
}

void LMOC::renderThread()
{
    
    window.setActive(true);
    //initgl
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_DOUBLE);
    glEnable(GL_LIGHTING);
    glEnable(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearDepth(1.0f);
    //glClearColor(0.0f, 0.749f, 1.0f, 1.0f); //Cyan
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    {
        sf::Vector2u windowSize = window.getSize();
        gluPerspective(viewanchor,(float)windowSize.x/(float)windowSize.y, 0.1f, 10000.f);
    }
    glMatrixMode(GL_MODELVIEW);
    
    // the rendering loop
    bool firstRun=true;
    while (rendering)
    {
        window.setActive(true);
        window.clear();
        glEnable(GL_LIGHTING);
        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(Eyes.getCam().x+playerPos.x, Eyes.getCam().y+playerPos.y, Eyes.getCam().z+playerPos.z, playerPos.x, playerPos.y, playerPos.z, 0.0, 1.0, 0.0);
        
        if (keyCaps) {
            shaders.setParameter("texture", keyboardTCaps);
        }else{
            shaders.setParameter("texture", keyboardT);
        }
        //shaders.setParameter("texture", sf::Shader::CurrentTexture);
        sf::Shader::bind(&shaders);
        
        if (firstRun) {
            //********* VBO PREP
            glGenBuffers(NUM_VBO, VBO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, vert_data.size()*sizeof(Vertex),&(vert_data[0]),GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_data.size()*sizeof(unsigned int),&(ind_data[0]),GL_STATIC_DRAW);
            
            firstRun=false;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
        
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
        if (keyCaps) {
            sf::Texture::bind(&keyboardTCaps);
        }else{
            sf::Texture::bind(&keyboardT);
        }
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
        
        //draw
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
        glDrawElements(GL_TRIANGLES, ind_data.size(), GL_UNSIGNED_INT, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        sf::Shader::bind(NULL);
        
        myFrameMu.lock();
        Leap::Frame renderLeapFrame = myFrame;
        myFrameMu.unlock();
        
        
        glDisable(GL_LIGHTING);
        
        
        Leap::HandList hList = renderLeapFrame.hands();
        Leap::FingerList fList = renderLeapFrame.fingers();
        
        float scale=16.0f;
        float yOffset=8;
        
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINT);
        glVertex3f(0.0f, 0.0f-yOffset, 0.0f);
        glEnd();


        if (fast) {
            
        }else{
        if (stab) {
            glPointSize(10);
            glColor3f(0.0, 0.0, 1.0);
            glBegin(GL_POINTS);
            for (int i=0; i<hList.count(); i++) {
                Leap::Vector palmPos = hList[i].stabilizedPalmPosition();
                glVertex3f(palmPos.x/scale, (palmPos.y/scale)-yOffset, palmPos.z/scale);
            }
            glEnd();
            
            
            glPointSize(6);
            glColor3f(1.0, 1.0, 0.0);
            glBegin(GL_POINTS);
            for (int i=0; i<fList.count(); i++) {
                Leap::Vector finPos = fList[i].stabilizedTipPosition();
                glVertex3f(finPos.x/scale, (finPos.y/scale)-yOffset, finPos.z/scale);
            }
            glEnd();
        }else{
            glPointSize(10);
            glColor3f(0.0, 0.0, 1.0);
            glBegin(GL_POINTS);
            for (int i=0; i<hList.count(); i++) {
                Leap::Vector palmPos = hList[i].palmPosition();
                glVertex3f(palmPos.x/scale, (palmPos.y/scale)-yOffset, palmPos.z/scale);
            }
            glEnd();
        
        
            glPointSize(6);
            glColor3f(1.0, 1.0, 0.0);
            glBegin(GL_POINTS);
            for (int i=0; i<fList.count(); i++) {
                Leap::Vector finPos = fList[i].tipPosition();
                glVertex3f(finPos.x/scale, (finPos.y/scale)-yOffset, finPos.z/scale);
            }
            glEnd();
        }
        }
        
        
        
        
        //    // Get the hand's sphere radius and palm position
        //    currentState << "Hand sphere radius: " << hand.sphereRadius()
        //              << " mm, palm position: " << hand.palmPosition() << std::endl;
        //
        //    // Get the hand's normal vector and direction
        //    const Vector normal = hand.palmNormal();
        //    const Vector direction = hand.direction();
        //
        //    // Calculate the hand's pitch, roll, and yaw angles
        //    currentState << "Hand pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
        //              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
        //              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
        
        //post
        glFlush();
        window.pushGLStates();
        
        textsMu.lock();
        for (int i=0; i<TEXTCNT; i++) {
            window.draw(texts[i]);
        }
        textsMu.unlock();
        window.popGLStates();
        
        // Update the window
        window.display();
        window.setActive(false);


    }
    running=false;
    std::cout << "renderingThread done"<< std::endl;
}

int LMOC::run()
{
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
            
            //mouse wheel
            if (event.type == sf::Event::MouseWheelMoved){
                Eyes.zoomCam(event.mouseWheel.delta);
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
                keyCaps=!keyCaps;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                stab=!stab;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) {
                fast=!fast;
            }
        }
        
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            sf::Vector2i mousePos= sf::Mouse::getPosition();
            Eyes.mouseMove(mousePos.x, mousePos.y);
            Eyes.setDown(true);
        }
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && Eyes.isDown()){
            Eyes.mouseRelease();
            Eyes.setDown(false);
        }
    }
    window.close();
    
    std::cout << "run done"<< std::endl;
    return EXIT_SUCCESS;
}