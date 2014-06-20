// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// PoseDesigner

#ifndef POSE_DESIGNER_H
#define POSE_DESIGNER_H

# include "boost/shared_ptr.hpp"
# include "opencv2/opencv.hpp"

# include "hand_camera_spec.h"
# include "hand_pose.h"
# include "hand_renderer.h"
# include "image_to_hog_calculator.h"
# include "scene_spec.h"
# include "simple_slider.h"

namespace libhand {

using namespace std;
using namespace boost;

class PoseDesigner {
 public:
  PoseDesigner();

  void Setup(int argc, char **argv);
  void Run();

  static const int kRenderWinWidth = 800;
  static const int kRenderWinHeight = 600;

  static const int kCamWinWidth = 200;
  static const int kCamWinHeight = 200;

  static const int kHogDispWidth = 200;
  static const int kHogDispHeight = 200;

  static const int kGUILeftX = 20;
  static const int kGUITopY = 40;

  static const int kGUIXSpace = 20;

 private:
  SimpleSlider *MakeAngleSlider(const string &name, const string &win_name);
  void UpdateSlider(SimpleSlider *slider);

  bool WarnKey(char cmd);
  bool ProcessKey(char cmd);

  void RenderEverything();
  void DisplayJointAngle();
  void DisplayClearWarning();
  void DisplayQuitWarning();
  void DisplayWarning(const string &line1, const string &line2);
  void DisplayHelp();
  void RenderHog();
  void DrawCameraSketch();

  void UpdateBSTSliders();

  void ZeroBend();
  void ZeroSide();
  void ZeroTwist();
  void ZeroJoints();

  void JointUp();
  void JointDown();

  void ResetCamera();

  string GetSceneFileName();

  void LoadPose();
  void SavePose();

  int cur_joint() { return sl_joint_no_->raw_val(); }

  int argc_;
  char **argv_;

  bool is_setup_;

  const string win_render_;
  const string win_camera_;
  SliderRefreshFn<PoseDesigner> update_slider_fn_;

  boost::shared_ptr<SimpleSlider> sl_bend_, sl_side_, sl_twist_, sl_joint_no_;
  boost::shared_ptr<SimpleSlider> sl_theta_, sl_phi_, sl_tilt_, sl_dist_mult_;

  SceneSpec scene_spec_;
  HandCameraSpec camera_spec_;
  HandRenderer hand_renderer_;
  ImageToHogCalculator hog_calc_;

  boost::shared_ptr<FullHandPose> hand_pose_;
  cv::Mat display_, cam_sketch_;
  bool render_help_;
  bool render_hog_;

  int warning_status_;
  char warning_key_;

  string pose_file_;

  // Disallow
  PoseDesigner(const PoseDesigner &rhs);
  PoseDesigner& operator= (const PoseDesigner &rhs);
};

}  // namespace libhand
#endif  // POSE_DESIGNER_H
