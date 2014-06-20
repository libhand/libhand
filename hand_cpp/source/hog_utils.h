// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// HogUtils

#ifndef HOG_UTILS_H
#define HOG_UTILS_H

# include "opencv2/opencv.hpp"

# include "hog_descriptor.h"

namespace libhand {

using namespace std;
class HogUtils {
 public:
  static void RenderHog(const HogDescriptor &hog_desc,
                        cv::Mat &out_image);

  static void RenderHog(const cv::Mat &in_image,
                        const HogDescriptor &hog_desc,
                        cv::Mat &out_image);

  static const int kDefaultRenderWidth = 400;
  static const int kDefaultRenderHeight = 400;

 private:

  // Disallow
  HogUtils();
  HogUtils(const HogUtils &rhs);
  HogUtils& operator= (const HogUtils &rhs);
};

}  // namespace libhand
#endif  // HOG_UTILS_H
