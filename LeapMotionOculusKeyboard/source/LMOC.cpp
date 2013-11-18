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

#include "Matrices.h"

Matrix4 setFrustum(float l, float r, float b, float t, float n, float f)
{
    Matrix4 mat;
    mat[0]  = 2 * n / (r - l);
    mat[2]  = (r + l) / (r - l);
    mat[5]  = 2 * n / (t - b);
    mat[6]  = (t + b) / (t - b);
    mat[10] = -(f + n) / (f - n);
    mat[11] = -(2 * f * n) / (f - n);
    mat[14] = -1;
    mat[15] = 0;
    return mat;
}

Matrix4 setFrustum(float fovY, float aspectRatio, float front, float back)
{
    float tangent = tanf(fovY/2 * 3.141593f / 180);   // tangent of half fovY
    float height = front * tangent;           // half height of near plane
    float width = height * aspectRatio;       // half width of near plane
    
    // params: left, right, bottom, top, near, far
    return setFrustum(-width, width, -height, height, front, back);
}

void lookAt(const Vector3& pos, const Vector3& dir, const Vector3& up)
{
	Vector3 dirN;
	Vector3 upN;
	Vector3 rightN;

	dirN = dir;
	dirN.normalize();

	upN = up;
	upN.normalize();

	rightN = dirN.cross(upN);
	rightN.normalize();

	upN = rightN.cross(dirN);
	upN.normalize();

        float mat[16];
	mat[ 0] = rightN.x;
	mat[ 1] = upN.x;
	mat[ 2] = -dirN.x;
	mat[ 3] = 0.0;

	mat[ 4] = rightN.y;
	mat[ 5] = upN.y;
	mat[ 6] = -dirN.y;
	mat[ 7] = 0.0;

	mat[ 8] = rightN.z;
	mat[ 9] = upN.z;
	mat[10] = -dirN.z;
	mat[11] = 0.0;

	mat[12] = -(rightN.dot(pos));
	mat[13] = -(upN.dot(pos));
	mat[14] = (dirN.dot(pos));
	mat[15] = 1.0;
	
	//glMultMatrixf(&mat[0]);
    glLoadMatrixf(&mat[0]);
}

LMOC::LMOC(){
    // Create the main window
	objDraw=0;
    Eyes.initCam(0,50,40);
	Eyes.mouseMove(0,0);
	Eyes.mouseRelease();
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
#ifdef WIN32
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
#endif
    
    
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
        texts[i].setPosition(1,((float)i*(texts[i].getCharacterSize()+1))+1);
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
    
    loadModel(resourcePath() + "keyboard.obj",&keyboardVert_data,&keyboardInd_data,true);
    loadModel(resourcePath() + "fingertip.obj",&fingerVert_data,&fingerInd_data,false);
    loadModel(resourcePath() + "palm.obj",&palmVert_data,&palmInd_data,false);
    return true;
}

bool LMOC::loadModel(sf::String path,std::vector<Vertex> *vert_data, std::vector<unsigned int> *ind_data,bool withBounds){
    std::vector<sf::Vector3f> v_data;
    std::vector<sf::Vector2f> vt_data;
    std::vector<sf::Vector3f> vn_data;
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
        textsMu.lock();
        for (int i = 0; i<TEXTCNT; i++) {
            texts[i].setString(sstext[i].str());
        }
        textsMu.unlock();
    }
    
    std::cout << "textThread done" << std::endl;
}

void LMOC::matrixThread(){ /////////////////////////// TO MUCH OVERHEAD AS A THREAD
    /////////////////// CALCULATE THE TRANSFORM MATRICES FOR THE HANDS AND FINGERS
    //    while(rendering){
    float scale=0.0625f;  //1.0f/16.0f;
    float yOffset=8;
    
    Leap::HandList handList = listener.frame.hands();
    matrixVectorHands.clear();
    matrixVectorFingers.clear();
    //    std::vector<Leap::Matrix> m_matrixVectorHands;
    //    std::vector<Leap::Matrix> m_matrixVectorFingers;
    
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
            //Leap::Vector transformedPosition = handTransform.transformPoint(leapFinger.tipPosition());
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
    //MatrixMu.lock();
    //        matrixVectorHands.swap(m_matrixVectorHands);
    //        matrixVectorFingers.swap(m_matrixVectorFingers);
    //MatrixMu.unlock();
    //    }
    //    std::cout << "MatrixThread done";
}


void LMOC::renderThread()
{
	if (firstRun){
    //////////////////////////////////////////////////// SETUP OPENGL STATES
    window.setActive();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_DOUBLE);
    glEnable(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearDepth(1.0f);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLoadIdentity();
    {
        sf::Vector2u windowSize = window.getSize();
        
        ///////// set perspective matrix without the deprecated function gluPerspective
        //gluPerspective(viewanchor,(float)windowSize.x/(float)windowSize.y, 0.1f, 10000.f);
        Matrix4 matProject = setFrustum(viewanchor,(float)windowSize.x/(float)windowSize.y, 0.1f, 10000.f);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(matProject.getTranspose());
    }
    
        
        ///////////////////////////////////////////////////////////// SETUP VBO'S
        window.setActive(true);
        
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
        
	}
    ///////////////////////////////////////////////////////////// RENDERLOOP
	while (rendering)
    {
		///////////////////////////////////////////////////////// WINDOWS WANTS EVENTS AND RENDERING IN MAIN THREAD... OK...
		#ifdef WIN32
        checkEvents();
		#endif

        ///////////////////////////////////////////////////////// EX MATRIXTHREAD, TO MUCH OVERHEAD NOW IN RENDERTHREAD
        matrixThread();
        
        ///////////////////////////////////////////////////////// ACTIVATE CONTEXT
        
        window.setActive(true);
        window.clear();
                

        ///////////////////////////////////////////////////////// UPDATE CAM
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //todo: replace gluLookAt with something not deprecated
		Vector3 upVec;
		upVec.set(0.0,1.0,0.0);
		Vector3 camPos;
		camPos.set(Eyes.getCam().x+playerPos.x, Eyes.getCam().y+playerPos.y, Eyes.getCam().z+playerPos.z);
		//lookAt(camPos,playerPos,upVec);
        gluLookAt(Eyes.getCam().x+playerPos.x, Eyes.getCam().y+playerPos.y, Eyes.getCam().z+playerPos.z, playerPos.x, playerPos.y, playerPos.z, 0.0, 1.0, 0.0);

        ///////////////////////////////////////////////////////// UPDATE SHADER PARAMETERS
        if (keyCaps) {
            keyboardS.setParameter("texture", keyboardTCaps);
        }else{
            keyboardS.setParameter("texture", keyboardT);
        }
        handS.setParameter("texture", palmT);
        handS.setParameter("edgefalloff", edgefalloff);
        handS.setParameter("intensity", intensity);
        handS.setParameter("ambient", ambient);
        
        /////////////////////////////////////////////////// Enable States
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
        /////////////////////////////////////////////////// Keyboard
        
        sf::Shader::bind(&keyboardS);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
        
        if (keyCaps) {
            sf::Texture::bind(&keyboardTCaps);
        }else{
            sf::Texture::bind(&keyboardT);
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
        glDrawElements(GL_TRIANGLES, keyboardInd_data.size(), GL_UNSIGNED_INT, 0);
        
        //////////////////////////////////////////////////// Haende
        
        sf::Shader::bind(&handS);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
        
        sf::Texture::bind(&palmT);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
        for (unsigned int i=0; i<matrixVectorHands.size(); i++) {
            glPushMatrix();
            glMultMatrixf(matrixVectorHands[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, palmInd_data.size(), GL_UNSIGNED_INT, 0);
            glPopMatrix();
        }
        
        //////////////////////////////////////////////////// Finger
        
        handS.setParameter("texture", fingerT);
        
        sf::Shader::bind(&handS);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
        glVertexPointer(3, GL_FLOAT,sizeof(Vertex), 0);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), (char*)NULL + 12);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*)NULL + 24);
        
        sf::Texture::bind(&fingerT);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[5]);
        for (unsigned int i=0; i<matrixVectorFingers.size(); i++) {
            glPushMatrix();
            glMultMatrixf(matrixVectorFingers[i].toArray4x4().m_array);
            glDrawElements(GL_TRIANGLES, fingerInd_data.size(), GL_UNSIGNED_INT, 0);
            glPopMatrix();
        }
        
        ////////////////////////////////////////////////////////// disable states,buffers,shaders

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        sf::Shader::bind(NULL);

        ////////////////////////////////////////////////////////// SFML POSTPROCESS (Draw Text)
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

void LMOC::run(){
    while (running)
    {
        // Process events
#ifdef __APPLE__
        checkEvents();
#endif

        //rotate 3rd Person cam on mouse left button
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