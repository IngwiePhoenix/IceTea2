#!/bin/bash

CC=$CC
CXX=$CXX

checkForCompiler() {
  # @var $1: Variable name
  # @var $2: check for these.
  echo -n "checking for $1 ... "
  if [ -z "${!1}" ]; then
    # Find a suitable compiler...
    for i in $2; do
      compiler="$(which $i)"
      if [ $? == 0 ]; then
        eval "$1=\"$compiler\""
        break
      fi
    done
    if [ -z "${!1}" ]; then
      echo
      echo "ERROR: You don't seem to have a C/C++ compiler..."
      echo "       Try again when you have one, or set it"
      echo "       via the $1 environment variable."
      exit 1
    fi
    echo "OK (${!1})"
  else
    which "${!1}" >/dev/null 2>/dev/null
    if [ $? == 0 ]; then
      echo "OK (${!1})"
    else
      echo "OK (${!1}): Could not access program as the command was not found."
    fi
  fi
}

# Find compilers. We also look for C++, because we can and because we
# actually need one later on anyway. :)
checkForCompiler CC "gcc clang"
checkForCompiler CXX "g++ clang++"

# TODO: This is not optimal... Re-building every time sounds a little bit
# too overkill for me. It's okay for a tiny program like this, but still...
# I believe the original cDetect's configure.sh solved this better. But, I
# am not sure if I can really adopt it.
$CC -I deps/cdetect build.c -o build.bin
chmod +x build.bin
./build.bin $*
