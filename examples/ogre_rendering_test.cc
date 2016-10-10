# include <string>
# include "opencv2/opencv.hpp"
# include "file_dialog.h"
# include "hand_pose.h"
# include "hand_renderer.h"
# include "scene_spec.h"
// Success/failure macro
# include <cstdlib>

/*  This file adds a simple test (based on the bend_one_finger example), which
    simple renders a hand, counts the number of non-black pixels. If the number is
    greater than some arbitrary constant (10 was chosen), the test is considered a
    success, showing the backend render (OGRE) was able to load and configure its
    plugins and render an image -- a basic behaviour that is surprisingly unreliable
    between different configurations and platforms.
		        
    If rendered image produced by OGRE were bit-for-bit identical on every
    system, it will have been a more robust integration test to do a
    comparison of each pixel in the image with a test image file bundled with
    LibHand. However, given OGRE supports different graphics cards and
    software renderers using different rendering APIs (OpenGL vs DirectX) on
    different operating systems, it's unlikely to be perfectly deterministic,
    but this assertion has not been verified.
*/

// Arbitrary success metric. Needs at least:
const int kNumNonBlackPixels = 10;

int main(int argc, char **argv) {
  try {
    if (argc <= 1 ) {
      std::cout << "Usage: " << argv[0] << " [path to resource directory]"<< std::endl;
      exit(0);
    }
    std::string resource_dir = argv[1];

    libhand::HandRenderer hand_renderer;
    hand_renderer.Setup(hand_renderer.kDefaultWidth, hand_renderer.kDefaultHeight);

    libhand::SceneSpec scene_spec(resource_dir + "/hand_model/scene_spec.yml");

    // Tell the renderer to load the scene
    hand_renderer.LoadScene(scene_spec);
    // Now we render a hand using a default pose
    hand_renderer.RenderHand();

    // We can get an OpenCV matrix from the rendered hand image
    cv::Mat pic = hand_renderer.pixel_buffer_cv();

    double num_non_black_pixels=0;
    for (int i=0; i< pic.rows;i++) {
      for (int j=0; j< pic.cols*pic.channels();j=j+pic.channels()) {
       if ((pic.ptr<uchar>(i)[j]==0) && (pic.ptr<uchar>(i)[j+1]==0) && (pic.ptr<uchar>(i)[j+2]==0)) {
          num_non_black_pixels++;
        }
      }
    }

    // We need more non-black pixels than the threshold to succeed
    if (num_non_black_pixels > kNumNonBlackPixels) {
      std::cout << "Image appeared to render correctly, with " << num_non_black_pixels << " non-black pixels in the rendered image" << std::endl;
      exit(EXIT_SUCCESS);
    } else {
      std::cerr << "Image failed to render correctly, with " << num_non_black_pixels << " non-black pixels in the rendered image" << std::endl;
      exit(EXIT_FAILURE);
    }

  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
