#ifndef Cam_h
#define Cam_h Cam_h

#include <math.h>
#include "Vectors.h"

class Cam {                            // 3. Person
public:
    // Konstruktor
    Cam();
    Cam(float H,float V, float R);
    
    void initCam(float H,float V, float R);
    // getter
    Vector3 getCam();
    bool isDown(void);
    
    // setter
    void setDown(bool state);
    
    // bewegungsmethoden
    inline void moveCamRight(float value);
    inline void moveCamUp(float value);
    void zoomCam(int value);
    
    // Mausschnitstelle
    void mouseMove(int x, int y);
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
    Vector3 pos;
    
    // Backup
    float resetval[3];
        
    // Mausvariablen
    int oldx;
    int oldy;
    bool  down;
};
#endif