#include "constants/player_avatar.h"
#include "constants/scrcmd.h"
#include "field/field_system.h"
#include "follower.h"
#include "generated/movement_types.h"
#include "map_object.h"
#include "party.h"
#include "pokemon.h"
#include "player_avatar.h"
#include "save_follow_mon.h"
#include "system_flags.h"
#include "unk_02054884.h"

static BOOL FollowMon_CanSpawn(FieldSystem *fieldSystem, u16 species, int mapId)
{
  SaveFollowMon *saveFollowMon = Save_FollowMon_Get(fieldSystem->saveData);

  if (Save_FollowMon_GetInhibitFlagState(saveFollowMon) != FALSE) {
    return FALSE;
  }

  if (SystemFlag_CheckHasPartner(SaveData_GetVarsFlags(fieldSystem->saveData)) == TRUE) {
    return FALSE;
  }

  return FollowMon_CanFollow(species, mapId);
}

static BOOL FollowMon_PlayerStateAllowsActive(FieldSystem *fieldSystem)
{
  switch (PlayerAvatar_GetPlayerState(fieldSystem->playerAvatar)) {
  case PLAYER_AVATAR_WALKING:
  case PLAYER_AVATAR_DISTORTION_WORLD:
  case PLAYER_AVATAR_HEARTHOME_GYM:
    return TRUE;
  default:
    return FALSE;
  }
}

static BOOL FollowMon_ShouldRestoreVanished(FieldSystem *fieldSystem, u16 species)
{
  SaveFollowMon *saveFollowMon = Save_FollowMon_Get(fieldSystem->saveData);

  if (Save_FollowMon_GetFollowState(saveFollowMon) != FOLLOW_MON_SAVE_VANISHED) {
    return FALSE;
  }

  return !FollowMon_CanFollow(species, Save_FollowMon_GetMapID(saveFollowMon));
}

static void FollowMon_SetSavedStateFromActive(FieldSystem *fieldSystem, BOOL active)
{
  SaveFollowMon *saveFollowMon = Save_FollowMon_Get(fieldSystem->saveData);

  if (active) {
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_FOLLOWING, saveFollowMon);
  } else {
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_VANISHED, saveFollowMon);
  }
}

static void FollowMon_SetMapObjectDefaults(MapObject *mapObj)
{
  MapObject_SetLocalID(mapObj, LOCALID_FOLLOW_MON);
  MapObject_SetTrainerType(mapObj, 0);
  MapObject_SetFlag(mapObj, 0);
  MapObject_SetScript(mapObj, 0);
  MapObject_SetDataAt(mapObj, 0, 0);
  MapObject_SetDataAt(mapObj, 0, 1);
  MapObject_SetDataAt(mapObj, 0, 2);
  MapObject_SetMovementRangeX(mapObj, -1);
  MapObject_SetMovementRangeZ(mapObj, -1);
  MapObject_SetFlagIsPersistent(mapObj, TRUE);
  MapObject_SetStatusFlagOn(mapObj, MAP_OBJ_STATUS_13);
  MapObject_SetStatusFlagOff(mapObj, MAP_OBJ_STATUS_LOCK_DIR | MAP_OBJ_STATUS_PAUSE_ANIMATION);
  MapObject_SetDynamicHeightCalculationEnabled(mapObj, TRUE);
  sub_02062D80(mapObj, TRUE);
  FollowMon_SetObjectActive(mapObj, TRUE);

  // TODO script/event equivalent if needed.
}

static void FollowMon_EnsureMovementType(MapObject *mapObj)
{
  if (MapObject_GetMovementType(mapObj) != MOVEMENT_TYPE_FOLLOW_PLAYER) {
    MapObject_SwitchMovementType(mapObj, MOVEMENT_TYPE_FOLLOW_PLAYER);
  }
}

static void FollowMon_LoadMapObjectInfo(FieldSystem *fieldSystem, MapObject *mapObj, Pokemon *mon)
{
  u16 species = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
  u8 form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
  u8 gender = Pokemon_GetValue(mon, MON_DATA_GENDER, NULL);
  BOOL isShiny = Pokemon_IsShiny(mon);
  BOOL active = FollowMon_PlayerStateAllowsActive(fieldSystem) && !FollowMon_ShouldRestoreVanished(fieldSystem, species);

  fieldSystem->followMon.mapObj = mapObj;
  fieldSystem->followMon.isFollowing = TRUE;
  FollowMon_SetInfo(&fieldSystem->followMon, species, form, gender, isShiny);
  FollowMon_SetObjectParams(mapObj, species, form, isShiny);
  FollowMon_SetObjectActive(mapObj, active);

  FollowMon_EnsureMovementType(mapObj);
  sub_02061AB4(mapObj, FollowMon_GetObjectGraphicsID(species, form, gender));
  FollowMon_SetSavedStateFromActive(fieldSystem, active);
}

static MapObject *FollowMon_FindMapObject(FieldSystem *fieldSystem)
{
  MapObject *mapObj;
  int objectIndex = 0;

  while (MapObjectMan_FindObjectWithStatus(fieldSystem->mapObjMan, &mapObj, &objectIndex, MAP_OBJ_STATUS_0)) {
    if (MapObject_GetLocalID(mapObj) == LOCALID_FOLLOW_MON) {
      return mapObj;
    }
  }

  return MapObjMan_LocalMapObjByIndex(fieldSystem->mapObjMan, LOCALID_FOLLOW_MON);
}

MapObject *FollowMon_GetMapObject(FieldSystem *fieldSystem)
{
  if (fieldSystem->followMon.mapObj != NULL) {
    return fieldSystem->followMon.mapObj;
  }

  return FollowMon_FindMapObject(fieldSystem);
}

MapObject *FollowMon_Init(FieldSystem *fieldSystem, int x, int z, int dir, int mapId)
{
  Party *party = SaveData_GetParty(fieldSystem->saveData);
  Pokemon *mon;
  MapObject *mapObj;
  int species;
  int form;
  int gender;
  int graphics;

  FollowMon_Clear(&fieldSystem->followMon);

  mon = FollowMon_GetLeadPokemon(party);
  if (mon == NULL) {
      return NULL;
  }

  species = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
  if (!FollowMon_CanSpawn(fieldSystem, species, mapId)) {
      return NULL;
  }

  form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
  gender = Pokemon_GetValue(mon, MON_DATA_GENDER, NULL);
  graphics = FollowMon_GetObjectGraphicsID(species, form, gender);

  mapObj = MapObjectMan_AddMapObject(fieldSystem->mapObjMan, x, z, dir, graphics, MOVEMENT_TYPE_FOLLOW_PLAYER, mapId);
  if (mapObj == NULL) {
      return NULL;
  }

  FollowMon_SetMapObjectDefaults(mapObj);
  FollowMonMood_SetPokemon(&fieldSystem->followMonMood, mon);
  FollowMon_LoadMapObjectInfo(fieldSystem, mapObj, mon);

  return mapObj;
}

void FollowMon_Load(FieldSystem *fieldSystem, int mapId)
{
  Party *party = SaveData_GetParty(fieldSystem->saveData);
  Pokemon *mon;
  int species;
  MapObject *mapObj;
  FollowMon_Clear(&fieldSystem->followMon);

  mon = FollowMon_GetLeadPokemon(party);
  if (mon == NULL) {
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, Save_FollowMon_Get(fieldSystem->saveData));
    return;
  }

  species = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
  FollowMonMood_SetPokemon(&fieldSystem->followMonMood, mon);
  mapObj = FollowMon_FindMapObject(fieldSystem);

  if (FollowMon_CanSpawn(fieldSystem, species, mapId)) {
    if (mapObj == NULL) {
      FollowMon_RequestReset(&fieldSystem->followMon);
      return;
    }

    FollowMon_LoadMapObjectInfo(fieldSystem, mapObj, mon);

    // TODO save state / restore / forbidden / hit
  } else if (mapObj != NULL) {
    MapObject_Delete(mapObj);
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, Save_FollowMon_Get(fieldSystem->saveData));
    FollowMon_RequestReset(&fieldSystem->followMon);
  } else {
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, Save_FollowMon_Get(fieldSystem->saveData));
  }
}

void FollowMon_Reset(FieldSystem *fieldSystem)
{
  MapObject *mapObj;
  u16 species;
  int mapId = fieldSystem->location->mapId;

  if (!fieldSystem->followMon.needReset) {
    return;
  }

  if (!fieldSystem->followMon.isFollowing) {
    if (FollowMon_Init(
        fieldSystem,
        PlayerAvatar_GetXPos(fieldSystem->playerAvatar),
        PlayerAvatar_GetZPos(fieldSystem->playerAvatar),
        PlayerAvatar_GetFacingDir(fieldSystem->playerAvatar),
        mapId) == NULL) {
      Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, Save_FollowMon_Get(fieldSystem->saveData));
    }
  } else {
    mapObj = fieldSystem->followMon.mapObj;

    if (mapObj != NULL) {
      species = FollowMon_GetObjectSpecies(mapObj);

      if (!FollowMon_CanSpawn(fieldSystem, species, mapId)) {
        MapObject_Delete(mapObj);
        FollowMon_Clear(&fieldSystem->followMon);
        Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, Save_FollowMon_Get(fieldSystem->saveData));
      }
    } else {
      FollowMon_Clear(&fieldSystem->followMon);
    }
  }

  fieldSystem->followMon.needReset = FALSE;
}

void FollowMon_SaveState(FieldSystem *fieldSystem)
{
  SaveFollowMon *saveFollowMon = Save_FollowMon_Get(fieldSystem->saveData);

  if (!fieldSystem->followMon.isFollowing || fieldSystem->followMon.mapObj == NULL) {
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, saveFollowMon);
    return;
  }

  FollowMon_SetSavedStateFromActive(fieldSystem, FollowMon_PlayerStateAllowsActive(fieldSystem) && FollowMon_ObjectIsActive(fieldSystem->followMon.mapObj));
}

void FollowMon_UpdatePlayerState(FieldSystem *fieldSystem)
{
  MapObject *mapObj = FollowMon_GetMapObject(fieldSystem);
  u16 species;
  BOOL active;

  if (mapObj == NULL) {
    return;
  }

  species = FollowMon_GetObjectSpecies(mapObj);
  if (!FollowMon_CanSpawn(fieldSystem, species, fieldSystem->location->mapId)) {
    MapObject_Delete(mapObj);
    FollowMon_Clear(&fieldSystem->followMon);
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, Save_FollowMon_Get(fieldSystem->saveData));
    return;
  }

  active = FollowMon_PlayerStateAllowsActive(fieldSystem);
  fieldSystem->followMon.mapObj = mapObj;
  fieldSystem->followMon.isFollowing = TRUE;
  FollowMon_SetObjectActive(mapObj, active);
  FollowMon_SetSavedStateFromActive(fieldSystem, active);
}

void FollowMon_SetInhibitFlag(FieldSystem *fieldSystem, BOOL inhibit)
{
  SaveFollowMon *saveFollowMon = Save_FollowMon_Get(fieldSystem->saveData);
  MapObject *mapObj;

  Save_FollowMon_SetInhibitFlagState(saveFollowMon, inhibit);

  if (inhibit) {
    mapObj = FollowMon_GetMapObject(fieldSystem);
    if (mapObj != NULL) {
      MapObject_Delete(mapObj);
    }

    FollowMon_Clear(&fieldSystem->followMon);
    Save_FollowMon_SetFollowState(FOLLOW_MON_SAVE_NONE, saveFollowMon);
  } else {
    FollowMon_RequestReset(&fieldSystem->followMon);
  }
}
