//
//  LMOC.cpp
//  LeapMotionOculusKeyboard
//
//  Created by FredMF on 09.11.13.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.

#include "LMOC.h"

#include <sstream>
#include <fstream>

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


#include "ran.h"
#include "Matrices.h"
#include "Shader.h"

float debugVal;


void checkGLError(const char* prefix = "")
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << prefix << " GL_ERROR: " << gluErrorString(err) << " (0x" << std::hex << err << ")" << std::endl;
	}
}

LMOC::LMOC(){
    debugVal=0;
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
    
    for (int x=0;x<4;x++){
        for (int y=0;y<4;y++){
            zeroMat[x][y]=0.0;
        }
    }
    touchedOMat=zeroMat;
    
    settings.depthBits = 32;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;
    
    //LEAP***************************************
    controller.addListener(listener);
    //LEAP***************************************
    
    //OCULUS*************************************
    //comments for oculus are numbered acoording to the 5 steps in the OculusSDK Documentation Page 16. Part 5.1
    
    //step 1, initialize LibOVR, OVR.h is already included in the header -OVR Doc 5.2
    //initialization is done by using an OVR::System object as class member
    pManager = *OVR::DeviceManager::Create();
    pHMD = *pManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
    
    unsigned int xRes,yRes;
    
    //print information about Device Found, if found
    if(pHMD && pManager){
        if(pHMD->GetDeviceInfo(&hmd)){
            std::cout << "\tDevice Name " << hmd.DisplayDeviceName
            << "\n\tEyeDistance " << hmd.InterpupillaryDistance
            << "\n\tDistortionK[0] " << hmd.DistortionK[0]
            << "\n\tDistortionK[1] " << hmd.DistortionK[1]
            << "\n\tDistortionK[2] " << hmd.DistortionK[2]
            << "\n\tDistortionK[3] " << hmd.DistortionK[3]
            << "\n\tHResolution " << hmd.HResolution
            << "\n\tVResolution " << hmd.VResolution
            << "\n\tHScreenSize " << hmd.HScreenSize
            << "\n\tVScreenSize " << hmd.VScreenSize
            << "\n\tEyeToScreenDistance " << hmd.EyeToScreenDistance
            << "\n\tLensSeparationDistance " << hmd.LensSeparationDistance
            << "\n\tInterpupillaryDistance " << hmd.InterpupillaryDistance
            << std::endl;
            oculusConnected = true;
            xRes=hmd.HResolution;
            yRes=hmd.VResolution;
        }else{
            xRes=1280;
            yRes=800;
        }
    }else{
        std::cout << "!!! Oculus Rift not connected !!!" << std::endl;
        oculusConnected = false;
        xRes=1280;
        yRes=800;
    }
    
    
    //OCULUS*************************************
    
    window.create(sf::VideoMode(xRes,yRes), "Leap Motion Oculus Keyboard", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    fullscreen=false;
    
    GLint zdeph=0;
    glGetIntegerv(GL_DEPTH_BITS,&zdeph);
    std::cout << "dephbuffer: " << zdeph << std::endl;
    
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
    
    //obsolet, using OVR::System object as member, destructor does the work.
    //OVR::System::Destroy(); //LibOVR Doc 5.2
}

void LMOC::touchedObjects(Leap::Vector tipP){
	for(unsigned int i=0;i<objBounds.size();i++){
		if(tipP.x>=objBounds[i].min.x &&
           tipP.x<=objBounds[i].max.x &&
           tipP.y>=objBounds[i].min.y &&
           tipP.y<=objBounds[i].max.y &&
           tipP.z>=objBounds[i].min.z &&
           tipP.z<=objBounds[i].max.z){
            toucedFingers.push_back((float)i);
        }
	}
}

void LMOC::calcColMat(void){
    
    touchedOMat=zeroMat;
    bool done=false;
    int outputIndex=0;
    if (toucedFingers.size() != 0) {
        for (int x=0; x<4; x++) {
            for (int y=0; y<4; y++) {
                if (!done) {
                    touchedOMat[x][y]=toucedFingers[outputIndex];
                    outputIndex++;
                    if (outputIndex==toucedFingers.size()) {
                        done=true;
                    }
                }
            }
        }
    }
    toucedFingers.clear();
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
        vert.v=v_data[f_data[i].vertInd.x-1];
        vert.t=vt_data[f_data[i].vertInd.y-1];
        vert.n=vn_data[f_data[i].vertInd.z-1];
        vert_data->push_back(vert);
    }
    for (unsigned int i=0;i<vert_data->size()*3;i++){
        ind_data->push_back(i);
    }
    std::cout << path.toAnsiString() << " loaded\n\tvert_data size: " << vert_data->size() << " f_data size:" << f_data.size() << std::endl;
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
			touchedObjects(fingerOrigin);
            
			Leap::Matrix fingerTransform = Leap::Matrix(fingerXBasis, fingerYBasis, fingerZBasis, fingerOrigin);
            matrixVectorFingers.push_back(fingerTransform);
        }
    }
}


void LMOC::renderInit()
{
    ////////////////////////////////////////// SETUP VBO'S
    window.setActive(true);
    
    //DEBUG INFORMATION
    std::cout << "render Init, OpenGL Overview" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION)<< std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)<< std::endl;
    
    
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

void LMOC::render(){
    /////////////////////////////////////////////////// PREPERATION
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDepthFunc(GL_LEQUAL);
    //glDepthRange(0.0f, 1.0f);
    
    window.setActive(true);
    window.clear();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);
    
    /////////////////////////////// GET WINDOW SIZE
    glm::vec2 windowSize = glm::vec2(window.getSize().x,window.getSize().y);
    
    //////////////////////////////// CALC MATRICES
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    
    //mouse controlled matrices and oculus matrices
    
    glm::mat4 perspectiveMatrixL;
    glm::mat4 lookAtMatrixL;
    glm::mat4 perspectiveMatrixR;
    glm::mat4 lookAtMatrixR;
    
    
    
    if(oculusConnected){
        float aspectRatio = float(hmd.HResolution * 0.5f)/float(hmd.VResolution);
        float halfScreenDistance = (hmd.VScreenSize/2.0f);
        float yfov = 2.0f * atan(halfScreenDistance/hmd.EyeToScreenDistance);
        float viewCenter = hmd.HScreenSize*0.25f;
        float eyeProjectionShift = viewCenter - hmd.LensSeparationDistance*0.5f;
        float projectionCenterOffset = 4.0f * eyeProjectionShift / hmd.HScreenSize;
        
        glm::mat4 centerPerspectiveMatrix=glm::perspective(yfov, aspectRatio, 0.1f, 5000.0f);
        perspectiveMatrixL=glm::translate(projectionCenterOffset, 0.0f, 0.0f)*centerPerspectiveMatrix;
        perspectiveMatrixR=glm::translate(-projectionCenterOffset, 0.0f, 0.0f)*centerPerspectiveMatrix;
        
        float halfIPD = (hmd.InterpupillaryDistance * 0.5f)+debugVal;
        glm::mat4 leftView = glm::translate(halfIPD, 0.0f, 0.0f) * viewCenter;
        glm::mat4 rightView = glm::translate(-halfIPD, 0.0f, 0.0f) * viewCenter;
        //WHERE TO USE THEM?????
        
        
        lookAtMatrixL=glm::lookAt(Eyes.getCam()+playerPos, playerPos, glm::vec3(0.0f, 1.0, 0.0f));
        lookAtMatrixR=glm::lookAt(Eyes.getCam()+playerPos, playerPos, glm::vec3(0.0f, 1.0, 0.0f));
        
        
        lookAtMatrixL=leftView*lookAtMatrixL;
        lookAtMatrixR=rightView*lookAtMatrixL;
    }else{
        float aspectRatio = windowSize.x/windowSize.y;
        
        perspectiveMatrixL=glm::perspective(viewanchor, aspectRatio, 0.1f, 5000.0f);
        lookAtMatrixL=glm::lookAt(Eyes.getCam()+playerPos, playerPos, glm::vec3(0.0f, 1.0, 0.0f));
        
        perspectiveMatrixR=glm::perspective(viewanchor, aspectRatio, 0.1f, 5000.0f);
        lookAtMatrixR=glm::lookAt(Eyes.getCam()+playerPos, playerPos, glm::vec3(0.0f, 1.0, 0.0f));
    }
    
    /////////////////////////////// ENABLE SHADER
    glUseProgram(lmocShader);
    
    //////////////////////////////// PASS MATRICES AND PRESSED BUTTONS
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_ViewMatrixL"), 1, GL_FALSE, glm::value_ptr(lookAtMatrixL));
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_ProjectionMatrixL"), 1, GL_FALSE, glm::value_ptr(perspectiveMatrixL));
    
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_ViewMatrixR"), 1, GL_FALSE, glm::value_ptr(lookAtMatrixR));
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_ProjectionMatrixR"), 1, GL_FALSE, glm::value_ptr(perspectiveMatrixR));
    
    //for the visualization of pressed buttons (violet light and translation)
	glUniformMatrix4fv(glGetUniformLocation(lmocShader, "pressedButtons"), 1, GL_FALSE, glm::value_ptr(touchedOMat));
    
    /////////////////////////////////////////////////// DRAW
    //////////////////////////////// RESET
    //glMatrixMode(GL_MODELVIEW);
    //////////////////////////////// ENABLE STATES
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    attribute_u_keyId = glGetAttribLocation(lmocShader, "u_keyId");
    glEnableVertexAttribArray(attribute_u_keyId);
    
    ////////////////////////////////////////////// DRAW KEYBOARD
    glUniform1i(glGetUniformLocation(lmocShader, "isKeyboard"), 1);
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
    
    if(oculusConnected){
        //L
        glViewport(0, 0, window.getSize().x/2, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 1);
        glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
        //R
        glViewport(window.getSize().x/2, 0, window.getSize().x/2, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 0);
        glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
    }else{
        glViewport(0, 0, window.getSize().x, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 1);
        glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
    }
    
    ////////////////////////////////////////////// DRAW HANDS
    //deactivate keyboard routines
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
    
    if(oculusConnected){
        //L
        glViewport(0, 0, window.getSize().x/2, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 1);
        for(int i=0;i<matrixVectorHands.size();i++){
            glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorHands[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
        }
        //R
        glViewport(window.getSize().x/2, 0, window.getSize().x/2, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 0);
        for(int i=0;i<matrixVectorHands.size();i++){
            glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorHands[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
        }
    }else{
        glViewport(0, 0, window.getSize().x, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 1);
        for(int i=0;i<matrixVectorHands.size();i++){
            glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorHands[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
        }
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
    if(oculusConnected){
        //L
        glViewport(0, 0, window.getSize().x/2, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 1);
        for(int i=0;i<matrixVectorFingers.size();i++){
            glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorFingers[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
        }
        
        //R
        glViewport(window.getSize().x/2, 0, window.getSize().x/2, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 0);
        for(int i=0;i<matrixVectorFingers.size();i++){
            glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorFingers[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
        }
    }else{
        glViewport(0, 0, window.getSize().x, window.getSize().y);
        glUniform1i(glGetUniformLocation(lmocShader, "isLeft"), 1);
        for(int i=0;i<matrixVectorFingers.size();i++){
            glUniformMatrix4fv(glGetUniformLocation(lmocShader, "u_modelMatrix"), 1, GL_FALSE, matrixVectorFingers[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
        }
    }
    
    //////////////////////////////// DISABLE STATES
    glUseProgram(NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDisableVertexAttribArray(attribute_u_keyId);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glFlush();
    
    /////////////////////////////////////////////////// END DRAW
    
    ////////////////////////////////////////////////////////// SFML POSTPROCESS
    //////////////////////////////// TEXT
    window.pushGLStates();
    for (int i=0; i<TEXTCNT; i++) {
        window.draw(texts[i]);
    }
    window.popGLStates();
    //////////////////////////////// SWAP BUFFERS AND DISPLAY
    window.display();
    window.setActive(false);
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
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            oculusConnected=!oculusConnected;
        }
        
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) {
            fullscreen=!fullscreen;
            sf::Vector2u winSize = window.getSize();
            if(fullscreen){
                window.setActive(false);
                window.create(sf::VideoMode(winSize.x,winSize.y), "Leap Motion Oculus Keyboard", sf::Style::Fullscreen, settings);
                window.setVerticalSyncEnabled(true);
                window.setActive(true);
            }else{
                window.setActive(false);
                window.create(sf::VideoMode(winSize.x,winSize.y), "Leap Motion Oculus Keyboard", sf::Style::Default, settings);
                window.setVerticalSyncEnabled(true);
                window.setActive(true);
            }
        }
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up) {
            debugVal+=0.1;
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down) {
            debugVal-=0.1;
        }
        
        //switch leap stability with button S
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
            stab=!stab;
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O) {
            //stab=!stab;
            if(objDraw < objBounds.size()){
                objDraw++;
            }
        }
    }
}



void LMOC::runLoop(){
    renderInit();
    while (rendering) {
        checkInput();//check for input
        checkEvents();//check for input events
        leapMatrix();//calculate transformation matrix for the fingers and collect colission
        calcColMat();//calculate the colission mat4
        textThread();//update text with new information
        render();//draw the scene
    }
}