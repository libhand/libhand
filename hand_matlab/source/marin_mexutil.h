// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

#ifndef __MARIN_MEXUTIL_H__
#define __MARIN_MEXUTIL_H__

# include <string>
# include <exception>

# include "mex.h"

#define MARIN_MEXUTIL_ERRSTRING_MAX 4096

namespace libhand {

using namespace std;

class MatlabErrorException : public std::exception {
public:
  MatlabErrorException(const char *, ...);
  virtual ~MatlabErrorException() throw() {}

  const char* what() const throw() { return str_data_; }

private:
  MatlabErrorException();
  char str_data_[MARIN_MEXUTIL_ERRSTRING_MAX];
};

void SendMatlabText(const char *format, ...);

void StoreMatlabPtr(const string &var_name, void *ptr);
void *LoadMatlabPtr(const string &var_name);

bool IsAString(const mxArray *arr);
string ToString(const mxArray *arr);

bool IsA2DMatrix(const mxArray *arr);

bool IsAScalar(const mxArray *arr);
double ToScalar(const mxArray *arr);

}  // namespace libhand

#endif  /* __MARIN_MEXUTIL_H__ */
