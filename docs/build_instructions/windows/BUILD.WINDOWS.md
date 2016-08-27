## Building LibHand under Windows

The following instructions allow compilation of LibHand without too much trouble on all versions of Windows currently supported by Microsoft.

See the 'Supported platform' section of README.md for exact OS versions LibHand currently supports.

There are three supported methods for compiling LibHand on Microsoft platforms

1. Linking against mostly pre-compiled dependencies (Quickest and easiest)
  * Due to upstream projects (OGRE dependency), we are locked into a toolchain targing a 32-bit x86 executable, and Microsoft's Visual C++ 11 compiler (or newer).
  * A helper (Bash) shell script has been provided to automate the build (currently using Visual C++ 11 only, but can be modified/extended in future)

2. Linking against key dependencies compiled from source (Advanced Users Only: time consuming, potentially error prone, but most flexible) 
  * Building from source frees us from being locked into a toolchain: in principle, allows building using VC++, MinGW (Minimalist GNU for Windows) g++, or even Cygwin's g++ and GNU Make
  * A helper (Bash) shell script has been provided to automate the build (currently using Visual C++ 11 only, but can be modified/extended in future)

3. Using [Windows Subsystem for Linux](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux) (WSL) (Large Linux learning curve)
  * WSL is a compatibility layer currently on available only on newer builds of Windows 10, only in developer mode, and only on x86_64 architecture. With it, we can use the Ubuntu g++ binary and link against unmodified Ubuntu dependencies, following LibHand's Debian/Ubuntu build instructions.
  * A third-party X Server such as the free XMing application is needed to allow applications running under the subsystem to display graphics. 
  * Any binaries produced when compiling with the Windows Subsystem For Linux are ELF64 so will not run on any Windows systems that do not have this compatibility layer installed
  * This is the quickest and easiest solution for developers with a clean Windows install to experiment with LibHand, but requires using UNIX environment.

If you are going with the first or second option, if you already have Visual Studio 2013 installed, you'll be up and running within minutes (otherwise it will take *hours*).

#### 1. Linking against mostly pre-compiled dependencies under Windows 10 64-bit downloaded with Chocolatey package manager 

Most of the required dependencies can be easily installed with Chocolatey. Chocolately automatically downloads installers directly from the official source website and installs with no user intervention needed. The scripts which enable this are maintained by the community, a similar methodology to the 'formula' scripts used by with Homebrew on macOS.

Some LibHand dependencies (such as OGRE3D) only release a 32-bit binary package, and even then, only release them for some compilers. This forces use to use Visual Studio 2013 targeting a 32-bit build. The dependencies force us to compile Boost from source with certain parameters to allow linking to succeed.

Additionally, because some of LibHand's dependencies in the Chocolatey repositories don't yet have a 32-bit package available, we can only use Chocolatey to automate the installation of dependencies only on a 64-bit OS, even though we are targeting a 32-bit build.

The following instructions have been tested on Windows 10 x86_64.

1. Install [Chocolatey](https://chocolatey.org/install) by running Command Prompt as Administrator and running the following
  *
	@powershell -NoProfile -ExecutionPolicy Bypass -Command "iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))" && SET PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin

2. After successful install of Chocolately, close all existing CMD windows and open a new Administrator Command Prompt and run the following line
  * 
	choco install -y --allow-empty-checksums opencv -version 2.4.9.20140518 && choco install -y cmake && choco install -y git && choco install -y --allow-empty-checksums boost-msvc-12 activetcl
  
  * Append the current users' path with "C:\OpenCV249\opencv\build\x86\vc12\bin"
  * 
	choco install -y --allow-empty-checksums --execution-timeout 7200 visualstudiocommunity2013
  
  * The install of Visual Studio 2013 may take *hours*, and 10gb of HDD space.

At this point, we are ready to use the supplied Bash helper script to build LibHand 

##### Building LibHand on Windows 10 with Visual Studio 2013 Community

1. Open a Git Bash shell run the following commands
  *
	git clone https://github.com/[user]/libhand
  	cd libhand
  	# The following command will take a while
  	./docs/build_instructions/windows/download_and_build.sh
  
  * After this command has completed, CMake should have generated a Visual Studio Project and built both Debug and Release profiles of it, built these profiles. Built the INSTALL.vcxproj profile, and generated and built the Release build of the examples
    * Currently rather than setup some environment variables (OGRE_HOME, OpenCV in PATH), the scripts instead automatically copied the relevant libraries into the build directory.)
    * If linking dynamically (default), note that the build helper script currently automatically copies some files from the OGRE install location. This means if you run a build clean, then you'll have to either copy the files back manually, or use the build script to do this automatically.
      * FIXME: Improve instructions/environment variables to prevent need to duplicate the dynamic library binaries.
2. Assuming the build suceeded, navigate to build\hand_cpp\source\Release (the location containing pose_designer.exe)
. . Launch pose_designer.exe
3. Assuming the runtime dependencies (ActiveTcl, OGRE and OpenCV) have been installed, you should see a 3D rendered hand!
4. The script also built the examples (under examples\build), which should also run fine

Subsequent builts can be completed with the normal Visual Studio workflow:
  * Using explorer to open ALL_BUILD.vcxproj. Visual Studio will open. With ALL_BUILD highlighted in the Solution Explorer window, select the desired build mode from the dropdown menu (Release or Debug), then select BUILD->Build Solution from the menu.
  * If developing the CMakeLists.txt file, or the build script, use the command-line argument flags to skip rebuilding dependencies:
    *
	./docs/build_instructions/windows/download_and_build.sh --skip-build-boost --skip-download-install-dependencies

#### 2. Linking against dependencies all compiled from source

Rather than being limited by the third-party dependencies' binaries, we can compile them ourselves.

To build LibHand, and to build (and link against) Boost and OGRE, we can invoke the same build helper script with different arguments.

1. Follow the instructions from Section 1 above to download the binary dependencies we'll be using.
2. In addition to those dependencies, Mercurial Software Configuration Management is also required (to download OGRE source code)
  * 
	choco install -y hg

3. Continue with the steps, but invoke the build script with the '--build-ogre-mode' flag:
  *
  	# The following command may many tens of minutes to build all the dependencies, including Boost.
	./docs/build_instructions/windows/download_and_build.sh --build-ogre-mode
  
  * If the script completed successfully, continue to follow the rest of the instructions

#### 3. Building LibHand on Windows 10 using Windows Subsystem For Linux (WSL)

1. Enable [developer mode](https://msdn.microsoft.com/en-au/commandline/wsl/install_guide)
  * Search for 'developer mode' in Start Menu open the 'Use Developer Features' settings panel and enable Developer m
ode
  * Search 'Windows features' in Start Menu to go to 'Turn Windows features on or off' control panel 
    * Enable 'Windows Subsystem for Linux (Beta)' feature and restart your PC.
2. From Start Menu, open Bash.exe and follow the initial prompts to create a new user with default configurations
3. At this point, you can follow the Debian/Ubuntu LibHand instruction unmodified.
4. After compilation has succeeded (and the export paths are setup!) we are nearly ready to start
5. Download, install and run Xming X Server for Windows from [Sourceforge](https://sourceforge.net/projects/xming/files/Xming/6.9.0.31/Xming-6-9-0-31-setup.exe/download)
6. Launch Pose Designer from Bash.exe with `DISPLAY=:0 ./pose_designer`, rather than simply `./pose_designer`

If all went well, Pose Designer should display a 3D rendered hand inside the XMing window.




