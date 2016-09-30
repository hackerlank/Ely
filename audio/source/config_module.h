#ifndef CONFIG_AUDIO_H
#define CONFIG_AUDIO_H

#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"


NotifyCategoryDecl(audio, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libaudio();

#endif //CONFIG_AUDIO_H
