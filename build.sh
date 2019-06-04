#!/usr/bin/env bash
#
# Build everything.
#
# Args
#  action -- build|rebuild|clean|test|tidy|install
#  flavor -- debug|release
#  target -- a make target name
#
# Environment
#  TC_MAX_PROCS -- number of make jobs to run simultaneously
#  TC_BUILDDIR  -- location of build artifacts
#
set -euo pipefail

usage() {
    echo "Usage: $0 ACTION FLAVOR [TARGET]"
    echo "   ACTION -- build|rebuild|clean|test|tidy|install"
    echo "   FLAVOR -- debug|release"
    echo "   TARGET -- name of a build target"
    exit 1
}

case $# in
    2)
        ;;
    3)
        target=$3
        ;;
    *)
        usage
esac

action=$1
flavor=$2
target=all

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
builddir=${TC_BUILDDIR-$rootdir/build/$flavor}

platform=$(uname)
case "$platform" in
    Linux)
        cmakeGenerator="Unix Makefiles"
        cmakeOutput=$builddir/Makefile
        defaultMaxProcs=1
        ;;

    Darwin)
        cmakeGenerator="Unix Makefiles"
        cmakeOutput=$builddir/Makefile
        defaultMaxProcs=$(sysctl -n hw.ncpu)
        ;;

    *)
        echo "Unknown OS"
        exit 1;
esac

runTarget() {
    jobs=${TC_MAX_PROCS-$defaultMaxProcs}
    make -j "${jobs}" $@
}

doClean() {
    rm -fr "$builddir"
}

doBuild() {
    mkdir -p "$builddir"
    cd "$builddir"

    if [ ! -f "${cmakeOutput}" ]; then
        cmake -b "$rootdir/cmake" -G "$cmakeGenerator" -DCMAKE_BUILD_TYPE:STRING=$config
    fi

    runTarget $@
}

doTest() {
    "$builddir/unittest/unittest"
}

doInstall() {
    runTarget install
}

case "$action" in
    clean)
        doClean
        ;;
    build)
        doBuild "${target}"
        ;;
    rebuild)
        doClean
        doBuild "${target}"
        ;;
    test)
        doBuild
        doTest
        ;;
    tidy)
        export CLANG_TIDY=yes
        doBuild "${target}"
        ;;
    install)
        doBuild "${target}"
        doInstall
        ;;
    *)
        usage;
esac
