#include "hal/system.h"
#include "stub/machine_io.h"
#include <stdio.h>
#include <stdlib.h>

void hal_system_reset(void) {
  io_log("SYSTEM",
         "System reset requested - performing graceful stub shutdown");
  io_log("SYSTEM",
         "In real hardware, this would trigger a complete system reset");
  io_log("SYSTEM", "Stub application exiting with code 0");
  exit(0);
}