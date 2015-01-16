#!/usr/bin/env bash
#
# Build everything.
#
# Args
#  action -- build|clean
#  flavor -- debug|release
set -euo pipefail

usage() {
    echo "Usage: $0 ACTION FLAVOR"
    echo "   ACTION -- build|clean"
    echo "   FLAVOR -- debug|release"
    exit 1
}

test $# -ne 2 && usage

action=$1
flavor=$2

case "$flavor" in
    "debug")
	config="Debug"
	;;
    "release")
	config="Release"
	;;
    *)
	usage
esac

rootdir=$(cd $(dirname $BASH_SOURCE) && echo $PWD)
builddir=$rootdir/build/$flavor

platform=$(uname)
case "$platform" in
    Linux)
	cmakeGenerator="Unix Makefiles"
	cmakeOutput=$builddir/Makefile

	runTarget() {
	    make $1
	}

	;;
    Darwin)
	;;
    *)
	echo "Unknown OS"
	exit 1;
esac

if [ "$action" = "build" -o "$action" = "install" ]; then
    mkdir -p "$builddir"
    cd "$builddir"
    #test -e "$cmakeOutput" || 
    cmake -b "$rootdir/cmake" -G "$cmakeGenerator" -DCMAKE_BUILD_TYPE:STRING=$config
    runTarget all
elif [ "$action" = "clean" ]; then
    rm -fr "$builddir"
else
    usage
fi

if [ "$action" = "install" ]; then
    runTarget install
fi
