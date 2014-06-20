// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// SimpleSlider

# include "simple_slider.h"

# include "opencv2/opencv.hpp"
# include "opencv2/highgui/highgui.hpp"

namespace libhand {

SimpleSlider::SimpleSlider(const string &name,
                           const string &window_name,
                           float interval_start,
                           float interval_end,
                           int raw_max_val,
                           SliderRefresh *refresh) :
  name_(name),
  window_name_(window_name),
  interval_start_(interval_start),
  interval_end_(interval_end),
  raw_max_val_((raw_max_val < 0) ? 0 : raw_max_val),
  cur_raw_val_(0),
  refresh_(refresh) {
  cv::createTrackbar(name_, window_name_,
                     &cur_raw_val_, raw_max_val_,
                     &SimpleSlider::Callback, (void *) this);
}

void SimpleSlider::Callback(int state, void *data) {
  SimpleSlider *instance = (SimpleSlider *) data;

  if (instance->refresh_) {
    instance->refresh_->RefreshMe(instance);
  }
}

int SimpleSlider::raw_val() const {
  return cur_raw_val_;
}

void SimpleSlider::set_raw_val(int raw_val) {
  if (raw_val < 0) raw_val = 0;
  if (raw_val > raw_max_val_) raw_val = raw_max_val_;

  cur_raw_val_ = raw_val;
  cv::setTrackbarPos(name_, window_name_, raw_val);
}

float SimpleSlider::val() const {
  if (raw_max_val_ == 0) {
    return interval_start_;
  }

  double range = (double) interval_end_ - (double) interval_start_;
  double step_size = range / (double) raw_max_val_;
  double d_val = interval_start_ + step_size * (double) raw_val();

  return d_val;
}

void SimpleSlider::set_val(float val) {
  if (raw_max_val_ == 0) {
    set_raw_val(0);
    return;
  }

  if (val < interval_start_) val = interval_start_;
  if (val > interval_end_) val = interval_end_;

  double range = (double) interval_end_ - (double) interval_start_;
  double step_mult = (double) raw_max_val_ / range;
  double d_val = ((double) val - (double) interval_start_) * step_mult;

  set_raw_val((int) d_val);
}

}  // namespace libhand
