//Fred Fluegge 2014
//Everyone is permitted to copy and distribute verbatim or modified
//copies of this document, and changing it is allowed as long
//as the name is changed.
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from the use of this software.

#include "Cam.h"
#include <cmath>
#include <iostream>

#define PI  3.1415f

Cam::Cam(){
    
}
Cam::Cam(float H, float V, float R)
: h(H), v(V+90.0f),r(R){
    resetval[0]=h;
    resetval[1]=v;
    resetval[2]=r;
    camCalculate();
    down = false;
    camCalculate();
}
void Cam::initCam(float H, float V, float R){
    h=H;
    v=V;
    r=R;
    resetval[0]=h;
    resetval[1]=v;
    resetval[2]=r;
    camCalculate();
    down = false;
    camCalculate();
}

glm::vec3 Cam::getCam(){
    return pos;
}

bool Cam::isDown(){
    return down;
}

void Cam::setDown(bool state){
    down=state;
}

inline void Cam::moveCamRight(float value){
    h-=value;
    if (h>360.0f)
        h-=360.0f;
    if (h<-360.0f)
        h+=360.0f;
    camCalculate();
}

inline void Cam::moveCamUp(float value){
    v-=value;
    if (v>177.0f)
        v=177.0f;
    if (v<1.0f)
        v=1.0f;
    camCalculate();
}

inline void Cam::moveCam(glm::ivec2 mPosDelta){
    glm::ivec2 hvTmp(h,v);
    hvTmp-=mPosDelta;
    
    if (hvTmp.x>360.0f)
        hvTmp.x-=360.0f;
    if (hvTmp.x<-360.0f)
        hvTmp.x+=360.0f;
    camCalculate();
    
    if (hvTmp.y>177.0f)
        hvTmp.y=177.0f;
    if (hvTmp.y<1.0f)
        hvTmp.y=1.0f;
    
    h=hvTmp.x;
    v=hvTmp.y;
    
    camCalculate();
}

void Cam::zoomCam(int value){
    r-=((float)value/10.0f);
    if (r <= 0.01)
        r=0.01f;
    camCalculate();
}

void Cam::mouseMove(glm::ivec2 mPos){
    if (oldMPos.x != 0 && oldMPos.y != 0) {
        moveCam(mPos-oldMPos);
        oldMPos=mPos;
    }else{
        oldMPos=mPos;
    }
}

void Cam::camReset(){
    h=resetval[0];
    v=resetval[1];
    r=resetval[2];
    camCalculate();
}

void Cam::mouseRelease(){
    oldMPos=glm::ivec2(0,0);
}

inline void Cam::camCalculate(){
    pos.x=sinf(v/180*PI)*sinf(h/180*PI)*r;
    pos.y=cosf(v/180*PI)*r;
    pos.z=sinf(v/180*PI)*cosf(h/180*PI)*r;
}

void Cam::printData(){
	std::cout << "h: " << h << " v: " << v << " r: " << r << std::endl;
}
