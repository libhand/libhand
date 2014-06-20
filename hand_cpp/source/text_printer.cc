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

# include "text_printer.h"

# include <cstdio>
# include <cstdarg>
# include <string>

# include "opencv2/opencv.hpp"

# include "printfstring.h"

namespace libhand {

TextPrinter::TextPrinter(const cv::Mat &display, int pos_x, int pos_y) :
  display_(display),
  cursor_pos_(pos_x, pos_y),
  font_id_(cv::FONT_HERSHEY_PLAIN),
  scale_(1),
  spacing_(8),
  thickness_(1),
  color_(255, 255, 255),
  right_align_(false) {
}


void TextPrinter::PrintF(const char *format, ...) {
  char str_data[PRINTFSTRING_MAX];
  va_list ap;

  va_start(ap, format);
  vsnprintf(str_data, PRINTFSTRING_MAX, format, ap);
  va_end(ap);

  Print(string(str_data));
}

void TextPrinter::Print(const string &str) {
  size_t last = 0;

  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '\n') {
      PrintLine(string(str, last, i-last));
      last = i + 1;
    }
  }

  if (last < str.size()) {
    PrintLine(string(str, last, str.size() - last));
  }
}

void TextPrinter::PrintLine(const string &str) {
  int rel_base = 0;
  cv::Size sz = cv::getTextSize(str, font_id_,
                                scale_, thickness_, &rel_base);
  cv::Point draw_point = cursor_pos_;

  if (right_align_) draw_point.x -= sz.width;

  cv::putText(display_, str, draw_point,
              font_id_, scale_, color_, thickness_);
  cursor_pos_.y += sz.height + spacing_;
}

}  // namespace libhand
