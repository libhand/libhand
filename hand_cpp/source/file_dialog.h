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
// FileDialog
//
// Often one needs to locate files on a system. The most convenient
// way for end users is to find a file using a file open dialog, but
// unfortunately this feature does not come in a standardized
// cross-platform way. LibHand attempts to provide a simple utility
// that should work on MacOS X and Linux.
//
// The file open dialog is implemented by starting a Tcl/Tk
// interpreter that comes preinstalled on MacOS X and most Linux
// systems. The Tcl/Tk interpreter reads from a file that is generated
// on the fly by the FileDialog class.

# include <string>
# include <vector>

#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

namespace libhand {

using namespace std;

struct FileExtension {
  string description;
  vector<string> patterns;

  FileExtension(const char *desc, int num_ext, ...);
};

class FileDialog {
 public:
  FileDialog();
  string Open();
  string Save();

  void AddExtension(const FileExtension &extension);
  void SetTitle(const string &title);
  void SetDefaultDir(const string &default_dir);
  void SetDefaultName(const string &default_name);
  void SetDefaultFile(const string &full_filepath);
  void SetDefaultExtension(const string &extension);

 private:
  string GenTkCode(const string &dialog_call);
  static string TkExec(const string &cmd);

  string title_;
  string default_dir_;
  string default_name_;
  string default_ext_;
  vector<FileExtension> extensions_;


  // Disallow
  FileDialog(const FileDialog &rhs);
  FileDialog& operator= (const FileDialog &rhs);
};

inline void FileDialog::SetTitle(const string &title) { title_ = title; }
inline void FileDialog::SetDefaultDir(const string &default_dir) {
  default_dir_ = default_dir;
}
inline void FileDialog::SetDefaultName(const string &default_name) {
  default_name_ = default_name;
}
inline void FileDialog::SetDefaultExtension(const string &extension) {
  default_ext_ = extension;
}

}  // namespace libhand
#endif  // FILE_DIALOG_H
