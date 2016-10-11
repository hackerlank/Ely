
#include "config_module.h"
#include "dconfig.h"

#include "btRigidBody.h"
#include "btSoftBody.h"
#include "gamePhysicsManager.h"


Configure( config_p3physics );
NotifyCategoryDef( p3physics , "");

ConfigureFn( config_p3physics ) {
  init_libp3physics();
}

void
init_libp3physics() {
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

