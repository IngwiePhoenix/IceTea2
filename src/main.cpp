#include <iostream>
#include <string>
#include <sstream>

#include "gravity_compiler.h"
#include "gravity_macros.h"
#include "gravity_core.h"
#include "gravity_vm.h"

#include "error_reporter.hpp"

using std::cout;
using std::endl;
using std::stringstream;

using IceTea::report_gravity_error;

int main(int argc, const char * argv[]) {

    // setup a minimal delegate
    gravity_delegate_t delegate = {
        .error_callback = report_gravity_error
    };

    // This is here for now. Later, this is to be replaced with either a script
    // being loaded off the file system or off the binary itself.
    stringstream source;
    source
      << "func main() {"
      << "  System.print(\"This is going to go away...\")"
      << "  System.print(_args)"
      << "  return 0;"
      << "}";

    // compile source into a closure
    gravity_compiler_t *compiler = gravity_compiler_create(&delegate);
    gravity_closure_t *closure = gravity_compiler_run(
      compiler,
      source.str().c_str(),
      source.str().size(),
      0,
      true,
      false
    );
    if (!closure) return -1;

    // setup a new VM and a new fiber
    gravity_vm *vm = gravity_vm_new(&delegate);

    // transfer memory from compiler to VM and then free compiler
    gravity_compiler_transfer(compiler, vm);
    gravity_compiler_free(compiler);

    // load closure into VM context
    gravity_vm_loadclosure(vm, closure);

    // @FIXME I need a better method for converting argc/argv to Gravity values...
    gravity_value_t args[argc];
    for(int i=0; i<argc; i++) {
      args[i] = VALUE_FROM_CSTRING(vm, argv[i]);
    }


    // lookup add closure
    gravity_value_t scriptMain = gravity_vm_getvalue(vm, "main", strlen("main"));
    if (!VALUE_ISA_CLOSURE(scriptMain)) return -2;

    // execute add closure and print result
    if (gravity_vm_runclosure(vm, VALUE_AS_CLOSURE(scriptMain), scriptMain, args, argc)) {
        gravity_value_t result = gravity_vm_result(vm);
        printf("main():\n");
        gravity_value_dump(vm, result, NULL, 0);
    }

    // free vm and core classes
    gravity_vm_free(vm);
    gravity_core_free();

    return 0;
}
