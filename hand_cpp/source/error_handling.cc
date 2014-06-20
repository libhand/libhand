// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// ErrorHandling

# include "error_handling.h"

# include <iostream>

namespace libhand {

void Warning(const string &str) {
  cerr << str << endl;
}

void Warning(const char *str) {
  cerr << str << endl;
}

void Error(const string &str) {
  cerr << str << endl;
}

void Error(const char *str) {
  cerr << str << endl;
}

}  // namespace libhand
