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

StandardLimb* gPonytailLimbs[6] = {
    &gPonytailSkelLimb_000,
    &gPonytailSkelLimb_001, 
    &gPonytailSkelLimb_002,
    &gPonytailSkelLimb_003,
    &gPonytailSkelLimb_004,
    &gPonytailSkelLimb_005
};

typedef enum PonytailBodyPart {
    /* 0x00 */ PONYTAIL_BODYPART_ROOT,      // Root
    /* 0x01 */ PONYTAIL_BODYPART_LIMB1,     // Limb1
    /* 0x02 */ PONYTAIL_BODYPART_LIMB2,     // Limb2
    /* 0x03 */ PONYTAIL_BODYPART_LIMB3,     // Limb3
    /* 0x04 */ PONYTAIL_BODYPART_LIMB4,     // Limb4
    /* 0x05 */ PONYTAIL_BODYPART_LIMB5,     // Limb5 (last limb)
    /* 0x06 */ PONYTAIL_BODYPART_MAX
} PonytailBodyPart;

PhysLimb ponytailRootLimb;
PhysLimb ponytailLimb1;
PhysLimb ponytailLimb2;
PhysLimb ponytailLimb3;
PhysLimb ponytailLimb4;
PhysLimb ponytailLimb5;

PhysLimb* ponytailPhysLimbs[6] = {
    &ponytailRootLimb,
    &ponytailLimb1,
    &ponytailLimb2,
    &ponytailLimb3,
    &ponytailLimb4,
    &ponytailLimb5
};

PhysBone ponytailRootLimbLimb1;
PhysBone ponytailLimb1Limb2;
PhysBone ponytailLimb2Limb3;
PhysBone ponytailLimb3Limb4;
PhysBone ponytailLimb4Limb5;

typedef enum PonytailBoneIndex {
    /* 0x00 */ PONYTAIL_BONE_ROOT_LIMB1,    // Root & Limb1
    /* 0x01 */ PONYTAIL_BONE_LIMB1_LIMB2,   // Limb1 & Limb2
    /* 0x02 */ PONYTAIL_BONE_LIMB2_LIMB3,   // Limb2 & Limb3
    /* 0x03 */ PONYTAIL_BONE_LIMB3_LIMB4,   // Limb3 & LimB4
    /* 0x04 */ PONYTAIL_BONE_LIMB4_LIMB5,   // Limb4 & LimB5
    /* 0x05 */ PONYTAIL_BONE_MAX            // Limb5
} PonytailBoneIndex;

PhysBone* ponytailPhysBones[5] = {
    &ponytailRootLimbLimb1,
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

// Custom Math functions not available in MM decomp library
s16 CustomMath_Vec3f_Pitch(Vec3f* b, Vec3f* a);
s16 CustomMath_Vec3f_Yaw(Vec3f* a, Vec3f* b);
void CustomMath_Vec3s_Sum(Vec3s* l, Vec3s* r, Vec3s* dest);
void CustomMath_Vec3s_Diff(Vec3s* l, Vec3s* r, Vec3s* dest);
void CustomMath_Vec3s_Scale_ToVec3f(Vec3s* target, f32 scale, Vec3f* dest);
void CustomMath_Vec3s_RotateByX(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_RotateByY(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_RotateByZ(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_Rotate(Vec3s* src, Vec3s* rotAngle, Vec3s* dest);

// Ponytail manipulation based on Verlet Integration
void Ponytail_RotateJoints(Ponytail* this, PhysBone* gPhysBones[]);
void Ponytail_SetDefaultBodyPartsPos(Ponytail* this, Player* player, StandardLimb* gLimbs[], PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]) ;
void Ponytail_UpdateBodyPartsPos(Ponytail* this, Player* player, Vec3f apply_force, StandardLimb* gLimbs[], PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]);

#endif