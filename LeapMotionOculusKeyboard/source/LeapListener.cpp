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
