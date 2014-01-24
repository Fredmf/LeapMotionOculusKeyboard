//Fred Fluegge 2014
//Everyone is permitted to copy and distribute verbatim or modified
//copies of this document, and changing it is allowed as long
//as the name is changed.
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from the use of this software.

#include "CamOVR.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <glm/gtx/transform.hpp>
#include <iostream>

#define CAMOVR_PI  3.1415f

CamOVR::CamOVR(){
    eye=glm::vec3(0.0f,0.17f, 0.1f); //initial value, player is 1m 80cm tall and stands 1m away from origin
    center=glm::vec3(0.0f,0.0f,0.0f);
    up=glm::vec3(0.0f,1.0f,0.0f);
    hv=glm::vec2(180.0f,90.0f);//and he is looking straingt forward, slightly down
    r=1.0f;
    
    rot_cw=glm::rotate(90.0f, 0.0f, 1.0f, 0.0f);
    rot_ccw=glm::rotate(-90.0f, 0.0f, 1.0f, 0.0f);
    
    fix=hv;
    first=true;
    moveScale= glm::scale(0.01f, 0.01f, 0.01f);
}
CamOVR::~CamOVR(){
}

void CamOVR::move(glm::vec3 in){
    if (in.x==0 && in.z ==0) { //up and down
        eye+=in;
    }else{
        glm::vec4 direction(center.x,0.0f,center.z,0.0f);
        std::cout << direction.x << std::endl;
        direction=moveScale*glm::normalize(direction);
        std::cout << direction.x << std::endl;
        glm::vec3 direction_three=glm::vec3(direction.x,direction.y,direction.z);
        if (in.z>0) {
            eye-=direction_three;
        }
        if (in.z<0) {
            eye+=direction_three;
        }
        direction=rot_cw*direction;
        direction_three=glm::vec3(direction.x,direction.y,direction.z);
        if (in.x>0) {//forward
            eye-=direction_three;
        }
        if (in.x<0) {
            eye+=direction_three;
        }
    }
}
void CamOVR::ovrInput(glm::vec3 yawPitchRoll){
    if (first){
        oculusZero=yawPitchRoll;
        first=false;
    }
    //std::cout << yawPitchRoll.x << std::endl;
    yawPitchRoll-=oculusZero;
    yawPitchRoll.x*=(-1.0);
    yawPitchRoll.z*=(-1.0);
    hv.x=fix.x+yawPitchRoll.x;
    hv.y=fix.y+yawPitchRoll.y;
    
    float tmp = yawPitchRoll.z;
    static float lastZ;
    yawPitchRoll.z-=lastZ;
    lastZ=tmp;
    glm::mat4 rMat=glm::rotate(yawPitchRoll.z, 0.0f, 0.0f, 1.0f);
    
    glm::vec4 tUp(up.x,up.y,up.z,0.0f);
    tUp=rMat*tUp;
    up=glm::vec3(tUp.x,tUp.y,tUp.z);
    
}
void CamOVR::orientation(glm::vec2 in){
    hv+=in;
    hv=glm::vec2(std::fmodf(hv.x, 360.0f),std::fmodf(hv.y, 360.0f));
    
    fix+=in;
    fix=glm::vec2(std::fmodf(fix.x, 360.0f),std::fmodf(fix.y, 360.0f));
}
void CamOVR::mouseMove(glm::vec2 in){
    in.x=in.x*(-1);
    float sensitivity = 3.0f;
    in.x=in.x/sensitivity;
    in.y=in.y/sensitivity;
    
    if (oldMPos!=glm::vec2(0.0f,0.0f)) {
        orientation(glm::vec2(in.x-oldMPos.x,in.y-oldMPos.y));
        oldMPos=in;
    }else{
        oldMPos=in;
    }
}
void CamOVR::yaw(float in){
    
}
void CamOVR::setIPD(float in){
    IPD=in;
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
    //glm::vec4 direction(center.x,0.0f,center.z,0.0f);
    //glm::vec4 offset=rot_ccw*direction;
    //float scale=((IPD*0.5f)/sqrtf(offset.x*offset.x+offset.z*offset.z));
    //offset=glm::scale(scale, 0.0f, scale)*offset;
    
    //return glm::translate(offset.x, offset.y, offset.z)*getLookAt();
	return glm::translate(IPD*0.5f, 0.0f, 0.0f)*getLookAt();
}
glm::mat4 CamOVR::getLookAtR(){
    //glm::vec4 direction(center.x,0.0f,center.z,0.0f);
    //glm::vec4 offset=rot_cw*direction;
    //float scale=((IPD*0.5f)/sqrtf(offset.x*offset.x+offset.z*offset.z));
    //offset=glm::scale(scale, 0.0f, scale)*offset;
    
    //return glm::translate(offset.x, offset.y, offset.z)*getLookAt();
	return glm::translate(-IPD*0.5f, 0.0f, 0.0f)*getLookAt();
}
void CamOVR::calculateCenter(void){
    center.x=std::sinf(hv.y/180.0f*CAMOVR_PI)*std::sinf(hv.x/180.0f*CAMOVR_PI)*r;
    center.y=std::cosf(hv.y/180.0f*CAMOVR_PI)*r;
    center.z=std::sinf(hv.y/180.0f*CAMOVR_PI)*std::cosf(hv.x/180.0f*CAMOVR_PI)*r;
}

float CamOVR::getOrientationAngle(void){
    glm::vec3 globalUpVec(0.0f,1.0f,0.0f);
    float theta = glm::dot(center, globalUpVec)/1;
    
    return acosf(theta);
}
glm::vec3 CamOVR::getOrientationVector(void){
    glm::vec3 globalUpVec(0.0f,1.0f,0.0f);
    glm::vec3 dir=glm::normalize(glm::cross(center, globalUpVec));
    
    return dir;
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