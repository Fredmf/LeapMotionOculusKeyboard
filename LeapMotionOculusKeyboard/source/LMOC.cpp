//
//  LMOC.cpp
//  LeapMotionOculusKeyboard
//
//  Created by FredMF on 09.11.13.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.

//HMD Variables (Head Mounted Display) size in meters
//149.76 x 93,6 mm
#define HSCREENSIZE 0.14976
#define VSCREENSIZE 0.0935
//center vertival 93,6 mm /2  in meters
#define VSCREENCENTER 0,04675

#include "LMOC.h"
#include <sstream>
#include <fstream>

#include "ran.h"

// vec3, vec4, ivec4, mat4
#include <glm/glm.hpp>

// translate, rotate, scale, perspective
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
////__for
//glVertex3fv(glm::value_ptr(v))
//glLoadMatrixfv(glm::value_ptr(m));
////__alternative without
//glVertex3fv(&v[0]);
//glLoadMatrixfv(&m[0][0]);

//#include <gli/gli.hpp>


#include "Matrices.h"
#include "Shader.h"



void checkGLError(const char* prefix = "")
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << prefix << " GL_ERROR: " << gluErrorString(err) << " (0x" << std::hex << err << ")" << std::endl;
	}
}

LMOC::LMOC(){
	//init oculus
//	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
//	pManager = *OVR::DeviceManager::Create();
//	pHMD = *pManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
//    
//    if (pHMD) {
//        if (pHMD->GetDeviceInfo(&hmd))
//        {
//            std::cout << "\n--- HMD INFO ---" <<
//            "\nMonitorName: " << hmd.DisplayDeviceName <<
//            "\nEyeDistance: " << hmd.InterpupillaryDistance <<
//            "\nDistortionK[0]: " << hmd.DistortionK[0] <<
//            "\nDistortionK[1]: " << hmd.DistortionK[1] <<
//            "\nDistortionK[2]: " << hmd.DistortionK[2] <<
//            "\nDistortionK[3]: " << hmd.DistortionK[3] << std::endl << std::endl;
//        }
//    }
	
	
    
    // Create the main window
	objDraw=0;
    Eyes.initCam(0,50,40);
	Eyes.mouseMove(glm::ivec2(0,0));
	Eyes.mouseRelease();
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
    
    //window.create(sf::VideoMode(1920, 1080), "Leap Motion Oculus Keyboard", sf::Style::Fullscreen, settings);
	window.create(sf::VideoMode(1920, 1080), "Leap Motion Oculus Keyboard", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    
    // activate the window's context
    window.setActive(true);
//    
//    // get information about window (to check if everything is ok)
//    settings = window.getSettings();
//    std::cout << "depth bits:" << settings.depthBits << std::endl;
//    std::cout << "stencil bits:" << settings.stencilBits << std::endl;
//    std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
//    std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;
//#ifdef WIN32
//	GLenum err = glewInit();
//	if (GLEW_OK != err)
//	{
//		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
//	}
//#endif
    
    
    //LEAP***************************************
    controller.addListener(listener);
    //LEAP***************************************
    
    loadResources();
    
    // Play the music
    music.play();
    
    running=true;
    rendering=true;
	firstRun=true;
}

LMOC::~LMOC(){
    std::cout << "~LMOC()" << std::endl;
    controller.removeListener(listener);
}

std::vector<std::string> LMOC::touchedObjects(Leap::Vector tipP){
	std::vector<std::string> output;
	for(unsigned int i=0;i<objBounds.size();i++){
		if(tipP.x>=objBounds[i].min.x &&
           tipP.x<=objBounds[i].max.x &&
           tipP.y>=objBounds[i].min.y &&
           tipP.y<=objBounds[i].max.y &&
           tipP.z>=objBounds[i].min.z &&
           tipP.z<=objBounds[i].max.z){
            std::cout << objBounds[i].name << std::endl;
		}
	}
	return output;
}

void LMOC::touchedObjectsV(Leap::Vector tipP){
	std::vector<std::string> output;
	for(unsigned int i=0;i<objBounds.size();i++){
		if(tipP.x>=objBounds[i].min.x &&
           tipP.x<=objBounds[i].max.x &&
           tipP.y>=objBounds[i].min.y &&
           tipP.y<=objBounds[i].max.y &&
           tipP.z>=objBounds[i].min.z &&
           tipP.z<=objBounds[i].max.z){
            std::cout << objBounds[i].name << std::endl;
		}
	}
}

void LMOC::touchedObjectsPil(Leap::Vector tipP){
	std::vector<std::string> output;
	for(unsigned int i=0;i<objBounds.size();i++){
		if(tipP.x>=objBounds[i].min.x &&
           tipP.x<=objBounds[i].max.x &&
           tipP.z>=objBounds[i].min.z &&
           tipP.z<=objBounds[i].max.z){
            std::cout << objBounds[i].name << " using Keytap in Pillar" << std::endl;
		}
	}
}

bool LMOC::loadResources(){
    //ICON
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
		std::cerr << "icon not found" << std::endl;
        return false;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    //SHADERS
    if (!loadShader(lmocShader,resourcePath() + "lmocShader.vert",resourcePath() + "lmocShader.frag")){
		std::cerr << "shaders not found under: " << resourcePath() << std::endl;
        return false;
    }
    
    /////////////////////////////////////////////////////////TEXTURES
    
    sf::Image img;
    img.loadFromFile(resourcePath() + "keyboardTex.png");
    img.flipVertically();
    keyboardT.loadFromImage(img);
    
    img.loadFromFile(resourcePath() + "keyboardTexCaps.png");
    img.flipVertically();
    keyboardTCaps.loadFromImage(img);
    
    img.loadFromFile(resourcePath() + "palmTex.png");
    img.flipVertically();
    palmT.loadFromImage(img);
    
    img.loadFromFile(resourcePath() + "fingerTex.png");
    img.flipVertically();
    fingerT.loadFromImage(img);
    
    
    //FONT
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
        texts[i].setPosition(1,((float)i*(texts[i].getCharacterSize()+1))+1);
    }
    
    // Load a music to play
    if (!music.openFromFile(resourcePath() + "nice_music.ogg")) {
		std::cerr << "music not found" << std::endl;
        return false;
    }
    
    loadModel(resourcePath() + "keyboard.obj",&keyboardVert_data,&keyboardInd_data,true);
    loadModel(resourcePath() + "fingertip.obj",&fingerVert_data,&fingerInd_data,false);
    loadModel(resourcePath() + "palm.obj",&palmVert_data,&palmInd_data,false);
    return true;
}

bool LMOC::loadModel(sf::String path,std::vector<Vertex> *vert_data, std::vector<unsigned int> *ind_data,bool withBounds){
    std::vector<glm::vec3> v_data;
    std::vector<glm::vec2> vt_data;
    std::vector<glm::vec3> vn_data;
    std::vector<Face> f_data;
    
	bool newObj = true;
    
	std::ifstream file(path.toAnsiString());
    if (!file.is_open()) {
        std::cerr << "MODEL NOT FOUND!" << std::endl;
    }
    char prefix[50];
    while (file.good()) {
        file >> prefix;
        switch (prefix[0]) {
			case 'o':
            {
                if(withBounds){
                    std::string dataStr;
                    file >> dataStr;
                    objectCount++;
                    GraphObj tmp;
                    tmp.objId=objectCount;
                    tmp.name=dataStr;
                    newObj=true;
                    objBounds.push_back(tmp);
                }
            }
                break;
            case 'v': //point normal or texcoord
            {
                switch (prefix[1]) {
                    case 't': //texcoord
                    {
                        glm::vec2 vt;
                        file >> vt.x >> vt.y;
                        vt_data.push_back(vt);
                    }
                        break;
                    case 'n': //normal
                    {
                        glm::vec3 vn;
                        file >> vn.x >> vn.y >> vn.z;
                        vn_data.push_back(vn);
                    }
                        break;
                    default: //points
                    {
                        glm::vec3 v;
                        file >> v.x >> v.y >> v.z;
						if (withBounds){
                            if(newObj){
                                objBounds[objectCount].max.x=v.x;
                                objBounds[objectCount].max.y=v.y;
                                objBounds[objectCount].max.z=v.z;
                                objBounds[objectCount].min.x=v.x;
                                objBounds[objectCount].min.y=v.y;
                                objBounds[objectCount].min.z=v.z;
                                newObj=false;
                            }else{
                                if(objBounds[objectCount].max.x < v.x){
                                    objBounds[objectCount].max.x=v.x;}
                                if(objBounds[objectCount].max.y < v.y){
                                    objBounds[objectCount].max.y=v.y;}
                                if(objBounds[objectCount].max.z < v.z){
                                    objBounds[objectCount].max.z=v.z;}
                                if(objBounds[objectCount].min.x > v.x){
                                    objBounds[objectCount].min.x=v.x;}
                                if(objBounds[objectCount].min.y > v.y){
                                    objBounds[objectCount].min.y=v.y;}
                                if(objBounds[objectCount].min.z > v.z){
                                    objBounds[objectCount].min.z=v.z;}
                            }
						}
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
    for (unsigned int i=0; i<f_data.size(); i++) {
        Vertex vert;
        vert.objId=f_data[i].objId;
        std::cout << vert.objId << std::endl;
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
    std::stringstream sstext[TEXTCNT];
    myFrame = listener.frame;
    sstext[0].str(std::string());
    sstext[0] << "Frame ID: " << myFrame.id();
    sstext[1].str(std::string());
    sstext[1] << "Timestamp: " << myFrame.timestamp();
    sstext[2].str(std::string());
    sstext[2] << "Hands Count: " << myFrame.hands().count();
    sstext[3].str(std::string());
    sstext[3] << "Fingers Count: " << myFrame.fingers().count();
    sstext[4].str(std::string());
    sstext[4] << "Tools Count: " << myFrame.tools().count();
    sstext[5].str(std::string());
    sstext[5] << "Gestures Count: " << myFrame.gestures().count();
    sstext[8].str(std::string());
    sstext[8] << "stab: " << stab;
    for (int i = 0; i<TEXTCNT; i++) {
        texts[i].setString(sstext[i].str());
    }
}

void LMOC::leapMatrix(){
    /////////////////////////// TO MUCH OVERHEAD AS A THREAD
    /////////////////// CALCULATE THE TRANSFORM MATRICES FOR THE HANDS AND FINGERS
    float scale=0.0625f;  //1.0f/16.0f;
    float yOffset=8;
    
    Leap::HandList handList = listener.frame.hands();
	Leap::GestureList gestList = listener.frame.gestures();
    matrixVectorHands.clear();
    matrixVectorFingers.clear();
    
    for (int h=0; h<handList.count(); h++) {
        Leap::Hand leapHand = handList[h];
        
        Leap::Vector handXBasis =  leapHand.palmNormal().cross(leapHand.direction()).normalized();
        Leap::Vector handYBasis = -leapHand.palmNormal();
        Leap::Vector handZBasis = -leapHand.direction();
        Leap::Vector handOrigin;
        if(stab)
            handOrigin = leapHand.stabilizedPalmPosition();
        else
            handOrigin = leapHand.palmPosition();
        handOrigin *= scale;
        handOrigin.y -= yOffset;
        Leap::Matrix handTransform = Leap::Matrix(handXBasis, handYBasis, handZBasis, handOrigin);
        
        matrixVectorHands.push_back(handTransform);
        
        for( int f = 0; f < leapHand.fingers().count(); f++ )
        {
            Leap::Finger leapFinger = leapHand.fingers()[f];
            Leap::Vector transformedDirection = handTransform.transformDirection(leapFinger.direction());
            
            Leap::Vector fingerNormal;
            fingerNormal.x = transformedDirection.x;
            fingerNormal.y = transformedDirection.z;
            fingerNormal.z = -transformedDirection.y;
            fingerNormal.normalized();
            
            Leap::Vector fingerXBasis = fingerNormal.cross(transformedDirection).normalized();
            Leap::Vector fingerYBasis = -fingerNormal;
            Leap::Vector fingerZBasis = -transformedDirection;
            Leap::Vector fingerOrigin;
            if(stab)
                fingerOrigin = leapFinger.stabilizedTipPosition();
            else
                fingerOrigin = leapFinger.tipPosition();
            fingerOrigin *= scale;
            fingerOrigin.y -= yOffset;
            
			//COLLISSION
			touchedObjectsV(fingerOrigin);
            
			Leap::Matrix fingerTransform = Leap::Matrix(fingerXBasis, fingerYBasis, fingerZBasis, fingerOrigin);
            matrixVectorFingers.push_back(fingerTransform);
        }
    }
	for (int i=0;i<gestList.count();i++){
		if (gestList[i].type() == Gesture::TYPE_KEY_TAP){
            Leap::Vector tapPos;
            tapPos *= scale;
            tapPos.y -= yOffset;
			KeyTapGesture tap = gestList[i];
			touchedObjectsPil(tapPos);
			std::cout << "tap " << tapPos.x << " " << tapPos.y << std::endl;
		}
	}
}

void LMOC::renderInit()
{
//    checkGLError("render init");
//	sf::Vector2u windowSize = window.getSize();
//	if (firstRun){
//        //////////////////////////////////////////////////// SETUP OPENGL STATES
//        window.setActive();
//        glEnable(GL_DEPTH_TEST);
//        checkGLError("render test1");
//        glDepthMask(GL_TRUE);
//        checkGLError("render test2");
//        //glEnable(GL_DOUBLE);
//        checkGLError("render test3");
//        //glEnable(GL_SMOOTH);
//        checkGLError("render test4");
//        glEnable(GL_TEXTURE_2D);
//        checkGLError("render test5");
//        glDisable(GL_COLOR_MATERIAL);
//        checkGLError("render test6");
//        glEnable(GL_BLEND);
//        checkGLError("render test7");
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glClearDepth(1.0f);
//        glClearColor(0.0, 0.0, 0.0, 0.0);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        glLoadIdentity();
////        {
////            ///////// set perspective matrix without the deprecated function gluPerspective
////            //gluPerspective(viewanchor,(float)windowSize.x/(float)windowSize.y, 0.1f, 10000.f);
////            Matrix4 matProject = setFrustum(viewanchor,(float)windowSize.x/(float)windowSize.y, 0.1f, 10000.f);
////            glMatrixMode(GL_PROJECTION);
////            glLoadMatrixf(matProject.getTranspose());
////        }
//        
//        checkGLError("post renderI2");
//        
//        ///////////////////////////////////////////////////////////// SETUP VBO'S
//        window.setActive(true);
//        
//        glGenBuffers(NUM_VBO, VBO);
//        
//        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
//        glBufferData(GL_ARRAY_BUFFER, keyboardVert_data.size()*sizeof(Vertex),&(keyboardVert_data[0]),GL_STATIC_DRAW);
//        
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, keyboardInd_data.size()*sizeof(unsigned int),&(keyboardInd_data[0]),GL_STATIC_DRAW);
//        
//        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
//        glBufferData(GL_ARRAY_BUFFER, palmVert_data.size()*sizeof(Vertex),&(palmVert_data[0]),GL_STATIC_DRAW);
//        
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, palmInd_data.size()*sizeof(unsigned int),&(palmInd_data[0]),GL_STATIC_DRAW);
//        
//        glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
//        glBufferData(GL_ARRAY_BUFFER, fingerVert_data.size()*sizeof(Vertex),&(fingerVert_data[0]),GL_STATIC_DRAW);
//        
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[5]);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, fingerInd_data.size()*sizeof(unsigned int),&(fingerInd_data[0]),GL_STATIC_DRAW);
//        
//        checkGLError("post renderI3");
//        
//	}
//    
}

void LMOC::renderMinimalInit()
{
    ////////////////////////////////////////// SETUP VBO'S
    window.setActive(true);
    
    glGenBuffers(NUM_VBO, VBO);
    
    //keyboard
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, keyboardVert_data.size()*sizeof(Vertex),&(keyboardVert_data[0]),GL_STATIC_DRAW);
        
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, keyboardInd_data.size()*sizeof(unsigned int),&(keyboardInd_data[0]),GL_STATIC_DRAW);
    
    //hand
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, palmVert_data.size()*sizeof(Vertex),&(palmVert_data[0]),GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, palmInd_data.size()*sizeof(unsigned int),&(palmInd_data[0]),GL_STATIC_DRAW);
    
    //finger
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, fingerVert_data.size()*sizeof(Vertex),&(fingerVert_data[0]),GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fingerInd_data.size()*sizeof(unsigned int),&(fingerInd_data[0]),GL_STATIC_DRAW);
}

void LMOC::renderMinimal(){
    /////////////////////////////////////////////////// PREPERATION
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    window.setActive(true);
    window.clear();
    
    
    
    /////////////////////////////// GET WINDOW SIZE
    glm::vec2 windowSize = glm::vec2(window.getSize().x,window.getSize().y);
    
    //////////////////////////////// CALC MATRICES
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 perspectiveMatrix=glm::perspective(viewanchor, windowSize.x/windowSize.y, 0.1f, 10000.0f);
    glm::mat4 lookAtMatrix=glm::lookAt(Eyes.getCam()+playerPos, playerPos, glm::vec3(0.0f, 1.0, 0.0f));
    
    /////////////////////////////// ENABLE SHADER
    glUseProgram(lmocShader);
    
    //////////////////////////////// PASS MATRICES
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_lookAtMatrix"), 1, GL_FALSE, glm::value_ptr(lookAtMatrix));
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_perspectiveMatrix"), 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
    
    glUniform1i(glGetUniformLocation(lmocShader, "isKeyboard"), 1);
    
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_perspectiveMatrix"), 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
    
    /////////////////////////////////////////////////// DRAW
    //////////////////////////////// RESET
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    //////////////////////////////// ENABLE STATES
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    attribute_u_keyId = glGetAttribLocation(lmocShader, "u_keyId");
    glEnableVertexAttribArray(attribute_u_keyId);
    
    ////////////////////////////////////////////// DRAW KEYBOARD
    //////////////////////////////// BIND TEXTURE
    //sadly i dont know why it does work... but it does and i have no time to do it the right way
    if(keyCaps)
        sf::Texture::bind(&keyboardT);
    else
        sf::Texture::bind(&keyboardTCaps);
    
    //////////////////////////////// BIND VERTEX DATA
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
    //////////////////////////////// BIND VERTEX ATTRIBUTES (KEYBOARD ONLY)
    glVertexAttribPointer(attribute_u_keyId, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
    
    //////////////////////////////// RENDER ACTION KEYBOARD
    glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
    
    ////////////////////////////////////////////// DRAW HANDS
    glUniform1i(glGetUniformLocation(lmocShader, "isKeyboard"), 0);
    //////////////////////////////// BIND TEXTURE
    //sadly i dont know why it does work... but it does and i have no time to do it the right way
    sf::Texture::bind(&palmT);
    
    //////////////////////////////// BIND VERTEX DATA
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
    
    //////////////////////////////// RENDER ACTION HANDS
    for(int i=0;i<matrixVectorHands.size();i++){
        glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorHands[i].toArray4x4().m_array);
        glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
    }
    
    ////////////////////////////////////////////// DRAW FINGERS
    //////////////////////////////// BIND TEXTURE
    //sadly i dont know why it does work... but it does and i have no time to do it the right way
    sf::Texture::bind(&fingerT);
    
    //////////////////////////////// BIND VERTEX DATA
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[5]);
    
    //////////////////////////////// RENDER ACTION FINGERS
    for(int i=0;i<matrixVectorFingers.size();i++){
        glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorFingers[i].toArray4x4().m_array);
        glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
    }
    
    
    
    //////////////////////////////// DISABLE STATES
    glUseProgram(NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glFlush();
    
    /////////////////////////////////////////////////// END DRAW
    
    ////////////////////////////////////////////////////////// SFML POSTPROCESS
    //////////////////////////////// TEXT
    for (int i=0; i<TEXTCNT; i++) {
        window.draw(texts[i]);
    }

    //////////////////////////////// SWAP BUFFERS AND DISPLAY
    window.pushGLStates();
    for (int i=0; i<TEXTCNT; i++) {
        window.draw(texts[i]);
    }
    window.popGLStates();
    window.display();
    window.setActive(false);
}

void LMOC::render()
{
//    ///////////////////////////////////////////////////////// ACTIVATE CONTEXT
//    
//    window.setActive(true);
//    window.clear();
//    
//    
//    ///////////////////////////////////////////////////////// UPDATE CAM
//    /////////////////////////////////////////////////////////////////// OCULUS STUFF
////    OVR::SensorFusion SFusion;
////    if (pHMD) {
////        pSensor = pHMD->GetSensor();
////    }
////    if (pSensor){
////        SFusion.AttachToSensor(pSensor);
////    }
////    OVR::Matrix4f viewCenterMatrix;
////    // Compute Aspect Ratio. Stereo mode cuts width in half.
////    float aspectRatio = float(hmd.HResolution * 0.5f) / float(hmd.VResolution);
////    // Compute Vertical FOV based on distance.
////    float halfScreenDistance = (hmd.VScreenSize / 2);
////    float yfov = 2.0f * atan(halfScreenDistance/hmd.EyeToScreenDistance);
////    // Post-projection viewport coordinates range from (-1.0, 1.0), with the
////    // center of the left viewport falling at (1/4) of horizontal screen size.
////    // We need to shift this projection center to match with the lens center.
////    // We compute this shift in physical units (meters) to correct
////    // for different screen sizes and then rescale to viewport coordinates.
////    float viewCenter = hmd.HScreenSize * 0.25f;
////    float eyeProjectionShift = viewCenter - hmd.LensSeparationDistance*0.5f;
////    float projectionCenterOffset = 4.0f * eyeProjectionShift / hmd.HScreenSize;
////    // Projection matrix for the "center eye", which the left/right matrices are based on.
////    OVR::Matrix4f projCenter = OVR::Matrix4f::PerspectiveRH(yfov, aspectRatio, 0.3f, 1000.0f);
////    OVR::Matrix4f projLeft = OVR::Matrix4f::Translation(projectionCenterOffset, 0, 0) * projCenter;
////    OVR::Matrix4f projRight = OVR::Matrix4f::Translation(-projectionCenterOffset, 0, 0) * projCenter;
////    // View transformation translation in world units.
////    float halfIPD = hmd.InterpupillaryDistance * 0.5f;
////    OVR::Matrix4f viewLeft = OVR::Matrix4f::Translation(halfIPD, 0, 0) * viewCenter;
////    OVR::Matrix4f viewRight= OVR::Matrix4f::Translation(-halfIPD, 0, 0) * viewCenter;
//    ////////////////////////////////////////////// myStuff
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glMatrixMode(GL_MODELVIEW);
//    //glLoadIdentity();
//    //todo: replace gluLookAt with something not deprecated
//    Vector3 upVec;
//    upVec.set(0.0,1.0,0.0);
//    Vector3 camPos;
//    camPos.set(Eyes.getCam().x+playerPos.x, Eyes.getCam().y+playerPos.y, Eyes.getCam().z+playerPos.z);
//    //LookAt(camPos,playerPos,upVec);
//    //Matrix4 lookAtMat = LookAt(playerPos, playerPos, upVec);
//    //gluLookAt(Eyes.getCam().x+playerPos.x, Eyes.getCam().y+playerPos.y, Eyes.getCam().z+playerPos.z, playerPos.x, playerPos.y, playerPos.z, 0.0, 1.0, 0.0);
//    
//
//    /////////////////////////////////////////////////// Enable States
//    
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_NORMAL_ARRAY);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    
//    ///////////////////////////////////////////////////////// UPDATE SHADER PARAMETERS
//    //TODO: change for the new Shader loading procedure
//    glUseProgram(keyboardS);
//    checkGLError("pre pre uniforms");
////    if (keyCaps) {
////        tex.bind(tex.TEX_KEYS);
////        glUniform1i(glGetUniformLocation(keyboardS, "texture"), tex.TEX_KEYS);
////        checkGLError("tex uniforms");
////    }else{
////        tex.bind(tex.TEX_KEY);
////        glUniform1i(glGetUniformLocation(keyboardS, "texture"), tex.TEX_KEY);
////        checkGLError("tex uniforms1");
////    }
//   // glUniformMatrix4fv(glGetUniformLocation(keyboardS, "projectionMatrix"), 1, GL_FALSE, lookAtMat.getTranspose());
//
//    
//    checkGLError("uniforms");
//    
//    /////////////////////////////////////////////////// Keyboard
//    
//    checkGLError("keyboard");
//    //TODO: SHADER
//    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
//    glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
//    glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
//    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
//    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
//    
//    Matrix4 identM;
//    glUniformMatrix4fv(glGetUniformLocation(keyboardS, "modelMatrix"), 1, GL_FALSE, &
//                       identM.identity().tm[0]);
//    ///////////////////////////// note: glViewport is NOT DEPRECATED
//    //leftEye
//    glViewport(0,0,window.getSize().x/2,window.getSize().y);
//    //glLoadMatrixf(&viewLeft.Transposed().M[0][0]);
//    glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
//    //rightEye
//    glViewport(window.getSize().x/2,0,window.getSize().x/2,window.getSize().y);
//    //glLoadMatrixf(&viewRight.Transposed().M[0][0]);
//    glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
////    glBegin(GL_POINTS);
////    for (int local_x=-500; local_x<500; local_x++) {
////        for (int local_y=-500;local_y<500;local_y++){
////            for (int local_z=-500; local_z<500; local_z++) {
////                glColor3f(1.0f, 1.0f, 1.0f);
////                glVertex3f((float)local_x/10.0f, (float)local_y/10.0f, (float)local_z/10.0f);
////            }
////        }
////    }
////    glEnd();
//    //////////////////////////////////////////////////// Haende
//    
////    checkGLError("haende");
////    glUseProgram(handS);
////    checkGLError("pre uniforms");
////    glUniform1f(glGetUniformLocation(handS, "edgefalloff"), edgefalloff);
////    checkGLError("uniforms1");
////    glUniform1f(glGetUniformLocation(handS, "intensity"), intensity);
////    checkGLError("uniforms2");
////    glUniform1f(glGetUniformLocation(handS, "ambient"), ambient);
////    
////    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
////    glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
////    glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
////    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
////    
////    sf::Texture::bind(&palmT);
////    
////    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
////    //leftEye
////    glViewport(0,0,window.getSize().x/2,window.getSize().y);
////    for (unsigned int i=0; i<matrixVectorHands.size(); i++) {
////        glPushMatrix();
////        glMultMatrixf(matrixVectorHands[i].toArray4x4().m_array);
////        glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
////        glPopMatrix();
////    }
////    //rightEye
////    glViewport(window.getSize().x/2,0,window.getSize().x/2,window.getSize().y);
////    for (unsigned int i=0; i<matrixVectorHands.size(); i++) {
////        glPushMatrix();
////        glMultMatrixf(matrixVectorHands[i].toArray4x4().m_array);
////        glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
////        glPopMatrix();
////    }
//    
//    
//    //////////////////////////////////////////////////// Finger
//    
////    glUseProgram(handS);
////    
////    checkGLError("finger");
////    
////    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
////    glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
////    glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
////    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
////    
////    sf::Texture::bind(&fingerT);
////    
////    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[5]);
////    //leftEye
////    glViewport(0,0,window.getSize().x/2,window.getSize().y);
////    for (unsigned int i=0; i<matrixVectorFingers.size(); i++) {
////        glPushMatrix();
////        glMultMatrixf(matrixVectorFingers[i].toArray4x4().m_array);
////        glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
////        glPopMatrix();
////    }
////    //rightEye
////    glViewport(window.getSize().x/2,0,window.getSize().x/2,window.getSize().y);
////    for (unsigned int i=0; i<matrixVectorFingers.size(); i++) {
////        glPushMatrix();
////        glMultMatrixf(matrixVectorFingers[i].toArray4x4().m_array);
////        glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
////        glPopMatrix();
////    }
//    
//    ////////////////////////////////////////////////////////// disable states,buffers,shaders
//    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glDisableClientState(GL_NORMAL_ARRAY);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glUseProgram(0);
//    
//    ////////////////////////////////////////////////////////// SFML POSTPROCESS (Draw Text)
////    glFlush();
////    window.pushGLStates();
////    
////    for (int i=0; i<TEXTCNT; i++) {
////        window.draw(texts[i]);
////    }
////    window.popGLStates();
////    
////    // Update the window
//    window.display();
////    window.setActive(false);
//    
//    checkGLError("end render");
}

void LMOC::checkInput(){
    //rotate 3rd Person cam on mouse left button
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        glm::ivec2 mousePos= glm::ivec2(sf::Mouse::getPosition().x,sf::Mouse::getPosition().y);
        Eyes.mouseMove(mousePos);
        Eyes.setDown(true);
    }
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && Eyes.isDown()){
        Eyes.mouseRelease();
        Eyes.setDown(false);
    }
}

void LMOC::checkEvents(){
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
        
        //switch keyboard texture to caps with button C
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
            keyCaps=!keyCaps;
        }
        
        //switch leap stability with button S
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
            stab=!stab;
        }            //switch leap stability with button S
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O) {
            //stab=!stab;
            if(objDraw < objBounds.size()){
                objDraw++;
            }
        }
    }
}



void LMOC::runLoop(){
    renderMinimalInit();
    while (rendering) {
        checkInput();
        checkEvents();
        leapMatrix();
        textThread();
        renderMinimal();
    }
}