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

# include "hog_descriptor.h"

# include <algorithm>
# include <vector>

# include "error_handling.h"
# include "hog_cell.h"

namespace libhand {

HogDescriptor::HogDescriptor(int num_rows,
                             int num_cols,
                             int cell_num_bins,
                             const float *hog_data_in) :
  num_rows_(num_rows),
  num_cols_(num_cols),
  cell_num_bins_(cell_num_bins),
  local_data_store_(data_store_size(), 0),
  hog_cells_(num_cells()) {
  InitializeHogCells();

  if (hog_data_in) {
    copy(hog_data_in, hog_data_in + data_store_size(), data_store());
  }
}

HogDescriptor::HogDescriptor(int num_rows,
                             int num_cols,
                             int cell_num_bins,
                             const vector<float> &hog_data_in) :
  num_rows_(num_rows),
  num_cols_(num_cols),
  cell_num_bins_(cell_num_bins),
  local_data_store_(hog_data_in),
  hog_cells_(num_cells()) {
  InitializeHogCells();
  if ((int) hog_data_in.size() != data_store_size()) {
    Error("HogDescriptor passed an incompatible data vector!");
    local_data_store_.resize(data_store_size());
    fill(data_store(), data_store() + data_store_size(), 0);
  }
}

HogDescriptor::HogDescriptor(const HogDescriptor &rhs) :
  num_rows_(rhs.num_rows_),
  num_cols_(rhs.num_cols_),
  cell_num_bins_(rhs.cell_num_bins_),
  local_data_store_(rhs.local_data_store_),
  hog_cells_(num_cells()) {
  InitializeHogCells();
}

HogDescriptor& HogDescriptor::operator= (const HogDescriptor &rhs) {
  CopyFromRHS(rhs);
  return *this;
}

void HogDescriptor::Zero() {
  fill(data_store(), data_store() + data_store_size(), 0.0);
}

void HogDescriptor::CopyFromRHS(const HogDescriptor &rhs) {
  AdjustHogCells(rhs);

  num_rows_ = rhs.num_rows_;
  num_cols_ = rhs.num_cols_;
  cell_num_bins_ = rhs.cell_num_bins_;
  local_data_store_ = rhs.local_data_store_;
}

void HogDescriptor::InitializeHogCells() {
  if ((int) hog_cells_.size() != num_cells()) {
    hog_cells_.resize(num_cells());
  }

  // Assign each hog cell the corresponding part of the datastore
  for (int r = 0, nr = num_rows(), nc = num_cols(); r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      hog_cells_[cell_number(r,c)].SetBins(cell_num_bins(), cell_data(r,c));
    }
  }
}

void HogDescriptor::AdjustHogCells(const HogDescriptor &rhs) {
  // Do nothing if the dimensions match
  if ((rhs.num_rows() == num_rows())
      && (rhs.num_cols() == num_cols())
      && (rhs.cell_num_bins() == cell_num_bins()))
    return;

  // Copy new dimensions
  num_rows_ = rhs.num_rows_;
  num_cols_ = rhs.num_cols_;
  cell_num_bins_ = rhs.cell_num_bins_;

  InitializeHogCells();
}

}  // namespace libhand
