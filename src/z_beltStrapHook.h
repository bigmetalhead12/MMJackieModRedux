/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

#ifndef Z_BELTSTRAPHOOK_H
#define Z_BELTSTRAPHOOK_H

#include "ultra64.h"
#include "global.h"
#include "gBeltStrapHookSkel.h"
#include "verlet_physics.h"


/***********************************************************************

	Belt Strap Hook Physics

***********************************************************************/
// Set up physics object that carries velocity and rotation of belt strap hook actor
PhysPlayer gBeltStrapHookPhysPlayer;

// StandardLimb of belt strap hook's model generated through Fast 64 (from gBeltStrapHookSkel)
extern StandardLimb gBeltStrapHookSkelLimb_000;     // Root
extern StandardLimb gBeltStrapHookSkelLimb_001;     // First limb of left chain of limbs
extern StandardLimb gBeltStrapHookSkelLimb_002;
extern StandardLimb gBeltStrapHookSkelLimb_003;
extern StandardLimb gBeltStrapHookSkelLimb_004;
extern StandardLimb gBeltStrapHookSkelLimb_005;
extern StandardLimb gBeltStrapHookSkelLimb_006;
extern StandardLimb gBeltStrapHookSkelLimb_007;
extern StandardLimb gBeltStrapHookSkelLimb_008;
extern StandardLimb gBeltStrapHookSkelLimb_009;
extern StandardLimb gBeltStrapHookSkelLimb_010;
extern StandardLimb gBeltStrapHookSkelLimb_011;
extern StandardLimb gBeltStrapHookSkelLimb_012;     // First limb of right chain of limbs
extern StandardLimb gBeltStrapHookSkelLimb_013;
extern StandardLimb gBeltStrapHookSkelLimb_014;
extern StandardLimb gBeltStrapHookSkelLimb_015;
extern StandardLimb gBeltStrapHookSkelLimb_016;
extern StandardLimb gBeltStrapHookSkelLimb_017;
extern StandardLimb gBeltStrapHookSkelLimb_018;
extern StandardLimb gBeltStrapHookSkelLimb_019;
extern StandardLimb gBeltStrapHookSkelLimb_020;
extern StandardLimb gBeltStrapHookSkelLimb_021;
extern StandardLimb gBeltStrapHookSkelLimb_022;

// Enum for indicating belt strap hook's skeleton limbs
typedef enum BeltStrapHookBodyPart {
    /* 0x00 */ BELTSTRAPHOOK_BODYPART_ROOT,         // Root
    /* 0x01 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB1,    // Left Limb1
    /* 0x02 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB2,    // Left Limb2
    /* 0x03 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB3,    // Left Limb3
    /* 0x04 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB4,    // Left Limb4
    /* 0x05 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB5,    // Left Limb5
    /* 0x06 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB6,    // Left Limb6
    /* 0x07 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB7,    // Left Limb7
    /* 0x08 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB8,    // Left Limb8
    /* 0x09 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB9,    // Left Limb9
    /* 0x10 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB10,   // Left Limb10
    /* 0x11 */ BELTSTRAPHOOK_BODYPART_LEFTLIMB11,   // Left Limb11
    /* 0x12 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB1,   // Right Limb1
    /* 0x13 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB2,   // Right Limb2
    /* 0x14 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB3,   // Right Limb3
    /* 0x15 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB4,   // Right Limb4
    /* 0x16 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB5,   // Right Limb5
    /* 0x17 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB6,   // Right Limb6
    /* 0x18 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB7,   // Right Limb7
    /* 0x19 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB8,   // Right Limb8
    /* 0x20 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB9,   // Right Limb9
    /* 0x21 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB10,  // Right Limb10
    /* 0x22 */ BELTSTRAPHOOK_BODYPART_RIGHTLIMB11,  // Right Limb11
    /* 0x23 */ BELTSTRAPHOOK_BODYPART_MAX
} BeltStrapHookBodyPart;

// Phys limb for each belt strap hook's skeleton limb
PhysLimb beltStrapHook_RootLimb;
PhysLimb beltStrapHook_LeftLimb1;
PhysLimb beltStrapHook_LeftLimb2;
PhysLimb beltStrapHook_LeftLimb3;
PhysLimb beltStrapHook_LeftLimb4;
PhysLimb beltStrapHook_LeftLimb5;
PhysLimb beltStrapHook_LeftLimb6;
PhysLimb beltStrapHook_LeftLimb7;
PhysLimb beltStrapHook_LeftLimb8;
PhysLimb beltStrapHook_LeftLimb9;
PhysLimb beltStrapHook_LeftLimb10;
PhysLimb beltStrapHook_LeftLimb11;
PhysLimb beltStrapHook_RightLimb1;
PhysLimb beltStrapHook_RightLimb2;
PhysLimb beltStrapHook_RightLimb3;
PhysLimb beltStrapHook_RightLimb4;
PhysLimb beltStrapHook_RightLimb5;
PhysLimb beltStrapHook_RightLimb6;
PhysLimb beltStrapHook_RightLimb7;
PhysLimb beltStrapHook_RightLimb8;
PhysLimb beltStrapHook_RightLimb9;
PhysLimb beltStrapHook_RightLimb10;
PhysLimb beltStrapHook_RightLimb11;

// Struct containing phys limbs of belt strap hook
PhysLimb* beltStrapHook_PhysLimbs[23] = {
    &beltStrapHook_RootLimb,
    &beltStrapHook_LeftLimb1,
    &beltStrapHook_LeftLimb2,
    &beltStrapHook_LeftLimb3,
    &beltStrapHook_LeftLimb4,
    &beltStrapHook_LeftLimb5,
    &beltStrapHook_LeftLimb6,
    &beltStrapHook_LeftLimb7,
    &beltStrapHook_LeftLimb8,
    &beltStrapHook_LeftLimb9,
    &beltStrapHook_LeftLimb10,
    &beltStrapHook_LeftLimb11,
    &beltStrapHook_RightLimb1,
    &beltStrapHook_RightLimb2,
    &beltStrapHook_RightLimb3,
    &beltStrapHook_RightLimb4,
    &beltStrapHook_RightLimb5,
    &beltStrapHook_RightLimb6,
    &beltStrapHook_RightLimb7,
    &beltStrapHook_RightLimb8,
    &beltStrapHook_RightLimb9,
    &beltStrapHook_RightLimb10,
    &beltStrapHook_RightLimb11
};

// Phys bone for belt strap hook's skeleton limbs, where phys bone contains 2 phys limbs
PhysBone beltStrapHook_RootLimbLeftLimb1;
PhysBone beltStrapHook_LeftLimb1LeftLimb2;
PhysBone beltStrapHook_LeftLimb2LeftLimb3;
PhysBone beltStrapHook_LeftLimb3LeftLimb4;
PhysBone beltStrapHook_LeftLimb4LeftLimb5;
PhysBone beltStrapHook_LeftLimb5LeftLimb6;
PhysBone beltStrapHook_LeftLimb6LeftLimb7;
PhysBone beltStrapHook_LeftLimb7LeftLimb8;
PhysBone beltStrapHook_LeftLimb8LeftLimb9;
PhysBone beltStrapHook_LeftLimb9LeftLimb10;
PhysBone beltStrapHook_LeftLimb10LeftLimb11;
PhysBone beltStrapHook_RootLimbRightLimb1;
PhysBone beltStrapHook_RightLimb1RightLimb2;
PhysBone beltStrapHook_RightLimb2RightLimb3;
PhysBone beltStrapHook_RightLimb3RightLimb4;
PhysBone beltStrapHook_RightLimb4RightLimb5;
PhysBone beltStrapHook_RightLimb5RightLimb6;
PhysBone beltStrapHook_RightLimb6RightLimb7;
PhysBone beltStrapHook_RightLimb7RightLimb8;
PhysBone beltStrapHook_RightLimb8RightLimb9;
PhysBone beltStrapHook_RightLimb9RightLimb10;
PhysBone beltStrapHook_RightLimb10RightLimb11;

// Enum for indicating belt strap hook's bones (where each bone contains 2 limbs)
typedef enum BeltStrapHook_BoneIndex {
    /* 0x00 */ BELTSTRAPHOOK_BONE_ROOT_LEFT_LIMB1,              // Root & Left Limb1
    /* 0x01 */ BELTSTRAPHOOK_BONE_LEFT_LIMB1_LEFT_LIMB2,        // Left Limb1 & Left Limb2
    /* 0x02 */ BELTSTRAPHOOK_BONE_LEFT_LIMB2_LEFT_LIMB3,        // Left Limb2 & Left Limb3
    /* 0x03 */ BELTSTRAPHOOK_BONE_LEFT_LIMB3_LEFT_LIMB4,        // Left Limb3 & Left Limb4
    /* 0x04 */ BELTSTRAPHOOK_BONE_LEFT_LIMB4_LEFT_LIMB5,        // Left Limb4 & Left Limb5
    /* 0x05 */ BELTSTRAPHOOK_BONE_LEFT_LIMB5_LEFT_LIMB6,        // Left Limb5 & Left Limb6
    /* 0x06 */ BELTSTRAPHOOK_BONE_LEFT_LIMB6_LEFT_LIMB7,        // Left Limb6 & Left Limb7
    /* 0x07 */ BELTSTRAPHOOK_BONE_LEFT_LIMB7_LEFT_LIMB8,        // Left Limb7 & Left Limb8
    /* 0x08 */ BELTSTRAPHOOK_BONE_LEFT_LIMB8_LEFT_LIMB9,        // Left Limb8 & Left Limb9
    /* 0x09 */ BELTSTRAPHOOK_BONE_LEFT_LIMB9_LEFT_LIMB10,       // Left Limb9 & Left Limb10
    /* 0x10 */ BELTSTRAPHOOK_BONE_LEFT_LIMB10_LEFT_LIMB11,      // Left Limb10 & Left Limb11
    /* 0x11 */ BELTSTRAPHOOK_BONE_ROOT_RIGHT_LIMB1,             // Root & Right Limb1
    /* 0x12 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB1_RIGHT_LIMB2,      // Right Limb1 & Right Limb2
    /* 0x13 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB2_RIGHT_LIMB3,      // Right Limb2 & Right Limb3
    /* 0x14 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB3_RIGHT_LIMB4,      // Right Limb3 & Right Limb4
    /* 0x15 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB4_RIGHT_LIMB5,      // Right Limb4 & Right Limb5
    /* 0x16 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB5_RIGHT_LIMB6,      // Right Limb5 & Right Limb6
    /* 0x17 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB6_RIGHT_LIMB7,      // Right Limb6 & Right Limb7
    /* 0x18 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB7_RIGHT_LIMB8,      // Right Limb7 & Right Limb8
    /* 0x19 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB8_RIGHT_LIMB9,      // Right Limb8 & Right Limb9
    /* 0x20 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB9_RIGHT_LIMB10,     // Right Limb9 & Right Limb10
    /* 0x21 */ BELTSTRAPHOOK_BONE_RIGHT_LIMB10_RIGHT_LIMB11,    // Right Limb10 & Right Limb11
    /* 0x22 */ BELTSTRAPHOOK_BONE_MAX                           
} BeltStrapHook_BoneIndex;

// Struct containing phys bones of belt strap hook
PhysBone* beltStrapHook_PhysBones[BELTSTRAPHOOK_BONE_MAX] = {
    &beltStrapHook_RootLimbLeftLimb1,       // Left chain of phys bones
    &beltStrapHook_LeftLimb1LeftLimb2,
    &beltStrapHook_LeftLimb2LeftLimb3,
    &beltStrapHook_LeftLimb3LeftLimb4,
    &beltStrapHook_LeftLimb4LeftLimb5,
    &beltStrapHook_LeftLimb5LeftLimb6,
    &beltStrapHook_LeftLimb6LeftLimb7,
    &beltStrapHook_LeftLimb7LeftLimb8,
    &beltStrapHook_LeftLimb8LeftLimb9,
    &beltStrapHook_LeftLimb9LeftLimb10,
    &beltStrapHook_LeftLimb10LeftLimb11,
    &beltStrapHook_RootLimbRightLimb1,      // Right chain of phys bones
    &beltStrapHook_RightLimb1RightLimb2,
    &beltStrapHook_RightLimb2RightLimb3,
    &beltStrapHook_RightLimb3RightLimb4,
    &beltStrapHook_RightLimb4RightLimb5,
    &beltStrapHook_RightLimb5RightLimb6,
    &beltStrapHook_RightLimb6RightLimb7,
    &beltStrapHook_RightLimb7RightLimb8,
    &beltStrapHook_RightLimb8RightLimb9,
    &beltStrapHook_RightLimb9RightLimb10,
    &beltStrapHook_RightLimb10RightLimb11
};

// BeltStrapHook struct 
struct BeltStrapHook;

typedef void (*BeltStrapHookActionFunc)(struct BeltStrapHook*, struct PlayState*);

typedef struct BeltStrapHook {
    Actor actor;
    SkelAnime skelAnime;
    Vec3s jointTable[GBELTSTRAPHOOKSKEL_NUM_LIMBS];
    Vec3s morphTable[GBELTSTRAPHOOKSKEL_NUM_LIMBS];
    Vec3f bodyPartsPos[BELTSTRAPHOOK_BODYPART_MAX];
    ColliderJntSph collider;
    u8 needsReset;
    u8 hideInFirstPerson;
} BeltStrapHook;


void BeltStrapHook_Init(Actor* thisx, PlayState* play);
void BeltStrapHook_Destroy(Actor* thisx, PlayState* play);
void BeltStrapHook_Update(Actor* thisx, PlayState* play);
void BeltStrapHook_Draw(Actor* thisx, PlayState* play);

void BeltStrapHook_InitChain(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], int chainStartIdx,
	int chainEndIdx);
void BeltStrapHook_InitChainBones(PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[], int chainStartIdx,
    int chainEndIdx, int boneStartIdx);
void BeltStrapHook_UpdateChain(BeltStrapHook* this, Player* player, Vec3f* apply_force, 
    PhysLimb* gPhysLimbs[], int chainStartIdx, int chainEndIdx);

void BeltStrapHook_RotateJoints(BeltStrapHook* this, PhysBone* gPhysBones[], Player* player);
void BeltStrapHook_SetDefaultBodyPartsPos(BeltStrapHook* this, Player* player, 
    PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]) ;
void BeltStrapHook_UpdateBodyPartsPos(BeltStrapHook* this, Player* player, Vec3f apply_force, 
    PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]);

#endif