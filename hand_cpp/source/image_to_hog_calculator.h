// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// ImageToHogCalculator

#ifndef IMAGE_TO_HOG_CALCULATOR_H
#define IMAGE_TO_HOG_CALCULATOR_H

# include <vector>

# include "opencv2/opencv.hpp"

# include "hog_cell.h"
# include "hog_cell_rectangles.h"
# include "hog_descriptor.h"

namespace libhand {

using namespace std;

class ImageToHogCalculator {
 public:
  ImageToHogCalculator();

  void CalcHog(const cv::Mat &image,
               const cv::Mat &mask,
               HogDescriptor *hog_desc);

  static void ConvertTo180Degrees(cv::Mat &deg_mat);

 private:
  void WeightedHistogramHogCell(int row, int col);

  // The image mask passed at input
  const cv::Mat *input_mask_;

  // The grayscale version of the input image
  cv::Mat gray_image_;

  // Pixel by pixel degree angles corresponding to the gradient
  cv::Mat deg_matrix_;

  // A per-pixel mask showing which pixels have OK gradients
  cv::Mat ok_gradients_;

  // Pixels to do a histogramming on
  cv::Mat pixels_to_use_;

  // The image HoG descriptor
  HogDescriptor *hog_desc_;

  // The corresponding HoG cell rectangles
  HogCellRectangles cell_rects_;

  // Bins for the OpenCV histogramming function
  vector<float> hist_bins_;

  // Disallow
  ImageToHogCalculator(const ImageToHogCalculator &rhs);
  ImageToHogCalculator& operator= (const ImageToHogCalculator &rhs);
};

}  // namespace libhand
#endif  // IMAGE_TO_HOG_CALCULATOR_H
