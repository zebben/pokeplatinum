#include "save_follow_mon.h"

#include <nitro.h>
#include <string.h>

#include "constants/savedata/save_table.h"

#include "follow_mon_feature.h"
#include "savedata.h"

int Save_FollowMon_sizeof(void)
{
    return sizeof(SaveFollowMon);
}

void Save_FollowMon_Init(SaveFollowMon *followMon)
{
    memset(followMon, 0, sizeof(SaveFollowMon));
    followMon->mapId = 0;
}

SaveFollowMon *Save_FollowMon_Get(SaveData *saveData)
{
#if FOLLOW_MON_SAVE_ENABLED
    return SaveData_SaveTable(saveData, SAVE_TABLE_ENTRY_FOLLOW_MON);
#else
    return NULL;
#endif
}

void Save_FollowMon_SetMapID(u32 mapId, SaveFollowMon *followMon)
{
    followMon->mapId = mapId;
}

u32 Save_FollowMon_GetMapID(SaveFollowMon *followMon)
{
    return followMon->mapId;
}

void Save_FollowMon_SetFollowState(FollowMonSaveState state, SaveFollowMon *followMon)
{
    followMon->followState = state;
}

FollowMonSaveState Save_FollowMon_GetFollowState(SaveFollowMon *followMon)
{
    return followMon->followState;
}

void Save_FollowMon_SetUnused2bitField(u8 value, SaveFollowMon *followMon)
{
    followMon->followState = value;
}

u8 Save_FollowMon_GetUnused2bitField(SaveFollowMon *followMon)
{
    return followMon->followState;
}

void Save_FollowMon_SetInhibitFlagState(SaveFollowMon *followMon, u8 flag)
{
    followMon->inhibitFlag = flag;
}

u8 Save_FollowMon_GetInhibitFlagState(SaveFollowMon *followMon)
{
    return followMon->inhibitFlag;
}
