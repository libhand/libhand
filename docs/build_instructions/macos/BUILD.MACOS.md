## Building LibHand on macOS 

The following instructions allow compilation of LibHand on macOS.

See the 'Supported platform' section of README.md for exact OS versions LibHand currently supports.

#### Satisfying software dependencies under macOS
First, install [homebrew](http://brew.sh/) if not already installed. In a Terminal window, run:
```bash
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
Then, add the Homebrew formulae taps and install the relevant dependencies.
```bash
brew tap homebrew/science
brew tap osrf/simulation
brew install boost opencv ogre
```
OGRE 1.7.4 has been tested and works, OGRE 1.9 might work, but isn't currently supported or tested.

### Compiling libhand and pose_designer
```bash
git clone https://github.com/libhand/libhand
cd libhand
mkdir build
cd build
cmake ..
make
# Collate libhand build artifacts into local "dist" directory (for external applications to link against)
make install
```
### Usage
Now, you can launch the graphical pose designer from the build directory:
```
./hand_cpp/source/pose_designer
```
...and then use the file dialog box to open hand_model/scene_spec.yml to design hand poses! From the Pose Designer you can use press 'r' to open another dialog box to load some pre-defined hand pose (located in poses/*.yml). Toggle the help popup by pressing 'h'.
