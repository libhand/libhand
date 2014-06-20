// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

#ifndef __PRINTFSTRING_H__
#define __PRINTFSTRING_H__

#include <string>

#define PRINTFSTRING_MAX 4096

namespace libhand {

using namespace std;

// Use PrintFString to print to a C++ string just like with printf()
string PrintFString(const char *format, ...)
  __attribute__ ((format (printf, 1, 2)));

};

#endif  // PRINTFSTRING_H__
