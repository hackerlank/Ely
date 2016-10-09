#ifndef CONFIG_PHYSICS_H
#define CONFIG_PHYSICS_H

#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"


NotifyCategoryDecl(p3control, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libp3control();

#endif //CONFIG_PHYSICS_H
