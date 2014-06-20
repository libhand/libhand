// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

# include <stdio.h>
# include <stdarg.h>
# include <string>

# include "printfstring.h"

namespace libhand {

string PrintFString(const char *format, ...) {
  char str_data[PRINTFSTRING_MAX];
  va_list ap;

  va_start(ap, format);
  vsnprintf(str_data, PRINTFSTRING_MAX, format, ap);
  va_end(ap);

  return string(str_data);
}

};
