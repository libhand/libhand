// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// TextPrinter

#ifndef TEXT_PRINTER_H
#define TEXT_PRINTER_H

# include <string>

# include "opencv2/opencv.hpp"

namespace libhand {

using namespace std;
class TextPrinter {
 public:
  TextPrinter(const cv::Mat &display, int pos_x = 0, int pos_y = 0);

  void Print(const string &str);
  void PrintF(const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));

  // Settings
  void SetFont(int font_id) { font_id_ = font_id; }
  void SetScale(double scale) { scale_ = scale; }
  void SetSpacing(int spacing) { spacing_ = spacing; }
  void SetThickness(int thickness) { thickness_ = thickness; }
  void SetColor(const cv::Scalar &color) { color_ = color; }
  void SetLeftAlign() { right_align_ = false; }
  void SetRightAlign() { right_align_ = true; }

 private:
  void PrintLine(const string &str);

  cv::Mat display_;
  cv::Point cursor_pos_;
  int font_id_;
  double scale_;
  int spacing_;
  int thickness_;
  cv::Scalar color_;
  bool right_align_;

  // Disallow
  TextPrinter();
  TextPrinter(const TextPrinter &rhs);
  TextPrinter& operator= (const TextPrinter &rhs);
};

}  // namespace libhand
#endif  // TEXT_PRINTER_H
