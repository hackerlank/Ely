
#include "config_module.h"
#include "dconfig.h"
#include "controlManager.h"
#include "p3Driver.h"
//#include "p3Chaser.h"


Configure( config_control );
NotifyCategoryDef( control , "");

ConfigureFn( config_control ) {
  init_libcontrol();
}

void
init_libcontrol() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  // Init your dynamic types here, e.g.:
  // MyDynamicClass::init_type();
  P3Driver::init_type();
  P3Chaser::init_type();
  P3Driver::register_with_read_factory();
  P3Chaser::register_with_read_factory();
  ControlManager::init_type();

  return;
}

