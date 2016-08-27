#!/bin/bash

# See BUILD.WINDOWS.md for more information -- be sure to install the relevant dependencies listed in BUILD.WINDOWS.md!
#
# This script is based on http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Building+Ogre+Dummy+Guide+MinGW
# which provides step by step instructions on how to build OGRE and dependencies against MinGW with several Batch
# scripts.
#
# To assist in cross-platform compatibility, the version of dependencies such as Boost or OGRE3D are currently
# selected for LibHand are based on the highest common version between Debian/Ubuntu and macOS.
#
# Given LibHand is primarily hosted on GitHub, it's expected all Windows users will have Git For Windows installed,
# and given Bash with common UNIX utilities are far more flexible and powerful that Windows Batch file, allowing OGRE
# and OGRE dependencies to be compiled in a fully automated manner. The downside is there's a bit of subtle fiddliness
# dealing with shell scripts on Windows (particularly invoking cmd.exe for Visual Studio compilation)
#
# For maximum compatiblility this script may only use the utilities provided with Git for Windows (curl, unzip etc),
# and Windows built-ins like Command Line (cmd.exe).

# Exit script if there is a single error (-e), among other things 
set -eu -o pipefail
DOWNLOAD_CACHE=$(echo `pwd`/download_cache)
mkdir -p ${DOWNLOAD_CACHE}
TARGET_DEPENDENCY_DIR="third_party"
BOOST_SRC=boost_1_58_0
TARGET_BUILD_DIR=build

CMAKE_BINARY="cmake.exe"

function usage {
	echo " Helper script to build LibHand on Windows"
	echo " See BUILD.WINDOWS.md for more information -- be sure to install the relevant dependencies listed in BUILD.WINDOWS.md!"
	echo " Usage (from top-level LibHand directory!):"
	echo "./download_and_build [--build-ogre-mode] [--skip-build-boost] [--skip-download-install-dependencies] [--skip-build-ogre] [--skip-build-libhand] [--skip-build-libhand-examples]"

}

BUILD_OGRE_MODE=false
SKIP_BUILD_BOOST=false
SKIP_DOWNLOAD_INSTALL_DEPENDENCIES=false
SKIP_BUILD_OGRE=false
SKIP_BUILD_LIBHAND=false
SKIP_BUILD_LIBHAND_EXAMPLES=false
GETOPT=`getopt -o "" --long build-ogre-mode,skip-build-boost,skip-download-install-dependencies,skip-build-ogre,skip-build-libhand,skip-build-libhand-examples,help,usage -n 'download_and_build.sh' -- "$@"`
eval set -- "$GETOPT"

while true; do
	case "$1" in
		--build-ogre-mode)
			BUILD_OGRE_MODE=true; shift;;
		--skip-build-boost)
			SKIP_BUILD_BOOST=true; shift;;
		--skip-download-install-dependencies)
			SKIP_DOWNLOAD_INSTALL_DEPENDENCIES=true; shift;;
		--skip-build-ogre)
			SKIP_BUILD_OGRE=true; shift;;
		--skip-build-libhand)
			SKIP_BUILD_LIBHAND=true; shift;;
		--skip-build-libhand-examples)
			SKIP_BUILD_LIBHAND_EXAMPLES=true; shift;;
		--help)
			usage; exit 0;;
		--usage)
			usage; exit 0;;
		--) shift; break;;
		*) echo "Error parsing arguments"; exit 1;
	esac
done

# Download with resume
function download {
	echo "Downloading $1 (if it doesn't already exist) into ${DOWNLOAD_CACHE}"
	pushd ${DOWNLOAD_CACHE}
	echo "Disabling exit on non-zero error code temporarily"
	set +e
	curl -L -O -C - "$2"
	exit_status=$?
	if [ $exit_status -eq 33 ]; then
		echo "curl reports the HTTP server doesn't seem to support byte ranges, so cannot resume. Retrying download from the start"
		curl -L -O "$2"
	elif [ $exit_status -ne 0 ]; then
		echo "curl exited with error code $?" 
		exit "$?"
	fi
	echo "Re-enabling exit on non-zero error code"
	set -e
	popd
}

function mercurial_clone {
		if [ ! -d "$1" ]; then
			echo "Cloning repository at $3 into $1. This may take a while."
			hg clone "$3" "$1"
			pushd "$1" 
			echo "Checking out $2 branch"
			hg update $2
			popd
		else
			echo "Directory named '$1' exists, not cloning"
		fi
}

function run_cmake {
	eval ${CMAKE_BINARY} -G'Visual\ Studio\ 12\ 2013' "$@"
}

function download_dependencies {
	download "DirectX dependency" "https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe"


	echo "Downloading Boost dependency"
	download Boost "http://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.zip"

	echo "Unzipping boost"
	unzip -o ${DOWNLOAD_CACHE}/boost_1_58_0.zip

	mercurial_clone ogre-deps default https://bitbucket.org/cabalistic/ogredeps/downloads
	mercurial_clone ogre v1-9 http://bitbucket.org/sinbad/ogre/
}

function install_dependencies {
	./${DOWNLOAD_CACHE}/DXSDK_Jun10.exe /U
}

# FIXME: Executables currently need DLLs in the local folder. Setup proper loading paths
function deploy_dependencies {
	# Copy OGRE DLL files, until plugin.cfg specifies the path, this is the best way.
	# FIXME: After plugin.cfg, this will be obselete and can be removed
	# Double quotes on each operand get converted to single quotes by Bash for Windows for some reason (so can't have spaces in the variables)
	cp ${TARGET_DEPENDENCY_DIR}/OgreSDK_vc11_v1-9-0/bin/$1/* "$2"

	# Copy OpenCV runtime dependencies
	# FIXME: Far more preferable to correctly set PATH environment variable, but this works for now
	cp C:/OpenCV249/opencv/build/x86/vc12/bin/* "$2"
}

function build_boost {
	echo "Preparing to build boost"
	pushd ${BOOST_SRC}
	# We the build in a single session of CMD, to retain changes to environment 
	# We add an extra forward slash to prevent arguments such as /c being automatically converted to a UNIX style path by Git for Windows' modified MSYS layer (lookup MSYS_NO_PATHCONV for more information)
	cmd //c "bootstrap.bat gcc & b2 install --prefix=build & b2 --build-dir=build toolset=gcc --build-type=complete stage"
	popd
}

function build_ogre {
	pushd ogre-deps
	mkdir -p ${TARGET_BUILD_DIR}

	# Side note: To avoid compatibility issues MinGW make using CMD (and also Microsoft Visual C Runtime), is named different to MSYS make using a UNIX shell (and POSIX via MSYS runtime)
	cp $(where mingw32-make.exe) ./make.exe

	# FindDirectX requires CMake 3.4.0 or higher
	run_cmake -G "Unix Makefiles" ..
	make
	popd
}

function download_deps_binary_ogre_mode {
	echo "Downloading Boost dependency"
	download Boost "http://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.zip"

	echo "Unzipping boost"
	unzip -o ${DOWNLOAD_CACHE}/boost_1_58_0.zip

	# This should be integrated into Chocolately
	echo "Downloading pre-compiled OGRE 1.9 SDK for Visual C++ 2012 (32-bit)"
	download OGRE "https://sourceforge.net/projects/ogre/files/ogre/1.9/1.9/OgreSDK_vc11_v1-9-0.exe"
}

function build_boost_binary_ogre_mode {
	echo "Preparing to build boost... Consider increasing/decreasing the number of threads with the -jY parameter proportional to number of CPU cores"
	pushd ${BOOST_SRC}
	# We do all the build in a single session of CMD, to retain changes to environment
	# Write a temporary batch file to avoid nested quote issues	
	echo '"%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86 & bootstrap.bat & b2 -j8 toolset=msvc-12.0 address-model=32 architecture=x86 link=static threading=multi runtime-link=shared --build-type=complete stage --stagedir=stage/win32'  > build_boost_binary_ogre_mode.bat
	cmd //c "build_boost_binary_ogre_mode.bat"
	popd
}

function install_deps_binary_ogre_mode {
	# The OGRE archive (7zip self-extractor) contains a folderr with the same name as the executable
	${DOWNLOAD_CACHE}/OgreSDK_vc11_v1-9-0.exe -o"./" -y
}

function build_libhand {
	rm -rf ${TARGET_BUILD_DIR}
	mkdir -p ${TARGET_BUILD_DIR}
	pushd ${TARGET_BUILD_DIR}
	run_cmake ..

	# Buffer the command in a batchfile to prevent quoting issues
	# Append execution of the INSTALL target (placing libraries in /dist).
	# FIXME: Only Release build being deployed, as using only a single /dist target directory
	echo '"%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86 ^
	& msbuild ALL_BUILD.vcxproj /p:configuration=debug /p:platform=win32 ^
	& msbuild ALL_BUILD.vcxproj /p:configuration=Release /p:platform=win32 ^
	& msbuild INSTALL.vcxproj /p:configuration=Release /p:platform=win32' >> build_libhand.bat

	# FIXME: Use OGRE plugin.cfg to fix running the Debug version
	cmd //c "build_libhand.bat"
#	rm build_libhand.bat
	popd

	deploy_dependencies Release "${TARGET_BUILD_DIR}/hand_cpp/source/Release"
	deploy_dependencies debug "${TARGET_BUILD_DIR}/hand_cpp/source/debug"
}


function build_libhand_examples {
	pushd examples
	rm -rf ${TARGET_BUILD_DIR}
	mkdir -p ${TARGET_BUILD_DIR}
	pushd ${TARGET_BUILD_DIR}
	run_cmake ..

	# Buffer the command in a batchfile to prevent quoting issues
	# FIXME: Only building Release build due to single /dist target directory at the moment
	echo '"%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86 ^
	& msbuild ALL_BUILD.vcxproj /p:configuration=Release /p:platform=win32' > build_libhand_examples.bat
	echo "WARNING: Script currently does not support automated building of examples"
	# FIXME: Use OGRE plugin.cfg to fix running the Debug version
	# FIXME: Re-enable building LibHand examples after fixing linking error. Static lib on filesystem has version num, but VS looking for slightly different name.
	cmd //c "build_libhand_examples.bat"
	rm build_libhand_examples.bat

	popd
	popd

	deploy_dependencies Release "examples/${TARGET_BUILD_DIR}/Release"
	cp dist/* "examples/${TARGET_BUILD_DIR}/Release"
}


function binary_ogre_mode {
	mkdir -p ${TARGET_DEPENDENCY_DIR}
	pushd ${TARGET_DEPENDENCY_DIR}
	if [ "$SKIP_DOWNLOAD_INSTALL_DEPENDENCIES" = false ]; then
		download_deps_binary_ogre_mode
		install_deps_binary_ogre_mode 
	fi
	if [ "$SKIP_BUILD_BOOST" = false ]; then
		build_boost_binary_ogre_mode
	fi
	popd
	if [ "$SKIP_BUILD_LIBHAND" = false ]; then
		build_libhand
	fi
	if [ "$SKIP_BUILD_LIBHAND_EXAMPLES" = false ]; then
		build_libhand_examples
	fi
}


function build_ogre_mode {
	mkdir -p ${TARGET_DEPENDENCY_DIR}
	pushd ${TARGET_DEPENDENCY_DIR}
	if [ "$SKIP_DOWNLOAD_INSTALL_DEPENDENCIES" = false ]; then
		download_dependencies
		install_dependencies
	fi
	if [ "$SKIP_BUILD_BOOST" = false ]; then
		build_boost
	fi
	if [ "$SKIP_BUILD_OGRE" = false ]; then
		build_ogre
	fi
	popd
	if [ "$SKIP_BUILD_LIBHAND" = false ]; then
		build_libhand
	fi
	if [ "$SKIP_BUILD_LIBHAND_EXAMPLES" = false ]; then
		build_libhand_examples
	fi
}


if [ "$BUILD_OGRE_MODE" = true ]; then
	echo "BUILD_OGRE_MODE is enabled. Building OGRE from source"
	build_ogre_mode
else
	echo "BUILD_OGRE_MODE is disabled. Using select pre-compiled dependencies"
	binary_ogre_mode
fi
