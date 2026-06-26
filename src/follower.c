#include "constants/species.h"
#include "field/field_system.h"
#include "follower.h"
#include "map_object.h"
#include "party.h"
#include "pokemon.h"
#include "unk_02054884.h"

int Follower_GetMetadataIndex(u16 species)
{
  return 0; // TODO
}

int Follower_GetFormCount(u16 species)
{
  return 0; // TODO
}

BOOL Follower_HasGenderDifference(u16 species)
{
  return 0; // TODO
}

FollowerInfo *Follower_GetMetadata(u16 species, u8 form)
{
  return NULL; // TODO
}

int Follower_GetObjectGraphicsID(u16 species, u8 form, u8 gender)
{
  return 0; // TODO
}

void Follower_Clear(Follower *follower)
{
	follower->mapObj = NULL;
	follower->animation = 0;
	follower->x = 0;
	follower->z = 0;
	follower->isFollowing = 0;
	follower->needReset = 0;
	follower->commandState = FOLLOWER_ANIM_NONE;
}

void Follower_SetInfo(Follower *follower, u16 species, u8 form, u8 gender, BOOL isShiny)
{
	follower->species = species;
	follower->form = form;
	follower->isShiny = isShiny;
	follower->gender = gender;
}

BOOL Follower_IsActive(FieldSystem *fieldSystem)
{
  // TODO check party too
  return fieldSystem->follower.isFollowing;
}

void Follower_RequestReset(Follower *follower)
{
  follower->needReset = 1;
}

void FollowerMood_SetPokemon(FollowerMood *mood, Pokemon *mon)
{
  u16 species;
  int personality;

  if (mon == NULL) {
      return;
  }

  species = Pokemon_GetValue(mon, MON_DATA_SPECIES, NULL);
  personality = Pokemon_GetValue(mon, MON_DATA_PERSONALITY, NULL);

  if (species == SPECIES_NONE) {
      return;
  }

  if (!mood->initialized || mood->species != species || mood->personality != personality)
  {
      mood->species = species;
      mood->personality = personality;
      mood->initialized = TRUE;
      // TODO reset follower mood
  }

  mood->mon = mon;
}

void FollowerMood_SetValue(FollowerMood *mood, s8 value)
{
  // TODO
}

s8 FollowerMood_GetValue(FollowerMood *mood)
{
  return 0; // TODO
}

void FollowerMood_DriftTowardNeutral(FollowerMood *mood)
{
  // TODO
}

BOOL Follower_CanFollow(u16 species, int mapId)
{
  return TRUE; // TODO
}

Pokemon *Follower_GetLeadPokemon(Party *party)
{
  if (Party_GetCurrentCount(party) == 0) {
    return NULL;
  }
  if (Party_AliveMonsCount(party) == 0) {
    return Party_GetPokemonBySlotIndex(party, 0);
  }
  return Party_FindFirstEligibleBattler(party);
}