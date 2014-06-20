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
// HogCellRectangles

# include "hog_cell_rectangles.h"

# include <vector>
# include "opencv2/opencv.hpp"

namespace libhand {

HogCellRectangles::HogCellRectangles() : 
  num_rows_(0),
  num_cols_(0),
  image_rect_(0, 0, 0, 0) {
}

HogCellRectangles::HogCellRectangles(const HogCellRectangles &rhs) :
  num_rows_(rhs.num_rows_),
  num_cols_(rhs.num_cols_),
  image_rect_(rhs.image_rect_),
  cell_rects_(rhs.cell_rects_) {
}

HogCellRectangles& HogCellRectangles::operator= (const HogCellRectangles &rhs) {
  num_rows_ = rhs.num_rows_;
  num_cols_ = rhs.num_cols_;
  image_rect_ = rhs.image_rect_;
  cell_rects_ = rhs.cell_rects_;

  return *this;
}

HogCellRectangles::HogCellRectangles(int num_rows, int num_cols,
                                     const cv::Rect &image_rect) :
  num_rows_(num_rows),
  num_cols_(num_cols),
  image_rect_(image_rect),
  cell_rects_(num_cells()) {
  SetRectangles();
}

HogCellRectangles::HogCellRectangles(int num_rows, int num_cols,
                                     const cv::Mat &image) :
  num_rows_(num_rows),
  num_cols_(num_cols),
  image_rect_(0, 0, image.rows, image.cols),
  cell_rects_(num_cells()) {
  SetRectangles();
}

HogCellRectangles::HogCellRectangles(const HogDescriptor &hog_descriptor,
                                     const cv::Rect &image_rect) :
  num_rows_(hog_descriptor.num_rows()),
  num_cols_(hog_descriptor.num_cols()),
  image_rect_(image_rect),
  cell_rects_(num_cells()) {
  SetRectangles();
}

HogCellRectangles::HogCellRectangles(const HogDescriptor &hog_descriptor,
                                     const cv::Mat &image) :
  num_rows_(hog_descriptor.num_rows()),
  num_cols_(hog_descriptor.num_cols()),
  image_rect_(0, 0, image.cols, image.rows),
  cell_rects_(num_cells()) {
  SetRectangles();
}

void HogCellRectangles::SetRectangles() {
  // There is a roundoff error when converting rectangles into integer
  // values corresponding to the underlying pixels. The code below
  // keeps the "perfect" rectangle coordinates as doubles (ending with
  // _d) and converts them to integers only at the "last moment". The
  // last cell columns and rows are treated so that they end on the
  // image width and height boundaries regardless of any roundoff
  // errors.

  double cell_width_d = (double) image_width() / (double) num_cols();
  double cell_height_d = (double) image_height() / (double) num_rows();

  double y0_d = 0, y1_d = cell_height_d;
  int y0_i = 0;

  int x_offset = image_rect().x;
  int y_offset = image_rect().y;

  for (int r = 0, nr = num_rows();
       r < nr;
       ++r, y0_d += cell_height_d, y1_d += cell_height_d) {
    double x0_d = 0, x1_d = cell_width_d;

    int cell_height = (r < (nr - 1)) ? 
      (int) ((double) y1_d - (double) y0_i) :
      image_height() - y0_i;

    int x0_i = 0;

    for (int c = 0, nc = num_cols();
         c < nc;
         ++c, x0_d += cell_width_d, x1_d += cell_width_d) {
      int cell_width = (c < (nc - 1)) ?
        (int) ((double) x1_d - (double) x0_i) :
        image_width() - x0_i;

      cv::Rect &cur_rect = rect_rw(r,c);

      cur_rect.x = x0_i + x_offset;
      cur_rect.y = y0_i + y_offset;
      cur_rect.width = cell_width;
      cur_rect.height = cell_height;

      x0_i += cell_width;
    }

    y0_i += cell_height;
  }
}

}  // namespace libhand
