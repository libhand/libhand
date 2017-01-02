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

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H


#include <iostream>
#include "hand_prereq.h"

namespace libhand {

using namespace std;

void HAND_EXPORT Warning(const string &str);
void HAND_EXPORT Warning(const char *str);
void HAND_EXPORT Error(const string &str);
void HAND_EXPORT Error(const char *str);

}  // namespace libhand
#endif  // ERROR_HANDLING_H
