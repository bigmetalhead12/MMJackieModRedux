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
PhysPlayer gJackiePhysPlayer;

extern StandardLimb gPonytailSkelLimb_000;
extern StandardLimb gPonytailSkelLimb_001;
extern StandardLimb gPonytailSkelLimb_002;
extern StandardLimb gPonytailSkelLimb_003;
extern StandardLimb gPonytailSkelLimb_004;
extern StandardLimb gPonytailSkelLimb_005;
extern StandardLimb gPonytailSkelLimb_006;
extern StandardLimb gPonytailSkelLimb_007;

StandardLimb* gPonytailLimbs[8] = {
    &gPonytailSkelLimb_000,
    &gPonytailSkelLimb_001, 
    &gPonytailSkelLimb_002,
    &gPonytailSkelLimb_003,
    &gPonytailSkelLimb_004,
    &gPonytailSkelLimb_005,
    &gPonytailSkelLimb_006,
    &gPonytailSkelLimb_007
};

typedef enum PonytailBodyPart {
    PONYTAIL_BODYPART_ROOT,         // Root
    PONYTAIL_BODYPART_UPPERCONTROL, // Upper Control Limb
    PONYTAIL_BODYPART_HEAD,         // Head limb
    PONYTAIL_BODYPART_LIMB1,        // Limb1
    PONYTAIL_BODYPART_LIMB2,        // Limb2
    PONYTAIL_BODYPART_LIMB3,        // Limb3
    PONYTAIL_BODYPART_LIMB4,        // Limb4
    PONYTAIL_BODYPART_LIMB5,        // Limb5 (last limb)
    PONYTAIL_BODYPART_MAX
} PonytailBodyPart;

PhysLimb ponytailRoot;
PhysLimb ponytailUpperControl;
PhysLimb ponytailHead;
PhysLimb ponytailLimb1;
PhysLimb ponytailLimb2;
PhysLimb ponytailLimb3;
PhysLimb ponytailLimb4;
PhysLimb ponytailLimb5;

PhysLimb* ponytailPhysLimbs[8] = {
    &ponytailRoot,
    &ponytailUpperControl,
    &ponytailHead,
    &ponytailLimb1,
    &ponytailLimb2,
    &ponytailLimb3,
    &ponytailLimb4,
    &ponytailLimb5
};

PhysBone ponytailRootLimbUpperControlLimb;
PhysBone ponytailUpperControlLimbHeadLimb;
PhysBone ponytailHeadLimbLimb1;
PhysBone ponytailLimb1Limb2;
PhysBone ponytailLimb2Limb3;
PhysBone ponytailLimb3Limb4;
PhysBone ponytailLimb4Limb5;

typedef enum PonytailBoneIndex {
    PONYTAIL_BONE_ROOT_UPPERCONTROL,    // Root & Upper Control
    PONYTAIL_BONE_UPPERCONTROL_HEAD,    // Upper Control & Head
    PONYTAIL_BONE_HEAD_LIMB1,           // Head & Limb 1
    PONYTAIL_BONE_LIMB1_LIMB2,          // Limb1 & Limb2
    PONYTAIL_BONE_LIMB2_LIMB3,          // Limb2 & Limb3
    PONYTAIL_BONE_LIMB3_LIMB4,          // Limb3 & LimB4
    PONYTAIL_BONE_LIMB4_LIMB5,          // Limb4 & LimB5
    PONYTAIL_BONE_MAX                   // Limb5
} PonytailBoneIndex;

PhysBone* ponytailPhysBones[7] = {
    &ponytailRootLimbUpperControlLimb,
    &ponytailUpperControlLimbHeadLimb,
    &ponytailHeadLimbLimb1,
    &ponytailLimb1Limb2,
    &ponytailLimb2Limb3,
    &ponytailLimb3Limb4,
    &ponytailLimb4Limb5
};

struct Ponytail;

typedef void (*PonytailActionFunc)(struct Ponytail*, struct PlayState*);

typedef struct Ponytail {
    Actor actor;
    SkelAnime skelAnime;
    Vec3s jointTable[GPONYTAILSKEL_NUM_LIMBS];
    Vec3s morphTable[GPONYTAILSKEL_NUM_LIMBS];
    Vec3f bodyPartsPos[PONYTAIL_BODYPART_MAX];
    ColliderJntSph collider;
} Ponytail;

// Ponytail manipulation based on Verlet Integration
void Ponytail_RotateJoints(Ponytail* this, Player* player, PhysBone* gPhysBones[]);
void Ponytail_SetDefaultBodyPartsPos(Ponytail* this, Player* player, StandardLimb* gLimbs[], PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]) ;
void Ponytail_UpdateBodyPartsPos(Ponytail* this, Player* player, Vec3f apply_force, StandardLimb* gLimbs[], PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]);

#endif