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
    echo "   ACTION -- build|rebuild|clean|test|tidy|install"
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
	config="RelWithDebInfo"
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
		;;

    Darwin)
		cmakeGenerator="Unix Makefiles"
		cmakeOutput=$builddir/Makefile
		;;

    *)
	echo "Unknown OS"
	exit 1;
esac

runTarget() {
    if [ $# -eq 2 ]; then
	if [ "$2" = "true" ]; then
	    sudo make $1
	    return
	fi
    fi
    make $1
}

doClean() {
    rm -fr "$builddir"
}

doBuild() {
    mkdir -p "$builddir"
    cd "$builddir"
    cmake -b "$rootdir/cmake" -G "$cmakeGenerator" -DCMAKE_BUILD_TYPE:STRING=$config
    runTarget all
}

doTest() {
	"$builddir/unittest/unittest"
}

doInstall() {
    runTarget install true
}

case "$action" in
	clean)
		doClean
		;;
	build)
		doBuild
		;;
	rebuild)
		doClean
		doBuild
		;;
	test)
		doBuild
		doTest
		;;
	tidy)
		export CLANG_TIDY=yes
		doBuild
		;;
	install)
		doBuild
		doInstall
		;;
	*)
		usage;
esac
