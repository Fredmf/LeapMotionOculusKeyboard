#include "Cam.h"
#include <cmath>

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
}

Vector3 Cam::getCam(){
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

void Cam::zoomCam(int value){
    r-=value;
    if (r <= 0.01)
        r=0.01f;
    camCalculate();
}

void Cam::mouseMove(int x, int y){
    if (oldx != 0 && oldy != 0) {
        moveCamUp((float)(y-oldy));
        moveCamRight((float)(x-oldx));
        oldx=x;
        oldy=y;
    }else{
        oldx=x;
        oldy=y;
    }
}

void Cam::camReset(){
    h=resetval[0];
    v=resetval[1];
    r=resetval[2];
    camCalculate();
}

void Cam::mouseRelease(){
        oldy=0;
        oldx=0;
}

inline void Cam::camCalculate(){
    pos.x=sinf(v/180*PI)*sinf(h/180*PI)*r;
    pos.y=cosf(v/180*PI)*r;
    pos.z=sinf(v/180*PI)*cosf(h/180*PI)*r;
}

