#ifndef CONFIG_AI_H
#define CONFIG_AI_H

#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"


NotifyCategoryDecl(p3ai, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libp3ai();

#endif //CONFIG_AI_H
