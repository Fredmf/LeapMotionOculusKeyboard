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

#include "Matrices.h"

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
    edgefalloff = 0.5f;
    intensity = 0.5f;
    ambient = 0.5f;
    
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
    if (!timg.loadFromFile(resourcePath() + "fingerTex.png")) {
        std::cerr << "image not found" << std::endl;
        return false;
    }
    timg.flipVertically();
    fingerT.loadFromImage(timg);
    if (!timg.loadFromFile(resourcePath() + "palmTex.png")) {
        std::cerr << "image not found" << std::endl;
        return false;
    }
    timg.flipVertically();
    palmT.loadFromImage(timg);
    
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
    
    if (!keyboardS.loadFromFile(resourcePath() + "keyboardS.vert", resourcePath() + "keyboardS.frag")){
		std::cerr << "shaders not found" << std::endl;
        return false;
    }
    if (!handS.loadFromFile(resourcePath() + "handS.vert", resourcePath() + "handS.frag")){
		std::cerr << "shaders not found" << std::endl;
        return false;
    }
    
    loadModel(resourcePath() + "keyboard.obj",&keyboardVert_data,&keyboardInd_data);
    std::cout << "keyok" << std::endl;
    loadModel(resourcePath() + "fingertip.obj",&fingerVert_data,&fingerInd_data);
    std::cout << "fingerok" << std::endl;
    loadModel(resourcePath() + "palm.obj",&palmVert_data,&palmInd_data);
    std::cout << "palmok" << std::endl;
    return true;
}

bool LMOC::loadModel(sf::String path,std::vector<Vertex> *vert_data, std::vector<unsigned int> *ind_data){
    std::vector<sf::Vector3f> v_data;
    std::vector<sf::Vector2f> vt_data;
    std::vector<sf::Vector3f> vn_data;
    std::vector<Face> f_data;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "MODEL NOT FOUND!" << std::endl;
    }
    char prefix[50];
    while (file.good()) {
        std::cout << "good: " << file.good() << std::endl;
        file >> prefix;
        std::cout << prefix << std::endl;
        switch (prefix[0]) {
            case 'v': //point normal or texcoord
            {
                switch (prefix[1]) {
                    case 'o':
                    {
                        objectCount++;
                        std::string bla;
                        file >> bla;
                        std::cout << bla << std::endl;
                    }
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
        
        std::cout << "good: " << file.good() << std::endl;
    }
    file.close();
    std::cout << v_data.size() << " " << vt_data.size() << " " << vn_data.size() << std::endl;
    for (int i=0; i<f_data.size(); i++) {
        Vertex vert;
        vert.objId=f_data[i].objId;
        vert.v=v_data[f_data[i].vertInd.x-1];
        vert.t=vt_data[f_data[i].vertInd.y-1];
        vert.n=vn_data[f_data[i].vertInd.z-1];
        vert_data->push_back(vert);
    }
    for (unsigned int i=0;i<vert_data->size()*3;i++){
        ind_data->push_back(i);
    }
    return true;
}

void LMOC::textThread(){
    while(rendering){
        
        std::stringstream sstext[TEXTCNT];
        myFrameMu.lock();
        myFrame = listener.frame;
        sstext[0] << "Frame ID: " << myFrame.id();
        myFrameMu.unlock();
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
    //glEnable(GL_LIGHTING);
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
            keyboardS.setParameter("texture", keyboardTCaps);
        }else{
            keyboardS.setParameter("texture", keyboardT);
        }
        handS.setParameter("texture", palmT);
        
        handS.setParameter("edgefalloff", edgefalloff);
        handS.setParameter("intensity", intensity);
        handS.setParameter("ambient", ambient);
        
        
        //shaders.setParameter("texture", sf::Shader::CurrentTexture);
        sf::Shader::bind(&keyboardS);
        
        if (firstRun) {
            //********* VBO PREP
            glGenBuffers(NUM_VBO, VBO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, keyboardVert_data.size()*sizeof(Vertex),&(keyboardVert_data[0]),GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, keyboardInd_data.size()*sizeof(unsigned int),&(keyboardInd_data[0]),GL_STATIC_DRAW);
            
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glBufferData(GL_ARRAY_BUFFER, palmVert_data.size()*sizeof(Vertex),&(palmVert_data[0]),GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, palmInd_data.size()*sizeof(unsigned int),&(palmInd_data[0]),GL_STATIC_DRAW);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
            glBufferData(GL_ARRAY_BUFFER, fingerVert_data.size()*sizeof(Vertex),&(fingerVert_data[0]),GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[5]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, fingerInd_data.size()*sizeof(unsigned int),&(fingerInd_data[0]),GL_STATIC_DRAW);
            
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
        glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        sf::Shader::bind(NULL);
        
        myFrameMu.lock();
        Leap::Frame renderLeapFrame = myFrame;
        myFrameMu.unlock();
        Leap::HandList hList = renderLeapFrame.hands();
        Leap::FingerList fList = renderLeapFrame.fingers();
        float scale=1.0f/16.0f;
        float yOffset=8;
        
        //////////////////////////////////////////////////// Haende
        std::vector<Leap::Matrix> matrixVector;
        std::vector<Leap::Matrix> matrixVectorF;
        for( int h = 0; h < hList.count(); h++ )
        {
            Leap::Hand leapHand = hList[h];

            Leap::Vector handXBasis =  leapHand.palmNormal().cross(leapHand.direction()).normalized();
            Leap::Vector handYBasis = -leapHand.palmNormal();
            Leap::Vector handZBasis = -leapHand.direction();
            Leap::Vector handOrigin =  leapHand.palmPosition();
            handOrigin *= scale;
            handOrigin.y -= yOffset;
            Leap::Matrix handTransform = Leap::Matrix(handXBasis, handYBasis, handZBasis, handOrigin);
            
            for( int f = 0; f < leapHand.fingers().count(); f++ )
            {
                Leap::Finger leapFinger = leapHand.fingers()[f];
                Leap::Vector transformedPosition = handTransform.transformPoint(leapFinger.tipPosition());
                Leap::Vector transformedDirection = handTransform.transformDirection(leapFinger.direction());
                
                Leap::Vector fingerXBasis = leapHand.palmNormal().cross(transformedDirection).normalized();
                Leap::Vector fingerYBasis = -leapHand.palmNormal();
                Leap::Vector fingerZBasis = -transformedDirection;
                Leap::Vector fingerOrigin = leapFinger.tipPosition();
                fingerOrigin *= scale;
                fingerOrigin.y -= yOffset;
                Leap::Matrix fingerTransform = Leap::Matrix(fingerXBasis, fingerYBasis, fingerZBasis, fingerOrigin);
                matrixVectorF.push_back(fingerTransform);
            }
            
            matrixVector.push_back(handTransform);
        }
        
        
        sf::Shader::bind(&handS);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
            
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
            
            sf::Texture::bind(&palmT);
            
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
            
            //draw
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
        
            for (int i=0; i<matrixVector.size(); i++) {
                glPushMatrix();
                glMultMatrixf(matrixVector[i].toArray4x4().m_array);
                glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
                glPopMatrix();
            }
        
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            
            
       // }
        sf::Shader::bind(NULL);
        
        //////////////////////////////////////////////////// Finger
        
        handS.setParameter("texture", fingerT);
        sf::Texture::bind(&fingerT);
        sf::Shader::bind(&handS);
        
        
//
            glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
            
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
        
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
            
            //draw
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[5]);
            
        
            for (int i=0; i<matrixVectorF.size()    ; i++) {
                glPushMatrix();
                glMultMatrixf(matrixVectorF[i].toArray4x4().m_array);
                glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
                glPopMatrix();
            }
        
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            sf::Shader::bind(NULL);
        
        
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