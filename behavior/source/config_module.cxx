
#include "config_module.h"
#include "dconfig.h"

#include "gameBehaviorManager.h"
#include "p3Activity.h"

Configure( config_p3behavior );
NotifyCategoryDef( p3behavior , "");

ConfigureFn( config_p3behavior ) {
  init_libp3behavior();
}

void
init_libp3behavior() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  // Init your dynamic types here, e.g.:
  // MyDynamicClass::init_type();
  P3Activity::init_type();
  P3Activity::register_with_read_factory();
  GameBehaviorManager::init_type();

  return;
}

