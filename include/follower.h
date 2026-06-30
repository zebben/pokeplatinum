#ifndef POKEPLATINUM_FOLLOW_MON_H
#define POKEPLATINUM_FOLLOW_MON_H

#include <nitro.h>

#include "follow_mon_feature.h"
#include "struct_decls/map_object.h"
#include "field/field_system_decl.h"

typedef enum {
  FOLLOW_MON_STATE_NOT_ALLOWED,
  FOLLOW_MON_STATE_SMALL_ONLY,
  FOLLOW_MON_STATE_ALLOWED,
} FollowMonState;

typedef enum {
  FOLLOW_MON_ATTR_DIR_UP,
  FOLLOW_MON_ATTR_DIR_DOWN,
  FOLLOW_MON_ATTR_DIR_RIGHT,
  FOLLOW_MON_ATTR_DIR_LEFT,
  FOLLOW_MON_ATTR_DIR_RIGHT_DOWN,
  FOLLOW_MON_ATTR_DIR_LEFT_DOWN,
} FollowMonAttrDir;

typedef enum {
  FOLLOW_MON_ANIM_NONE,
  FOLLOW_MON_ANIM_START,
  FOLLOW_MON_ANIM_READY,
  FOLLOW_MON_ANIM_DONE,
} FollowMonAnimationCommandState;

typedef struct FollowMon {
  MapObject *mapObj;
  int animation;
  int x;
  int z;
  u16 species;
  u8 form;
  u8 gender;
  BOOL isFollowing;
  BOOL isShiny;
  BOOL needReset;
  FollowMonAnimationCommandState commandState;
} FollowMon;

typedef struct Party Party;
typedef struct Pokemon Pokemon;

// TODO actually add mood to mondata
typedef struct FollowMonMood {
  int personality;
  u16 species;
  BOOL initialized;
  Pokemon *mon;
} FollowMonMood;

typedef struct FollowMonInfo {
  u8 unk0;
  u8 tooTall;
  u8 movementFlags;
  u8 unk3;
} FollowMonInfo;

int FollowMon_GetMetadataIndex(u16 species);
int FollowMon_GetFormCount(u16 species);
BOOL FollowMon_HasGenderDifference(u16 species);
FollowMonInfo *FollowMon_GetMetadata(u16 species, u8 form);
int FollowMon_GetObjectGraphicsID(u16 species, u8 form, u8 gender);
void FollowMon_SetObjectParams(MapObject *mapObj, u16 species, u8 form, BOOL isShiny);
u16 FollowMon_GetObjectSpecies(MapObject *mapObj);
BOOL FollowMon_ObjectIsShiny(MapObject *mapObj);
void FollowMon_SetObjectActive(MapObject *mapObj, BOOL active);
BOOL FollowMon_ObjectIsActive(MapObject *mapObj);
void FollowMon_Clear(FollowMon *followMon);
void FollowMon_SetInfo(FollowMon *followMon, u16 species, u8 form, u8 gender, BOOL isShiny);
BOOL FollowMon_IsActive(FieldSystem *fieldSystem);
void FollowMon_RequestReset(FollowMon *followMon);
void FollowMonMood_SetPokemon(FollowMonMood *mood, Pokemon *mon);
void FollowMonMood_SetValue(FollowMonMood *mood, s8 value);
s8 FollowMonMood_GetValue(FollowMonMood *mood);
void FollowMonMood_DriftTowardNeutral(FollowMonMood *mood);
BOOL FollowMon_CanFollow(u16 species, int mapId);
Pokemon *FollowMon_GetLeadPokemon(Party *party);
MapObject *FollowMon_GetMapObject(FieldSystem *fieldSystem);
MapObject *FollowMon_Init(FieldSystem *fieldSystem, int x, int z, int dir, int mapId);
void FollowMon_Load(FieldSystem *fieldSystem, int mapId);
void FollowMon_Reset(FieldSystem *fieldSystem);
void FollowMon_SaveState(FieldSystem *fieldSystem);
void FollowMon_UpdatePlayerState(FieldSystem *fieldSystem);
void FollowMon_SetInhibitFlag(FieldSystem *fieldSystem, BOOL inhibit);

#endif // POKEPLATINUM_FOLLOW_MON_H
