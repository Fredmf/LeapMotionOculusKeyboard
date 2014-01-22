//Fred Fluegge 2014
//Everyone is permitted to copy and distribute verbatim or modified
//copies of this document, and changing it is allowed as long
//as the name is changed.
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from the use of this software.

#include "LeapListener.h"
#include <iostream>


void LeapListener::onInit(const Controller& controller) {
    isConnected=false;
}

void LeapListener::onConnect(const Controller& controller) {
    isConnected=true;
	//controller.enableGesture(Gesture::TYPE_KEY_TAP);
}

void LeapListener::onDisconnect(const Controller& controller) {
    isConnected=false;
}

void LeapListener::onExit(const Controller& controller) {
    std::cout << "LeapListener Exited" << std::endl;
}

void LeapListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  frame = controller.frame();

}

void LeapListener::onFocusGained(const Controller& controller) {
    hasFocus="true";
}

void LeapListener::onFocusLost(const Controller& controller) {
    hasFocus="false";
}
