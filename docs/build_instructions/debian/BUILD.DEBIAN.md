## Building LibHand on Debian/Ubuntu Linux

The following instructions allow compilation of LibHand without too much trouble on all versions of Debian/Ubuntu currently supported by the Debian Project and Canonical.

See the 'Supported platform' section of README.md for exact OS versions LibHand currently supports.

#### Satisfying software dependencies under Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git libogre-1.9-dev libglu1-mesa-dev libxt-dev libopencv-dev tcl tk libboost-system-dev libboost-filesystem-dev
```
On Ubuntu 12.04 and Debian Wheezy, there is no libogre-1.9-dev package available, so substitute with libogre-dev (which is v1.7.4 on those systems).

On a fresh Ubuntu 12.04/14.04/16.04 install, if apt-get is unable to locate the OpenCV (libopencv-dev) and OGRE packages, append 'universe' to your sources.list, run `sudo apt-get update` and try again.

### Compiling libhand and pose_designer
```bash
git clone https://github.com/[user]/libhand
cd libhand
mkdir build
cd build
cmake ..
make
# Collate libhand build artifacts into local "dist" directory (for external applications to link against)
make install
```
### Usage
Unfortunately, the OGRE package in the Debian and Ubuntu repositories does not configure its own runtime linking search paths. Instead, we supply the path manually and update the loader. On a 64-bit Ubuntu 16.04 system linux system for example, before running libhand you may need to run:
```bash
# Ensure path exists first: on 32-bit, x86_64 is replaced with i386. Replace OGRE version with whatever was installed (likely OGRE-1.9.0, OGRE-1.8.0, or OGRE-1.7.4).
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/x86_64-linux-gnu/OGRE-1.9.0
sudo ldconfig
```
Once the OGRE library path is configured, you can launch the graphical pose designer from the build directory:
```
./hand_cpp/source/pose_designer
```
...and then use the file dialog box to open hand_model/scene_spec.yml to design hand poses! From the Pose Designer you can use press 'r' to open another dialog box to load some pre-defined hand pose (located in poses/*.yml). Toggle the help popup by pressing 'h'.


