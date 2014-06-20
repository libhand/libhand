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
//
// The SceneSpec describes the location and the contents of the Hand
// 3D information. We need to know the name of the hand object, the
// names of the hand bones, the number of bones, etc. Instead of
// hard-coding this information, we can load it and save it into scene
// spec files.

#ifndef SCENE_SPEC_H
#define SCENE_SPEC_H

# include <string>
# include <vector>

namespace libhand {

using namespace std;

class SceneSpec {
 public:
  // Constructs a scene spec
  SceneSpec() : scene_spec_dir_(".") {}

  // Loads the spec from an XML or YAML file
  // filename is a full pathname to the YAML file
  SceneSpec(const string &filename);

  // Saves the spec into an XML ("name.xml") or a YAML ("name.yml") file.
  // It will also automatically adjust the scene_spec_dir so it corresponds
  // to the location of the scene spec file.
  void SaveToFile(const string &filename,
                  bool adjust_scene_spec_dir = true);

  // Returns true if all needed variables are specified
  bool IsComplete() const;

  // Use this method to retrieve the full path to the scene directory
  // (The scene_dir path is relative to the location of the scene spec file)
  string SceneDirFullPath() const;

  // scene_spec_dir is constructed from the scene_spec filename automatically
  // you can adjust it here
  const string &scene_spec_dir() const;
  void set_scene_spec_dir(const string &scene_spec_dir);

  // Trivial accessors
  const string &scene_dir() const;
  void set_scene_dir(const string &scene_dir);

  const string &scene_file() const;
  void set_scene_file(const string &scene_file);

  const string &hand_object_name() const;
  void set_hand_object_name(const string &hand_object_name);

  inline int num_bones() const;

  // Specifying hand bone names
  void ClearBoneMap();
  void AddBoneToMap(const string &bone);
  void SetBoneMap(const vector<string> &bone_map);

  // Retrieveing a bone name by index into the map
  // Returns an empty string if the index is invalid
  string bone_name(int index) const;

  // Retrieveing a bone index by its name
  // Returns -1 if a bone by the name does not exist
  int bone_index(const string &bone_name) const;

 private:
  string scene_spec_dir_;
  string scene_dir_;
  string scene_file_;
  string hand_object_name_;
  vector<string> bone_map_;
};

// Inlined methods follow

inline bool SceneSpec::IsComplete() const {
  return !scene_dir().empty() && !scene_file().empty()
    && !hand_object_name().empty() && num_bones() > 0;
}

inline const string &SceneSpec::scene_spec_dir() const { return scene_spec_dir_; }
inline void SceneSpec::set_scene_spec_dir(const string &scene_spec_dir) {
  scene_spec_dir_ = scene_spec_dir;
}

inline const string &SceneSpec::scene_dir() const { return scene_dir_; }
inline void SceneSpec::set_scene_dir(const string &scene_dir) {
  scene_dir_ = scene_dir;
}

inline const string &SceneSpec::scene_file() const { return scene_file_; }
inline void SceneSpec::set_scene_file(const string &scene_file) {
  scene_file_ = scene_file;
}

inline const string &SceneSpec::hand_object_name() const {
  return hand_object_name_;
}

inline void SceneSpec::set_hand_object_name(const string &hand_object_name) {
  hand_object_name_ = hand_object_name;
}

inline int SceneSpec::num_bones() const { return (int) bone_map_.size(); }

inline void SceneSpec::ClearBoneMap() { bone_map_.clear(); }
inline void SceneSpec::SetBoneMap(const vector<string> &bone_map) {
  bone_map_ = bone_map;
}

inline void SceneSpec::AddBoneToMap(const string &bone) {
  bone_map_.push_back(bone);
}

inline string SceneSpec::bone_name(int index) const {
  if (index >= 0 && index < num_bones()) {
    return bone_map_[index];
  } else {
    return "";
  }
}

inline int SceneSpec::bone_index(const string &bone_name) const {
  for (int i = 0; i < num_bones(); ++i) {
    if (bone_name == bone_map_[i]) return i;
  }

  return -1;
}

}  // namespace libhand

#endif  // SCENE_SPEC_H
