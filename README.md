LibHand v0.9
Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
All rights reserved.

http://www.libhand.org/  -- the most up-to-date information on LibHand.

About LibHand
----------------------------------------

LibHand is an open-source permissively licensed portable library for
rendering and recognizing articulations of human hand. The goal of LibHand
is to provide as simple programming interface as possible to manipulate
human hand pose information, render a human hand and analyze the resulting
image. Potential scientific applications include Computer Graphics, Computer
Vision and Robotics.

LibHand is available at <http://www.libhand.org/>
Feel free to contact LibHand's author Marin Saric at: marin.saric@gmail.com

LibHand is implemented in C++ and offers a MATLAB interface to facilitate
experimentation. LibHand is currently actively supported for MacOS X and
Ubuntu Linux on 64-bit x86 processors.

By leveraging OGRE, a modern 3D engine, LibHand can render, deform and
analyze a realistic 70k+ triangle hand model at high framerates on mid-range
computer hardware.

LibHand provides OpenCV-friendly interfaces to make it very easy for the
Computer Vision and Robotics community to use it in their research. LibHand
is designed to make it easier to reproduce and extend.the research in areas
such as hand grasping, hand pose recognition, hand gesture recognition,
visual servoing, etc.

Additionaly LibHand provides a realistic Human Hand 3D model available in
Blender and OGRE formats under a very permissive license (see (6)).

The LibHand was inspired by concepts from recent research in human hand to
robot grasping, for example:

Installing
----------------------------------------

Please see the INSTALL.txt file for directions

What's contained in this distribution?
----------------------------------------

The file INSTALL.txt contains instructions on how to compile and set up
LibHand. Here is a general overview of the LibHand directory structure.

- hand_cpp - compile this first.
 The hand_cpp directory provides the LibHand C++ library and the
 pose_designer utility. 

The pose_designer utility can be used to "test drive" LibHand and experiment
with different hand poses.

- hand_model - this is the main hand 3D data directory. There is a
  "scene_spec.yml" file that is used by the LibHand software to load in the
  hand 3d scene, the hand 3D mesh, textures, skeletons, etc. 

- examples - this directory contains the tutorial code with detailed
  comments. The examples directory requires the hand_cpp directory to be
  compiled and setup first

- hand_matlab - contains the interface code for MATLAB, along with MATLAB
  examples. To compile hand_matlab, you need a recent version of MATLAB that
  is setup to compile with mex. Make sure you can compile C the example code
  that comes with your installation of MATLAB before trying to compile
  the LibHand MATLAB interface. Alternatively, you can download a
  pre-compiled MATLAB binary from <http://www.libhand.org/>

- poses - contains some example hand poses as YAML files, to be used with
  the pose_designer and the example code for both C++ and MATLAB.

- install_help - contains some utilities to make installing the LibHand
  dependecies easier.

Citation
----------------------------------------

  * If this software or its derivative is used to produce an academic
publication, you are required to cite this work by using the following
citation or an alternate form provided on "http://www.libhand.org/" :
```
    @misc{libhand,
      author = "Marin \v{S}ari\'{c}",
      title = "LibHand: A Library for Hand Articulation",
      year = "2011",
      url = "http://www.libhand.org/",
      note = "Version 0.9"
    }
```

License
----------------------------------------

LibHand is available under a derivative of a BSD-license that requires the
academic community to provide attribution to LibHand by means of
citations. Essentially, you can use, modify and sell LibHand or the derived
software in academic and non-academic, commercial and non-commercial,
open-source and closed-source settings, but you have to give proper
credit. Please see the license.txt file for the exact licensing information.

The Hand 3D Model License
----------------------------------------

LibHand comes with a textured, rigged and skinned realistic model of a human
hand. The LibHand 3D Hand Model is located in the hand_model directory and
it's available under the Creative Commons Attribution 3.0 Unported License.

Please see the hand_model_license.txt file in the hand_model directory for
the exact licensing information.


Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
All rights reserved.

This file is a part of LibHand. LibHand is open-source software. You can
redistribute it and/or modify it under the terms of the LibHand license. The
LibHand license is the BSD license with an added clause that requires
academic citation. You should have received a copy of the LibHand license
(the license.txt file) along with LibHand. If not, see
<http://www.libhand.org/>
