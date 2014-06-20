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

# include "file_dialog.h"
# include "file_tools.h"

# include <cstdarg>
# include <cstdlib>
# include <iostream>
# include <fstream>

# include <stdexcept>
# include <string>
# include <unistd.h>

namespace libhand {

using namespace std;

FileExtension::FileExtension(const char *desc, int num_ext, ...) {
  description = string(desc);
  va_list ap;

  va_start(ap, num_ext);
  for (int i = 0; i < num_ext; ++i) {
    char *ext = va_arg(ap, char *);
    patterns.push_back(string(ext));
  }
  va_end(ap);
}

FileDialog::FileDialog() :
  title_("Choose a file"),
  default_dir_("."),
  default_name_(""),
  default_ext_("") {
}

void FileDialog::AddExtension(const FileExtension &extension) {
  extensions_.push_back(extension);
}

string FileDialog::GenTkCode(const string &dialog_call) {
  string out_code;

  out_code += "wm withdraw .;\n";
  out_code += "set types {\n";

  for (size_t i = 0; i < extensions_.size(); ++i) {
    const FileExtension &ext = extensions_[i];
    out_code += "  { {" + ext.description + "} {";

    for (size_t j = 0; j < ext.patterns.size(); ++j) {
      out_code += "{" + ext.patterns[j] + "} ";
    }

    out_code += "} }\n";
  }

  out_code += "};\n";
  out_code += "puts stdout [" + dialog_call + " ";
  out_code += "-title {" + title_ + "} ";
  out_code += "-initialdir {" + default_dir_ + "} ";
  out_code += "-filetypes $types ";
  out_code += "-initialfile {" + default_name_ + "} ";
  out_code += "-defaultextension {" + default_ext_ +"} ";
  out_code += "];\n";
  out_code += "exit;\n";
  return out_code;
}

string FileDialog::Open() {
  return TkExec(GenTkCode("tk_getOpenFile"));
}

string FileDialog::Save() {
  return TkExec(GenTkCode("tk_getSaveFile"));
}

string FileDialog::TkExec(const string &cmd) {
  char tmpfile_pat[] = "/tmp/opendlg.XXXXXX";
  string temp_base(mktemp(tmpfile_pat));
  string temp_cmd_fname(temp_base + ".cmd");
  string temp_out_fname(temp_base + ".out");

  ofstream cmd_file(temp_cmd_fname.c_str());
  if (!cmd_file.is_open()) {
    throw runtime_error("Could not create the command file!");
  }
  cmd_file << cmd;
  cmd_file.close();

  string syscmd = "wish " + temp_cmd_fname + " >" + temp_out_fname;
  system(syscmd.c_str());

  string out_string;

  ifstream out_file(temp_out_fname.c_str());
  if (!out_file.is_open()) {
    throw runtime_error("Could not read back the output file!");
  }
  getline(out_file, out_string);
  out_file.close();

  unlink(temp_cmd_fname.c_str());
  unlink(temp_out_fname.c_str());

  return out_string;
}

void FileDialog::SetDefaultFile(const string &full_filepath) {
  SetDefaultDir(FileTools::Dirname(full_filepath));
  SetDefaultName(FileTools::Basename(full_filepath));
}

}  // namespace libhand
