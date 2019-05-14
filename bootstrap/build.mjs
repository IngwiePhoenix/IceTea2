// FFI bindings
let getenv = ffi("char* getenv(char*)");
let putenv = ffi("char* putenv(char*)");

let chdir = ffi("int chdir(char*)");

let exec = ffi("int exec(char*)");
let system = ffi("void system(char*)");

/* <!--- missing
let getFileList
let getDirList
let getList
missing ---> */

// Variables
let pathSep = getenv("ICETEA_OS_PATHSEP");

// Paths: Basics
let rootPath = getenv("ICETEA_PROJECT_ROOT");
let bootstrap = rootPath + pathSep + "bootstrap";
let src = bootstrap + pathSep + "src";
let third_party = rootPath + pathSep + "third_party";
let buildPath = rootPath + pathSep + "build.bootstrap";

// modules
load(src + pathSep + "utils.mjs");
//load(join(pathSep, [src, "make-dyncall.mjs"]), makeDyncall);
//load(join(pathSep, [src, "make-cpu_features.mjs"]), makeCpuFeatures);
load(join(pathSep, [src, "make-jerryscript.mjs"]), makeJerryScript);
//load(join(pathSep, [src, "make-minizip.mjs"]), makeMinizip);
load(join(pathSep, [src, "make-icetea.mjs"]), makeIceTea);

/*
    Building IceTea requires: JerryScript (which is the JS engine IceTea runs on).

    This is a very rural and rough build script, so we do as little as we need here.
    But what we can do, despite this being rather rural and very "hard to use", is running
    a very small selection of configure scripts.
 */

 makeJerryScript.configure();
 makeIceTea.configure();

 makeJerryScript.make();
 makeIceTea.make();

 // Now, we can instruct IceTea to build itself.
 let itBin = join(pathSep, [buildPath, "icetea"]);
 putenv("ICETEA_LIBDIR=" + makeIceTea.lib);
 chdir(rootPath);
 system(itBin);