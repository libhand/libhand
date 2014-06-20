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
// Hog

#ifndef HOG_CELL_H
#define HOG_CELL_H

# include <cstring>

# include <algorithm>
# include <numeric>
# include <vector>

namespace libhand {

using namespace std;

class HogDescriptor;

class HogCell {
 public:
  // Standard constructor; bin_data points to the existing
  // histogram data
  HogCell(int num_bins, float *bin_data);

  // Default constructors
  HogCell();
  HogCell(const HogCell &rhs);

  // Default assignment
  HogCell& operator= (const HogCell &rhs);

  // Trivial accessors
  float& bin(int bin_no) { return bins_[bin_no]; }
  const float& bin(int bin_no) const { return bins_[bin_no]; }

  int num_bins() const { return num_bins_; }

  // trivial iterators
  float *begin() { return bins_; }
  float *end() { return bins_ + num_bins_; }
  const float *begin() const { return bins_; }
  const float *end() const { return bins_ + num_bins_; }

  // Shallow setters
  void SetBins(int num_bins, float *bin_data);

  // Deep setters
  void Zero();
  void LoadBins(const vector<int> &bin_data);
  void LoadBins(const vector<float> &bin_data);

  // Calculations
  float Sum() const;
  HogCell& operator*= (float k);
  void Normalize();
  bool IsZero() const;
  int MaxElementIdx() const;

  static const float kNoiseFloor = 1e-8;

 private:
  float *bins_;
  int num_bins_;
};

// Inlined methods follow

inline HogCell::HogCell() :
  bins_(NULL),
  num_bins_(0) {}

inline HogCell::HogCell(int num_bins, float *bin_data) :
  bins_(bin_data),
  num_bins_(num_bins) {}

inline HogCell::HogCell(const HogCell &rhs) :
  bins_(rhs.bins_),
  num_bins_(rhs.num_bins_) {}

inline HogCell& HogCell::operator= (const HogCell &rhs) {
  bins_ = rhs.bins_;
  num_bins_ = rhs.num_bins_;
  return *this;
}

inline void HogCell::SetBins(int num_bins, float *bin_data) {
  bins_ = bin_data;
  num_bins_ = num_bins;
}

inline void HogCell::LoadBins(const vector<int> &bin_data) {
  copy(&(bin_data[0]), &(bin_data[num_bins_]), begin());
}

inline void HogCell::LoadBins(const vector<float> &bin_data) {
  copy(&(bin_data[0]), &(bin_data[num_bins_]), begin());
}

inline void HogCell::Zero() {
  fill(begin(), end(), 0.0);
}

inline float HogCell::Sum() const {
  return accumulate(begin(), end(), 0.0);
}

inline HogCell& HogCell::operator*= (float k) {
  for (int i = 0, nb = num_bins_; i < nb; ++i) {
    bins_[i] *= k;
  }
  return *this;
}

inline void HogCell::Normalize() {
  const float sum = Sum();

  if (sum < kNoiseFloor) {
    Zero();
  } else {
    *this *= 1./sum;
  }
}

inline bool HogCell::IsZero() const {
  return Sum() < kNoiseFloor;
}

inline int HogCell::MaxElementIdx() const {
  int nb = num_bins_ ;
  if (nb == 0) return 0;

  int max_idx = 0;
  float max = bin(0);

  for (int i = 1; i < nb; ++i) {
    float v = bin(i);

    if (v > max) { max_idx = i; max = v; }
  }

  return max_idx;
}

}  // namespace libhand
#endif  // HOG_CELL_H
