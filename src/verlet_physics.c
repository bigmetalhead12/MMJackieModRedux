/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

#include "verlet_physics.h"

#include "proxymm_custom_actor.h"// delete after stop using recomp_printf

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
/***********************************************************************

	Verlet Integration

***********************************************************************/
// Initialize player velocity and rotation
void Verlet_InitPhysPlayer(PhysPlayer* target_player, Player* player) {
    target_player->curr_vel = player->actor.velocity;
    target_player->prev_vel = player->actor.velocity;
    target_player->rot = player->actor.world.rot;
}

// Update player velocity
void Verlet_UpdatePhysPlayerVelocity(PhysPlayer* target_player, Player* player) {
    target_player->prev_vel = target_player->curr_vel;
    target_player->curr_vel = player->actor.velocity;
    target_player->rot = player->actor.world.rot;
}

// Calculate net force based on gravity and movement acceleration
void Verlet_CalcNetForce(PhysPlayer* target_player, f32 grav_force, Vec3f* net_force) {
    net_force->y = grav_force;
    Vec3f velocity_diff = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&target_player->curr_vel, &target_player->prev_vel, &velocity_diff);
    Math_Vec3f_Sum(net_force, &velocity_diff, net_force);
}

// Initialize PhysLimb with input position
void Verlet_InitLimb(PhysLimb* target_limb, Vec3f pos, Vec3f vel, f32 limb_mass, u8 pin_status) {
    // Initialize position
    target_limb->curr_pos = pos;
    target_limb->prev_pos = pos;

    // Initialize velocity
    target_limb->curr_vel = vel;
    target_limb->prev_vel = vel;

    // Initialize mass
    target_limb->mass = limb_mass;

    // Initialize pin status
    target_limb->pinned = pin_status;
}

// Update position of limb
void Verlet_LimbUpdatePos(PhysLimb* target_limb, Vec3f* apply_force, Vec3f* apply_vel) {
    if (target_limb->pinned == 0) {
        // Velocity based on change of limb's positions
        Vec3f new_velocity = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_Diff(&target_limb->curr_pos, &target_limb->prev_pos, &new_velocity);

        // test segment for adding opposite vel for hanging ponytail limbs
        Vec3f opposing_vel = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_ScaleAndStore(apply_vel, -1.f, &opposing_vel);
        Math_Vec3f_Sum(&new_velocity, &opposing_vel, &new_velocity);
        Math_Vec3f_Copy(&target_limb->curr_vel, &new_velocity);

        // Save limb's current position 
        target_limb->prev_pos = target_limb->curr_pos;

        // Find acceleration based on force (accel = Force/mass)
        Vec3f accel = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_ScaleAndStore(apply_force, (1/target_limb->mass), &accel);

        // New position with Verlet Integration
        // curr_pos += new_vel + accel * (dt^2) where dt = 1
        Vec3f new_position =  {(f32)0, (f32)0, (f32)0 };
        Math_Vec3f_Copy(&new_position, &target_limb->curr_pos);
        Math_Vec3f_Sum(&new_position, &new_velocity, &new_position);
        Math_Vec3f_Sum(&new_position, &accel, &new_position);
        Math_Vec3f_Copy(&target_limb->curr_pos, &new_position);
    }
}

// Set two PhysLimbs into target PhysBone
void Verlet_InitBone(PhysBone* target_bone, PhysLimb* limb_a, PhysLimb* limb_b) {
    // target_bone's first limb
    target_bone->limb_a = limb_a;

    // target_bone's second limb
    target_bone->limb_b = limb_b;

    // bone's length
    target_bone->bone_length = Math_Vec3f_DistXYZ(&target_bone->limb_a->curr_pos, &target_bone->limb_b->curr_pos);
}

// Apply constraint to limbs based on bone
void Verlet_BoneConstraint(PhysBone* target_bone) {
    // current distance between target_bone's two limbs
    f32 curr_dist = Math_Vec3f_DistXYZ(&target_bone->limb_a->curr_pos, &target_bone->limb_b->curr_pos);
    f32 diff = target_bone->bone_length - curr_dist;
    f32 percent = (diff/curr_dist);

    // Calculate offset to apply to limbs' position to retain bone's length
    // Direction Vector = Limb B's position - Limb A's position
    Vec3f direction_vec = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&target_bone->limb_b->curr_pos, &target_bone->limb_a->curr_pos, &direction_vec);
    
    Vec3f offset = { (f32)0, (f32)0, (f32)0 };
    
    // If target bone's both limbs are unpinned, percent is divided by 2
    if (target_bone->limb_a->pinned == 0 && target_bone->limb_b->pinned == 0) {
        percent = percent/1.f;
        Math_Vec3f_ScaleAndStore(&direction_vec, percent, &offset);
        //Math_Vec3f_Diff(&target_bone->limb_a->curr_pos, &offset, &target_bone->limb_a->curr_pos);
        Math_Vec3f_Sum(&target_bone->limb_b->curr_pos, &offset, &target_bone->limb_b->curr_pos);
    }
    else if (target_bone->limb_a->pinned == 1 && target_bone->limb_b->pinned == 0) {
        percent = percent/1.f;
        Math_Vec3f_ScaleAndStore(&direction_vec, percent, &offset);
        Math_Vec3f_Sum(&target_bone->limb_b->curr_pos, &offset, &target_bone->limb_b->curr_pos);
    }
    
}