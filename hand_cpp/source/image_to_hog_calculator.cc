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

# include "image_to_hog_calculator.h"

# include <algorithm>

# include "opencv2/opencv.hpp"

# include "error_handling.h"
# include "hog_cell_rectangles.h"
# include "hog_descriptor.h"
# include "image_utils.h"

namespace libhand {

ImageToHogCalculator::ImageToHogCalculator() :
  input_mask_(NULL),
  hog_desc_(NULL) {
}

void ImageToHogCalculator::CalcHog(const cv::Mat &image,
                                   const cv::Mat &mask,
                                   HogDescriptor *hog_desc) {
  if (image.rows < 1 || image.cols < 1 \
      || mask.rows < 1 || mask.cols < 1) {
    hog_desc->Zero();
    return;
  }

  input_mask_ = &mask;
  hog_desc_ = hog_desc;

  gray_image_ = ImageUtils::GrayscaleFloat(image);

  // alloc deg matrix
  deg_matrix_ = cv::Mat::zeros(image.size(), CV_32F);

  // get degrees
  ok_gradients_ = cv::Mat::zeros(image.size(), CV_8UC1);

  // calculate the 360 degree gradient
  cv::calcMotionGradient(gray_image_, // input grayscale iamge
                         ok_gradients_, // output mask of gradients
                         deg_matrix_, // output matrix of degree info
                         1, // ignore very very small magnitudes
                         10000, // no specified maximum magnitude
                         3); // Sobel operator aperture size

  // 360 to 180 degree representation
  ConvertTo180Degrees(deg_matrix_);

  // Set pixels to use for histogramming
  pixels_to_use_ = *input_mask_ & ok_gradients_;

  // Get hog cell rectangles
  cell_rects_ = HogCellRectangles(*hog_desc_, image);

  // Adjust the number of histogram bins to the specification
  // by the HoG descriptor
  hist_bins_.resize(hog_desc->cell_num_bins());

  // histogram all hog cells
  for (int r = 0, nr = cell_rects_.num_rows(); r < nr; ++r) {
    for (int c = 0, nc = cell_rects_.num_cols(); c < nc; ++c) {
      WeightedHistogramHogCell(r,c);
    }
  } 
}

void ImageToHogCalculator::ConvertTo180Degrees(cv::Mat &deg_mat) {
  switch(deg_mat.type()) {
  case CV_8UC1: {
    for (int r = 0; r < deg_mat.rows; ++r) {
      for (int c = 0; c < deg_mat.cols; ++c) {
        unsigned char &deg_pix = deg_mat.at<unsigned char>(r,c);
        
        if (deg_pix > 179) deg_pix -= 180;
      }
    }
  }
    break;

  case CV_32F: {
    for (int r = 0; r < deg_mat.rows; ++r) {
      for (int c = 0; c < deg_mat.cols; ++c) {
        float &deg_pix = deg_mat.at<float>(r,c);
        float deg_pix_m = deg_pix - 180.;

        if (deg_pix_m > 0) deg_pix = deg_pix_m;
      }
    }
  }
    break;

  default:
    Error("ConvertTo180Degrees passed a matrix of unknown type");
    break;
  }
}

void ImageToHogCalculator::WeightedHistogramHogCell(int row, int col) {
  // The hog cell to calculate
  HogCell &hog_cell = hog_desc_->hog_cell(row, col);

  // roi: the region of interest -- the boundary of the current hog cell
  const cv::Rect roi = cell_rects_.rect(row, col);

  if ((roi.width < 1) || (roi.height < 1)) {
    hog_cell.Zero();
    return;
  }

  // Histogram arrays (shallow window into deg_matrix)
  const cv::Mat hist_arrays[] = { deg_matrix_(roi) };  
  int hist_channel_list[] = { 0 };  // Only channel 0 is used
  // Dimension #1: number of bins in histogram
  int hist_dims[] = { (int) hog_desc_->cell_num_bins() };
  float hist_range1d[] = { 0, 180 }; // The histogram range in 1d
  const float* hist_ranges[] =
    { hist_range1d }; // Histogram ranges, per dimension

  const cv::Mat pixels_to_use_roi(pixels_to_use_(roi));
  
  int num_ok_pixels = cv::countNonZero(pixels_to_use_roi);

  if (num_ok_pixels < 1) {
    hog_cell.Zero();
    return;
  }

  cv::Mat_<float> hist_out(hist_bins_);

  cv::calcHist(hist_arrays,
               1, // number of histogram arrays
               hist_channel_list, // the list of channels to use
               pixels_to_use_roi, // per-pixel mask
               hist_out, // calcHist output
               1, // number of dimensions: 1, it's a 1D histogram
               hist_dims, // number of bins per each dimension
               hist_ranges); // histogram range per each dimension

  double roi_area = (double) roi.width * (double) roi.height;
  float ok_pixels_weighting_factor = (float) ((double) num_ok_pixels / roi_area);

  hog_cell.LoadBins(hist_bins_);
  hog_cell.Normalize();
  hog_cell *= ok_pixels_weighting_factor;
}

}  // namespace libhand
