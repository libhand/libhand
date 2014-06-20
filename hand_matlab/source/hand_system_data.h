// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// HandSystemData

#ifndef HAND_SYSTEM_DATA_H
#define HAND_SYSTEM_DATA_H

# include "hand_pose.h"
# include "hand_renderer.h"
# include "scene_spec.h"

namespace libhand {

using namespace std;

class HandSystemData {
 public:
  static HandSystemData *Init();
  static HandSystemData *GetHandleFromMatlab();

  HandRenderer &hand_renderer() { return hand_renderer_; }

  const SceneSpec &scene_spec() const { return scene_spec_; }
  void set_scene_spec(const SceneSpec &scene_spec) {
    scene_spec_ = scene_spec;
    hand_pose_ = FullHandPose(scene_spec_.num_bones());
  }

  bool has_scene() const { return has_scene_; }
  void set_has_scene(bool has_scene) { has_scene_ = has_scene; }

  const FullHandPose &hand_pose() const { return hand_pose_; }
  void set_hand_pose(const FullHandPose &hand_pose) {
    hand_pose_ = hand_pose;
  }

 private:
  static void SetHandleInMatlab(HandSystemData *sys_data);
  static void InitInternal();
  static void UnloadData();

  FullHandPose hand_pose_;
  HandRenderer hand_renderer_;
  SceneSpec scene_spec_;
  bool has_scene_;

  HandSystemData();
  HandSystemData(const HandSystemData &rhs);
  HandSystemData& operator= (const HandSystemData &rhs);
};

}  // namespace libhand
#endif  // HAND_SYSTEM_DATA_H
