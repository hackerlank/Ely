#ifndef CONFIG_CONTROL_H
#define CONFIG_CONTROL_H

#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"


NotifyCategoryDecl(control, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libcontrol();

#endif //CONFIG_CONTROL_H
