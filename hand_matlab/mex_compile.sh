#!/bin/bash

MATLAB_VERSIONS=$( echo \
"2011b" \
"2011a" \
"2010b" \
"2010a" )

MATLAB_DIRPATS=$( echo \
"/usr/local/MATLAB/R@VER@" \
"/Applications/MATLAB_R@VER@.app" )

# These path is relative to the source directory
CPP_CODE_PATH="../../hand_cpp"

# These strict warning flags are there to assist development
# A subsequent line disables them. You can re-enable for development
MEX_WARNING_FLAGS=$( echo "-Wall -Werror=address -Werror=char-subscripts" \
    "-Werror=comment -Werror=format -Werror=missing-include-dirs" \
    "-Werror=parentheses -Werror=reorder -Werror=return-type" \
    "-Werror=sequence-point -Werror=sign-compare -Werror=strict-aliasing" \
    "-Werror=switch -Werror=trigraphs -Werror=uninitialized" \
    "-Werror=unknown-pragmas -Werror=unused-function -Werror=unused-variable" \
    "-Werror=volatile-register-var" )

# Comment the following line if you want the above flags to work
MEX_WARNING_FLAGS=""

# This file path is relative to the source directory
CUSTOM_MEXOPTS="./custom_mexopts.sh"

function info() {
    echo "mex_compile.sh: $*" >/dev/stderr
}

function error() {
    echo "mex_compile.sh: Error: $*" >/dev/stderr

    return 0
}

function interror() {
    error "$1: internal error"

    return 0
}

function find_mex_bin() {
    local version dirpat testdir

    for version in ${MATLAB_VERSIONS}; do
	for dirpat in ${MATLAB_DIRPATS}; do
	    testdir=${dirpat//@VER@/$version}
	    testmexbin="${testdir}/bin/mex"

	    info "Looking for mex in: $testmexbin"

	    if [ -e "$testmexbin" ]; then
		echo "$testmexbin"
		info "Found mex in $testmexbin"
		return 0
	    fi
	done
    done
    return 1
}

function find_mexopts_sh() {
    [[ x"$MEX" == x"" ]] \
	&& error "find_mexopts_sh: mex not found" && return 1

    local mexdir=$( dirname "$MEX" )
    local mexopts_file="${mexdir}/mexopts.sh"

    if ! [ -e "${mexopts_file}" ]; then
	error "Couldn't find mexopts.sh at ${mexopts_file}!!!"
	return 1
    fi
    
    echo "${mexopts_file}"
    return 0
}

function invoke_cmake_with_mode() {
    local cur_exitcode
    local cmake_out

    [[ "$1" == "" ]] \
	&& interror "invoke_cmake_with_mode (empty param list)" && exit 1

    ( cd source && rm -rf CMakeFiles )
    cmake_out=$( cd source && cmake \
	--find-package \
	-DCMAKE_MODULE_PATH=${CPP_CODE_PATH}/dist \
	-DNAME=LibHand \
	-DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=$1 )
    cur_exitcode=$?
    ( cd source && rm -rf CMakeFiles )

    if (( cur_exitcode )); then
	error "Bad CMake output: ${cmake_out}"
	echo "ERROR"
    else
	echo "${cmake_out}"
    fi

    return ${cur_exitcode}
}

function emit_mexopts() {
    info "Generating ${CUSTOM_MEXOPTS} ..."
    cat "${MEXOPTS_SH}" >"${CUSTOM_MEXOPTS}"

    local c_cxx_flags=$( echo \
	"${MEX_WARNING_FLAGS}" \
	"${MEX_COMPILE_INCLUDE_FLAGS}" \
	)

    echo 'CFLAGS="$CFLAGS '"${c_cxx_flags}"'"' \
	>>"${CUSTOM_MEXOPTS}"
    echo 'CXXFLAGS="$CXXFLAGS '"${c_cxx_flags}"'"' \
	>>"${CUSTOM_MEXOPTS}"
    echo 'CLIBS="$CLIBS '"${MEX_COMPILE_LINK_FLAGS}"'"' \
	>>"${CUSTOM_MEXOPTS}"
    echo 'CXXLIBS="$CXXLIBS '"${MEX_COMPILE_LINK_FLAGS}"'"' \
	>>"${CUSTOM_MEXOPTS}"
}

function mex_compile() {
    [[ x"$1" == x"" ]] && interror "mex_compile (empty param list)" && exit 1
    local main_file="$1"
    shift

    local auxfile_msg=""
    [[ x"$*" != x"" ]] && auxfile_msg="[auxilliary files: $*]"

    info "Mex compile of ${main_file}... ${auxfile_msg}"

    if ! $MEX -v -f ${CUSTOM_MEXOPTS} -largeArrayDims ${main_file} $*; then
	error "[FAIL] Compiling ${main_file} failed!"
	return 1
    fi

    info "[OK] Compiling ${main_file} successful. ${auxfile_msg}"
    return 0
}

function do_compile() {
    MEX=$( find_mex_bin )

    if [[ x"$MEX" == x"" ]]; then
	error "mex_compile.sh: Could not find mex, check" \
	    "MATLAB_VERSIONS and MATLAB_DIRPATS in this script"
	exit 1
    fi

    MEXOPTS_SH=$( find_mexopts_sh )

    if [[ x"${MEXOPTS_SH}" == x"" ]]; then
	error "mex_compile.sh: Could not find mexopts.sh!"
	exit 1
    fi

    info "Gathering compile and link flags from the cmake project.."

    HANDHOGDEPS_INCLUDE_FLAGS=$( invoke_cmake_with_mode COMPILE )
    HANDHOGDEPS_LINK_FLAGS=$( invoke_cmake_with_mode LINK )

    if [[ x"${HANDHOGDEPS_INCLUDE_FLAGS}" == x"ERROR" ]] \
	|| [[ x"${HANDHOGDEPS_LINK_FLAGS}" == x"ERROR" ]]; then
	error "mex_compile.sh: cmake did not run properly!"
	exit 1
    fi

    MEX_COMPILE_INCLUDE_FLAGS="-I${CPP_CODE_PATH}/source ${HANDHOGDEPS_INCLUDE_FLAGS}"
    MEX_COMPILE_LINK_FLAGS="-L/opt/local/lib ${HANDHOGDEPS_LINK_FLAGS}"

    info "--------------------------------------------------"
    info "MEX_COMPILE_INCLUDE_FLAGS=${MEX_COMPILE_INCLUDE_FLAGS}"
    info "MEX_COMPILE_LINK_FLAGS=${MEX_COMPILE_LINK_FLAGS}"
    info "MEX=$MEX"
    info "MEXOPTS_SH=${MEXOPTS_SH}"
    info "--------------------------------------------------"

    rm -f source/hand_system.mex*

    (
	cd source && \
	    emit_mexopts && \
	    mex_compile hand_system.cc hand_system_data.cc marin_mexutil.cc
    )

    cp source/hand_system.mex* dist/
}

do_clean() {
    rm -f source/hand_system.mex*
    rm -f dist/hand_system.mex*
    rm -f source/${CUSTOM_MEXOPTS}
}

option="$1"

case "$option" in
    build) do_compile ;;
    clean) do_clean ;;
    "") do_compile ;;
    *) error "Unknown option $option"; exit 1; ;;
esac
