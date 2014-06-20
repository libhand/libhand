// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// ImageUtils

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

# include <vector>

# include "opencv2/opencv.hpp"

namespace libhand {

using namespace std;

typedef vector<cv::Point> Contour;
typedef vector<Contour> Contours;

class ImageUtils {
 public:

  static cv::Mat Grayscale8Bit(const cv::Mat &image_in, bool clone=false);
  static cv::Mat Grayscale8BitClone(const cv::Mat &image_in) {
    return Grayscale8Bit(image_in, true);
  }
  static cv::Mat GrayscaleFloat(const cv::Mat &image_in, bool clone=false);
  static cv::Mat GrayscaleFloatClone(const cv::Mat &image_in) {
    return GrayscaleFloat(image_in, true);
  }

  static cv::Mat MaskFromNonZero(const cv::Mat &image_in);

  static Contours FindContours(const cv::Mat &image);
  static vector<cv::Rect> FindBoundingBoxes(const Contours &contours);
  static vector<cv::Rect> FindBoundingBoxes(const cv::Mat &image);
  static vector<cv::Rect> PruneSmallBoxes(const vector<cv::Rect> &boxes_in,
                                          int min_area);

  static cv::Rect FindBoundingBox(const vector<cv::Rect> &boxes_in);
  static cv::Rect FindBoundingBox(const cv::Mat &image, int min_area=0);

 private:

  // Disallow
  ImageUtils();
  ImageUtils(const ImageUtils &rhs);
  ImageUtils& operator= (const ImageUtils &rhs);
};

}  // namespace libhand
#endif  // IMAGE_UTILS_H
