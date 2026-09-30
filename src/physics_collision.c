/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Physics collision system to be used alongside verlet physics designed for Majora's Mask recomp    

========================================================================
*/

// MACROS
#define GREATER_THAN_ZERO    0.0001f

#include "physics_collision.h"
#include "customMath.h"

// remove later
#include "ultra64.h"
#include "global.h"

#include "recomputils.h"

/***********************************************************************

	Physics Collision System

***********************************************************************/

/**
 * @brief Push target phys limb out of a sphere collider in the direction of the sphere collider's center to phys
 *        limb's current position inside the collider.
 * 
 * This function finds the direction from the sphere collider's center to the target phys limb's position if said
 * phys limb is inside the collider. Afterward, the phys limb is pushed out in that direction so that it is on
 * the surface. 
 * 
 * @param targetPhysLimb    Phys limb that is to be pushed out of target collider
 * @param targetCollider    Collider that the target phys limb needs to be pushed out of
 */
void PhysCol_SolveCollision(PhysLimb* targetPhysLimb, PhysSphereCollider* targetCollider) {
    // If phys limb is pinned, then do not solve collision and leave function.
    if (targetPhysLimb->pinned) {
        return;
    }

    // Find direction to push target limb out of target sphere collider (sphere collider center to phys limb's 
    // current position)
    Vec3f directionVec = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&targetPhysLimb->curr_pos, &targetCollider->center, &directionVec);

    // Find minimum distance that should be between limb and collider to make them not collide
    f32 distSquared = Math3D_Vec3fMagnitudeSq(&directionVec);
    f32 minDist = targetPhysLimb->collision_radius + targetCollider->radius;
    f32 minDistSquared = minDist * minDist;

    // If limb and collider do not overlap, there is no collision happening. Leave function.
    if (distSquared >= minDistSquared || distSquared < GREATER_THAN_ZERO) {
        return;
    }

    // Take direction vector from center of collider to phys limb's position and then scale it down to minimum
    // distance.
    CustomMath_Vec3f_Normalize(&directionVec, &directionVec);
    Math_Vec3f_Scale(&directionVec, minDist);

    // Override current positions so that target phys limb and target collider don't collide
    Vec3f newPos = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(&targetCollider->center, &directionVec, &newPos);
    Math_Vec3f_Copy(&targetPhysLimb->curr_pos, &newPos);
}


// Push limb out of capsule collider in the direction of capsule's inside to limb's current position inside collider
/**
 * @brief Push target phys limb out of the capsule collider in the direction of the capsule's inside to the
 *        phys limb's current position inside the collider.
 * 
 * This function finds the direction from the capsule collider's center to the target phys limb's position if said
 * phys limb is inside the collider. Afterward, the phys limb is pushed out in that direction so that it is on
 * the surface. 
 * 
 * @param physLimb      Target phys limb to be pushed out of collider
 * @param sphereColA    First endpoint of capsule collider
 * @param sphereColB    Second endpoint of capsule collider 
 */
void PhysCol_SolveCapsuleFromSpheres(PhysLimb* physLimb, PhysSphereCollider* sphereColA, 
    PhysSphereCollider* sphereColB) {
    // If phys limb is pinned, then do not solve collision and leave function.
    if (physLimb->pinned) {
        return;
    }

    // Calculate line segment from one endpoint of capsule to another.
    Vec3f segment = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Diff(&sphereColB->center, &sphereColA->center, &segment);
    f32 segLengthSq = Math3D_Vec3fMagnitudeSq(&segment);

    // If both sphere centers are basically in identical positions, just solve this collision as phys limb's
    // collision with first sphere collider.
    if (segLengthSq < GREATER_THAN_ZERO) {
        PhysCol_SolveCollision(physLimb, sphereColA);
        return;
    }

    // Set up distance from first endpoint to the target phys limb.
    Vec3f sphereToLimb = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Diff(&physLimb->curr_pos, &sphereColA->center, &sphereToLimb);

    // Find percentage along the segment where point in segment is closest to the phys limb.
    f32 t = CustomMath_Vec3f_Dot(&sphereToLimb, &segment) / segLengthSq;

    // Keep the closest point between sphere A and sphere B, but also keep within the range of the distance
    // between the two endpoints.
    if (t < 0.0f) {
        t = 0.0f;
    }
    else if (t > 1.0f) {
        t = 1.0f;
    }

    // Find the closest point on center segment using t.
    Vec3f scaledSegment = {0.f, 0.f, 0.f};
    Vec3f closestPos = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&segment, t, &scaledSegment);
    Math_Vec3f_Sum(&sphereColA->center, &scaledSegment, &closestPos);   // Store closest position in capsule

    // Interpolate radius between sphere A and sphere B (in case radius is different for both Sphere A and Sphere B)
    f32 capsuleRadius = sphereColA->radius + ((sphereColB->radius - sphereColA->radius) * t);

    // Treat this point along the capsule as a temporary sphere center
    PhysSphereCollider tempSphere = {closestPos, capsuleRadius};

    // Reuse normal sphere collision solver at this point in the capsule.
    PhysCol_SolveCollision(physLimb, &tempSphere);
}
