let paths = {
    src: path_joinMany(
        join(pathSep, [third_party, "jerryscript"]),
        [
            "jerry-core",
            join(pathSep, ["jerry-core", "api"]),
            join(pathSep, ["jerry-core", "debugger"]),
            join(pathSep, ["jerry-core", "ecma"]),
            join(pathSep, ["jerry-core", "ecma", "base"]),
            join(pathSep, ["jerry-core", "ecma", "builtin-object"]),
            join(pathSep, ["jerry-core", "ecma", "builtin-object", "typedarray"]),
            join(pathSep, ["jerry-core", "ecma", "operations"]),
            join(pathSep, ["jerry-core", "jcontext"]),
            join(pathSep, ["jerry-core", "jmem"]),
            join(pathSep, ["jerry-core", "jrt"]),
            join(pathSep, ["jerry-core", "lit"]),
            join(pathSep, ["jerry-core", "parser"]),
            join(pathSep, ["jerry-core", "vm"]),

            join(pathSep, ["jerry-ext", "arg"]),
            join(pathSep, ["jerry-ext", "common"]),
            join(pathSep, ["jerry-ext", "debugger"]),
            join(pathSep, ["jerry-ext", "handle-scope"]),
            join(pathSep, ["jerry-ext", "handler"]),
            join(pathSep, ["jerry-ext", "module"]),

            join(pathSep, ["jerry-port", "default"])
        ]
    ),
    inc: path_joinMany(
        third_party + pathSep + "jerryscript",
        [
            join(pathSep, ["jerry-core", "include"]),
            join(pathSep, ["jerry-ext", "include"]),
            join(pathSep, ["jerry-port", "default", "include"])
        ]
    )
};

function make() {
    
}