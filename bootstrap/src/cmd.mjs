function _createToolFn(funcs) {
    return function(o) {
        return (
            getenv("ICETEA_FLAVOR") == "unix" 
            ? funcs.unix(o) 
            : funcs.win32(o)
        );
    }
}

function intern_CC(C, input, O, output, D, defines, I, includes, extra, cc_bin) {
    cc_bin = cc_bin === undefined ? getenv("CC") : cc_bin;
    let extra = extra === undefined ? "" : extra;
    let cmd = join(" ", [
        cc_bin,
        extra,
        prepend(D, defines),
        prepend(I, includes),
        C + input,
        O + output
    ]);

    print(cmd);
    return system(cmd);
}

let CC = _createToolFn({
    unix: function(o) {
        return intern_CC(
            "-c", o.input,
            "-o", o.output,
            "-D", o.defines,
            "-I", o.includes
            getenv("CFLAGS"),
            getenv("CC")
        );
    },
    win32: function(o) {
        return intern_CC(
            "", o.input,
            "/Fo", o.output,
            "/D", o.defines,
            "/I", o.includes,
            getenv("CFLAGS"),
            getenv("CC")
        );
    }
});
let CXX = _createToolFn({
    unix: function(o) {
        return intern_CC(
            "-c", o.input,
            "-o", o.output,
            "-D", o.defines,
            "-I", o.includes,
            getenv("CXXFLAGS"),
            getenv("CXX")
        );
    },
    win32: function(o) {
        return intern_CC(
            "", o.input,
            "/Fo", o.output,
            "/D", o.defines,
            "/I", o.includes,
            getenv("CXXFLAGS"),
            getenv("CXX")
        );
    }
});

function intern_LINK(input, O, output, l, libs, L, libdirs, extra) {
    let link_bin = getenv("LD");
    let cmd = join(" ", [
        link_bin,
        getenv("LDFLAGS"),
        extra,
        join(" ", input),
        O + output,
        prepend(L, libdirs),
        prepend(l, libs)
    ]);
    print(cmd);
    return system(cmd);
}

let LINK = _createToolFn({
    unix: function(o) {
        return intern_LINK(
            o.input,
            "-o", o.output,
            "-l", o.libs,
            "-L", o.libdirs
        );
    },
    win32: function(o) {
        return intern_LINK(
            o.input,
            "/out:", o.output,
            "", o.libs
            "/L", o.libdirs
        );
    }
});

function intern_LIB(input, output, createFlag) {
    let ar_bin = getenv("AR");
    let cmd = join(" ", [ar_bin, createFlag, output, input]);
    print(cmd);
    return system(cmd);
}

let LIB = _createToolFn({
    unix: function(o) { return intern_LIB(o.input, o.output, "rcs"); },
    win32: function(o) { return intern_lib(o.input, o.output, ""); }
});

function canCompile(method, source) {
    // test weather source compiles
}

function canRun(method, source) {
    // test weather this can be compiled and run
}