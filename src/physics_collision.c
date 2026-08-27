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

#include "physics_collision.h"
#include "customMath.h"

// remove later
#include "ultra64.h"
#include "global.h"

#include "recomputils.h"

/***********************************************************************

	Physics Collision System

***********************************************************************/

u8 PhysCol_boneIsColliding(PhysBone* target_bone, PhysSphereCollider* sphere_collider) {
    // Check for contact between bone and sphere collider by checking bone's limb b's path from prev point to curr point
    Vec3f point_in_bone_path = { 0.f, 0.f, 0.f };
    CustomMath_Vec3f_ClosestPoint(&sphere_collider->center, &target_bone->limb_b->prev_pos, &target_bone->limb_b->curr_pos, &point_in_bone_path);

    f32 dist_bone_sphere = Math_Vec3f_DistXYZ(&point_in_bone_path, &sphere_collider->center);

    if (dist_bone_sphere <= sphere_collider->radius) {
        return 1;
    }
    else {
        return 0;
    }

}

u8 PhysCol_limbIsColliding(PhysLimb* target_limb, PhysSphereCollider* sphere_collider) {
    Vec3f point_in_limb_path = { (f32)0, (f32)0, (f32)0 };

    CustomMath_Vec3f_ClosestPoint(&sphere_collider->center, &target_limb->prev_pos, &target_limb->curr_pos, &point_in_limb_path);

    f32 dist_limb_sphere = Math_Vec3f_DistXYZ(&point_in_limb_path, &sphere_collider->center);

    if (dist_limb_sphere <= sphere_collider->radius) {
        return 1;
    }
    else {
        return 0;
    }
}

// Push limb out of collider 
void PhysCol_SolveCollision(PhysLimb* limb, PhysSphereCollider* collider) {
    if (limb->pinned) {
        return;
    }

    // Find direction to push limb out of sphere collider (sphere collider center to limb's current position)
    Vec3f dir = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&limb->curr_pos, &collider->center, &dir);

    // Find minimum distance that should be between limb and collider to make them not collide
    f32 distSquared = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);
    f32 minDist = limb->collision_radius + collider->radius;
    f32 minDistSquared = minDist * minDist;

    // If limb and collider do not overlap, return
    if (distSquared >= minDistSquared || distSquared < 0.0001f) {
        return;
    }

    // Find vector from limb's position to middle point, with length of 1/2 min distance
    Vec3f toLimb = { (f32)0, (f32)0, (f32)0 };
    Vec3f dirFromLimbToMiddle = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&limb->curr_pos, &collider->center, &toLimb);
    CustomMath_Vec3f_Normalize(&toLimb, &toLimb);
    Math_Vec3f_Scale(&toLimb, minDist);

    // Override current positions so that limb and collider don't collide
    Vec3f newPos = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Sum(&collider->center, &toLimb, &newPos);
    Math_Vec3f_Copy(&limb->curr_pos, &newPos);
}
