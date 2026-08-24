/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Physics collision system to be used alongside verlet physics designed for 
    Majora's Mask recomp    

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
    Vec3f center;
    f32 radius;
} PhysSphereCollider;


u8 PhysCol_boneIsColliding(PhysBone* target_bone, PhysSphereCollider* sphere_collider);
u8 PhysCol_limbIsColliding(PhysLimb* target_limb, PhysSphereCollider* sphere_collider);

void PhysCol_SphereCollider(PhysBone* target_bone, PhysSphereCollider* sphere_collider);
//void PhysCol_SphereCollider(PhysLimb* target_limb, PhysSphereCollider* sphere_collider);


#endif