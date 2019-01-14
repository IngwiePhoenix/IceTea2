/**
 * @file
 * Contains the functionality to report error messages.
 */

#include "gravity_vm.h"
#include "gravity_core.h"

namespace IceTea {
  void report_gravity_error(
    gravity_vm *vm,
    error_type_t error_type,
    const char *message,
    error_desc_t error_desc,
    void *xdata
  );
}
