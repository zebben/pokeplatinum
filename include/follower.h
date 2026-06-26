#ifndef POKEPLATINUM_FOLLOWER_H
#define POKEPLATINUM_FOLLOWER_H

#include "struct_decls/map_object.h"
#include "field/field_system_decl.h"

typedef enum {
  FOLLOWER_STATE_NOT_ALLOWED,
  FOLLOWER_STATE_SMALL_ONLY,
  FOLLOWER_STATE_ALLOWED,
} FollowerState;

typedef enum {
  FOLLOWER_ATTR_DIR_UP,
  FOLLOWER_ATTR_DIR_DOWN,
  FOLLOWER_ATTR_DIR_RIGHT,
  FOLLOWER_ATTR_DIR_LEFT,
  FOLLOWER_ATTR_DIR_RIGHT_DOWN,
  FOLLOWER_ATTR_DIR_LEFT_DOWN,
} FollowerAttrDir;

typedef enum {
  FOLLOWER_ANIM_NONE,
  FOLLOWER_ANIM_START,
  FOLLOWER_ANIM_READY,
  FOLLOWER_ANIM_DONE,
} FollowerAnimationCommandState;

typedef struct Follower {
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
  FollowerAnimationCommandState commandState;
} Follower;

typedef enum {
  FOLLOWER_SAVE_NONE,
  FOLLOWER_SAVE_FOLLOWING,
  FOLLOWER_SAVE_VANISHED,
} FollowerSaveState;

typedef struct FollowerSave {
  int oldMapOrZoneId;
  u8 followState : 2;
  u8 forbidden : 1;
} FollowerSave;

typedef struct Pokemon Pokemon;

// TODO actually add mood to mondata
typedef struct FollowerMood {
  int personality;
  u16 species;
  BOOL initialized;
  Pokemon *mon;
} FollowerMood;

typedef struct FollowerInfo {
	u8 footType;
	u8 bigSize;
	u8 flyFlag;
	u8 roomFlag;
} FollowerInfo;

int Follower_GetMetadataIndex(u16 species);
int Follower_GetFormCount(u16 species);
BOOL Follower_HasGenderDifference(u16 species);
FollowerInfo *Follower_GetMetadata(u16 species, u8 form);
int Follower_GetObjectGraphicsID(u16 species, u8 form, u8 gender);
void Follower_Clear(Follower *follower);
void Follower_SetInfo(Follower *follower, u16 species, u8 form, u8 gender, BOOL isShiny);
BOOL Follower_IsActive(FieldSystem *fieldSystem);
void Follower_RequestReset(Follower *follower);
void FollowerMood_SetPokemon(FollowerMood *mood, Pokemon *mon);
void FollowerMood_SetValue(FollowerMood *mood, s8 value);
s8 FollowerMood_GetValue(FollowerMood *mood);
void FollowerMood_DriftTowardNeutral(FollowerMood *mood);
BOOL Follower_CanFollow(u16 species, int mapId);
Pokemon *Follower_GetLeadPokemon(Party *party);
MapObject *Follower_Init(FieldSystem *fieldSystem, int x, int z, int dir, int mapId);
void Follower_Load(FieldSystem *fieldSystem, int mapId);

#endif // POKEPLATINUM_FOLLOWER_H
