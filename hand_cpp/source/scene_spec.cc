// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// SceneSpec

# include <stdexcept>

# include "scene_spec.h"

# include "opencv2/opencv.hpp"

# include "file_tools.h"
# include "printfstring.h"

namespace libhand {

using namespace std;

// String constants for XML/YAML files
static const char * const kStrSceneDir = "scene_dir";
static const char * const kStrSceneFile = "scene_file";
static const char * const kStrHandObjectName = "hand_object_name";
static const char * const kStrBoneMap = "bone_map";

static inline cv::FileNode look(cv::FileStorage &fs, const char *str) {
   if (fs[str].empty())
    throw runtime_error(PrintFString("Missing field %s in the file",
                                     str));

  return fs[str];
}

SceneSpec::SceneSpec(const string &filename) {
  if (filename.empty()) {
    throw runtime_error("Empty filename specified for a SceneSpec");
  }

  scene_spec_dir_ = FileTools::Dirname(filename);

  cv::FileStorage fs(filename, cv::FileStorage::READ);
  if (!fs.isOpened()) {
    throw runtime_error(PrintFString("Cannot load the file %s",
                                     filename.c_str()));
  }

  string scene_dir, scene_file, hand_object_name;

  look(fs, kStrSceneDir) >> scene_dir;
  look(fs, kStrSceneFile) >> scene_file;
  look(fs, kStrHandObjectName) >> hand_object_name;

  set_scene_dir(scene_dir);
  set_scene_file(scene_file);
  set_hand_object_name(hand_object_name);

  cv::FileNode bone_map_node(look(fs, kStrBoneMap));
  if (bone_map_node.type() != cv::FileNode::SEQ) {
    throw runtime_error("bone_map is supposed to be a sequence");
  }

  for (cv::FileNodeIterator i = bone_map_node.begin(),
         e = bone_map_node.end();
       i != e;
       ++i) {
    AddBoneToMap((string) *i);
  }
       
  fs.release();
}

string SceneSpec::SceneDirFullPath() const {
  return scene_spec_dir() + "/" + scene_dir();
}

void SceneSpec::SaveToFile(const string &filename,
                           bool adjust_scene_spec_dir) {
  if (filename.empty()) {
    throw runtime_error("Empty filename specified for a scene spec");
  }

  if (adjust_scene_spec_dir) {
    set_scene_spec_dir(FileTools::Dirname(filename));
  }

  cv::FileStorage fs(filename, cv::FileStorage::WRITE);
  if (!fs.isOpened()) {
    throw runtime_error(PrintFString("Cannot save to file %s",
                                     filename.c_str()));
  }

  fs << kStrSceneDir << scene_dir();
  fs << kStrSceneFile << scene_file();
  fs << kStrHandObjectName << hand_object_name();

  fs << kStrBoneMap << "[";

  for (int i = 0; i < num_bones(); ++i) {
    fs << bone_name(i);
  }

  fs << "]";

  fs.release();
}

}  // namespace libhand
