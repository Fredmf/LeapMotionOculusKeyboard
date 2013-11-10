/******************************************************************************\
* Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include "LeapListener.h"
#include <iostream>

enum td{
    isInit,
    isConnect,
    hasFocus,
    id,
    timestamp,
    handsCount,
    fingersCount,
    toolsCount,
    gesturesCount,
    numberDisplays
};

void LeapListener::onInit(const Controller& controller) {
    isConnected=false;
}

void LeapListener::onConnect(const Controller& controller) {
    isConnected=true;
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

//
//  if (!frame.hands().isEmpty()) {
//    // Get the first hand
//    const Hand hand = frame.hands()[0];
//
//    // Check if the hand has any fingers
//    const FingerList fingers = hand.fingers();
//    if (!fingers.isEmpty()) {
//      // Calculate the hand's average finger tip position
//      Vector avgPos;
//      for (int i = 0; i < fingers.count(); ++i) {
//        avgPos += fingers[i].tipPosition();
//      }
//      avgPos /= (float)fingers.count();
//      currentState << "Hand has " << fingers.count()
//                << " fingers, average finger tip position" << avgPos << std::endl;
//    }
//
//    // Get the hand's sphere radius and palm position
//    currentState << "Hand sphere radius: " << hand.sphereRadius()
//              << " mm, palm position: " << hand.palmPosition() << std::endl;
//
//    // Get the hand's normal vector and direction
//    const Vector normal = hand.palmNormal();
//    const Vector direction = hand.direction();
//
//    // Calculate the hand's pitch, roll, and yaw angles
//    currentState << "Hand pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
//              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
//              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
//  }
//
//  // Get gestures
//  const GestureList gestures = frame.gestures();
//  for (int g = 0; g < gestures.count(); ++g) {
//    Gesture gesture = gestures[g];
//
//    switch (gesture.type()) {
//      case Gesture::TYPE_CIRCLE:
//      {
//        CircleGesture circle = gesture;
//        std::string clockwiseness;
//
//        if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
//          clockwiseness = "clockwise";
//        } else {
//          clockwiseness = "counterclockwise";
//        }
//
//        // Calculate angle swept since last frame
//        float sweptAngle = 0;
//        if (circle.state() != Gesture::STATE_START) {
//          CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
//          sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
//        }
//        currentState << "Circle id: " << gesture.id()
//                  << ", state: " << gesture.state()
//                  << ", progress: " << circle.progress()
//                  << ", radius: " << circle.radius()
//                  << ", angle " << sweptAngle * RAD_TO_DEG
//                  <<  ", " << clockwiseness << std::endl;
//        break;
//      }
//      case Gesture::TYPE_SWIPE:
//      {
//        SwipeGesture swipe = gesture;
//        currentState << "Swipe id: " << gesture.id()
//          << ", state: " << gesture.state()
//          << ", direction: " << swipe.direction()
//          << ", speed: " << swipe.speed() << std::endl;
//        break;
//      }
//      case Gesture::TYPE_KEY_TAP:
//      {
//        KeyTapGesture tap = gesture;
//        currentState << "Key Tap id: " << gesture.id()
//          << ", state: " << gesture.state()
//          << ", position: " << tap.position()
//          << ", direction: " << tap.direction()<< std::endl;
//        break;
//      }
//      case Gesture::TYPE_SCREEN_TAP:
//      {
//        ScreenTapGesture screentap = gesture;
//        currentState << "Screen Tap id: " << gesture.id()
//        << ", state: " << gesture.state()
//        << ", position: " << screentap.position()
//        << ", direction: " << screentap.direction()<< std::endl;
//        break;
//      }
//      default:
//        currentState << "Unknown gesture type." << std::endl;
//        break;
//    }
//  }
//
//  if (!frame.hands().isEmpty() || !gestures.isEmpty()) {
//    currentState << std::endl;
//  }
}

void LeapListener::onFocusGained(const Controller& controller) {
    hasFocus="true";
}

void LeapListener::onFocusLost(const Controller& controller) {
    hasFocus="false";
}
