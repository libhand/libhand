// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// HogCellRectangles

// Each HoG descriptor is divided into rectangles that are
// approximately equal in size called HoG cells. The HogCellRectangles
// class is used to calculate the rectangle coordinates based on the
// input image.
//
// Images are discrete and thus the rectangles cannot be perfectly
// equal. This class does error diffusion to have the dimensions of
// each rectangle be as close as possible to the other rectangles.

#ifndef HOG_CELL_RECTANGLES_H
#define HOG_CELL_RECTANGLES_H

# include <vector>
# include "opencv2/opencv.hpp"

# include "hog_descriptor.h"

namespace libhand {

using namespace std;
class HogCellRectangles {
 public:
  // Creates an empty (0 by 0) array of rectangles
  HogCellRectangles();

  // Makes a copy from an existing HogCellRectangles
  HogCellRectangles(const HogCellRectangles &rhs);
  HogCellRectangles& operator= (const HogCellRectangles &rhs);

  // Creates an array of num_rows by num_cols rectangles
  // that uniformly divide the bounding box image_rect
  HogCellRectangles(int num_rows, int num_cols,
                    const cv::Rect &image_rect);

  // Same as above but acts on an OpenCV matrix
  HogCellRectangles(int num_rows, int num_cols,
                    const cv::Mat &image);

  // Uses an existing HogDescriptor to determine num_rows and num_cols
  // and the image_rect bounding box rectangle to determine the
  // coordinates of the individual HoG cells.
  HogCellRectangles(const HogDescriptor &hog_descriptor,
                    const cv::Rect &image_rect);

  HogCellRectangles(const HogDescriptor &hog_descriptor,
                    const cv::Mat &image);

  // Simple accessors
  int num_rows() const { return num_rows_; }
  int num_cols() const { return num_cols_; }
  int num_cells() const { return num_rows() * num_cols(); }

  // Read-only accessors into rectangle data
  const cv::Rect &rect(int row, int col) const {
    return cell_rects_[cell_number(row, col)];
  }

  const cv::Rect &image_rect() const { return image_rect_; }

  int image_width() const { return image_rect_.width; }
  int image_height() const { return image_rect_.height; }

 private:
  // Sets all the cell rectangles based on image width/height
  void SetRectangles();

  // Row-major cell 2D index into row-by-row 1D index
  int cell_number(int row, int col) const { return row * num_cols() + col; }

  // Read-write accessor
  cv::Rect &rect_rw(int row, int col) {
    return cell_rects_[cell_number(row, col)];
  }
  
  // The number of HoG cell rows the image is divided into
  int num_rows_;

  // The number of HoG cell columns the image is divided into
  int num_cols_;

  // The rectangle holding the whole image
  cv::Rect image_rect_;
 
  // HoG cell rectangles
  vector<cv::Rect> cell_rects_;
};

}  // namespace libhand
#endif  // HOG_CELL_RECTANGLES_H
