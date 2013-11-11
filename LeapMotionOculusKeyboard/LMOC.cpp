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
    
    if (!texture.loadFromFile(resourcePath() + "keyboardTex.png")) {
        std::cerr << "image not found" << std::endl;
        return false;
    }
    
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
    
    if (!shaders.loadFromFile(resourcePath() + "minimal.vert", resourcePath() + "minimal.frag")){
		std::cerr << "shaders not found" << std::endl;
        return false;
    }
    
    std::vector<float> v_data;
    std::vector<float> vt_data;
    std::vector<float> vn_data;
    std::vector<unsigned short> f_data;
    
    std::ifstream file(resourcePath() + "keyboard.fm");
    char prefix[50];
    int vertexCount, texcoordCount, normalCount, faceCount;
    file >> vertexCount >> texcoordCount >> normalCount >> faceCount;
    //std::cout << "cv: " << vertexCount << " cvt: " << texcoordCount << " cvn: " << normalCount << " cf: " << faceCount << std::endl;
    while (file.good()) {
        file >> prefix;
        switch (prefix[0]) {
            case 'v': //point normal or texcoord
                switch (prefix[1]) {
                    case 't': //texcoord
                        float u,v;
                        file >> u >> v;
                        vt_data.push_back(u);
                        vt_data.push_back(v);
                        break;
                    case 'n': //normal
                        float xn,yn,zn;
                        file >> xn >> yn >> zn;
                        vn_data.push_back(xn);
                        vn_data.push_back(yn);
                        vn_data.push_back(zn);
                        break;
                    default: //points
                        float x,y,z;
                        file >> x >> y >> z;
                        v_data.push_back(x);
                        v_data.push_back(y);
                        v_data.push_back(z);
                        break;
                }
                break;
            case 'f':
                unsigned short av,avt,avn,bv,bvt,bvn,cv,cvt,cvn;
                file >> av >> avt >> avn >> bv >> bvt >> bvn >> cv >> cvt >> cvn;
                f_data.push_back(av);
                f_data.push_back(avt);
                f_data.push_back(avn);
                f_data.push_back(bv);
                f_data.push_back(bvt);
                f_data.push_back(bvn);
                f_data.push_back(cv);
                f_data.push_back(cvt);
                f_data.push_back(cvn);
                break;
            default:
                break;
        }
    }
    file.close();
    for (int i=0; i<f_data.size(); i++) {
        Vertex vert;
        vert.x=v_data[(f_data[i]-1)*3];
        vert.y=v_data[(f_data[i]-1)*3+1];
        vert.z=v_data[(f_data[i]-1)*3+2];
        vert.u=vt_data[(f_data[i+1]-1)*2];
        vert.v=vt_data[(f_data[i+1]-1)*2+1];
        vert.nx=vn_data[(f_data[i+2]-1)*3];
        vert.ny=vn_data[(f_data[i+2]-1)*3+1];
        vert.nz=vn_data[(f_data[i+2]-1)*3+2];
//        std::cout << i << ": " << tmp.x << " " << tmp.y << " " << tmp.z << " "
//        << tmp.u << " " << tmp.v << " "
//        << tmp.nx << " " << tmp.ny << " " << tmp.nz << " " << std::endl;
        vert_data.push_back(vert);
    }
    for (unsigned int i=1;i<=vert_data.size();i++){
        ind_data.push_back(i);
    }
    
    return true;
}

void LMOC::renderThread()
{
    // the rendering loop
    bool firstRun=true;
    while (rendering)
    {


    }
    running=false;
    std::cout << "renderingThread done"<< std::endl;
}

int LMOC::run()
{
    bool firstRun = true;
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
        std::stringstream sstext[TEXTCNT];
        sstext[0] << "Frame ID: " << myFrame.id();
        sstext[1] << "Timestamp: " << myFrame.timestamp();
        sstext[2] << "Hands Count: " << myFrame.hands().count();
        sstext[3] << "Fingers Count: " << myFrame.fingers().count();
        sstext[4] << "Tools Count: " << myFrame.tools().count();
        sstext[5] << "Gestures Count: " << myFrame.gestures().count();
        
        if (myFrame.hands().count()>0) {
            sstext[6] << "Hand 0 sphereRadius: " << myFrame.hands()[0].sphereRadius() << "mm";
            sstext[7] << "Hand 0 palmPosition: " << myFrame.hands()[0].palmPosition().x << " " << myFrame.hands()[0].palmPosition().y << " " << myFrame.hands()[0].palmPosition().z;
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
        
        
        textsMu.lock();
        for (int i = 0; i<TEXTCNT; i++) {
            texts[i].setString(sstext[i].str());
        }
        textsMu.unlock();
        
        
        //RENDER THREAD HERE**********************************************
        
        window.setActive(true);
        //window.clear();
        sf::Shader::bind(&shaders);
        
        if (firstRun) {
            //********* VBO PREP
            glGenBuffers(NUM_VBO, VBO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, vert_data.size()*sizeof(Vertex),&(vert_data[0]),GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_data.size()*sizeof(unsigned short),&(ind_data[0]),GL_STATIC_DRAW);
            
            firstRun=false;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT,sizeof(Vertex), BUFFER_OFFSET(0));
        
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
        
        sf::Texture::bind(&texture);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
        
        //draw
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
        glDrawElements(GL_POINTS, ind_data.size(), GL_UNSIGNED_SHORT, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        //sf::Shader::bind(NULL);
        //post
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
    window.close();
    
    std::cout << "run done"<< std::endl;
    return EXIT_SUCCESS;
}