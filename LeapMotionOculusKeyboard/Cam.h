
//  GineCam.h
//
//  Created by Fred Flügge on 11.05.11

#ifndef Cam_h
#define Cam_h Cam_h

#include <math.h>
#include <SFML/Window.hpp>

class Cam {                            // 3. Person
public:
    // Konstruktor
    Cam();
    Cam(float H,float V, float R);
    
    void initCam(float H,float V, float R);
    // getter
    sf::Vector3f getCam();
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
    
private:
    // Kugelkoordinaten
    float r;
    float h;
    float v;
    
    // Kartesische Koordinaten
    sf::Vector3f pos;
    
    // Backup
    float resetval[3];
        
    // Mausvariablen
    int oldx;
    int oldy;
    bool  down;
};
#endif