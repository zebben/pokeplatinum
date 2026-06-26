#include "constants/scrcmd.h"
#include "constants/species.h"
#include "field/field_system.h"
#include "follower.h"
#include "generated/movement_types.h"
#include "map_object.h"
#include "party.h"
#include "pokemon.h"
#include "unk_02054884.h"

static void Follower_SetMapObjectDefaults(MapObject *mapObj)
{
  MapObject_SetLocalID(mapObj, LOCALID_FOLLOWER);
  MapObject_SetDataAt(mapObj, 0, 0);
  MapObject_SetDataAt(mapObj, 0, 1);
  MapObject_SetDataAt(mapObj, 0, 2);
  MapObject_SetMovementRangeX(mapObj, 0);
  MapObject_SetMovementRangeZ(mapObj, 0);

  // TODO vanish/hidden state
  // TODO follower-specific movement type
  // TODO script/event equivalent if needed
  // TODO collision/height/status flags
}

static void Follower_LoadMapObjectInfo(FieldSystem *fieldSystem, MapObject *mapObj, Pokemon *mon)
{
  u16 species = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
  u8 form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
  u8 gender = Pokemon_GetValue(mon, MON_DATA_GENDER, NULL);
  BOOL isShiny = Pokemon_IsShiny(mon);

  fieldSystem->follower.mapObj = mapObj;
  fieldSystem->follower.isFollowing = TRUE;
  Follower_SetInfo(&fieldSystem->follower, species, form, gender, isShiny);

  // TODO shiny
  sub_02061AB4(mapObj, Follower_GetObjectGraphicsID(species, form, gender));
}

// TODO LOCALID_FOLLOWER
static MapObject *Follower_FindMapObject(FieldSystem *fieldSystem)
{
  return MapObjMan_LocalMapObjByIndex(fieldSystem->mapObjMan, LOCALID_FOLLOWER);
}

MapObject *Follower_Init(FieldSystem *fieldSystem, int x, int z, int dir, int mapId)
{
  Party *party = SaveData_GetParty(fieldSystem->saveData);
  Pokemon *mon = Follower_GetLeadPokemon(party);
  MapObject *mapObj;
  int species;
  int form;
  int gender;
  int graphics;

  if (mon == NULL) {
      return NULL;
  }

  species = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
  if (!Follower_CanFollow(species, mapId)) {
      return NULL;
  }

  form = Pokemon_GetValue(mon, MON_DATA_FORM, NULL);
  gender = Pokemon_GetValue(mon, MON_DATA_GENDER, NULL);
  graphics = Follower_GetObjectGraphicsID(species, form, gender);

  mapObj = MapObjectMan_AddMapObject(fieldSystem->mapObjMan, x, z, dir, graphics, MOVEMENT_TYPE_NONE, mapId);
  if (mapObj == NULL) {
      return NULL;
  }

  Follower_SetMapObjectDefaults(mapObj);
  Follower_Clear(&fieldSystem->follower);
  FollowerMood_SetPokemon(&fieldSystem->followerMood, mon);
  Follower_LoadMapObjectInfo(fieldSystem, mapObj, mon);

  return mapObj;
}

void Follower_Load(FieldSystem *fieldSystem, int mapId)
{
	Party *party = SaveData_GetParty(fieldSystem->saveData);
	Pokemon *mon;
	int species;
	MapObject *mapObj;
	Follower_Clear(&fieldSystem->follower);

  mon = Follower_GetLeadPokemon(party);
  if (mon == NULL) {
    return;
  }

  species = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
  FollowerMood_SetPokemon(&fieldSystem->followerMood, mon);
  mapObj = Follower_FindMapObject(fieldSystem);

  if (Follower_CanFollow(species, mapId)) {
    if (mapObj == NULL) {
      Follower_RequestReset(&fieldSystem->follower);
      return;
    }

    Follower_LoadMapObjectInfo(fieldSystem, mapObj, mon);

    // TODO save state / restore / forbidden / hit
  } else if (mapObj != NULL) {
    Follower_LoadMapObjectInfo(fieldSystem, mapObj, mon);
    Follower_RequestReset(&fieldSystem->follower);
  }
}