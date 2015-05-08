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

# include <boost/filesystem.hpp>
# include <string.h>
# include <string>
# include "file_tools.h"

namespace libhand {

string FileTools::Basename(const string &full_filepath) {
  return string(boost::filesystem::basename(full_filepath));
}

string FileTools::Basename(const char *full_filepath) {
  return Basename(string(full_filepath));
}

string FileTools::Dirname(const string &full_filepath) {
	boost::filesystem::path path(full_filepath);
  
  return string(path.parent_path().string());
}

string FileTools::Dirname(const char *full_filepath) {
  return Dirname(string(full_filepath));
}

bool FileTools::IsFileNameOnly(const string &filepath) {
  return Basename(filepath) == filepath;
}

bool FileTools::IsFileNameOnly(const char *filepath) {
  return IsFileNameOnly(string(filepath));
}

};  // namespace kthrobot
