#!/bin/bash

instopt=$1

info() {
    echo "ogre_install.sh: $*" >/dev/stderr
}

error() {
    info "ERROR: $*"
}

interror() {
    info "Internal ERROR: $*"
}

asterisks() {
    local a="**********"
    local l="$a$a$a$a$a$a$a"

    echo "$l" >/dev/stderr
}

. install_config.sh || \
    ( error "Cannot find install_config.sh! Exiting!"
    exit 1 )

has_cmd() {
    [[ x"$1" == x"" ]] && interror "has_cmd" && return 1
    local cmd=$1
    local cmd_path=$( which "$cmd" )

    [[ x"${cmd_path}" != x"" ]]
}

download_from_url() {
    [[ x"$1" == x"" ]] && interror "download_from_url" && return 1

    local url="$1"

    if has_cmd wget; then
	info "Found wget for download"
	asterisks
	if ! wget "$url"; then
	    asterisks
	    error "Download failed!"
	    return 1
	fi
    elif has_cmd curl; then
	info "Found curl for download"
	if ! curl -O "$url"; then
	    asterisks
	    error "Download failed!"
	    return 1
	fi
    else
	error "No wget or curl found :("
	return 1
    fi

    asterisks
    info "Download complete"

    if ! has_cmd bzip2; then
	error "No bunzip2 present :("
	return 1
    fi

    info "Unpacking..."
    if ! ( bzip2 -cd "${ARCHIVE_FILE}" | tar xf - ); then
	asterisks
	error "Extracting [${ARCHIVE_FILE}] failed!"
	return 1
    fi

    asterisks
    info "Download and unpack successful"
    return 0
}

help() {
    cat <<"EOF"

install_ogre_static.sh
  Installs a static distribution of Ogre v1.7.3

Usage:
  cd into a directory where you want to hold the ogre source

  Run the following commands

  ./install_ogre_static.sh download
  ./install_ogre_static.sh build
  sudo ./install_ogre_static.sh install

  If everything goes without problems, you now have a static install
  of Ogre v1.7.3

  You can do:
  ./install_ogre_static.sh clean

  to clean up this directory
EOF
}

build_ogre() {
    (
	if ! cd "${UNPACKED_DIR}"; then
	    error "Cannot find directory [${UNPACKED_DIR}]"
	    return 1
	fi

	asterisks
	info "Applying patches"
	asterisks

	if ! patch -p0 <../patches/FindOGRE.cmake.patch; then
	    error "Patch unsuccessful!"
	    info "Press Y or y to continue"
	    read -e ans
	    if ! [[ x"$ans" == x"Y" || x"$ans" == x"y" ]]; then
		error "Giving up because of a failed patch"
		return 1
	    fi
	else
	    info "Patching OK"
	fi

	asterisks
	info "Setting up a build directory"
	asterisks
	mkdir -p build
	rm -rf build/*

	asterisks
	info "Running cmake"
	asterisks
	cd build
	if ! cmake .. ${CMAKE_FLAGS}; then
	    error "cmake failed!!"
	    exit 1
	fi
	asterisks
	asterisks
	info
	info "Building..."
	info
	asterisks
	if ! make ${MAKE_FLAGS}; then
	    asterisks
	    error "Running make failed!!"
	    return 1
	fi
    ) || return 1

    asterisks
    info "Build successful"
    return 0
}

do_make_install() {
    asterisks
    info "Installing..."
    asterisks
    (
	if ! cd "${UNPACKED_DIR}/build"; then
	    error "Cannot find build directory [${UNPACKED_DIR}/build]"
	    return 1
	fi

	if ! make install ${MAKE_INSTALL_FLAGS}; then
	    asterisks
	    error "Make install failed! Check permissions?"
	    return 1
	fi
    ) || return 1

    asterisks
    info "Install complete."
}

do_clean() {
    asterisks
    info "Cleaning..."
    asterisks

    if ! rm -f "${ARCHIVE_FILE}"; then
	error "Deleting [${ARCHIVE_FILE}] failed!"
	return 1
    fi

    if [[ x"${UNPACKED_DIR}" != x"" ]] \
	&& [ -e "${UNPACKED_DIR}" ]; then
	info "Wiping [${UNPACKED_DIR}]..."
	if ! rm -rf "${UNPACKED_DIR}"; then
	    error "Wiping [${UNPACKED_DIR}] failed!"
	    return 1
	fi
    else
	info "FYI, [${UNPACKED_DIR}] doesn't exist..."
    fi

    return 0
}

case ${instopt} in
    download)
	if ! download_from_url "${DOWNLOAD_URL}"; then
	    error "Download process failed"
	    exit 1
	fi
	;;

    build)
	if ! build_ogre; then
	    error "OGRE build failed!"
	    exit 1
	fi
	;;

    install)
	if ! do_make_install; then
	    error "OGRE install failed!"
	    exit 1
	fi
	;;

    clean)
	if ! do_clean; then
	    error "Cleaning up failed?"
	    exit 1
	fi
	;;

    --help | -h | -help)
	help
	exit 0
	;;
    *)
	help
	exit 1
	;;
esac

exit 0