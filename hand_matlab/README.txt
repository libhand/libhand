LibHand v0.9
Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
All rights reserved.

http://www.libhand.org/  -- the most up-to-date information on LibHand.


    hand_matlab README
 (please check INSTALL.txt in the root LibHand directory for more
 detailed instructions)

1) Compiling

  After building the LibHand C++ library in hand_cpp, you can compile
  the MATLAB stuff here
    ./mex_compile.sh

  Hopefully nothing else will be needed for compilation.

  There are some known fixable problems with MATLAB installations,
  please check the INSTALL.txt in the LibHand root directory for more
  detailed information.

2) MATLAB setup

  The compiled plugin will reside in the directory 'dist', along with
  some helper files.

  The MATLAB examples are in the 'examples' directory

  You just need to add 'dist' to your MATLAB path and you are good to
  go. The program "add_paths.m" in the examples directory adds the
  path for you

3) Running the examples

  cd into the examples directory and try them out

  If you didn't add the dist folder to your MATLAB path, just run
  add_paths before running the examples.

====
Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
All rights reserved.

This file is a part of LibHand. LibHand is open-source software. You can
redistribute it and/or modify it under the terms of the LibHand license. The
LibHand license is the BSD license with an added clause that requires
academic citation. You should have received a copy of the LibHand license
(the license.txt file) along with LibHand. If not, see
<http://www.libhand.org/>
