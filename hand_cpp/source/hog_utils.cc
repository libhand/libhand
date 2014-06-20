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

# include "hog_utils.h"

# include <cmath>

# include "opencv2/opencv.hpp"

# include "hog_cell.h"
# include "hog_cell_rectangles.h"
# include "hog_descriptor.h"
# include "printfstring.h"

namespace libhand {

void HogUtils::RenderHog(const HogDescriptor &hog_desc,
                         cv::Mat &out_image) {
  HogCellRectangles hog_rects(hog_desc, out_image);
  cv::Scalar pure_green(0, 255, 0);

  double angle_step = M_PI / (double) hog_desc.cell_num_bins();

  for (int r = 0, nr = hog_rects.num_rows(), nc = hog_rects.num_cols();
       r < nr;
       ++r) {
    for (int c = 0; c < nc; ++c) {
      const cv::Rect &cur_rect = hog_rects.rect(r,c);
      const HogCell &cur_cell = hog_desc.hog_cell(r,c);

      cv::rectangle(out_image, cur_rect, pure_green);
      cv::putText(out_image,
                  PrintFString("%03.1f", cur_cell.Sum()),
                  cv::Point(cur_rect.x + 3,
                            cur_rect.y + cur_rect.height - 3),
                  cv::FONT_ITALIC,
                  0.3,
                  pure_green);

      if (cur_cell.IsZero()) continue;

      double xc = (double) cur_rect.x + (double) cur_rect.width / 2.;
      double yc = (double) cur_rect.y + (double) cur_rect.height / 2.;

      int max_idx = cur_cell.MaxElementIdx();
      double angle = (M_PI / 2.) + (double) max_idx * angle_step;

      double radius = (double) cur_rect.height * 0.9 / 2.;
      double max_val = cur_cell.bin(max_idx);
      double line_halflength = max_val * radius;

      double x0 = xc - line_halflength * cos(angle);
      double x1 = xc + line_halflength * cos(angle);
      double y0 = yc - line_halflength * sin(angle);
      double y1 = yc + line_halflength * sin(angle);

      cv::line(out_image, cv::Point(x0, y0), cv::Point(x1, y1), pure_green);
    }
  }
}

void HogUtils::RenderHog(const cv::Mat &in_image,
                         const HogDescriptor &hog_desc,
                         cv::Mat &out_image) {
  if (out_image.rows < 1 || out_image.cols < 1) {
    out_image.create(cv::Size(kDefaultRenderWidth,
                              kDefaultRenderHeight), CV_8UC1);
  }

  cv::resize(in_image, out_image, out_image.size(), 0, 0, cv::INTER_LANCZOS4);
  RenderHog(hog_desc, out_image);
}


}  // namespace libhand
