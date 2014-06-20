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
// HandRenderer
//
// The HandRenderer class sets up a 3D engine used to render the hand. 
//
// The class uses a technique known as PImpl (Pointer to
// Implementation) or "compiler firewalling" to isolate the user from
// the OGRE 3D engine code.
//
// As such the header file does not include any OGRE 3D engine code
// although the hand_renderer class uses it internally in a private
// implementation.

#ifndef HAND_RENDERER_H
#define HAND_RENDERER_H

# include "opencv2/opencv.hpp"

# include "hand_camera_spec.h"
# include "hand_pose.h"
# include "scene_spec.h"

namespace libhand {

using namespace std;

class HandRendererPrivate;

class HandRenderer {
 public:
  // You should have one instance of HandRenderer class that lives as
  // long as you plan to render hands. There should not be multiple
  // instances of HandRenderer existing at the same time.
  HandRenderer();
  ~HandRenderer();

  // The default output width and height
  static const int kDefaultWidth = 400;
  static const int kDefaultHeight = 400;
  
  // The setup routine must be called first.
  //    width - the width of the output buffer in pixels
  //    height - the height of the output buffer in pixels
  void Setup(int width = kDefaultWidth,
             int height = kDefaultHeight);

  // Partially reloads the 3D engine to adjust all the buffers to the
  // desired output width and height. SetRenderSize() can be called
  // throughout the program.
  void SetRenderSize(int width = kDefaultWidth,
                     int height = kDefaultHeight);

  // Loads the 3D scene based on the description in the scene_spec.yml file
  void LoadScene(const SceneSpec &scene_spec);

  // Updates the hand pose (but does not actually render the hand).
  // If update_camera is set to true, then the camera is going to be moved
  // corresponding to the camera information in the FullHandPose class.
  void SetHandPose(const FullHandPose &hand_pose,
                   bool update_camera = false);

  // Read and write accessors to camera information.
  HandCameraSpec camera_spec() const;
  void set_camera_spec(const HandCameraSpec &camera_spec);

  // Renders the hand into the pixel buffer.
  void RenderHand();

  // The camera distance from the center of the hand object when the
  // scene file was loaded
  float initial_cam_distance() const;

  // The width and height of the pixel buffer.
  int render_width() const;
  int render_height() const;

  // Provides raw access to the pixel buffer. The buffer is
  // contiguous, row-major, in the BGR888 format, no alpha channel.
  const char *pixel_buffer_raw() const;

  // Provides a light wrapper around the buffer as an OpenCV matrix.
  const cv::Mat pixel_buffer_cv() const;

 private:
  // PIMPL (Private Implementation pointer)
  HandRendererPrivate *private_;

  void DestroyScene();

  // Disallow
  HandRenderer(const HandRenderer &rhs);
  HandRenderer& operator= (const HandRenderer &rhs);
};

}  // namespace libhand
#endif  // HAND_RENDERER_H
