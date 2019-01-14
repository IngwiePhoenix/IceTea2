#include <iostream>
#include <string>

#include "error_reporter.hpp"

namespace IceTea {

using std::cout;
using std::endl;
using std::string;

/**
 * Report a warning or an error emitted from Gravity.
 *
 * @param vm         Gravity vm
 * @param error_type Type of warning/error
 * @param message    Message
 * @param error_desc Description of error
 * @param xdata      User-Data assigned with Gravity.
 *
 * @note Some errors are in fact recoverable. Maybe we can even use a scripted
 *       logger in some cases - like for warnings, runtime and others that may
 *       not have "crashed" the VM.
 *       However, if the error stems from such a reporter, this may lead to an
 *       insane amount of blurt...
 * @note Being able to print a stacktrace would definitively be sweet!
 */
void report_gravity_error(
  gravity_vm *vm, error_type_t error_type, const char *message,
  error_desc_t error_desc, void *xdata
) {
    #pragma unused(vm, xdata)

    string type = "N/A";
    switch (error_type) {
        case GRAVITY_ERROR_NONE:
          type = "";
          break;
        case GRAVITY_ERROR_SYNTAX:
          type = "Syntax error";
          break;
        case GRAVITY_ERROR_SEMANTIC:
          type = "Semantic error";
          break;
        case GRAVITY_ERROR_RUNTIME:
          type = "Runtime error";
          break;
        case GRAVITY_WARNING:
          type = "Warning";
          break;
        case GRAVITY_ERROR_IO:
          type = "I/O";
          break;
    }

    cout << "[gravity] "
         << type << " in <"
         << error_desc.fileid
         << "> (line " << error_desc.lineno
         << ", column " << error_desc.colno << "):" << endl
         << message << endl;
}

} // end namespace IceTea
