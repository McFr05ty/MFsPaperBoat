#include "port/ShipInit.hpp"
#include "port/Engine.h"
#include "port/hooks/Events.h"
#include "port/ui/cvar_prefixes.h"

#include "common.h"
#include "pit_of_trials.h"

void RegisterPitOfTrials_Init() {
    REGISTER_LISTENER(GameFrameUpdate, EVENT_PRIORITY_NORMAL, [](IEvent*) { pit_poll(); });
}

static RegisterShipInitFunc initPitOfTrialsFunc(RegisterPitOfTrials_Init);
