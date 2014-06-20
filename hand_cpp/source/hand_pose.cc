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
// HandPose

# include "hand_pose.h"

# include <algorithm>
# include <stdexcept>
# include <iostream>

# include "OGRE/OgreMatrix3.h"
# include "OGRE/OgreQuaternion.h"

# include "opencv2/opencv.hpp"

# include "hand_camera_spec.h"
# include "scene_spec.h"
# include "printfstring.h"

namespace libhand {

void FullHandPose::SetRotMatrix(const HandCameraSpec &camera_spec) {
  SetRotMatrixOgre(camera_spec.GetRotMatrix());
}

Ogre::Matrix3 FullHandPose::GetRotMatrixOgre() const {
  Ogre::Matrix3 ogre_mtx;

  copy(rotation_begin(), rotation_end(), &(ogre_mtx[0][0]));
  return ogre_mtx;
}

void FullHandPose::SetRotMatrixOgre(const Ogre::Matrix3 &m3) {
  copy(&(m3[0][0]), &(m3[0][0]) + kRotMatrixElements, rotation_begin());
}

Ogre::Quaternion FullHandPose::GetRotQuaternionOgre() const {
  return Ogre::Quaternion(GetRotMatrixOgre());
}

void FullHandPose::SetRotQuaternionOgre(const Ogre::Quaternion &q) {
  Ogre::Matrix3 ogre_mtx;

  q.ToRotationMatrix(ogre_mtx);
  SetRotMatrixOgre(ogre_mtx);
}

HandJoint::HandJoint(const Ogre::Quaternion &q) {
  Ogre::Matrix3 m;
  Ogre::Radian pitch, yaw, roll;

  q.ToRotationMatrix(m);
  m.ToEulerAnglesXYZ(yaw, pitch, roll);

  bend = yaw.valueRadians();
  twist = pitch.valueRadians();
  side = roll.valueRadians();
}

Ogre::Quaternion HandJoint::ToQuaternion() {
  Ogre::Matrix3 m;

  m.FromEulerAnglesXYZ(Ogre::Radian(bend),   // Yaw
                       Ogre::Radian(twist),  // Pitch
                       Ogre::Radian(side));  // Roll
  return Ogre::Quaternion(m);
}

void FullHandPose::GetRotEulerPYR(float *pitch,
                                  float *yaw,
                                  float *roll) const {
  Ogre::Radian ogre_pitch, ogre_yaw, ogre_roll;
  Ogre::Matrix3 m = GetRotMatrixOgre();

  m.ToEulerAnglesXYZ(ogre_yaw, ogre_pitch, ogre_roll);
  *pitch = ogre_pitch.valueRadians();
  *yaw = ogre_yaw.valueRadians();
  *roll = ogre_roll.valueRadians();
}

void FullHandPose::SetRotEulerPYR(float pitch, float yaw, float roll) {
  Ogre::Matrix3 m;

  m.FromEulerAnglesXYZ(Ogre::Radian(yaw),
                       Ogre::Radian(pitch),
                       Ogre::Radian(roll));

  SetRotMatrixOgre(m);
}

// Serialization routines
//    Serializing HandJoint
cv::FileStorage& operator << (cv::FileStorage &store,
                              const HandJoint &joint) {
  store << "[:" << joint.bend << joint.side << joint.twist << "]";
  return store;
}

void operator >> (const cv::FileNode &store_node, HandJoint &joint) {
  if ((store_node.type() != cv::FileNode::SEQ) ||
      (store_node.size() != 3)) {
    throw runtime_error("A hand joint must be a 3 element sequence");
  }

  joint.bend = (float) store_node[0];
  joint.side = (float) store_node[1];
  joint.twist = (float) store_node[2];
}

//    Serializing FullHandPose
void FullHandPose::Load(const string &filename,
                        const SceneSpec &scene_spec,
                        FullHandPose::IfUnknownBoneDo if_unknown_bone_do) {
  if (filename.empty()) {
    throw runtime_error("Empty filename specified to FullHandPose::Load");
  }

  cv::FileStorage fs(filename, cv::FileStorage::READ);
  if (!fs.isOpened()) {
    throw runtime_error(PrintFString("Can't open file \"%s\" for reading",
                                     filename.c_str()));
  }

  Load(fs, scene_spec, if_unknown_bone_do);

  fs.release();
}

void FullHandPose::Save(const string &filename,
                        const SceneSpec &scene_spec) const {
  if (filename.empty()) {
    throw runtime_error("Empty filename specified to FullHandPose::Save");
  }

  cv::FileStorage fs(filename, cv::FileStorage::WRITE);
  if (!fs.isOpened()) {
    throw runtime_error(PrintFString("Can't open file \"%s\" for writing",
                                     filename.c_str()));
  }

  Save(fs, scene_spec);

  fs.release();
}

void FullHandPose::Save(cv::FileStorage &store,
                        const SceneSpec &scene_spec) const {
  store << "rotation" << "[:";
  for (int i = 0; i < kRotMatrixElements; ++i) {
    store << rotation(i);
  }
  store << "]";

  store << "hand_joints" << "{";
  for (int i = 0; i < num_joints(); ++i) {
    store << scene_spec.bone_name(i) << joint(i);
  }
  store << "}";
}

void FullHandPose::Load(cv::FileStorage &store,
                        const SceneSpec &scene_spec,
                        FullHandPose::IfUnknownBoneDo if_unknown_bone_do) {
  cv::FileNode rot_node = store["rotation"];
  if (rot_node.empty()) {
    throw runtime_error("No rotation information present!");
  }

  if ((rot_node.type() != cv::FileNode::SEQ) ||
      ((int) rot_node.size() != kRotMatrixElements)) {
    throw runtime_error(PrintFString
                        ("A rotation must be a %d element sequence",
                         kRotMatrixElements));
  }

  for (int i = 0; i < kRotMatrixElements; ++i) {
    rotation(i) = rot_node[i];
  }

  cv::FileNode joints_node = store["hand_joints"];
  if (joints_node.empty()) {
    throw runtime_error("No hand joint informaton present!");
  }

  if (joints_node.type() != cv::FileNode::MAP) {
    throw runtime_error("Hand joints are supposed to be "
                        "specified as a mapping");
  }


  for (cv::FileNodeIterator i = joints_node.begin(), e = joints_node.end();
       i != e;
       ++i) {
    int bone_idx = scene_spec.bone_index((*i).name());

    if (bone_idx == -1) {
      switch (if_unknown_bone_do) {
      case FAIL:
        throw runtime_error(PrintFString("Unknown bone: %s",
                                         (*i).name().c_str()));
        break;
      case WARN:
        cout << "Warning: Unknown bone: " << (*i).name().c_str();
        break;
      case NOTHING: break;
      }
    } else {
      HandJoint hand_joint;
      (*i) >> hand_joint;
      set_joint(bone_idx, hand_joint);
    }
  }
}

}  // namespace libhand
