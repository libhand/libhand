// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>
//
// HandCameraSpec
//
// The camera view the hand is assumed to be somewhere on the surface
// of a sphere centered around the hand.
//
// As such, the camera's location can be described using polar coordinates
// (r, theta, phi). Additonally, the camera can tilt to the side.
//
// This class allows a conversion from a (r, theta, phi, tilt) representation
// to 3D (x,y,z) coordinates and 

#ifndef HAND_CAMERA_SPEC_H
#define HAND_CAMERA_SPEC_H

namespace Ogre {
class Vector3;
class Matrix3;
class Quaternion;
};

namespace libhand {

using namespace std;

class FullHandPose;

struct HandCameraSpec {
  // The distance from the center of the hand
  float r;
  // theta and phi are the angles along the sphere centered on the hand
  // tilt represents the camera tilt(roll) angel.
  //
  // Alternatively:
  // theta -> pitch, phi -> yaw, tilt -> roll
  float theta, phi, tilt;

  HandCameraSpec(float in_r = 10) : r(in_r),
                                    theta(0), phi(0), tilt(0) {}

  HandCameraSpec(float in_r, float in_theta, float in_phi, float in_tilt) :
    r(in_r), theta(in_theta), phi(in_phi), tilt(in_tilt) {}

  HandCameraSpec(const Ogre::Vector3 &pos, float in_tilt = 0);
  HandCameraSpec(const FullHandPose &pose, float in_r = 10);

  void GetPosition(float *x, float *y, float *z) const;
  Ogre::Vector3 GetPosition() const;
  Ogre::Matrix3 GetRotMatrix() const;
  Ogre::Quaternion GetQuaternion() const;

  void SetFromPosition(const Ogre::Vector3 &pos);
  void SetFromPosition(const Ogre::Vector3 &pos, float in_tilt);

  void SetFromRotMatrix(const Ogre::Matrix3 &rot);
  void SetFromRotMatrix(const Ogre::Matrix3 &rot, float in_r);

  void SetFromHandPose(const FullHandPose &pose);
  void SetFromHandPose(const FullHandPose &pose, float in_r);
};

}  // namespace libhand
#endif  // HAND_CAMERA_SPEC_H
