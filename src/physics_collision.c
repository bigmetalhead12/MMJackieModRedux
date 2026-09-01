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

// MACROS
#define MIN_DIST    0.0001f

#include "physics_collision.h"
#include "customMath.h"

// remove later
#include "ultra64.h"
#include "global.h"

#include "recomputils.h"

/***********************************************************************

	Physics Collision System

***********************************************************************/

// Check if target bone is colliding with given sphere collider
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

// Check if target limb is colliding with given sphere collider
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

// Push limb out of a sphere collider in the direction of sphere collider's center to limb's current position inside collider
void PhysCol_SolveCollision(PhysLimb* limb, PhysSphereCollider* collider) {
    if (limb->pinned) {
        return;
    }

    // Find direction to push limb out of sphere collider (sphere collider center to limb's current position)
    Vec3f dir = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&limb->curr_pos, &collider->center, &dir);

    // Find minimum distance that should be between limb and collider to make them not collide
    f32 distSquared = Math3D_Vec3fMagnitudeSq(&dir);
    f32 minDist = limb->collision_radius + collider->radius;
    f32 minDistSquared = minDist * minDist;

    // If limb and collider do not overlap, return
    if (distSquared >= minDistSquared || distSquared < MIN_DIST) {
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


// Push limb out of capsule collider in the direction of capsule's inside to limb's current position inside collider
void PhysCol_SolveCapsuleFromSpheres(PhysLimb* limb, PhysSphereCollider* sphereA, PhysSphereCollider* sphereB) {
    // If input limb is pinned, exit function since this limb should not move
    if (limb->pinned) {
        return;
    }

    // Segment from sphere A center to sphere B center
    Vec3f segment = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Diff(&sphereB->center, &sphereA->center, &segment);
    f32 segLengthSq = Math3D_Vec3fMagnitudeSq(&segment);

    // If both sphere centers are basically in identical positions, just fall back to sphere A collision
    if (segLengthSq < MIN_DIST) {
        PhysCol_SolveCollision(limb, sphereA);
        return;
    }

    // Vector from sphere A center to limb
    Vec3f sphereToLimb = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Diff(&limb->curr_pos, &sphereA->center, &sphereToLimb);

    // Find percentage along the segment where point in segment is closest to limb
    f32 t = CustomMath_Vec3f_Dot(&sphereToLimb, &segment) / segLengthSq;

    // Keep the closest point between sphere A and sphere B
    if (t < 0.0f) {
        t = 0.0f;
    }
    else if (t > 1.0f) {
        t = 1.0f;
    }

    // Find closest point on center segment using t
    Vec3f closest = { sphereA->center.x + (segment.x * t), sphereA->center.y + (segment.y * t), sphereA->center.z + (segment.z * t) };

    // Interpolate radius between sphere A and sphere B (in case radius is different for both Sphere A and Sphere B)
    f32 capsuleRadius = sphereA->radius + ((sphereB->radius - sphereA->radius) * t);

    // Treat this point along the capsule as a temporary sphere center
    PhysSphereCollider tempSphere = { closest, capsuleRadius };

    // Reuse normal sphere collision solver
    PhysCol_SolveCollision( limb, &tempSphere );
}