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
//
// The HandPose class contains the hand joint angle information along
// with the relation ship between the hand and the camera. The camera
// is assumed to be somewhere on the surface of a sphere centered
// around the hand.
//
// The distance from the camera is not considered important, so the
// camera location is encoded as a rotation matrix. One can use the
// HandCameraSpec class (in hand_camera_spec.h) to convert between
// different ways of describing the camera position. The HandPose
// class provides for some conversions for convenience.
//
// Each joint is described using the bend, side and twist angles in
// radians. This is the angle closed between the parent and the child
// bone along the 3D coordinate axes relative to the tip (head) of the
// child bone. One needs to view the hand skeleton in a 3D program
// such as Blender to understand the exact relationship between the
// bones. The blender hand object and armature are provided with this
// package. Each joint's angles can be converted to and from a
// quaternion.
//
// The FullHandPose class supports "serialization", i.e. being written
// out to different media.
//
// The FullHandPose class can be saved to arrays or STL containers of floats,
// or written to and read from the human-readable YAML files.

#ifndef HAND_POSE_H
#define HAND_POSE_H

# include <algorithm>
# include <string>
# include <vector>

// Forward Ogre::Quaternion
// Forward Ogre::Matrix3
namespace Ogre {
class Quaternion;
class Matrix3;
}

namespace cv {
class FileStorage;
class FileNode;
}

namespace libhand {

using namespace std;

class HandCameraSpec;
class SceneSpec;

struct HandJoint {
  float bend;   // yaw
  float side;   // roll
  float twist;  // pitch

  HandJoint() : bend(0), side(0), twist(0) {}
  HandJoint(float bend_in, float side_in, float twist_in) :
    bend(bend_in), side(side_in), twist(twist_in) {}

  HandJoint(const Ogre::Quaternion &q);
  Ogre::Quaternion ToQuaternion();
};

// These two methods allow us to read and write joint information into
// already opened YAML files (serialization).
cv::FileStorage& operator << (cv::FileStorage &store,
                              const HandJoint &handjoint);
void operator >> (const cv::FileNode &store_node, HandJoint &handjoint);

class FullHandPose {
public:
  FullHandPose(int num_joints = 15);

  static const int kRotMatrixN = 3;
  static const int kRotMatrixElements = kRotMatrixN * kRotMatrixN;
  static const int kElementsPerJoint = 3;

  int num_joints() const;
  int total_joint_elements() const;
  int total_elements() const;

  inline int data_size() const { return sizeof(data_); }

  // Easy-to-use accessors for the rotation
  float rotation(int index) const;
  float &rotation(int index);

  void GetRotMatrix(float *rot_mtx) const;
  void SetRotMatrix(const float *rot_mtx);

  void SetRotMatrix(const HandCameraSpec &camera_spec);

  Ogre::Quaternion GetRotQuaternionOgre() const;
  void SetRotQuaternionOgre(const Ogre::Quaternion &q);

  void GetRotEulerPYR(float *pitch, float *yaw, float *roll) const;
  void SetRotEulerPYR(float pitch, float yaw, float roll);

  Ogre::Matrix3 GetRotMatrixOgre() const;
  void SetRotMatrixOgre(const Ogre::Matrix3 &m3);

  // Easy-to-use accessors for joints
  HandJoint joint(int joint) const;
  void set_joint(int joint_num, HandJoint joint);

  float bend(int joint) const;
  float &bend(int joint);
  float side(int joint) const;
  float &side(int joint);
  float twist(int joint) const;
  float &twist(int joint);

  void Clear();
  void ClearRotation();
  void ClearJoints();

  // Trivial iterators
  float *begin();
  float *end();
  const float *begin() const;
  const float *end() const;

  float *rotation_begin();
  float *rotation_end();
  const float *rotation_begin() const;
  const float *rotation_end() const;

  float *joints_begin();
  float *joints_end();
  const float *joints_begin() const;
  const float *joints_end() const;

  // Serialization

  // What to do if running into an unknown bone
  enum IfUnknownBoneDo {
    FAIL,
    WARN,
    NOTHING
  };

  // A pose can be loaded from a YAML (.yml or .yaml) file A scene
  // spec is required to relate the bone names in the hand 3D object
  // skeleton to indices in the HandPose structure.
  void Load(const string &file,
            const SceneSpec &scene_spec,
            IfUnknownBoneDo if_unknown_bone_do = WARN);

  // A pose can be saved to a YAML (.yml or .yaml) file
  // A scene spec is required to relate the indices in the HandPose structure
  // to bone names in the hand 3D object skeleton.
  void Save(const string &file, const SceneSpec &scene_spec) const;

  // Provides a way to read/parse a pose from an already opened YAML file
  void Load(cv::FileStorage &store,
            const SceneSpec &scene_spec,
            IfUnknownBoneDo if_unknown_bone_do = WARN);

  // Provides a way to write the pose information into an already
  // opened YAML file
  void Save(cv::FileStorage &store,
            const SceneSpec &scene_spec) const;

private:
  int JointOffset(int joint) const;

  int num_joints_;
  vector<float> data_;
};

inline int FullHandPose::num_joints() const {
  return num_joints_;
}

inline int FullHandPose::total_joint_elements() const {
  return kElementsPerJoint * num_joints();
}

inline int FullHandPose::total_elements() const {
  return kRotMatrixElements + total_joint_elements();
}

inline FullHandPose::FullHandPose(int num_joints)
  : num_joints_(num_joints),
    data_(total_elements()) {
  ClearRotation();
}

inline float *FullHandPose::begin() { return data_.data(); }
inline float *FullHandPose::end() { return begin() + total_elements(); }
inline const float *FullHandPose::begin() const { return data_.data(); }
inline const float *FullHandPose::end() const {
  return begin() + total_elements();
}

inline float FullHandPose::rotation(int index) const {
  return rotation_begin()[index];
}

inline float &FullHandPose::rotation(int index) {
  return rotation_begin()[index];
}

inline int FullHandPose::JointOffset(int joint) const {
  return kRotMatrixElements + joint * kElementsPerJoint;
}

inline float &FullHandPose::bend(int joint) {
  return data_[JointOffset(joint)];
}

inline float FullHandPose::bend(int joint) const {
  return data_[JointOffset(joint)];
}

inline float &FullHandPose::side(int joint) {
  return data_[JointOffset(joint) + 1];
}

inline float FullHandPose::side(int joint) const {
  return data_[JointOffset(joint) + 1];
}

inline float &FullHandPose::twist(int joint) {
  return data_[JointOffset(joint) + 2];
}

inline float FullHandPose::twist(int joint) const {
  return data_[JointOffset(joint) + 2];
}

inline HandJoint FullHandPose::joint(int joint) const {
  return HandJoint(bend(joint),
                   side(joint),
                   twist(joint));
}

inline void FullHandPose::set_joint(int joint_num,
                                    HandJoint joint) {
  bend(joint_num) = joint.bend;
  side(joint_num) = joint.side;
  twist(joint_num) = joint.twist;
}

inline void FullHandPose::GetRotMatrix(float *rot_mtx) const {
  copy(rotation_begin(), rotation_end(), rot_mtx);
}

inline void FullHandPose::SetRotMatrix(const float *rot_mtx) {
  copy(rot_mtx, rot_mtx + kRotMatrixElements, rotation_begin());
}

inline float *FullHandPose::rotation_begin() {
  return data_.data();
}

inline const float *FullHandPose::rotation_begin() const {
  return data_.data();
}

inline float *FullHandPose::rotation_end() {
  return rotation_begin() + kRotMatrixElements;
}

inline const float *FullHandPose::rotation_end() const {
  return rotation_begin() + kRotMatrixElements;
}

inline float *FullHandPose::joints_begin() {
  return rotation_end();
}

inline const float *FullHandPose::joints_begin() const {
  return rotation_end();
}

inline float *FullHandPose::joints_end() {
  return joints_begin() + total_joint_elements();
}

inline const float *FullHandPose::joints_end() const {
  return joints_begin() + total_joint_elements();
}

inline void FullHandPose::Clear() {
  ClearRotation();
  ClearJoints();
}

inline void FullHandPose::ClearRotation() {
  fill(rotation_begin(), rotation_end(), 0.);

  float *rot_arr = rotation_begin();

  rot_arr[0] = 1.; rot_arr[4] = 1.; rot_arr[8] = 1;
}

inline void FullHandPose::ClearJoints() {
  fill(joints_begin(), joints_end(), 0.);
}

}  // namespace libhand
#endif  // HAND_POSE_H
