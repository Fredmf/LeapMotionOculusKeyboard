//
//  CamOVR.cpp
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 14.01.14.
//  Copyright (c) 2014 Fachbereich Informatik. All rights reserved.
//

#include "CamOVR.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <glm/gtx/transform.hpp>

CamOVR::CamOVR(){
    eye=glm::vec3(0.0f,800.0f,800.0f);
    center=glm::vec3(0.01f,2.01f,0.01f);
    up=glm::vec3(0.0f,1.0f,0.0f);
    hv=glm::vec2(180.0f,135.0f);
    r=1.0f;
}
CamOVR::~CamOVR(){
}

void CamOVR::move(glm::vec3 in){
    in.x=in.x*5;
    in.y=in.y*5;
    in.z=in.z*5;
    eye+=in;
}
void CamOVR::orientation(glm::vec2 in){
    hv+=in;
    hv=glm::vec2(std::fmodf(hv.x, 360.0f),std::fmodf(hv.y, 360.0f));
}
void CamOVR::mouseMove(glm::vec2 in){
    float sensitivity = 2.0f;
    in.x=in.x/sensitivity;
    in.y=in.y/sensitivity;
    
    if (oldMPos!=glm::vec2(0.0f,0.0f)) {
        in.x=in.x*(-1);
        orientation(in-oldMPos);
        oldMPos=in;
    }else{
        oldMPos=in;
    }
}
void CamOVR::yaw(float in){
    
}
void CamOVR::setIPD(float in){
    float IPDScale=40.0f;//ipd scale to world, or 3d effect scale
    IPD=in*IPDScale;
}

glm::vec3 CamOVR::getEye(){
    return eye;
}
glm::vec3 CamOVR::getCenter(){
    calculateCenter();
    return center+eye;
}
glm::vec3 CamOVR::getUp(){
    return up;
}
glm::mat4 CamOVR::getLookAt(){
    return glm::lookAt(getEye(), getCenter(), getUp());
}
glm::mat4 CamOVR::getLookAtL(){
    glm::vec4 direction(center.x,0.0f,center.z,0.0f);
    glm::mat4 rotMat=glm::rotate(-90.0f, 0.0f, 1.0f, 0.0f);
    glm::vec4 offset=rotMat*direction;
    float scale=((IPD*0.5)/sqrtf(offset.x*offset.x+offset.z*offset.z));
    offset=glm::scale(scale, 0.0f, scale)*offset;
    
    return glm::translate(offset.x, offset.y, offset.z)*getLookAt();
}
glm::mat4 CamOVR::getLookAtR(){
    glm::vec4 direction(center.x,0.0f,center.z,0.0f);
    glm::mat4 rotMat=glm::rotate(90.0f, 0.0f, 1.0f, 0.0f);
    glm::vec4 offset=rotMat*direction;
    float scale=((IPD*0.5)/sqrtf(offset.x*offset.x+offset.z*offset.z));
    offset=glm::scale(scale, 0.0f, scale)*offset;
    
    return glm::translate(offset.x, offset.y, offset.z)*getLookAt();
}
void CamOVR::calculateCenter(void){
    center.x=std::sinf(hv.y/180.0*M_PI)*std::sinf(hv.x/180*M_PI)*r;
    center.y=std::cosf(hv.y/180.0*M_PI)*r;
    center.z=std::sinf(hv.y/180.0*M_PI)*std::cosf(hv.x/180*M_PI)*r;
}

std::string CamOVR::getData(){
    std::stringstream output;
    
    output << "CamOVR: h: " << hv.x << " v: " << hv.y << " eye.x: " << eye.x << " eye.y: " << eye.y << " eye.z: " << eye.z;
    return output.str();
}

void CamOVR::setMouseDown(bool in){
    mouseIsDown=in;
    if (in==false) {
        oldMPos=glm::vec2(0.0f,0.0f);
    }
}