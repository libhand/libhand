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

# include "pose_designer.h"

# include <iostream>
# include <stdexcept>
# include <string>

# include "boost/math/constants/constants.hpp"
# include "opencv2/opencv.hpp"
# include "opencv2/highgui/highgui.hpp"

# include "file_dialog.h"
# include "hog_cell_rectangles.h"
# include "hog_descriptor.h"
# include "hog_utils.h"
# include "image_to_hog_calculator.h"
# include "image_utils.h"
# include "printfstring.h"
# include "simple_slider.h"
# include "text_printer.h"

namespace libhand {

PoseDesigner::PoseDesigner()
  : argc_(0),
    argv_(NULL),
    is_setup_(false),
    win_render_("Pose Designer - Main Window"),
    win_camera_("Camera Window"),
    update_slider_fn_(this, &PoseDesigner::UpdateSlider),
    cam_sketch_(kCamWinHeight, kCamWinWidth, CV_8UC3),
    render_help_(false),
    render_hog_(false),
    warning_status_(0),
    warning_key_(-1),
    pose_file_("") {
}

void PoseDesigner::Setup(int argc, char **argv) {
  argc_ = argc;
  argv_ = argv;

  string scene_file = GetSceneFileName();
  if (scene_file == "") {
    throw runtime_error("No scene file chosen!");
  }

  scene_spec_ = SceneSpec(scene_file);

  hand_renderer_.Setup(kRenderWinWidth, kRenderWinHeight);
  hand_renderer_.LoadScene(scene_spec_);

  hand_pose_.reset(new FullHandPose(scene_spec_.num_bones()));

  cv::namedWindow(win_render_, CV_WINDOW_AUTOSIZE);
  cvMoveWindow(win_render_.c_str(), kGUILeftX, kGUITopY);

  sl_bend_.reset(MakeAngleSlider("bend", win_render_));
  sl_side_.reset(MakeAngleSlider("side", win_render_));
  sl_twist_.reset(MakeAngleSlider("twist", win_render_));

  sl_joint_no_.reset(new SimpleSlider("joint_no", win_render_,
                                      0, hand_pose_->num_joints() - 1,
                                      hand_pose_->num_joints() - 1,
                                      &update_slider_fn_));

  cv::namedWindow(win_camera_, CV_WINDOW_AUTOSIZE);
  cvMoveWindow(win_camera_.c_str(),
               kGUILeftX + kRenderWinWidth + kGUIXSpace,
               kGUITopY);

  sl_theta_.reset(MakeAngleSlider("theta", win_camera_));
  sl_phi_.reset(MakeAngleSlider("phi", win_camera_));
  sl_tilt_.reset(MakeAngleSlider("tilt", win_camera_));
  sl_dist_mult_.reset(new SimpleSlider("distance", win_camera_,
                                       0.1, 5., 1000, &update_slider_fn_));

  ZeroBend(); ZeroSide(); ZeroTwist();
  ResetCamera();

  is_setup_ = true;
}

void PoseDesigner::Run() {
  if (!is_setup_) {
    throw runtime_error("Setup() not called for PoseDesigner");
  }

  RenderEverything();

  for (;;) {
    char cmd = cv::waitKey();

    if (!ProcessKey(cmd)) break;

    RenderEverything();
  }
}

SimpleSlider *PoseDesigner::MakeAngleSlider(const string &name,
                                            const string &win_name) {
  SimpleSlider *slider = new SimpleSlider(name, win_name,
                                          -math::constants::pi<float>(),
                                          math::constants::pi<float>(),
                                          720,
                                          &update_slider_fn_);
  slider->set_val(0);
  return slider;
}

void PoseDesigner::UpdateSlider(SimpleSlider *slider) {
  // Some releases of OpenCV have a nasty habit of calling UpdateSlider
  // immediately...
  if (!is_setup_) return;

  if (slider->name() == "bend") {
    hand_pose_->bend(cur_joint()) = slider->val();
  } else if (slider->name() == "side") {
    hand_pose_->side(cur_joint()) = slider->val();
  } else if (slider->name() == "twist") {
    hand_pose_->twist(cur_joint()) = slider->val();
  } else if (slider->name() == "joint_no") {
    UpdateBSTSliders();
  } else if (slider->name() == "theta") {
    camera_spec_.theta = sl_theta_->val();
  } else if (slider->name() == "phi") {
    camera_spec_.phi = sl_phi_->val();
  } else if (slider->name() == "tilt") {
    camera_spec_.tilt = sl_tilt_->val();
  } else if (slider->name() == "distance") {
    camera_spec_.r = ( hand_renderer_.initial_cam_distance()
                       * sl_dist_mult_->val() );
  }

  hand_pose_->SetRotMatrix(camera_spec_);

  RenderEverything();
}

bool PoseDesigner::WarnKey(char cmd) {
  warning_key_ = cmd; ++warning_status_;
  return warning_status_ == 2;
}

bool PoseDesigner::ProcessKey(char cmd) {
  if (cmd == -1) return true;
  
  if (warning_status_ && warning_key_ != -1 && (cmd != warning_key_)) {
    warning_status_ = 0;
    warning_key_ = -1;
    return true;
  }

  switch(cmd) {
  case 27:
  case 'q':
    if (WarnKey(cmd)) return false;
    break;
  case 'h': render_help_ = !render_help_; break;
  case 'd': render_hog_ = !render_hog_; break;
  case 'b': ZeroBend(); break;
  case 's': ZeroSide(); break;
  case 't': ZeroTwist(); break;
  case 'a': JointUp(); break;
  case 'z': JointDown(); break;
  case 'c':
    if (WarnKey(cmd)) ZeroJoints();
    break;

  case 'p':
    break;
  case 'w': SavePose(); break;
  case 'r': LoadPose(); break;

  case '1': ResetCamera(); break;
  }

  if (warning_status_ >= 2) {
    warning_status_ = 0;
    warning_key_ = -1;
  }

  return true;
}

void PoseDesigner::UpdateBSTSliders() {
  sl_bend_->set_val(hand_pose_->bend(cur_joint()));
  sl_side_->set_val(hand_pose_->side(cur_joint()));
  sl_twist_->set_val(hand_pose_->twist(cur_joint()));
}

void PoseDesigner::ZeroBend() {
  hand_pose_->bend(cur_joint()) = 0;
  sl_bend_->set_val(0);
}

void PoseDesigner::ZeroSide() {
  hand_pose_->side(cur_joint()) = 0;
  sl_side_->set_val(0);
}

void PoseDesigner::ZeroTwist() {
  hand_pose_->twist(cur_joint()) = 0;
  sl_twist_->set_val(0);
}

void PoseDesigner::JointUp() {
  int next_joint = cur_joint() + 1;
  if (next_joint >= hand_pose_->num_joints()) return;

  sl_joint_no_->set_raw_val(next_joint);
  UpdateBSTSliders();
}

void PoseDesigner::JointDown() {
  int prev_joint = cur_joint() - 1;
  if (prev_joint < 0) return;

  sl_joint_no_->set_raw_val(prev_joint);
  UpdateBSTSliders();
}

void PoseDesigner::ResetCamera() {
  camera_spec_ = HandCameraSpec(hand_renderer_.initial_cam_distance());
  sl_theta_->set_val(0.);
  sl_phi_->set_val(0.);
  sl_tilt_->set_val(0.);
  sl_dist_mult_->set_val(1.0);
  hand_pose_->SetRotMatrix(camera_spec_);
}

void PoseDesigner::RenderEverything() {
  // Render the hand
  hand_renderer_.set_camera_spec(camera_spec_);
  hand_renderer_.SetHandPose(*hand_pose_);
  hand_renderer_.RenderHand();
  display_ = hand_renderer_.pixel_buffer_cv().clone();

  if (render_hog_) RenderHog();

  DisplayJointAngle();

  TextPrinter printer_rt_(display_,
                          kRenderWinWidth - 10,
                          kRenderWinHeight - 10);
  printer_rt_.SetRightAlign();
  printer_rt_.Print("Press 'h' for help");

  if (render_help_) DisplayHelp();

  if (warning_status_ == 1) {
    switch(warning_key_) {
    case 'c': DisplayClearWarning(); break;
    case 27:
    case 'q': DisplayQuitWarning(); break;
    default: break;
    }
  }

  DrawCameraSketch();

  cv::imshow(win_render_, display_);
  cv::imshow(win_camera_, cam_sketch_);
}

void PoseDesigner::DisplayJointAngle() {
  int n = cur_joint();
  float b = hand_pose_->bend(n),
    s = hand_pose_->side(n),
    t = hand_pose_->twist(n);

  float pi = math::constants::pi<float>();
  float deg_b = b * 180. / pi, deg_s = s * 180. / pi, deg_t = t * 180. / pi;

  TextPrinter printer_(display_, 10, kRenderWinHeight - 50);

  printer_.PrintF("Joint #%d: %s", cur_joint(),
                  scene_spec_.bone_name(cur_joint()).c_str());
  printer_.PrintF("Degrees: b: %+6.1f s: %+6.1f t: %+6.1f",
                  deg_b, deg_s, deg_t);
  printer_.PrintF("Radians: b: %+6.4f s: %+6.4f t: %+6.4f", b, s, t);
}

void PoseDesigner::DisplayHelp() {
  int center_x = kRenderWinWidth / 2, center_y = kRenderWinHeight / 2;
  int rect_w = ((double) kRenderWinWidth * 0.8),
    rect_h = ((double) kRenderWinHeight * 0.8);

  cv::Point rect_ul(center_x - rect_w / 2, center_y - rect_h / 2);
  cv::Point rect_br(center_x + rect_w / 2, center_y + rect_h / 2);

  cv::rectangle(display_, rect_ul, rect_br, cv::Scalar(40, 0, 0), CV_FILLED);

  TextPrinter printer_(display_, rect_ul.x + 10, rect_ul.y + 10);

  printer_.Print("Pose Designer - v1.0\n"
                 "\n"
                 "Keyboard Shortcuts:\n"
                 "  h - This help screen (press again to dismiss)\n"
                 "  ESC or q - Quit\n"
                 "\n"
                 "  r - Read a pose file\n"
                 "  w - Write a pose file\n"
                 "\n"
                 "  d - Toggle HOG descriptor view\n"
                 "\n"
                 "  b / s / t - Reset (b)end or (s)ide or (t)wist to 0\n"
                 "  c - clear all angles back to zero\n"
                 "\n"
                 "  a / z - Select previous/next joint"
                 "\n"
                 "  1 - Reset the camera to original view");
}

void PoseDesigner::DisplayClearWarning() {
  DisplayWarning("WARNING! All angles will be cleared!",
                 "Press 'c' to continue, any other key to cancel");
}

void PoseDesigner::DisplayQuitWarning() {
  DisplayWarning("WARNING! All unsaved data will be lost",
                 "Press q or ESC to quit, any other key to cancel");
}

void PoseDesigner::DisplayWarning(const string &line1, const string &line2) {
  int center_x = kRenderWinWidth / 2, center_y = kRenderWinHeight / 2;
  int rect_w = ((double) kRenderWinWidth * 0.8),
    rect_h = 50;

  cv::Point rect_ul(center_x - rect_w / 2, center_y - rect_h / 2);
  cv::Point rect_br(center_x + rect_w / 2, center_y + rect_h / 2);

  cv::rectangle(display_, rect_ul, rect_br, cv::Scalar(0, 0, 80), CV_FILLED);

  TextPrinter printer_(display_, rect_ul.x + 10, rect_ul.y + 15);

  printer_.SetThickness(2);
  printer_.Print(line1 + "\n");
  printer_.SetThickness(1);
  printer_.Print(line2);
}

void PoseDesigner::ZeroJoints() {
  hand_pose_->ClearJoints();
  ZeroBend(); ZeroSide(); ZeroTwist();
}

void PoseDesigner::RenderHog() {
  cv::Mat image_mask =
    ImageUtils::MaskFromNonZero(hand_renderer_.pixel_buffer_cv());

  cv::Rect roi_box = ImageUtils::FindBoundingBox(image_mask);
  cv::Mat roi_image(hand_renderer_.pixel_buffer_cv(), roi_box);
  cv::Mat roi_mask(image_mask, roi_box);

  cv::Rect hog_rect(kRenderWinWidth - kHogDispWidth,
                    0,
                    kHogDispWidth, kHogDispHeight);
  cv::Mat roi_display(display_, hog_rect);

  HogDescriptor hog_desc(8,8,8);

  hog_calc_.CalcHog(roi_image, roi_mask, &hog_desc);
  HogUtils::RenderHog(roi_image, hog_desc, roi_display);
}

void PoseDesigner::DrawCameraSketch() {
  const int sw = kCamWinWidth, sh = kCamWinHeight;

  float cx = sw / 2., cy = sh / 2.;

  HandCameraSpec h_cam(camera_spec_);
  float sp_r = 0.4 * (float) sw;
  h_cam.r = sp_r;

  float s_x = 0, s_y = 0, s_z = 0;
  h_cam.GetPosition(&s_x, &s_y, &s_z);

  float persp_f = sp_r * 1.2;
  float z_min = -sp_r;
  float z_max = -z_min;
  float z_range = z_max - z_min;
  float scale_f = (z_max - s_y + persp_f) / (z_range + persp_f);
  float y_2d = cy - (s_z * scale_f);
  float x_2d = cx + (s_x * scale_f);

  cv::rectangle(cam_sketch_, cv::Point(0, 0), cv::Point(sw, sh),
                cv::Scalar(40, 0, 0), CV_FILLED);
  
  cv::rectangle(cam_sketch_,
                cv::Point(x_2d - 3 * scale_f, y_2d - 3 * scale_f),
                cv::Point(x_2d + 3 * scale_f, y_2d + 3 * scale_f),
                cv::Scalar(0, 0, 200), CV_FILLED);
}

void PoseDesigner::LoadPose() {
  FileDialog dialog;

  dialog.SetTitle("Choose a pose file to load");

  dialog.AddExtension(FileExtension("YAML file", 2, ".yml", ".yaml"));
  dialog.AddExtension(FileExtension("All files", 1, "*"));

  dialog.SetDefaultExtension(".yml");

  string pose_file = dialog.Open();
  if (pose_file == "") return;

  try {
    hand_pose_->Load(pose_file, scene_spec_);
    camera_spec_.SetFromHandPose(*hand_pose_);
    pose_file_ = pose_file;
  } catch (const std::exception &e) {
    cout << "Exception caught! What:" << e.what() << endl;
  }
}

void PoseDesigner::SavePose() {
  FileDialog dialog;

  dialog.SetTitle("Save a pose file");

  dialog.AddExtension(FileExtension("YAML file", 2, ".yml", ".yaml"));
  dialog.AddExtension(FileExtension("All files", 1, "*"));

  dialog.SetDefaultFile(pose_file_);
  dialog.SetDefaultExtension(".yml");

  string pose_file = dialog.Save();
  if (pose_file == "") return;

  try {
    hand_pose_->Save(pose_file, scene_spec_); 
    pose_file_ = pose_file;
  } catch (const std::exception &e) {
    cout << "Exception caught! What:" << e.what() << endl;
  }
}

string PoseDesigner::GetSceneFileName() {
  FileDialog dialog;

  dialog.SetTitle("Choose a scene spec file");

  dialog.AddExtension(FileExtension("YAML file", 2, ".yml", ".yaml"));
  dialog.AddExtension(FileExtension("All files", 1, "*"));

  dialog.SetDefaultName("scene_spec.yml");
  dialog.SetDefaultExtension(".yml");

  return dialog.Open();
}

}  // namespace libhand
