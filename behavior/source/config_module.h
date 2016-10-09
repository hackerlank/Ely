#ifndef CONFIG_BEHAVIOR_H
#define CONFIG_BEHAVIOR_H

#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"


NotifyCategoryDecl(p3behavior, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libp3behavior();

#endif //CONFIG_BEHAVIOR_H
