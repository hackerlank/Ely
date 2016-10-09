
#include "config_module.h"
#include "dconfig.h"

#include "btRigidBody.h"
#include "btSoftBody.h"
#include "gamePhysicsManager.h"


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
  BTSoftBody::init_type();
  BTRigidBody::init_type();
  BTSoftBody::register_with_read_factory();
  BTRigidBody::register_with_read_factory();
  GamePhysicsManager::init_type();

  return;
}

