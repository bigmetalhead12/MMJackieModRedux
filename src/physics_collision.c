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

void PhysCol_SphereCollider(PhysLimb* target_limb, PhysSphereCollider* sphere_collider) {
    // Check if contact actually happened
    if (!PhysCol_limbIsColliding(target_limb, sphere_collider)) {
        return;
    }

    /*
    This currently fails to account for when both previous position and current position are 
    both within sphere collider.

    Basically, it can handle "impact" very well, but it can't handle the resting at all.
    Find a way to fix this.
    */

    // Calculate the ray-sphere intersection to find where the limb first contacted the sphere collider
    // Good source: https://www.youtube.com/watch?v=HFPlKQGChpE
    // Goal is to calculate t1 and get the point located there
    // First, take segment from limb's previous to current position
    Vec3f limb_direction = { 
        target_limb->curr_pos.x - target_limb->prev_pos.x,
        target_limb->curr_pos.y - target_limb->prev_pos.y,
        target_limb->curr_pos.z - target_limb->prev_pos.z
    };

    // Then, take segment from limb's previous position to sphere collider's center
    Vec3f limb_prev_to_sphere = {
        sphere_collider->center.x - target_limb->prev_pos.x,
        sphere_collider->center.y - target_limb->prev_pos.y,
        sphere_collider->center.z - target_limb->prev_pos.z
    };

    // Find time (t)
    f32 t = CustomMath_Vec3f_Dot(&limb_prev_to_sphere, &limb_direction)/
            CustomMath_Vec3f_Dot(&limb_direction, &limb_direction);

    // Find closest point on the ray to sphere center
    Vec3f closest_point = {
        target_limb->prev_pos.x + (limb_direction.x * t),
        target_limb->prev_pos.y + (limb_direction.y * t),
        target_limb->prev_pos.z + (limb_direction.z * t)
    };

    // Distance from closest point to sphere center
    f32 closest_dist = Math_Vec3f_DistXYZ(&closest_point, &sphere_collider->center);

    // If closest point is farther than radius, ray misses sphere
    if (closest_dist > sphere_collider->radius) {
        return;
    }
    
    // Distance from closest point back to first sphere intersection
    f32 intersection_offset = sqrtf(
        (sphere_collider->radius * sphere_collider->radius) -
        (closest_dist * closest_dist)
    );

    // Length of limb movement vector
    f32 limb_direction_length = sqrtf(
        CustomMath_Vec3f_Dot(&limb_direction, &limb_direction)
    );

    // Convert intersection distance into t-space
    f32 t_offset = intersection_offset / limb_direction_length;

    // First intersection
    f32 t1 = t - t_offset;

    if (t1 < 0.0f || t1 > 1.0f) {
        //recomp_printf("ERROR: %f\n", t1);
        return;
    }
    //recomp_printf("Not Err: %f\n", t1);

    // Use t1 to assign target limb's new position
    target_limb->curr_pos.x = target_limb->prev_pos.x + (limb_direction.x * t1);
    target_limb->curr_pos.y = target_limb->prev_pos.y + (limb_direction.y * t1);
    target_limb->curr_pos.z = target_limb->prev_pos.z + (limb_direction.z * t1);
}
