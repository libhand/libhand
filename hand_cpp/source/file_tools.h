// Copyright (c) 2010 Marin Saric <marin.saric@gmail.com>

// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.

// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef __FILE_TOOLS_H__
#define __FILE_TOOLS_H__

# include <string>

namespace libhand {

using namespace std;

// FileTools provides utility routines for manipulating files
class FileTools {
public:
  // Extracts the name of the file from the full path
  static string Basename(const char *full_filepath);

  // Extracts the name of the file from the full path
  static string Basename(const string &full_filepath);

  // Extracts the path to the file's directory from the full path
  static string Dirname(const char *full_filepath);

  // Extracts the path to the file's directory from the full path
  static string Dirname(const string &full_filepath);

  // Returns true if the given string is only a filename, eg. "picture.jpg"
  static bool IsFileNameOnly(const string &filepath);

  // Returns true if the given string is only a filename, eg. "picture.jpg"
  static bool IsFileNameOnly(const char *filepath);
};

};

#endif  // FILE_TOOLS_H
