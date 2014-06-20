// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

# include <iostream>
# include <stdexcept>
# include <string>

# include "opencv2/opencv.hpp"

# include "mex.h"
# include "marin_mexutil.h"

# include "hand_renderer.h"
# include "hand_system_data.h"
# include "image_utils.h"

using namespace std;
using namespace libhand;

static inline HandSystemData *GetSys() {
  HandSystemData *sys = HandSystemData::GetHandleFromMatlab();
  if (!sys) {
    throw MatlabErrorException("hand system not initialized, "
                               "did you call hand_system setup?");
  }

  return sys;
}

static inline HandSystemData *GetSysWithScene() {
  HandSystemData *sys = GetSys();
  if (!sys->has_scene()) {
    throw MatlabErrorException("No scene loaded, use load_scene first");
  }

  return sys;
}

static string StringWithChecks(const mxArray *arr,
                               const string &cmd_name,
                               int param_no,
                               const string &meaning) {
  if (!IsAString(arr)) {
    throw MatlabErrorException("%s parameter #%d needs to be a string (%s)",
                               cmd_name.c_str(), param_no, meaning.c_str());
  }

  return ToString(arr);
}

static double ScalarWithChecks(const mxArray *arr,
                               const string &cmd_name,
                               int param_no,
                               const string &meaning) {
  if (!IsAScalar(arr)) {
    throw MatlabErrorException("%s parameter #%d needs to be a scalar (%s)",
                               cmd_name.c_str(), param_no, meaning.c_str());
  }

  return ToScalar(arr);
}

static void Check2DMatrix(const mxArray *arr,
                          const string &cmd_name,
                          int param_no,
                          const string &meaning) {
  if (!IsA2DMatrix(arr)) {
    throw MatlabErrorException("%s parameter #%d needs to be a 2D matrix (%s)",
                               cmd_name.c_str(), param_no, meaning.c_str());
  }
}

static void VerifyJointsMatrix(const mxArray *arr) {
  HandSystemData *sys = GetSysWithScene();
  const int num_bones = sys->scene_spec().num_bones();

  if ((int) mxGetN(arr) != FullHandPose::kElementsPerJoint) {
    throw MatlabErrorException("The joint matrix has %d columns, but it "
                               "needs to have %d, since each joint has "
                               "%d elements.",
                               mxGetN(arr),
                               FullHandPose::kElementsPerJoint,
                               FullHandPose::kElementsPerJoint);
  }
  
  if ((int) mxGetM(arr) != num_bones) {
    throw MatlabErrorException("The joint matrix has %d rows, but it "
                               "needs to have %d, since there are %d "
                               "bones.",
                               mxGetM(arr), num_bones, num_bones);
  }
}

static void VerifyRotationMatrix(const mxArray *arr) {
  if ((int) mxGetN(arr) != FullHandPose::kRotMatrixN) {
    throw MatlabErrorException("The rotation matrix has %d columns, but it"
                               "needs to have %d, since it is supposed to "
                               "be a %d by %d matrix.",
                               mxGetN(arr),
                               FullHandPose::kRotMatrixN,
                               FullHandPose::kRotMatrixN,
                               FullHandPose::kRotMatrixN);
  }
  
  if ((int) mxGetM(arr) != FullHandPose::kRotMatrixN) {
    throw MatlabErrorException("The rotation matrix has %d rows, but it"
                               "needs to have %d, since it is supposed to "
                               "be a %d by %d matrix.",
                               mxGetM(arr),
                               FullHandPose::kRotMatrixN,
                               FullHandPose::kRotMatrixN,
                               FullHandPose::kRotMatrixN);
  }
}

static void PoseJointsToMatlabMatrix(const FullHandPose &pose,
                                     mxArray **arr) {
  const int num_components = FullHandPose::kElementsPerJoint;
  const int num_joints = pose.num_joints();

  *arr = mxCreateDoubleMatrix(num_joints, num_components, mxREAL);
  double *output = mxGetPr(*arr);

  const float *data_in = pose.joints_begin();
  for (int joint_no = 0; joint_no < num_joints; ++joint_no) {
    for (int component = 0; component < num_components; ++component) {
      output[joint_no + component * num_joints] = *data_in;
      ++data_in;
    }
  }
}

static void PoseRotationToMatlabMatrix(const FullHandPose &pose,
                                       mxArray **arr) {
  const int N = FullHandPose::kRotMatrixN;

  *arr = mxCreateDoubleMatrix(N, N, mxREAL);
  double *output = mxGetPr(*arr);
    
  const float *data_in = pose.rotation_begin();
  for (int r = 0; r < N; ++r) {
    for (int c = 0; c < N; ++c) {
      output[r + c * N] = *data_in;
      ++data_in;
    }
  }
}

static void MatlabMatrixToPoseJoints(const mxArray *arr, 
                                     FullHandPose *pose) {
  const int num_components = FullHandPose::kElementsPerJoint;
  const int num_joints = pose->num_joints();

  const double *data_in = mxGetPr(arr);

  float *output = pose->joints_begin();
  for (int joint_no = 0; joint_no < num_joints; ++joint_no) {
    for (int component = 0; component < num_components; ++component) {
      *output = data_in[joint_no + component * num_joints];
      ++output;
    }
  }
}

static void MatlabMatrixToPoseRotation(const mxArray *arr,
                                       FullHandPose *pose) {
  static const int N = FullHandPose::kRotMatrixN;

  const double *data_in = mxGetPr(arr);

  float *output = pose->rotation_begin();
  for (int r = 0; r < N; ++r) {
    for (int c = 0; c < N; ++c) {
      *output = data_in[r + c * N];
      ++output;
    }
  }
}

void mexFunctionSetup (int nlhs,
                       mxArray *plhs[],
                       int nrhs,
                       const mxArray *prhs[]) {
  if (nlhs != 0) {
    throw MatlabErrorException("The setup command takes no extra parameters");
  }

  if (nrhs != 0) {
    throw MatlabErrorException("The setup command does not return any values");
  }

  HandSystemData::Init();
}

void mexFunctionLoadScene (int nlhs,
                           mxArray *plhs[],
                           int nrhs,
                           const mxArray *prhs[]) {

  if (nrhs != 1) {
    throw MatlabErrorException("load_scene takes 1 parameter (a file path)");
  }

  if (nlhs != 0) {
    throw MatlabErrorException("load_scene must receive no output arguments");
  }

  const string spec_file = StringWithChecks(prhs[0], "load_scene",
                                            1, "a file path");

  HandSystemData *sys = GetSys();

  SceneSpec spec(spec_file);

  // If any of the code below throws, we will consider the scene not loaded
  sys->set_has_scene(false);
  sys->set_scene_spec(spec);
  sys->hand_renderer().LoadScene(spec);

  // Everything went through OK, scene is loaded
  sys->set_has_scene(true);
}

void mexFunctionRenderHand (int nlhs,
                            mxArray *plhs[],
                            int nrhs,
                            const mxArray *prhs[]) {

  if (nrhs > 1) {
    throw MatlabErrorException("render_hand takes at most "
                               "one parameter (output format)");
  }

  if (nlhs > 1) {
    throw MatlabErrorException("render_hand takes just one output argument");
  }

  string output_format = "color";

  if (nrhs > 0) {
    output_format = StringWithChecks(prhs[0], "render_hand",
                                     1, "output format");
  }

  enum { GRAY=0, COLOR=1 } format;

  if (output_format == "gray") format=GRAY;
  else if (output_format == "color") format=COLOR;
  else {
    throw MatlabErrorException("render_hand does not know a format '%s'",
                               output_format.c_str());
  }

  HandRenderer &renderer = GetSysWithScene()->hand_renderer();

  renderer.RenderHand();

  int h = renderer.render_height();
  int w = renderer.render_width();

  switch (format) {
  case GRAY:
    {
      cv::Mat_<float> gray_float =
        ImageUtils::GrayscaleFloat(renderer.pixel_buffer_cv());

      plhs[0] = mxCreateDoubleMatrix(h, w, mxREAL);        
      double *output = mxGetPr(plhs[0]);
        
      for (int c = 0; c < w; ++c) {
        for (int r = 0; r < h; ++r, ++output) {
          *output=gray_float(r,c) / 255.;
        }
      }
    }
    break;

  case COLOR:
    {
      cv::Mat_<cv::Vec3b> cv_buffer = renderer.pixel_buffer_cv();

      mwSize dims[] = { h, w, 3 };

      plhs[0] = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);

      char *output = (char *) mxGetData(plhs[0]);

      for (int chan = 2; chan >= 0; --chan) {
        for (int c = 0; c < w; ++c) {
          for (int r = 0; r < h; ++r, ++output) {
            *output=cv_buffer(r,c)[chan];
          }
        }
      }
    }
    break;
  default: break;
  }
}

void mexFunctionGetHandPose (int nlhs,
                             mxArray *plhs[],
                             int nrhs,
                             const mxArray *prhs[]) {
  if (nrhs != 0) {
    throw MatlabErrorException("get_hand_pose takes no parameters");
  }

  if (nlhs > 2) {
    throw MatlabErrorException("get_hand_pose takes at most two "
                               "output arguments");
  }

  HandSystemData *sys = GetSysWithScene();

  FullHandPose output_pose = sys->hand_pose();

  PoseJointsToMatlabMatrix(output_pose, &(plhs[0]));
  if (nlhs > 1) PoseRotationToMatlabMatrix(output_pose, &(plhs[1]));
}

void mexFunctionSetHandPose (int nlhs,
                             mxArray *plhs[],
                             int nrhs,
                             const mxArray *prhs[]) {
  if (nrhs > 2) {
    throw MatlabErrorException("set_hand_pose takes at most "
                               "two parameters (joints, rotation)");
  }

  if (nlhs != 0) {
    throw MatlabErrorException("set_hand_pose takes no output arguments");
  }

  if (nrhs > 0) Check2DMatrix(prhs[0], "set_hand_pose", 1, "joints");
  if (nrhs > 1) Check2DMatrix(prhs[1], "set_hand_pose", 2, "rotation");

  HandSystemData *sys = GetSysWithScene();

  int num_bones = sys->scene_spec().num_bones();

  if (nrhs > 0) VerifyJointsMatrix(prhs[0]);
  if (nrhs > 1) VerifyRotationMatrix(prhs[1]);

  FullHandPose output_pose = sys->hand_pose();

  if (nrhs == 0) output_pose = FullHandPose(num_bones);
  if (nrhs > 0) MatlabMatrixToPoseJoints(prhs[0], &output_pose);
  if (nrhs > 1) MatlabMatrixToPoseRotation(prhs[1], &output_pose);

  sys->set_hand_pose(output_pose);
  sys->hand_renderer().SetHandPose(output_pose, true);
}

void mexFunctionLoadHandPose (int nlhs,
                              mxArray *plhs[],
                              int nrhs,
                              const mxArray *prhs[]) {
  if (nrhs != 1) {
    throw MatlabErrorException("load_hand_pose takes 1 parameter "
                               "(a pose file)");
  }

  if (nlhs > 2) {
    throw MatlabErrorException("load_hand_pose takes at most two "
                               "output arguments");
  }

  HandSystemData *sys = GetSysWithScene();

  FullHandPose output_pose(sys->scene_spec().num_bones());

  const string pose_file = StringWithChecks(prhs[0], "get_hand_pose",
                                            1, "pose file");
  
  output_pose.Load(pose_file, sys->scene_spec(), FullHandPose::WARN);

  PoseJointsToMatlabMatrix(output_pose, &(plhs[0]));
  if (nlhs > 1) PoseRotationToMatlabMatrix(output_pose, &(plhs[1]));
}

void mexFunctionSaveHandPose (int nlhs,
                              mxArray *plhs[],
                              int nrhs,
                              const mxArray *prhs[]) {
 if (nrhs != 3) {
   throw MatlabErrorException("save_hand_pose takes 3 parameters "
                              "(joints, rotation, pose file)");
 }
 
 if (nlhs != 0) {
   throw MatlabErrorException("save_hand_pose does not return anything");
 }

 Check2DMatrix(prhs[0], "save_hand_pose", 1, "joints");
 Check2DMatrix(prhs[1], "save_hand_pose", 2, "rotation");

 const string pose_file = StringWithChecks(prhs[2], "save_hand_pose",
                                           3, "pose file");

 HandSystemData *sys = GetSysWithScene();

 VerifyJointsMatrix(prhs[0]);
 VerifyRotationMatrix(prhs[1]);

 FullHandPose pose(sys->scene_spec().num_bones());

 MatlabMatrixToPoseJoints(prhs[0], &pose);
 MatlabMatrixToPoseRotation(prhs[1], &pose);

 pose.Save(pose_file, sys->scene_spec());
}

void mexFunctionNumBones (int nlhs,
                          mxArray *plhs[],
                          int nrhs,
                          const mxArray *prhs[]) {
  if (nrhs != 0) {
    throw MatlabErrorException("num_bones takes no arguments");
  }

  if (nlhs > 1) {
    throw MatlabErrorException("num_bones returns just a single scalar");
  }

  HandSystemData *sys = GetSysWithScene();

  if (nlhs == 1) {
    plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);

    mxGetPr(plhs[0])[0] = (double) sys->scene_spec().num_bones();
  }
}

void mexFunctionBoneName (int nlhs,
                          mxArray *plhs[],
                          int nrhs,
                          const mxArray *prhs[]) {
  if (nrhs != 1) {
    throw MatlabErrorException("bone_name takes 1 argument (bone index)");
  }

  if (nlhs > 1) {
    throw MatlabErrorException("bone_name returns a string, nothing more");
  }

  int bone_index_matlab = ScalarWithChecks(prhs[0], "bone_name",
                                           1, "bone index");

  HandSystemData *sys = GetSysWithScene();

  int bone_index_c = bone_index_matlab - 1;
  int num_bones = sys->scene_spec().num_bones();

  if (bone_index_c < 0 || bone_index_c >= num_bones) {
    throw MatlabErrorException("The passed bone_index (%d) is out of range "
                               "(num_bones = %d)",
                               bone_index_matlab, num_bones);
  }

  const string bone_name = sys->scene_spec().bone_name(bone_index_c);
  
  plhs[0] = mxCreateString(bone_name.c_str());
}

void mexFunctionBoneIndex (int nlhs,
                           mxArray *plhs[],
                           int nrhs,
                           const mxArray *prhs[]) {
  if (nrhs != 1) {
    throw MatlabErrorException("bone_by_name takes 1 argument (bone name)");
  }

  if (nlhs > 1) {
    throw MatlabErrorException("bone_by_name returns just a single scalar");
  }

  const string bone_name = StringWithChecks(prhs[0], "bone_by_name",
                                            1, "bone name");

  HandSystemData *sys = GetSysWithScene();

  plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);

  int bone_index = sys->scene_spec().bone_index(bone_name);

  mxGetPr(plhs[0])[0] = (double) (bone_index + 1);
}

void mexFunction (int nlhs,
                  mxArray *plhs[],
                  int nrhs,
                  const mxArray *prhs[]) {
  if (nrhs == 0) {
    mexErrMsgTxt("hand_system must receive at least one argument");
  }

  if (!IsAString(prhs[0])) {
    mexErrMsgTxt("The first argument must be a string "
                 "(the hand_system command)");
  }

  string cmd=ToString(prhs[0]);

  const mxArray **sub_prhs=prhs + 1;
  int sub_nrhs = nrhs - 1;

  try {
    if (cmd == "setup") {
      mexFunctionSetup(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "load_scene") {
      mexFunctionLoadScene(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "render_hand") {
      mexFunctionRenderHand(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "get_hand_pose") {
      mexFunctionGetHandPose(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "set_hand_pose") {
      mexFunctionSetHandPose(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "load_hand_pose") {
      mexFunctionLoadHandPose(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "save_hand_pose") {
      mexFunctionSaveHandPose(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "num_bones") {
      mexFunctionNumBones(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "bone_name") {
      mexFunctionBoneName(nlhs, plhs, sub_nrhs, sub_prhs);
    } else if (cmd == "bone_index") {
      mexFunctionBoneIndex(nlhs, plhs, sub_nrhs, sub_prhs);
    } else {
      throw MatlabErrorException("Unknown command: %s", cmd.c_str());
    }
  } catch (const std::exception &e) {
    mexErrMsgTxt(e.what());
  } catch (...) {
    mexErrMsgTxt("hand_system: Exception caught!");
  }
}
