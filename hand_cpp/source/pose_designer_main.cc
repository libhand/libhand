// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

# include <exception>
# include <iostream>

# include "printfstring.h"

# include "pose_designer.h"

using namespace std;
using namespace libhand;

int main(int argc, char **argv) {
  PoseDesigner pose_designer;

  try {
    pose_designer.Setup(argc, argv);
    pose_designer.Run();
  } catch (const std::exception &e) {
    cerr << "Exception: " << e.what() << endl;
  }

  return 0;
}
