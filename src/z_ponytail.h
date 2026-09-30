/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

#ifndef Z_PONYTAIL_H
#define Z_PONYTAIL_H

#include "ultra64.h"
#include "global.h"
#include "gPonytailSkel.h"
#include "verlet_physics.h"


/***********************************************************************

	Ponytail Physics

***********************************************************************/
// Set up physics object that carries velocity and rotation of ponytail actor
PhysPlayer gJackiePhysPlayer;

// StandardLimb of ponytail's model generated through Fast 64
extern StandardLimb gPonytailSkelLimb_000;
extern StandardLimb gPonytailSkelLimb_001;
extern StandardLimb gPonytailSkelLimb_002;
extern StandardLimb gPonytailSkelLimb_003;
extern StandardLimb gPonytailSkelLimb_004;
extern StandardLimb gPonytailSkelLimb_005;

// Enum for indicating ponytail's skeleton limbs
typedef enum PonytailBodyPart {
    /* 0x00 */ PONYTAIL_BODYPART_ROOT,      // Root
    /* 0x01 */ PONYTAIL_BODYPART_LIMB1,     // Limb1
    /* 0x02 */ PONYTAIL_BODYPART_LIMB2,     // Limb2
    /* 0x03 */ PONYTAIL_BODYPART_LIMB3,     // Limb3
    /* 0x04 */ PONYTAIL_BODYPART_LIMB4,     // Limb4
    /* 0x05 */ PONYTAIL_BODYPART_LIMB5,     // Limb5 (last limb)
    /* 0x06 */ PONYTAIL_BODYPART_MAX
} PonytailBodyPart;

// Phys limb for each ponytail's skeleton limb
PhysLimb ponytailRootLimb;
PhysLimb ponytailLimb1;
PhysLimb ponytailLimb2;
PhysLimb ponytailLimb3;
PhysLimb ponytailLimb4;
PhysLimb ponytailLimb5;

// Struct containing phys limbs of ponytail
PhysLimb* ponytailPhysLimbs[6] = {
    &ponytailRootLimb,
    &ponytailLimb1,
    &ponytailLimb2,
    &ponytailLimb3,
    &ponytailLimb4,
    &ponytailLimb5
};

// Phys bone for ponytail's skeleton limbs, where phys bone contains 2 phys limbs
PhysBone ponytailRootLimbLimb1;
PhysBone ponytailLimb1Limb2;
PhysBone ponytailLimb2Limb3;
PhysBone ponytailLimb3Limb4;
PhysBone ponytailLimb4Limb5;

// Enum for indicating ponytail's bones (where each bone contains 2 limbs)
typedef enum PonytailBoneIndex {
    /* 0x00 */ PONYTAIL_BONE_ROOT_LIMB1,    // Root & Limb1
    /* 0x01 */ PONYTAIL_BONE_LIMB1_LIMB2,   // Limb1 & Limb2
    /* 0x02 */ PONYTAIL_BONE_LIMB2_LIMB3,   // Limb2 & Limb3
    /* 0x03 */ PONYTAIL_BONE_LIMB3_LIMB4,   // Limb3 & LimB4
    /* 0x04 */ PONYTAIL_BONE_LIMB4_LIMB5,   // Limb4 & LimB5
    /* 0x05 */ PONYTAIL_BONE_MAX            // Limb5
} PonytailBoneIndex;

// Struct containing phys bones of ponytail
PhysBone* ponytailPhysBones[5] = {
    &ponytailRootLimbLimb1,
    &ponytailLimb1Limb2,
    &ponytailLimb2Limb3,
    &ponytailLimb3Limb4,
    &ponytailLimb4Limb5
};

// Ponytail struct 
struct Ponytail;

typedef void (*PonytailActionFunc)(struct Ponytail*, struct PlayState*);

typedef struct Ponytail {
    Actor actor;
    SkelAnime skelAnime;
    Vec3s jointTable[GPONYTAILSKEL_NUM_LIMBS];
    Vec3s morphTable[GPONYTAILSKEL_NUM_LIMBS];
    Vec3f bodyPartsPos[PONYTAIL_BODYPART_MAX];
    u8 needsReset;
    u8 needsResetShape;
    u8 hideInFirstPerson;
} Ponytail;


void Ponytail_Init(Actor* thisx, PlayState* play);
void Ponytail_Destroy(Actor* thisx, PlayState* play);
void Ponytail_Update(Actor* thisx, PlayState* play);
void Ponytail_Draw(Actor* thisx, PlayState* play);

void Ponytail_SetResetShape(Ponytail* this, PhysLimb* limbs[]);
void Ponytail_RotateFromPinned(Vec3f* direction, Vec3s* rotation);

void Ponytail_RotateJoints(Ponytail* this, PhysBone* gPhysBones[], Player* player);
void Ponytail_SetDefaultBodyPartsPos(Ponytail* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[]) ;
void Ponytail_UpdateBodyPartsPos(Ponytail* this, Player* player, Vec3f apply_force, 
    PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]);

#endif