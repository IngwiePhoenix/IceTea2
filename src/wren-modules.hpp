#include <vector>
#include <string>

#include "wren/include/wren.hpp"
#include "module.h"

#include "stlplus/portability/file_system.hpp"

#ifndef ICETEA_NO_INTERNALS
// Standard TTVFS:
#include "ttvfs/ttvfs/VFSFile.h"
#include "ttvfs/ttvfs/VFSRefcounted.h"
// Zip support:
#include "ttvfs/ttvfs_zip/VFSZipArchiveLoader.h"
#include "ttvfs/ttvfs_zip/VFSDirZip.h"
#endif

namespace IceTea {
namespace Wren {

/**
 * A list of file extensions that should be used when resolving a module
 * path.
 */
extern const std::vector<std::string> scriptExtensions;

/**
 * A list of extensions that should be used when resolving native module
 * paths.
 */
extern const std::vector<std::string> nativeExtensions;

/**
 * A list of folders that should be used to search for modules.
 * These folders are essentially opened subsequently backwards on the
 * current module's path.
 */
extern const std::vector<std::string> moduleDirs;

/**
 * @section moduleSearch Module Search pattern
 * Imagine you are importing the module "foo". The module registry will
 * begin to look in the current module's folder for either of [moduleDirs],
 * and if it found that, it will look for a sub-folder matching the given
 * module name, load it's package.toml and pick up it's "main" field. This
 * file is expected to be the module's "entry". If the file is not found,
 * then the registry will attempt to look for similiarily named files with
 * either of the [scriptExtensions]. If this fails, then [nativeExtensions]
 * is attempted.
 *
 * The example above in a listing, where the current module is at
 * /foo/bar/baz/module.wren and the requested module is "utils":
 *
 * - /foo/bar/baz/deps/utils/package.toml
 *   - /foo/bar/baz/deps/utils/$main
 *   - /foo/bar/baz/deps/utils/$main.it
 *   - /foo/bar/baz/deps/utils/$main.wren
 *   - /foo/bar/baz/deps/utils/$main.nit
 *   - /foo/bar/baz/deps/utils/$main.nwren
 * - /foo/bar/baz/wren_modules/utils/package.toml
 *   - /foo/bar/baz/wren_modules/utils/$main
 *   - /foo/bar/baz/wren_modules/utils/$main.it
 *   - /foo/bar/baz/wren_modules/utils/$main.wren
 *   - /foo/bar/baz/wren_modules/utils/$main.nit
 *   - /foo/bar/baz/wren_modules/utils/$main.nwren
 * - /foo/bar/baz/deps/utils/index.it
 * - /foo/bar/baz/deps/utils/index.wren
 * - /foo/bar/baz/deps/utils/index.nit
 * - /foo/bar/baz/deps/utils/index.nwren
 * - /foo/bar/baz/wren_modules/utils/index.it
 * - /foo/bar/baz/wren_modules/utils/index.wren
 * - /foo/bar/baz/wren_modules/utils/index.nit
 * - /foo/bar/baz/wren_modules/utils/index.nwren
 *
 * If this fails, the very same method is resumed at the parent directory:
 * - /foo/bar/wren_modules/utils/package.toml
 *   - /foo/bar/wren_modules/utils/$main
 *   - ...
 * - /foo/bar/wren_modules/utils/index.it
 * - ...
 */

/**
 * Searches the given module.
 * @param vm       Wren virtual maschine.
 * @param importer The module requesting the import.
 * @param name     The module to be resolved.
 * @return         Returns the full module name (path).
 */
const char* resolveModule(::WrenVM* vm, const char* importer, const char* name);

/**
 * Uses the given name to load a module off the file system and return it's
 * plain source code that Wren should interpret.
 *
 * @param  vm   Wren virtual maschine.
 * @param  name Full module path, given by @see resolveModule(...) .
 * @return      Source of the resolved module.
 */
const char* loadModule(::WrenVM* vm, const char* name);

/**
 * Searches the module registry for the given foreign [className] within
 * [module].
 *
 * @param  vm        Wren virtual maschine.
 * @param  module    Module, in which the class should be searched for.
 * @param  className The class to be searched.
 * @return           An instance of @type WrenForeignClassMethods, which
 *                   defines the allocation and deallocation (finalizer)
 *                   methods for instancing and de-instancing this class.
 */
::WrenForeignClassMethods* findForeignClass(
  ::WrenVM* vm, const char* module, const char* className
);

/**
 * Searches the registry for a given [module], it's [class] and tries to
 * return a method of [signature].
 *
 * @param  vm        [description]
 * @param  module    [description]
 * @param  className [description]
 * @param  isStatic  [description]
 * @param  signature [description]
 * @return           [description]
 */
::WrenForeignMethodFn* findForeignMethod(
  ::WrenVM* vm,
  const char* module, const char* className,
  bool isStatic, const char* signature
);

/**
 * @class InternalModules
 * This class is supposed to hold or resolve internal modules.
 * It is always asked first by the above resolvers, so that no search is
 * started on the user's file system - which will speed up loading times
 * a little bit.
 */
class InternalModules {
private:
#ifndef ICETEA_NO_INTERNALS
  // When we embed the IceTea script library into the binary, we use TTVFS to
  // read the zip archive that had been embedded as a const char[].
  // This may even be useful for debugging...
  ttvfs::VFSZipArchiveLoader loader;
  ttvfs::CountedPtr<ttvfs::MemFile> zipData;
  ttvfs::CountedPtr<ttvfs::ZipDir> internalLibrary;
#else
  // But when we do not, we look at this specific folder instead.
  const char* libraryPath;
#endif
public:
  InternalModules();
  ~InternalModules();

  /**
   * When ICETEA_NO_INTERNALS is set, this method will allow you to set the
   * path to the lib/ dir, which will be done via the environment variable
   * ICETEA_LIB.
   * If not set, this method is pretty much a noop().
   *
   * @param libPath Path to the library files. Make sure it exists!
   */
  void setLibraryPath(const char* libPath);

  /**
   * If ICETEA_NO_INTERNALS is NOT set, this method will consume the given Zip
   * data and construct a virtual file system ala TTVFS, from which it will
   * now load the internal modules.
   *
   * @param bytes  ZIP bytes
   * @param length ZIP length
   */
  void setZipData(const char* bytes, uint32_t length);

  /**
   * Checks if a given module is internal or not. For instance:
   * - import "fs" for File <- Internal.
   * - import "./fs" for File <- External.
   * - import "foo/bar" for Baz <- If a sub-folder named foo exists within the
   *   library folder or ZIP file, it is considered internal. Otherwise, not.
   *
   * @param  module Module to inspect
   * @return        Wether the module is or is not internal.
   */
  bool isInternal(std::string module);

  /**
   * Returns the source for an internal module.
   *
   * @param  module Module to retrive.
   * @return        Contents of the module.
   */
  const char* getModuleSource(std::string module);
};

/**
 * @class ModuleRegistry
 * This class acts as a module registry for NATIVE modules. Wren already has
 * a registry/cache for scripted modules, but not for native ones. This is
 * what this class does.
 *
 * Module names are, just like with scripted modules, __fully resolved and
 * absolute paths__ to the respective file.
 *
 * A module ID is the identifier used to create the module with the MODULE(ref)
 * macro.
 *
 * This class is stored into WrenConfiguration::userData, so that only one registry is ever
 * associated with one VM - or, each VM that this very config is passed to.
 */
class ModuleRegistry {
public:
  class Entry {
  private:
    std::string path;
    std::string id;
    libdylib::dylib* moduleHandle;
    ::WrenModule* moduleDef;
    ::wren_module_create* cb_modCreate;
    ::wrem_module_exit* cb_modExit;

  public:
    Entry();
    ~Entry();

    /**
     * A special constructor that is used to register internal native bindings.
     *
     * @param name    Name of the module
     * @param id      ID of the module
     * @param module  Module definition
     * @param exit_cb Exit callback
     */
    Entry(
      std::string name,
      ::WrenModule* module,
      ::wren_module_exit* exit_cb
    );

    /**
     * Load the native module via libdylib, as a shared library.
     *
     * @param  fullPath Absolute path to the native module.
     *                  Passing NULL will load the running binary itself.
     * @param  id       Identifier used to build the symbol names.
     * @return          Wether the loading was successful or not.
     *
     * @see libdylib::dylib_self
     */
    bool load(const char* fullPath, const char* id);
    inline bool load(std::string fp, std::string n) {
      return this->load(fp.c_str(), n.c_str());
    }

    /**
     * Verifies if a module is properly opened and ready to use.
     *
     * @return Wether the module is open, or not.
     */
    bool isOpen();

    /**
     * Check if a loaded module has a given class.
     * @param  className Class to look for.
     * @return           True if successful, false if not.
     */
    bool hasClass(const char* className);
    inline bool hasClass(std::string cn) {
      return this->hasClass(cn.c_str());
    }

    /**
     * Look up a method and, if it exists, return it's C implementation.
     * @param  className Class to look for
     * @param  methodSig Method to look for
     * @return           A pointer of type [WrenForeignMethodFn] on success,
     *                   and NULL on failure.
     */
    WrenForeignMethodFn* getClassMethod(
      const char* className,
      const char* methodSig
    );
    inline WrenForeignMethodFn* getClassMethod(std::string cn, std::string ms) {
      return this->getClass(cn.c_str(), ms.c_str());
    }
  };

private:
  std::map<std::string, Entry> registry;
  InternalModules internals;

public:
  ModuleRegistry();
  ~ModuleRegistry();

  /**
   * Get a module's entry in the registry.
   * @param  name Name of the module
   * @return      Entry instance
   */
  Entry* getModule(std::string name);

  /**
   * Load a new module into the registry.
   * @param  path Fully resolved path to the module.
   * @return      Wether the loading was successful or not.
   */
  bool loadModule(std::string path);

  /**
   * Check if a module is currently loaded or not.
   * @param  name Name of the module to search for.
   * @return      Wether the module is or is not present.
   */
  bool hasModule(std::string name);

  /**
   * Delete a module from the registry.
   * @param  name Name of the module.
   * @return      Wether deletion was successful.
   */
  bool dropModule(std::string name);

  /**
   * This is a rather dangerous method: It allows you to add a module
   * manually. Be careful with this.
   * @param  module    Module that should be loaded, full path!
   * @param  overwrite Set to true to override an existing module.
   * @return           True if adding was successful, false if not.
   *                   False is also returned if the module exists.
   */
  bool addManually(Entry* module, bool overwrite);

  /**
   * Returns a pointer to the privately stored internals.
   *
   * @return Instance-pointer for InternalModules.
   */
  InternalModules* getInternals();
};

namespace _internal_module {
/**
 * Makes the registry and other tool functions available to the scripting
 * language itself.
 *
 * @param  vm Wren virtual maschine
 * @return    Wren Module description
 */
::WrenModule* reigister(::WrenVM* vm);
} // end: _internal_module

} // end: Wren
} // end: IceTea
