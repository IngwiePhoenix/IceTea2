#!/bin/sh -x

if [ "x$CC" = "x" ]
then
    for tool in clang gcc icc pcc tcc
    do
        which $tool >/dev/null
        if [ $? = 0 ]
        then
            CC=$tool
            break
        fi
    done
fi

if [ "x$CXX" = "x" ]
then
    for tool in "clang++" clang "g++" gcc icc pcc tcc
    do
        which $tool >/dev/null
        if [ $? = 0 ]
        then
            CXX=$tool
            break
        fi
    done
fi

which ar >/dev/null
[ ! $? = 0 ] && echo "! The ar tool is missing." && exit 1
AR=ar

ICETEA_ROOT=$(dirname "$0")/..
ICETEA_FLAVOR=unix
ICETEA_PATHSEP=/

mkdir $ICETEA_ROOT/build.bootstrap

$CC -DMJS_MAIN -I $ICETEA_ROOT/third_party/mjs $ICETEA_ROOT/third_party/mjs/mjs.c -o $ICETEA_ROOT/build.bootstrap/mjs
$ICETEA_ROOT/build.bootstrap/mjs $(dirname "$0")/build.mjs