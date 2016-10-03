
#include "config_module.h"
#include "dconfig.h"
#include "gameControlManager.h"
#include "p3Driver.h"
#include "p3Chaser.h"


Configure( config_p3control );
NotifyCategoryDef( p3control , "");

ConfigureFn( config_p3control ) {
  init_libp3control();
}

void
init_libp3control() {
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
  GameControlManager::init_type();

  return;
}

