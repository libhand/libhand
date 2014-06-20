// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

# include <cstdio>
# include <cstdarg>

# include "boost/shared_array.hpp"

# include <string>

# include "marin_mexutil.h"
# include "mex.h"

namespace libhand {

using namespace boost;
using namespace std;

MatlabErrorException::MatlabErrorException(const char *format, ...) {
  va_list ap;

  va_start(ap, format);
  vsnprintf(str_data_, MARIN_MEXUTIL_ERRSTRING_MAX, format, ap);
  va_end(ap);
}

void SendMatlabText(const char *format, ...) {
  char str_data[MARIN_MEXUTIL_ERRSTRING_MAX];
  va_list ap;

  va_start(ap, format);
  vsnprintf(str_data, MARIN_MEXUTIL_ERRSTRING_MAX, format, ap);
  va_end(ap);

  mexPrintf("%s", str_data);
}

void StoreMatlabPtr(const string &var_name, void *ptr) {
  mxArray *array_ptr = mexGetVariable("global", var_name.c_str());

  if (array_ptr == NULL) {
    array_ptr = mxCreateDoubleMatrix(2, 2, mxREAL);
  }

  char *raw_data_out=(char *) mxGetPr(array_ptr);
  int inptr_size = sizeof(void *);
  char *raw_data_in = (char *) &(ptr);

  for (int i = 0; i < inptr_size; ++i) {
    raw_data_out[i] = raw_data_in[i];
  }

  mexPutVariable("global", var_name.c_str(), array_ptr);

  mxDestroyArray(array_ptr);
}

void *LoadMatlabPtr(const string &var_name) {
  mxArray *array_ptr = mexGetVariable("global", var_name.c_str());

  if (array_ptr == NULL) return NULL;

  char *raw_data_in=(char *) mxGetPr(array_ptr);
  void *output_ptr = NULL;
  int outptr_size = sizeof(void *);
  char *raw_data_out = (char *) &(output_ptr);

  for (int i = 0; i < outptr_size; ++i) {
    raw_data_out[i] = raw_data_in[i];
  }

  mxDestroyArray(array_ptr);

  return output_ptr;
}

bool IsAString(const mxArray *str_arr) {
  return !mxIsSparse(str_arr) && (mxGetClassID(str_arr) == mxCHAR_CLASS);
}

string ToString(const mxArray *arr) {
  if (!IsAString(arr)) return "";

  int buf_size = mxGetNumberOfElements(arr) + 1;

  shared_array<char> buf(new char[buf_size]);

  if (mxGetString(arr, buf.get(), buf_size) != 0) return "";

  return string(buf.get());
}

bool IsA2DMatrix(const mxArray *arr) {
  return !mxIsSparse(arr)
    && mxGetNumberOfDimensions(arr) == 2 
    && mxGetClassID(arr) == mxDOUBLE_CLASS;
}

bool IsAScalar(const mxArray *arr) {
  return IsA2DMatrix(arr) && mxGetM(arr) == 1 && mxGetN(arr) == 1;
}

double ToScalar(const mxArray *arr) {
  if (!IsAScalar(arr)) return 0.;

  return mxGetPr(arr)[0];
}

}  // namespace libhand
