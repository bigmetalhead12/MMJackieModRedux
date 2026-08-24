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

void PhysCol_SphereCollider(PhysBone* target_bone, PhysSphereCollider* sphere_collider) {
    if (!PhysCol_limbIsColliding(target_bone->limb_b, sphere_collider)) {
        return;
    }

    Vec3f s = sphere_collider->center;
    f32 r = sphere_collider->radius;

    // Determine which point is more outside the sphere
    f32 prevDist = Math_Vec3f_DistXYZ(&target_bone->limb_b->prev_pos, &s);
    f32 currDist = Math_Vec3f_DistXYZ(&target_bone->limb_b->curr_pos, &s);

    Vec3f Ro;
    Vec3f Rd_target;

    if (prevDist >= currDist) {
        Math_Vec3f_Copy(&Ro, &target_bone->limb_b->prev_pos);
        Math_Vec3f_Copy(&Rd_target, &target_bone->limb_b->curr_pos);
    } else {
        Math_Vec3f_Copy(&Ro, &target_bone->limb_b->curr_pos);
        Math_Vec3f_Copy(&Rd_target, &target_bone->limb_b->prev_pos);
    }

    // Ray direction from outer point toward inner point
    Vec3f nonnormal_Rd = {
        Rd_target.x - Ro.x,
        Rd_target.y - Ro.y,
        Rd_target.z - Ro.z
    };
    Vec3f Rd = { 0.0f, 0.0f, 0.0f };
    CustomMath_Vec3f_Normalize(&nonnormal_Rd, &Rd);

    // Ray-sphere intersection
    Vec3f s_Ro = {
        s.x - Ro.x,
        s.y - Ro.y,
        s.z - Ro.z
    };

    f32 t = CustomMath_Vec3f_Dot(&s_Ro, &Rd);

    Vec3f Rdt = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_ScaleAndStore(&Rd, t, &Rdt);

    Vec3f p = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Sum(&Ro, &Rdt, &p);

    f32 y = Math_Vec3f_DistXYZ(&s, &p);

    if (y >= r) return;

    f32 x = sqrtf((r * r) - (y * y));
    f32 t1 = t - x;

    // Find first contact point
    Vec3f Rd_t1 = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_ScaleAndStore(&Rd, t1, &Rd_t1);

    Vec3f first_contact = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Sum(&Ro, &Rd_t1, &first_contact);

    // Push direction: from sphere center through t1
    Vec3f push_dir = { 0.0f, 0.0f, 0.0f };
    Vec3f nonnormal_push = {
        first_contact.x - s.x,
        first_contact.y - s.y,
        first_contact.z - s.z
    };
    CustomMath_Vec3f_Normalize(&nonnormal_push, &push_dir);

    // Start from sphere surface along push direction
    Vec3f surface_point = {
        s.x + push_dir.x * r,
        s.y + push_dir.y * r,
        s.z + push_dir.z * r
    };

    // Step outward until bone segment clears sphere
    Vec3f limb_a_pos = target_bone->limb_a->curr_pos;
    f32 d = 0.0f;
    f32 step = r * 0.05f;
    Vec3f new_pos = { 0.0f, 0.0f, 0.0f };
    Vec3f closest = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < 100; i++) {
        new_pos.x = surface_point.x + push_dir.x * d;
        new_pos.y = surface_point.y + push_dir.y * d;
        new_pos.z = surface_point.z + push_dir.z * d;

        CustomMath_Vec3f_ClosestPoint(&s, &limb_a_pos, &new_pos, &closest);

        f32 closest_dist = Math_Vec3f_DistXYZ(&closest, &s);

        if (closest_dist >= r) {
            break;
        }

        d += step;
    }

    // Apply new position and kill velocity to prevent oscillation
    Math_Vec3f_Copy(&target_bone->limb_b->curr_pos, &new_pos);
    Math_Vec3f_Copy(&target_bone->limb_b->prev_pos, &new_pos);
}

/*
void PhysCol_SphereCollider(PhysLimb* target_limb, PhysSphereCollider* sphere_collider) {
    // Check if contact actually happened
    if (!PhysCol_limbIsColliding(target_limb, sphere_collider)) {
        return;
    }
    

    // Calculate the ray-sphere intersection to find where the limb first contacted the sphere collider
    // Good source: https://www.youtube.com/watch?v=HFPlKQGChpE
    // Origin of the vector from target limb's previous position to current position
    Vec3f Ro = target_limb->prev_pos;

    // Vector of target limb's previous position to current position
    Vec3f Rd = { (f32)0, (f32)0, (f32)0 };
    Vec3f nonnormal_Rd = {
        target_limb->curr_pos.x - Ro.x,
        target_limb->curr_pos.y - Ro.y,
        target_limb->curr_pos.z - Ro.z
    };
    CustomMath_Vec3f_Normalize(&nonnormal_Rd, &Rd);

    // Sphere Collider's center and radius
    Vec3f s = sphere_collider->center;
    f32 r = sphere_collider->radius;

    // Segment from target limb's previous position to current position
    Vec3f s_Ro = {
        s.x - Ro.x,
        s.y - Ro.y,
        s.z - Ro.z
    };

    // Calculate scalar value (t) that can locate point in Rd that is closest to sphere collider center
    f32 t = CustomMath_Vec3f_Dot(&s_Ro, &Rd);

    // Closest point (p) between target limb vector and sphere center
    Vec3f p = { (f32)0, (f32)0, (f32)0 };
    Vec3f Rdt = { (f32)0, (f32)0, (f32)0 };

    Math_Vec3f_ScaleAndStore(&Rd, t, &Rdt);     // Rd * t
    Math_Vec3f_Sum(&Ro, &Rdt, &p);

    // Distance (y) between sphere center (s) and closest point (p)
    f32 y = Math_Vec3f_DistXYZ(&s, &p);

    // Distance (x) between closest point (p) and t1 (first contact point in sphere by vector)
    f32 x = (f32)0;
    f32 t1 = (f32)0;
    f32 t2 = (f32)0;

    if (y < r) {
        x = sqrtf((r*r) - (y*y));
        t1 = t-x;
        //t2 = t+x;

        recomp_printf("t1: %f\t", t1);
    }

    else {
        return;
    }

    // Length of actual movement segment from prev_pos to curr_pos
    f32 segment_length = Math_Vec3f_DistXYZ(
        &target_limb->prev_pos,
        &target_limb->curr_pos
    );

    recomp_printf("segment: %f\n", segment_length);

    // First contact must lie along the actual prev_pos -> curr_pos segment
    if (t1 > segment_length) {
        return;
    }

    // Find position of first contact: Ro + (Rd * t1)
    Vec3f Rd_t1 = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_ScaleAndStore(&Rd, t1, &Rd_t1);

    Vec3f first_contact = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Sum(&Ro, &Rd_t1, &first_contact);

    // Move limb to first contact point
    Math_Vec3f_Copy(&target_limb->curr_pos, &first_contact);

}
*/