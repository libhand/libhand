// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// HogDescriptor
//
// The HogDescriptor class defines a Histogram of Gradients feature
// descriptor that is an array of histograms. In this HoG
// implementation, the region of interest is divided into
// approximately equal rectangular regions called HoG cells.
//
// Each HoG cell contains a histogram corresponding to the histogram
// of gradients for the portion of the image defined by the hog cell
// rectangle.
//
// The HoG cell rectangle is defined by the HogCellRectangles class
// in hog_cell_rectangles.h

#ifndef HOG_DESCRIPTOR_H
#define HOG_DESCRIPTOR_H

# include <cstring>
# include <vector>

# include "hog_cell.h"

namespace libhand {

using namespace std;

class HogDescriptor {
 public:
  // Common HoG descriptor constructor
  // By default, HogDescriptor stores all the data inside the class.
  // However, if the data is stored externally, a pointer to it can be
  // passed with hog_data_in.
  // All the HoG cells are initialized to 0 with this constructor.
  HogDescriptor(int num_rows = kDefaultNumRows,
                int num_cols = kDefaultNumCols,
                int cell_num_bins = kDefaultCellNumBins,
                const float *hog_data_in = NULL);

  // This constructor allows for initializing the descriptor from an
  // existing vector of floats.
  HogDescriptor(int num_rows,
                int num_cols,
                int cell_num_bins,
                const vector<float> &hog_data_in);

  // Copy and assignment constructors
  HogDescriptor(const HogDescriptor &rhs);
  HogDescriptor& operator= (const HogDescriptor &rhs);

  // Simple accessors
  int num_rows() const { return num_rows_; }
  int num_cols() const { return num_cols_; }
  int num_cells() const { return num_rows() * num_cols(); }
  int cell_num_bins() const { return cell_num_bins_; }

  // Simple setters
  void Zero();

  // Access to individual HoG cells
  HogCell &hog_cell(int row, int col) {
    return hog_cells_[cell_number(row, col)];
  }

  const HogCell &hog_cell(int row, int col) const {
    return hog_cells_[cell_number(row, col)];
  }

  // Data-store size (in number of elements)
  int data_store_size() const { return num_cells() * cell_num_bins(); }

  // The default number of cell rows the image is divided into
  static const int kDefaultNumRows = 8;

  // The default number of cell column the image is divided into
  static const int kDefaultNumCols = 8;

  // The default number of histogram bins in a cell
  static const int kDefaultCellNumBins = 8;

private:
  // Private accessors (read-write and read-only)
  float *data_store() { return local_data_store_.data(); };
  const float *data_store() const { return local_data_store_.data(); };

  // Row-major cell 2D index into row-by-row 1D index
  int cell_number(int row, int col) const { return row * num_cols() + col; }

  // Access into individual cell data (read-write and read-only)
  float *cell_data(int row, int col) {
    return data_store() + cell_number(row, col) * cell_num_bins();
  }
  const float *cell_data(int row, int col) const {
    return data_store() + cell_number(row, col) * cell_num_bins();
  }

  // Utility routines for descriptor initialization and copying
  void InitializeHogCells();
  void AdjustHogCells(const HogDescriptor &rhs);
  void CopyFromRHS(const HogDescriptor &rhs);

  // The number of HoG cell rows the image is divided into
  int num_rows_;

  // The number of HoG cell columns the image is divided into
  int num_cols_;

  // The number of bins for each HoG cell
  int cell_num_bins_;

  // Local data store
  vector<float> local_data_store_;

  // HoG Cell structures
  vector<HogCell> hog_cells_;
};

}  // namespace libhand
#endif  // HOG_DESCRIPTOR_H
