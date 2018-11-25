/**
 * @file
 * This file acts as a header for each and every module that is being defined
 * as a native module.
 *
 * Now, why is this written in C and not C++? Because this way, it is easier
 * to adopt into a Wren project that does not use IceTea. Wren itself is
 * written in C - so, extending Wren should probably happen in C too, right?
 * Therefore, the actual module logic is written in C, while the registry and
 * related features are written in C++. The registry is specificly written for
 * IceTea - so, it is rather unlikely that it "has to be" in C.
 */

#include "wren.h"
#include "libdylib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This structure holds information about a foreign method.
 *
 * @prop isStatic: Is this method static?
 * @prop sig     : The signature by which this method should be called.
 *                 I.e.: "foo(_,_)" == foo(int a, int b)
 *                 Honestly, I actually don't know if you "have" to use
 *                 underscores in signatures.
 * @prop method  : The actual C implementation of the method.
 */
typedef struct {
  bool isStatic;
  const char* sig;
  WrenForeignMethodFn* method;
} WrenModuleClassMethod;

/**
 * This struct holds information about a class.
 *
 * @prop name    : The class name.
 * @prop handlers: The native class allocation and deallocation functions.
 * @prop methods : An array of method descriptions.
 */
typedef struct {
  const char* name;
  WrenForeignClassMethods handlers;
  WrenModuleClassMethod[] methods;
} WrenModuleClass;

/**
 * This struct holds information about a module. Namely a name, and a few
 * other attributes.
 *
 * @prop name: Module name
 * @prop copyright: You can add a copyright here, if you like.
 * @prop version  : You can add a version string here, if you like. However, if
 *                  you do so, it has to be conform with SemVer.
 * @prop classes  : An array of foreign class descriptions.
 * @prop src      : The module registry will execute that in the name of this
 *                  module. This way, you can add your foreign class
 *                  definitions into the source code, if you like.
 *                  As a small hint: You can use the preprocessor to generate
 *                  a quoted string that has no newlines. This should serve
 *                  the purpose of creating an inline-source.
 * @prop userData : This allows you to store any kind of data with this struct
 *                  that you like. You may utilize this in the module's exit
 *                  function.
 */
typedef struct {
  // Initial
  const char* name;
  const char* copyright;
  const char* version;

  // Native classes
  WrenModuleClass[] classes;

  // Additional source code (may include foreign decls)
  const char* src;

  // Additional user data. Can be anything.
  void* userData;
} WrenModule;

/**
 * Callback for creating a module.
 * @param  vm Wren virtual maschine
 * @return    A new [WrenModule] instance.
 */
typedef WrenModule* (*wren_module_create)(WrenVM*);

/**
 * Callback for freeing resources and otherwise destroying the module instance
 * properly.
 * @param  WremVM*     Wren virtual maschine
 * @param  WrenModule* Wren Module instance
 */
typedef void (*wren_module_exit)(WrenVM*,WrenModule*);

/**
 * Macro to define a module entry function.
 * @param  modName Name of the module.
 */
#define WREN_MODULE_ENTRY(modName) \
  LIBDYLIB_EXPORT WrenModule* \
  wrenModule_##modName##_create(WrenVM* vm)

/**
 * Macro to define a module exit function, which is used to allow the user
 * to destruct anything that Wren did not.
 */
#define WREN_MODULE_EXIT(modName) \
  LIBDYLIB_EXPORT void \
  wrenModule_##modName##_exit(WrenVM* vm, WrenModule* modName)

// These macros help you to define classes. The module registry will go through
// the definitions in order to find classes and methods when they are being
// required by a script.
// Now, as writing struct props is not that easy, these macros will help you.

#define MODULE_VNAME(name) name##_wren_module
#define MODULE(name) \
  WrenModule* MODULE_VNAME(name) = (WrenModule*)malloc(sizeof(WrenModule));
#define MODULE_FREE(ref) \
  (void)free(MODULE_VNAME(ref));
#define MODULE_NAME(ref, modName) \
  MODULE_VNAME(ref)->name = modName;
#define MODULE_COPYRIGHT(ref, modCopyright) \
  MODULE_VNAME(ref)->copyright = modCopyright;
#define MODULE_VERSION(ref, modVersion) \
  MODULE_VNAME(ref)->version = modVersion;

// Via: https://stackoverflow.com/questions/13741048/return-quoted-string-in-c-macro
#define stringify_literal(x) # x
#define stringify_expanded(x) stringify_literal(x)
#define MODULE_SRC(ref, modSrc) \
  MODULE_VNAME(ref)->src = stringify_expanded(modSrc);

// Now, let's make classes.
#define MODULE_CLASSES(modName) \
  MODULE_VNAME(modName)->classes = {
#define MODULE_CLASSES_END() \
  NULL };

#define CLASS_START(name, alloc, final) { \
                                          .name = name, \
                                          .handlers = { \
                                            .allocate = alloc, \
                                            .finalize = final \
                                          }, \
                                          .methods = {
#define CLASS_METHOD(sig, func, static)     { \
                                              .isStatic = static, \
                                              .sig = sig, \
                                              .method = func \
                                            },
#define CLASS_END()                         NULL \
                                          } \
                                        }


// Example usage:
/*
WREN_MODULE_ENTRY(foo) {
  // @var vm: Wren virtual maschine.

  MODULE(foo)
  MODULE_NAME(foo, "foo")
  MODULE_VERSION(foo, "0.0.1")
  MODULE_COPYRIGHT(foo, "")

  MODULE_CLASSES(foo)
    CLASS_START("FooLibrary", foo_wren_create, foo_wren_destroy)
      CLASS_METHOD("add(_,_)", foo_add, false)
    CLASS_END()
  MODULE_CLASSES_END()

  MODULE_SRC(
    foreign class FooLibrary {
      foreign add(_,_)
    }
  )

  return foo;
}

WREN_MODULE_EXIT(foo) {
  // @var vm: Wren virtual maschine
  // @var foo: Wren Module pointer. Free it here.
  MODULE_FREE(foo)
}
*/

#ifdef __cplusplus
} // end extern "C"
#endif
