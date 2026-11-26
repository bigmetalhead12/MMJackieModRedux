/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Physics collision system to be used alongside verlet physics designed for Majora's Mask Recomp

========================================================================
*/

#ifndef Z_PHYS_COLLISION_H
#define Z_PHYS_COLLISION_H

#include "math.h"
#include "ultra64.h"
#include "global.h"
#include "verlet_physics.h"


/***********************************************************************

	Physics Collision System

***********************************************************************/
typedef struct {
    Vec3f localOffset;
    f32 tempRadius;
} PhysSphereSettings;


typedef struct {
    Vec3f center;
    f32 radius;
    PhysSphereSettings settings;
    u8 enabled;
} PhysSphereCollider;


typedef enum {
    PHYS_COLLIDER_CONSTANT,             // Always enabled and uses default settings
    PHYS_COLLIDER_APPEAR_WHEN,          // Collider will only appear in certain condition
    PHYS_COLLIDER_DISAPPEAR_WHEN,       // Collider will disappear in certain conditions
    PHYS_COLLIDER_SWITCH_SETTINGS_WHEN  // Collider settings will switch based on current condition
} PhysColliderBehavior;


typedef struct {
    u8 (*customCheck)(Player* player);  // Store boolean function that checks if specified animation or state is true
    // Checks a gameplay condition: animations, state flags, etc.
} PhysColliderCondition;


typedef struct {
    PhysSphereSettings* settings;
    PhysColliderCondition* condition;
} PhysSphereAltSettingsAndCondition;


typedef struct {
    PhysSphereCollider* collider;
    int limbIndex;

    PhysColliderBehavior behavior;      // Determine what kind of collider this would be

    PhysSphereSettings* defaultSettings;
    PhysSphereAltSettingsAndCondition* altSettingsAndConditions;    // Array of pairs of collider setting and condition
    int altSettingsAndConditionsCount;
} PhysSphereColliderEntry;


typedef struct {
    // Two endpoint sphere colliders making up capsule collider
    PhysSphereCollider endpoints[2];

    // Measurements based on these endpoint sphere colliders
    Vec3f axis;             // Direction and distance vector from starting endpoint[0]'s center to ending endpoint[1]'s center
    f32 radiusDifference;   // Endpoint[1]'s radius minus endpoint[0]'s radius
    f32 maxRadius;          // Larger of the two endpoints' radius
} PhysCapsuleCollider;


void PhysCol_SolveCollision(PhysLimb* limb, PhysSphereCollider* collider);
void PhysCol_SolveCapsuleFromSpheres(PhysLimb* limb, PhysSphereCollider* sphereA, PhysSphereCollider* sphereB);

void PhysCol_UpdateCapsule(PhysCapsuleCollider* capsuleCollider, PhysSphereCollider* startEndpoint, 
    PhysSphereCollider* endEndpoint);
Vec3f PhysCol_FindClosestPointOnCapsule(Vec3f targetPos, PhysCapsuleCollider* capsuleCollider, f32* colRadius);
f32 PhysCol_GroupContact(Vec3f targetPos, f32 pointRadius, PhysCapsuleCollider* capsulesCols, int capsuleColCount,
    Vec3f* contactColCenter, f32* contactColRadius);
u8 PhysCol_GroupOutside(Vec3f targetPos, f32 pointRadius, PhysCapsuleCollider* capsuleCols, int capsuleColCount);
u8 PhysCol_GroupRayExit(Vec3f targetPos, Vec3f physLimbDir, f32 pointRadius, PhysCapsuleCollider* capsulesCol,
    int capsuleColCount, f32 padding, int exitSearchSteps, Vec3f* exitPosition);

void PhysCol_InitSphereColliderEntries(PhysSphereColliderEntry* sphereColliderEntries, int entryCount);
void PhysCol_ApplySphereColliderEntries(PhysSphereColliderEntry* sphereColliderEntries, int entryCount, 
    Player* player);
void PhysCol_UpdateSphereCentersForLimb(PhysSphereColliderEntry* sphereColliderEntries, int entryCount, 
    int limbIndex);
void PhysCol_UpdateSphereRadii(PhysSphereColliderEntry* sphereColliderEntries, int entryCount, f32 colliderScale);
s16 PhysCol_BuildActiveCapsulePairs(PhysSphereCollider* potentialPairs[][2], int potentialPairCount,
    PhysSphereCollider* activePairs[][2]);
u8 PhysCol_NormalizeSubstepDirectionVec(Vec3f* targetVec);
void PhysCol_SolveColliderGroup(PhysLimb* physLimb, PhysCapsuleCollider* capsuleCols, int capsuleColCount,
    Vec3f fallbackDirection, int exitSearchSteps);
void PhysCol_SolveMidpointGroup(PhysLimb* physLimb_a, PhysLimb* physLimb_b, PhysCapsuleCollider* capsuleCols,
    int capsuleColCount, Vec3f fallbackDirection, int exitSearchSteps);

#endif