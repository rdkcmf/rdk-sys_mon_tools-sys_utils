#!/bin/bash
##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

#######################################
#
# Build Framework standard script for
#
# Sysint component

# use -e to fail on any shell issue
# -e is the requirement from Build Framework
set -e


# default PATHs - use `man readlink` for more info
# the path to combined build
export BUILD_PATH=${BUILD_PATH-`readlink -m ..`}
export COMBINED_ROOT=$BUILD_PATH

# path to build script (this script)
export SCRIPTS_PATH=${SCRIPTS_PATH-`readlink -m $0 | xargs dirname`}

# path to components sources and target
export SOURCE_PATH=${SOURCE_PATH-`readlink -m .`}
export TARGET_PATH=${TARGET_PATH-$SOURCE_PATH}

# default component name
export COMPONENT_NAME=${COMPONENT_NAME-`basename $SOURCE_PATH`}
export OPENSOURCE_LIB="$RDK_PROJECT_ROOT_PATH/opensource/lib"
export OPENSRC=$RDK_PROJECT_ROOT_PATH/opensource

export FSROOT=${RDK_FSROOT_PATH}

if [ "x$RDK_PLATFORM_SOC" == "xintel" ]; then
    export CROSS_TOOLCHAIN="$RDK_PROJECT_ROOT_PATH/sdk/toolchain/staging_dir"
    export CROSS_COMPILE="$CROSS_TOOLCHAIN/bin/i686-cm-linux"
    export ADDED_LDFLAGS="-L$OPENSOURCE_LIB"
elif [ "x$RDK_PLATFORM_SOC" == "xbroadcom" ]; then
    export COMCAST_PLATFORM=${RDK_PLATFORM_DEVICE^^}
    export CROSS_TOOLCHAIN="$RDK_PROJECT_ROOT_PATH/sdk/toolchain/staging_dir"
    export WORK_DIR=$RDK_PROJECT_ROOT_PATH/work$COMCAST_PLATFORM
    export MFR_FPD_PATH="$WORK_DIR/svn/sdk/mfrlib/include"
    export SDK_FSROOT="$WORK_DIR/rootfs"

    if [ -f $WORK_DIR/svn/pace_rng150/build_scripts/setBcmEnv.sh ]; then
       . $WORK_DIR/svn/pace_rng150/build_scripts/setBcmEnv.sh
    fi

    if [ -f $RDK_PROJECT_ROOT_PATH/build_scripts/setBCMenv.sh ]; then
       . $RDK_PROJECT_ROOT_PATH/build_scripts/setBCMenv.sh
    fi

    export GLIB_LIBRARY_PATH=$APPLIBS_TARGET_DIR/usr/local/lib/
    export ADDED_LDFLAGS="-L$OPENSOURCE_LIB -L$SDK_FSROOT/lib -L$SDK_FSROOT/usr/lib -L$SDK_FSROOT/usr/local/lib -L$GLIB_LIBRARY_PATH -lintl -ldl"

    export CROSS_COMPILE=mipsel-linux
fi


# parse arguments
INITIAL_ARGS=$@

function usage()
{
    set +x
    echo "Usage: `basename $0` [-h|--help] [-v|--verbose] [action]"
    echo "    -h    --help                  : this help"
    echo "    -v    --verbose               : verbose output"
    echo
    echo "Supported actions:"
    echo "      clean, install"
}

# options may be followed by one colon to indicate they have a required argument
if ! GETOPT=$(getopt -n "build.sh" -o hv -l help,verbose -- "$@")
then
    usage
    exit 1
fi

eval set -- "$GETOPT"

while true; do
  case "$1" in
    -h | --help ) usage; exit 0 ;;
    -v | --verbose ) set -x ;;
    -- ) shift; break;;
    * ) break;;
  esac
  shift
done

ARGS=$@


# component-specific vars



# functional modules

function configure()
{
    true #use this function to perform any pre-build configuration
}

function clean()
{
    cd $RDK_SOURCE_PATH
    make clean
}

function build()
{
    cd $RDK_SOURCE_PATH
    make all
}

function rebuild()
{
    clean
    build
}

function install()
{
   cd $RDK_SOURCE_PATH
   make install

}


# run the logic

#these args are what left untouched after parse_args
HIT=false

for i in "$ARGS"; do
    case $i in
        configure)  HIT=true; configure ;;
        clean)      HIT=true; clean ;;
        build)      HIT=true; build ;;
        rebuild)    HIT=true; rebuild ;;
        install)    HIT=true; install ;;
        *)
            #skip unknown
        ;;
    esac
done

# if not HIT do build by default
if ! $HIT; then
  build
fi

