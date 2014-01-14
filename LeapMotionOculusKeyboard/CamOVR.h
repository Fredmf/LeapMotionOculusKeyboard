//
//  CamOVR.h
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 14.01.14.
//  Copyright (c) 2014 Fachbereich Informatik. All rights reserved.
//

#ifndef __LeapMotionOculusKeyboard__CamOVR__
#define __LeapMotionOculusKeyboard__CamOVR__

#include <glm/glm.hpp>
#include <string>

class CamOVR{
public:
    CamOVR();
    ~CamOVR();
    
public:
    void move(glm::vec3 in);
    void orientation(glm::vec2 in);
    void mouseMove(glm::vec2 in);
    void yaw(float in);
    void setIPD(float in);
    
public:
    glm::vec3 getEye(void);
    glm::vec3 getCenter(void);
    glm::vec3 getUp(void);
    glm::mat4 getLookAt(void);
    glm::mat4 getLookAtL(void);
    glm::mat4 getLookAtR(void);
    
public:
    void setMouseDown(bool in);
    bool isMouseDown(){return mouseIsDown;};
    std::string getData();
    
private:
    void calculateCenter(void);
    
private:
    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;
    float IPD;
    glm::vec2 hv;
    float r;
    
    
    glm::vec2 oldMPos;
    bool mouseIsDown;
};

#endif /* defined(__LeapMotionOculusKeyboard__CamOVR__) */
