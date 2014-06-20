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

# include "hand_camera_spec.h"

# include <cmath>

# include "OGRE/OgreVector3.h"
# include "OGRE/OgreMatrix3.h"

# include "hand_pose.h"

namespace libhand {

using namespace Ogre;

HandCameraSpec::HandCameraSpec(const Ogre::Vector3 &pos, float in_tilt) {
  SetFromPosition(pos, in_tilt);
}

HandCameraSpec::HandCameraSpec(const FullHandPose &pose, float in_r) {
  SetFromHandPose(pose, in_r);
}

void HandCameraSpec::SetFromPosition(const Ogre::Vector3 &pos) {
  SetFromPosition(pos, tilt);
}

void HandCameraSpec::SetFromPosition(const Ogre::Vector3 &pos,
                                     float in_tilt) {
  r = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
  theta = atan2(pos.x, pos.z);
  phi = atan2(-pos.y, pos.z);
  tilt = in_tilt;
}

void HandCameraSpec::SetFromHandPose(const FullHandPose &pose) {
  SetFromHandPose(pose, r);
}

void HandCameraSpec::SetFromHandPose(const FullHandPose &pose, float in_r) {
  SetFromRotMatrix(pose.GetRotMatrixOgre(), in_r);
}

void HandCameraSpec::SetFromRotMatrix(const Ogre::Matrix3 &rot) {
  SetFromRotMatrix(rot, r);
}

void HandCameraSpec::SetFromRotMatrix(const Ogre::Matrix3 &rot, float in_r) {
  Ogre::Radian pitch, yaw, roll;
  rot.ToEulerAnglesXYZ(yaw, pitch, roll);

  phi = yaw.valueRadians();
  theta = pitch.valueRadians();
  tilt = roll.valueRadians();

  r = in_r;
}

void HandCameraSpec::GetPosition(float *x, float *y, float *z) const {
  *x = r * sin(theta);
  *y = -r * cos(theta) * sin(phi);
  *z = r * cos(theta) * cos(phi);
}

Ogre::Vector3 HandCameraSpec::GetPosition() const {
  float x, y, z;
  GetPosition(&x, &y, &z);

  return Vector3(x, y, z);
}

Ogre::Matrix3 HandCameraSpec::GetRotMatrix() const {
  Matrix3 m;

  m.FromEulerAnglesXYZ(Radian(phi), Radian(theta), Radian(tilt));
  //m.FromEulerAnglesXYZ(Radian(phi), Radian(theta), Radian(tilt));
  return m;
}

Ogre::Quaternion HandCameraSpec::GetQuaternion() const {
  return Quaternion(GetRotMatrix());
}

}  // namespace libhand
