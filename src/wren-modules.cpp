#include <stdio.h>

#include "wren-modules.hpp"

#include "stlplus/portability/file_system.hpp"
#include "wren/src/vm/wren_vm.h"

namespace IceTea {
namespace Wren {

using std::string;
using std::vector;

const vector<string> scriptExtensions = {
  "it",
  "wren"
};

const vector<string> nativeExtensions = {
  "nit",
  "nwren"
};

const vector<string> moduleDirs = {
  "deps",
  "wren_modules"
};

string lookupFile(string path) {
  if(stlplus::is_file(path)) {
    return path;
  } else {
    // Combine the extension vectors.
    vector<string> exts;
    exts.reserve(scriptExtensions.size() + nativeExtensions.size());
    exts.insert(exts.end(), scriptExtensions.begin(), scriptExtensions.end());
    exts.insert(exts.end(), nativeExtensions.begin(), nativeExtensions.end());

    vector<string>::iterator ext;
    for(ext=exts.begin(); ext != exts.end(); ext++) {
      string currFile = stlplus::create_filespec(
        stlplus::folder_part(path),
        stlplus::filename_part(path),
        (*ext)
      );
      if(stlplus::is_file(currFile)) {
        return currFile;
      }
    }

    // Could not find.
    return string("");
  }
}

const char* resolveModule(::WrenVM* vm, const char* importer, const char* name) {
  // First, check the internals.
  ModuleRegistry* reg = (ModuleRegistry*)vm->config.userData;
  if(reg->getInternal()->isInternal(name)) {
    // OH, this is actually an internal module. Nice!
    return string("internal:"+name).c_str();
  }

  // Resolve the module from disk.
  sring importerStr = importer;
  string importerDir;
  string nameStr = name;

  if(importerStr == "@") {
    // This is an import from the binary itself.
    // We use the current folder as the starting part, now.
    importerDir = folder_current();
  } else {
    importerDir = stlplus::folder_part(importerStr);
  }

  if(nameStr.substr(0,1) == "/" && stlplus::is_full_path(nameStr)) {
    // Path is absolute.
    return lookupFile(nameStr);
  } else if(nameStr.substr(0,2) == ".." || nameStr.subStr(0,1) == ".") {
    // This path is relative
    return lookupFile(nameStr);
  }

  // Package-manager-ish resolving.
  /**
   * @TODO:
   * When a module of string "a/b/c" is being required,
   */
  vector<string>::iterator dir_ti; moduleDirs.iterator();
  string currentSearchDir = importerDir;
  while(!stlplus::path_compare(currentSearchDir, stlplus::folder_up(currentSearchDir))) {
    for(dir_ti = moduleDirs.begin(); dir_ti != moduleDirs.end(); dir_ti++) {
      string moduleSearchDir = stlplus::folder_down(currentSearchDir, *dir_ti);
      string moduleDir = stlplus::folder_down(moduleSearchDir, name);
      if(stlplus::is_folder(moduleDir)) {
        // The module has been found.
      }
    }
  }
}

} // end ns IceTea
} // end ns Wren
