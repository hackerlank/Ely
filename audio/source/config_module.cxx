
#include "config_module.h"
#include "dconfig.h"
#include "gameAudioManager.h"
#include "p3Sound3d.h"
#include "p3Listener.h"


Configure( config_audio );
NotifyCategoryDef( audio , "");

ConfigureFn( config_audio ) {
  init_libaudio();
}

void
init_libaudio() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  // Init your dynamic types here, e.g.:
  // MyDynamicClass::init_type();
  P3Sound3d::init_type();
  P3Listener::init_type();
  P3Sound3d::register_with_read_factory();
  P3Listener::register_with_read_factory();
  GameAudioManager::init_type();

  return;
}

