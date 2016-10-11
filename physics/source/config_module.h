#ifndef CONFIG_PHYSICS_H
#define CONFIG_PHYSICS_H

#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"


NotifyCategoryDecl(p3physics, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libp3physics();

#endif //CONFIG_PHYSICS_H
