//Fred Fluegge 2014
//Everyone is permitted to copy and distribute verbatim or modified
//copies of this document, and changing it is allowed as long
//as the name is changed.
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from the use of this software.

#ifndef __LeapMotionOculusKeyboard__CamOVR__
#define __LeapMotionOculusKeyboard__CamOVR__

#define CAM_RAD_TO_DEG 57.2957795

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
    float getOrientationAngle(void);
    glm::vec3 getOrientationVector(void);
    
public:
    void ovrInput(glm::vec3 yawPitchRoll);
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
    glm::vec2 fix;
    float r;
    
    bool first;
    glm::mat4 moveScale;
    glm::vec3 oculusZero;
    
    glm::mat4 rot_cw;
    glm::mat4 rot_ccw;
    
    glm::vec2 oldMPos;
    bool mouseIsDown;
};

#endif /* defined(__LeapMotionOculusKeyboard__CamOVR__) */
