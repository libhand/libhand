# This is a config file for install_ogre_static.sh
# It should be in the same directory with install_ogre_static.sh

DOWNLOAD_URL="http://downloads.sourceforge.net/project/ogre/ogre/1.7/ogre_src_v1-7-3.tar.bz2"
ARCHIVE_FILE="ogre_src_v1-7-3.tar.bz2"
UNPACKED_DIR="ogre_src_v1-7-3"
CMAKE_FLAGS="-DOGRE_STATIC=ON -DOGRE_CONFIG_THREAD_PROVIDER=tbb -DOGRE_INSTALL_SAMPLES=TRUE"
MAKE_FLAGS="-j2"
MAKE_INSTALL_FLAGS=""
