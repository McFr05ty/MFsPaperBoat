#include "tik_02.h"

EntryList N(Entrances) = {
    [tik_02_ENTRY_0]    { -360.0,  -10.0,    0.0,   90.0 },
    [1]                 {  175.0,   25.0,  -65.0,  180.0 },  // Pit of Trials entrance pipe
};

MapSettings N(settings) = {
    .main = &N(EVS_Main),
    .entryList = &N(Entrances),
    .entryCount = ENTRY_COUNT(N(Entrances)),
    .tattle = { MSG_MapTattle_tik_02 },
};
