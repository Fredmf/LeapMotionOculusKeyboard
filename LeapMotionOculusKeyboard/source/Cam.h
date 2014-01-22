//Fred Fluegge 2014
//Everyone is permitted to copy and distribute verbatim or modified
//copies of this document, and changing it is allowed as long
//as the name is changed.
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from the use of this software.

#ifndef Cam_h
#define Cam_h Cam_h

#include <math.h>
#include <glm/glm.hpp>

class Cam {                            // 3. Person
public:
    // Konstruktor
    Cam();
    Cam(float H,float V, float R);
    
    void initCam(float H,float V, float R);
    // getter
    glm::vec3 getCam();
    bool isDown(void);
    
    // setter
    void setDown(bool state);
    
    // bewegungsmethoden
    inline void moveCamRight(float value);
    inline void moveCamUp(float value);
    inline void moveCam(glm::ivec2 mPosDelta);
    void zoomCam(int value);
    
    // Mausschnitstelle
    void mouseMove(glm::ivec2 mPos);
    void mouseRelease(void);
    void mouseReset();
    
    // Hilfsfunktionen
    void camReset();
    inline void camCalculate();
	void printData();
    
private:
    // Kugelkoordinaten
    float r;
    float h;
    float v;
    
    // Kartesische Koordinaten
    glm::vec3 pos;
    
    // Backup
    float resetval[3];
        
    // Mausvariablen
    glm::ivec2 oldMPos;
    bool  down;
};
#endif