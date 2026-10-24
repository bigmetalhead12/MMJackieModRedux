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
#include "physics_collision.h"


/***********************************************************************

	Belt Strap Hook Physics

***********************************************************************/
// Set up physics object that carries velocity and rotation of belt strap hook actor
PhysPlayer gBeltStrapHookPhysPlayer;

// Set up physics object that carries Epona movement for belt strap hook actor
PhysEponaMovement gBeltStrapHookEpona;

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

typedef struct BeltStrapHookRotationSettings {
    f32 resetAngle; // Degrees relative to torso outward: positive up, negative down
    s16 defUpperChainLimAngle;
    s16 defLowerChainLimAngle;
    s16 currUpperChainLimAngle;
    s16 currLowerChainLimAngle;
    u8 numTopBonesWithNoInwardRot;
} BeltStrapHookRotationSettings;

typedef struct BeltStrapHook {
    Actor actor;
    SkelAnime skelAnime;
    Vec3s jointTable[GBELTSTRAPHOOKSKEL_NUM_LIMBS];
    Vec3s morphTable[GBELTSTRAPHOOKSKEL_NUM_LIMBS];
    Vec3f bodyPartsPos[BELTSTRAPHOOK_BODYPART_MAX];
    BeltStrapHookRotationSettings rotLimits;
    u8 needsReset;
    u8 wasTransformingIntoGiant;
    u8 wasGiant;
    u8 wasKamaroDancing;
} BeltStrapHook;

typedef struct BeltStrapHookLimbChain {
    s16 startLimb;   // First phys limb in the chain, which is pinned
    s16 limbCount;   // Number of phys limbs, including the pinned limb
    s16 startBone;   // First phys bone, connecting the root to the pinned limb
} BeltStrapHookLimbChain;


void BeltStrapHook_Init(Actor* thisx, PlayState* play);
void BeltStrapHook_Destroy(Actor* thisx, PlayState* play);
void BeltStrapHook_Update(Actor* thisx, PlayState* play);
void BeltStrapHook_Draw(Actor* thisx, PlayState* play);

void BeltStrapHook_InitChain(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[], int startLimb, int limbCount, int startBone);
void BeltStrapHook_SetDefaultBodyPartsPos(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[]);
u8 BeltStrapHook_IsKamaroDancing(Player* player);
u8 BeltStrapHook_IsInFirstPerson(PlayState* play, Player* player);
void BeltStrapHook_SetDefaultRotLimitsAndReset(BeltStrapHook* this);
void BeltStrapHook_SetResetAngle(BeltStrapHook* this, f32 reset_angle);
void BeltStrapHook_SetRotLimitAngles(BeltStrapHook* this, f32 upper_chain_lim, f32 lower_chain_lim);
void BeltStrapHook_UpdateRotLimitsAndReset(BeltStrapHook* this, Player* player);
void BeltStrapHook_CaptureLimitAxes(MtxF* mtx);
void BeltStrapHook_ApplyBoneRotationLimit(PhysLimb* parentPhysLimb, PhysLimb* childPhysLimb, 
    Vec3f* outwardDirVec, u8 limitInward, s16 maxUpwardRotLim);
void BeltStrapHook_ApplyChainRotationLimits(PhysLimb* gPhysLimbs[], const BeltStrapHookLimbChain chainStarts[],
    int chainCount, const BeltStrapHookRotationSettings* limits);
bool BeltStrapHook_NormalizeSubstepDirectionVec(Vec3f* targetVec);
Vec3f BeltStrapHook_FindClosestPointOnCapsule(Vec3f targetPos, PhysSphereCollider* capsuleColStart, 
    PhysSphereCollider* capsuleColEnd, f32* colRadius);
f32 BeltStrapHook_GroupContact(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* capsulePairs[][2], 
    int capsuleColCount, Vec3f* contactColCenter, f32* contactColRadius);
bool BeltStrapHook_GroupOutside(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* capsulePairs[][2], 
    int capsuleColCount);
bool BeltStrapHook_GroupRayExit(Vec3f targetPhysLimbPos, Vec3f physLimbDir, f32 pointRadius,
    PhysSphereCollider* capsulePairs[][2], int capsuleColCount, f32 padding, Vec3f* exitPosition);
void BeltStrapHook_SolveColliderGroup(PhysLimb* physLimb, PhysSphereCollider* collider_pairs[][2], 
    int capsuleColCount);
void BeltStrapHook_SetResetShape(BeltStrapHook* this, PhysLimb* gPhysLimbs[], 
    const BeltStrapHookLimbChain chains[], int chainCount);
void BeltStrapHook_RotatePointAroundY(Vec3f* targetPoint, Vec3f* pivotPoint, s16 rotAngle);
void BeltStrapHook_InterpolatePosWithRot(Vec3f* prevFramePos, Vec3f* currFrameTargetPos,
	Vec3f* prevFramePivotPoint, Vec3f* currFramePivotPoint, s16 yawRotation, f32 t, 
	Vec3f* currSubstepNewPos);
void BeltStrapHook_SetSubstepDirAxes(Vec3f targetForward, Vec3f targetDown, f32 t);
void BeltStrapHook_UpdateChain(BeltStrapHook* this, Player* player, Vec3f* apply_force, PhysLimb* gPhysLimbs[], 
	int chainStartIdx, int chainEndIdx);
void BeltStrapHook_UpdatePinnedLimb(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], 
    int pinnedLimbIdx);
void BeltStrapHook_UpdateColliderRadii(Player* player);
void BeltStrapHook_SolvePhysAtSubsteps(BeltStrapHook* this, Player* player, Vec3f apply_force, 
    PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[], Vec3f limbsPrevPos[], 
    const BeltStrapHookLimbChain chains[], int chainCount, PhysSphereCollider* capsulePairs[][2], 
    int capsuleCount, int substepCount);
void BeltStrapHook_UpdateBodyPartsPos(BeltStrapHook* this, Player* player, Vec3f apply_force, 
	PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]);
void BeltStrapHook_RotateChain(BeltStrapHook* this, PhysBone* gPhysBones[], int chainStartIdx, int chainEndIdx);
void BeltStrapHook_RotateJoints(BeltStrapHook* this, PhysBone* gPhysBones[], Player* player);

#endif