@echo off
REM This is by far not completed...

REM Compilers that (i know of) may exist within %PATH%
set COMPILERS=("clang-cl" "clang" "gcc")

REM Those are neccessary for building.
set CC:="not-set"
set CXX:="not-set"

REM To find a C/C++ compiler, we'll do:
REM - Search for MSVC and inherit it's variables
REM - Look in %PATH% for one of the compilers
REM If this fails, we exit and tell the user to please install either of them.
