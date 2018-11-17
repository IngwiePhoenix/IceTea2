/**
 * @file
 * This file is the initial build script for running pre-tests and building
 * IceTea very, very manually. However, it isn't hard to build IceTea, really.
 * However, in order to automate things across platforms, I use cDetect here
 * to accomplish this. It just makes things a little easier...
 *
 * cDetect is actually what inspired the Detector API in IceTea.
 *
 * @author Kevin Ingwersen <ingwie2000@gmail.com>
 */

#include <stdio.h>

#define CDETECT_CHOST_FILE "deps/cdetect/cdetect/chost.c"
#include <cdetect/cdetect.c>

int main(int argc, char** argv) {
  //config_copyright_notice();
  config_begin();

  if(config_options(argc, argv)) {
    // Perform basic checks.
    config_compiler_check();
  }

  config_end();

  // Remove superficial cDetect files that we don't need anymore.
  cdetect_file_remove("./config.h");
  cdetect_file_remove("./cachect.txt");

  return 0;
}
