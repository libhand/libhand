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

# include "image_utils.h"

# include "opencv2/opencv.hpp"

namespace libhand {

cv::Mat ImageUtils::Grayscale8Bit(const cv::Mat &image_in, bool clone) {
  if(image_in.type() == CV_8UC1) {
    if (clone) {
      return image_in.clone();
    } else {
      return image_in;
    }
  }

  cv::Mat gray_image;

  cv::cvtColor(image_in, gray_image, CV_BGR2GRAY);
  return gray_image;
}

cv::Mat ImageUtils::GrayscaleFloat(const cv::Mat &image_in, bool clone) {
  if (image_in.type() == CV_32F) {
    if (clone) {
      return image_in.clone();
    } else {
      return image_in;
    }
  }

  cv::Mat gray_image_8bit = Grayscale8Bit(image_in);
  cv::Mat gray_image_float;

  gray_image_8bit.convertTo(gray_image_float, CV_32F);
  return gray_image_float;
}

cv::Mat ImageUtils::MaskFromNonZero(const cv::Mat &image_in) {
  cv::Mat gray_image = Grayscale8Bit(image_in);
  cv::Mat mask(gray_image.size(), CV_8UC1);
  
  mask = gray_image > 0;
  return mask;
}

Contours ImageUtils::FindContours(const cv::Mat &image) {
  Contours contours;
  cv::Mat gray_image = Grayscale8BitClone(image);

  cv::findContours(gray_image, contours,
                   CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  return contours;
}

vector<cv::Rect> ImageUtils::FindBoundingBoxes(const Contours &contours) {
  vector<cv::Rect> bounding_boxes;

  for (size_t i = 0; i < contours.size(); ++i) {
    bounding_boxes.push_back(cv::boundingRect(cv::Mat(contours[i])));
  }

  return bounding_boxes;
}

vector<cv::Rect> ImageUtils::FindBoundingBoxes(const cv::Mat &image) {
  return FindBoundingBoxes(FindContours(image));
}

vector<cv::Rect>
ImageUtils::PruneSmallBoxes(const vector<cv::Rect> &boxes_in,
                            int min_area) {
  vector<cv::Rect> boxes_out;
  for (size_t i = 0; i < boxes_in.size(); ++i) {
    if (boxes_in[i].width * boxes_in[i].height >= min_area)
      boxes_out.push_back(boxes_in[i]);
  }

  return boxes_out;
}

cv::Rect ImageUtils::FindBoundingBox(const vector<cv::Rect> &boxes_in) {
  if (boxes_in.size() == 0) return cv::Rect(0,0,0,0);

  cv::Rect rect_out = boxes_in[0];

  for (size_t i = 1; i < boxes_in.size(); ++i) {
    rect_out |= boxes_in[i];
  }

  return rect_out;
}

cv::Rect ImageUtils::FindBoundingBox(const cv::Mat &image, int min_area) {
  return FindBoundingBox(PruneSmallBoxes
                         (FindBoundingBoxes(FindContours(image)),
                          min_area));
}

}  // namespace libhand
