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
#include <string.h>

#define CDETECT_CHOST_FILE "deps/cdetect/cdetect/chost.c"
#include <cdetect/cdetect.c>

// Post-defining some funcs that cDetect doesnt have - oddly...
int config_execute_source_rt(
  char* src, char* cflags, char* args,
  cdetect_string_t &result
) {
  cdetect_bool_t success;
  cdetect_string_t sourcecode;
  cdetect_string_t compile_flags;
  cdetect_string_t link_flags;
  cdetect_string_t arguments;

  cdetect_log("cdetect_execute_source_rt(source, cflags = %'s, args = %'s)\n", cflags, args);

  sourcecode = cdetect_string_format("%s", source);
  compile_flags = cdetect_string_format("%s", cflags);
  link_flags = cdetect_string_format("");
  arguments = cdetect_string_format("%s", args);

  success = cdetect_compile_source(
    sourcecode,
    compile_flags,
    link_flags,
    args ? arguments : 0,
    CDETECT_TRUE,
    (cdetect_bool_t)(cdetect_command_remote != 0),
    result
  );

  cdetect_string_destroy(result);
  cdetect_string_destroy(arguments);
  cdetect_string_destroy(link_flags);
  cdetect_string_destroy(compile_flags);
  cdetect_string_destroy(sourcecode);

  return success;
}

// Configuring deps
// Ported to cDetect!

cdetect_bool_t configure_sharedlibpp() {
  /*
    @TODO:
    - sharedlibpp:
      * Get sizeof(void*), write to shlibpp/config.h
      * Write version information to shlibpp/version.h
  */
  return CDETECT_TRUE;
}

// Convenience macros

/**
 * A very, very minimal macro that doesn't implement Exceptions, but rather
 * serves as a convenience for exiting main() immediately.
 * @param  _try   A block to execute
 * @param  _catch A block to execute on failure
 */
#define try_catch(_try, _catch) { \
    cdetect_bool_t __result = CDETECT_TRUE; \
    char* __message = NULL; \
    _try; \
    if(__result = CDETECT_FALSE) { \
      _catch; \
      if(__message!=NULL) printf("[%s:%s] %s\n", __FILE__, __LINE__, __message); \
      config_abort(); \
    } \
  }

/**
 * Sets `__result` to FALSE and `__message` to the given string, effectively
 * making try_catch() output an error message before exiting the process.
 * @param  msg The error to be displayed
 */
#define throw(msg) { \
  __result = CDETECT_FALSE; \
  __message = msg; \
}

/**
 * Marks the action having failed - which also exits the process, as `__result`
 * will be set to CDETECT_FALSE.
 */
#define failed() __result = CDETECT_FALSE

/*
 Roadmap / TODOs:
 - See which dependencies _really_ need configuration, and implement that using
   cDetect.
 - Add configuration options for IceTea, which actually use defines.
 - Write cDetect-esque functions that will actually compile, by using the found
   compiler(s).
   * compile_cc(char*[] infiles, char* outfile, char* cflags, char* ldflags)
   * compile_cxx(char*[] infiles, char* outfile, char* cflags, char* ldflags)
*/

int main(int argc, char** argv) {
  //config_copyright_notice();
  config_begin();

  if(config_options(argc, argv)) {
    // Perform basic checks.
    config_compiler_check();

    try_catch({
      // ...
    }, {});
  }

  config_end();

  // Remove superficial cDetect files that we don't need anymore.
  cdetect_file_remove("./config.h");
  cdetect_file_remove("./cachect.txt");

  return 0;
}
