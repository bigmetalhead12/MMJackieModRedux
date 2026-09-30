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


void PhysCol_SolveCollision(PhysLimb* limb, PhysSphereCollider* collider);
void PhysCol_SolveCapsuleFromSpheres(PhysLimb* limb, PhysSphereCollider* sphereA, PhysSphereCollider* sphereB);

#endif