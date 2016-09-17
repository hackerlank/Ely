
#include "config_module.h"
#include "dconfig.h"
#include "aiManager.h"
#include "osSteerVehicle.h"
#include "osSteerPlugIn.h"
#include "rnCrowdAgent.h"
#include "rnNavMesh.h"


Configure( config_ai );
NotifyCategoryDef( ai , "");

ConfigureFn( config_ai ) {
  init_libai();
}

void
init_libai() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  // Init your dynamic types here, e.g.:
  // MyDynamicClass::init_type();
  OSSteerPlugIn::init_type();
  OSSteerVehicle::init_type();
  OSSteerPlugIn::register_with_read_factory();
  OSSteerVehicle::register_with_read_factory();
  RNNavMesh::init_type();
  RNCrowdAgent::init_type();
  RNNavMesh::register_with_read_factory();
  RNCrowdAgent::register_with_read_factory();
  AIManager::init_type();

  return;
}

