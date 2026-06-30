#ifndef POKEPLATINUM_SAVE_FOLLOW_MON_H
#define POKEPLATINUM_SAVE_FOLLOW_MON_H

#include <nitro.h>

#include "savedata.h"

typedef enum FollowMonSaveState {
    FOLLOW_MON_SAVE_NONE,
    FOLLOW_MON_SAVE_FOLLOWING,
    FOLLOW_MON_SAVE_VANISHED,
} FollowMonSaveState;

typedef struct SaveFollowMon {
    u32 mapId;
    u32 followState : 2;
    u32 inhibitFlag : 1;
    u32 padding : 29;
} SaveFollowMon;

int Save_FollowMon_sizeof(void);
void Save_FollowMon_Init(SaveFollowMon *followMon);
SaveFollowMon *Save_FollowMon_Get(SaveData *saveData);
void Save_FollowMon_SetMapID(u32 mapId, SaveFollowMon *followMon);
u32 Save_FollowMon_GetMapID(SaveFollowMon *followMon);
void Save_FollowMon_SetFollowState(FollowMonSaveState state, SaveFollowMon *followMon);
FollowMonSaveState Save_FollowMon_GetFollowState(SaveFollowMon *followMon);
void Save_FollowMon_SetUnused2bitField(u8 value, SaveFollowMon *followMon);
u8 Save_FollowMon_GetUnused2bitField(SaveFollowMon *followMon);
void Save_FollowMon_SetInhibitFlagState(SaveFollowMon *followMon, u8 flag);
u8 Save_FollowMon_GetInhibitFlagState(SaveFollowMon *followMon);

#endif // POKEPLATINUM_SAVE_FOLLOW_MON_H
