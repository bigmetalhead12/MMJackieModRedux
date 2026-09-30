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

/*
=================
Ponytail Update
=================
*/

/**
 * @brief Initialize the player's world velocity and rotation for Verlet Integration.
 * 
 * @param target_player     Phys player object representing player that is used for Verlet Integration
 * @param player            Player actor to take world velocity and rotation from
 */
void Verlet_InitPhysPlayer(PhysPlayer* target_player, Player* player) {
    target_player->curr_vel = player->actor.velocity;
    target_player->prev_vel = player->actor.velocity;
    target_player->rot = player->actor.world.rot;
}


/**
 * @brief Update the player's velocity for Verlet Integration.
 * 
 * @param target_player     Phys player object representing player that is used for Verlet Integration
 * @param player            Player actor to take world velocity and rotation from
 */
void Verlet_UpdatePhysPlayerVelocity(PhysPlayer* target_player, Player* player) {
    target_player->prev_vel = target_player->curr_vel;
    target_player->curr_vel = player->actor.velocity;
    target_player->rot = player->actor.world.rot;
}


/**
 * @brief Calculate the net force based on gravity and movement acceleration.
 * 
 * Note: Currently, this function does not do much aside from applying gravity to the net force.
 * 
 * @param target_player     Phys player object representing player that is used for Verlet Integration
 * @param player            Player actor to take gravity from
 * @param net_force         Net force on player (currently only taking gravity)
 */
void Verlet_CalcNetForce(PhysPlayer* target_player, f32 grav_force, Vec3f* net_force) {
    net_force->y = grav_force;
}


/**
 * @brief Initialize target phys limb with input position, velocity, mass, and pin status
 * 
 * The function also applies small colliders on the phys limbs themselves to help with collision if needed.
 * 
 * @param target_limb               Target phys limb to apply physics data to
 * @param pos                       Position of target phys limb
 * @param vel                       World velocity of target phys limb
 * @param limb_mass                 Mass of target phys limb
 * @param pin_status                Is target phys limb pinned or unpinned?
 * @param sphere_collider_radius    Small sphere collider to apply to target phys limb
 */
void Verlet_InitLimb(PhysLimb* target_limb, Vec3f pos, Vec3f vel, f32 limb_mass, u8 pin_status, 
    f32 sphere_collider_radius) {
    // Initialize position of phys limb.
    target_limb->curr_pos = pos;
    target_limb->prev_pos = pos;

    // Initialize velocity of phys limb.
    target_limb->curr_vel = vel;
    target_limb->prev_vel = vel;

    // Initialize mass of phys limb.
    target_limb->mass = limb_mass;

    // Initialize pin status of phys limb.
    target_limb->pinned = pin_status;

    // Initialize the radius for a sphere collider that is to be on the phys limb.
    target_limb->collision_radius = sphere_collider_radius;
}


/**
 * @brief Update target phys limb's position based on velocity and force.
 * 
 * Note: This function assumes that delta time (dt) is 1 frame in Majora's Mask.
 * 
 * @param target_limb   Target phys limb
 * @param apply_force   Force to apply to phys limb to change its world position
 * @param apply_vel     Velocity to apply to phy slimb to change its world position
 */
void Verlet_LimbUpdatePos(PhysLimb* target_limb, Vec3f* apply_force, Vec3f* apply_vel) {
    // Only apply this function to phys limbs that are unpinned.
    if (target_limb->pinned == 0) {
        // Calculate target phys limb's velocity, change in position from its previous position to its 
        // current position.
        Vec3f new_velocity = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_Diff(&target_limb->curr_pos, &target_limb->prev_pos, &new_velocity);

        // Subtract the input velocity from the phys limb's velocity from movement. This lets the 
        // subsequent phys limbs below current phys limb to react opposite to the movement of their
        // parent phys limbs.
        Vec3f opposing_vel = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_ScaleAndStore(apply_vel, 1.f, &opposing_vel);
        Math_Vec3f_Diff(&new_velocity, &opposing_vel, &new_velocity);
        // Store the resulting velocity as the phys limb's current velocity.
        Math_Vec3f_Copy(&target_limb->curr_vel, &new_velocity);

        // Save the phys limb's current position.
        Math_Vec3f_Copy(&target_limb->prev_pos, &target_limb->curr_pos);
        
        // Find acceleration based on force (accel = Force/mass)
        Vec3f accel = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_ScaleAndStore(apply_force, (1/(target_limb->mass)), &accel);

        // Calculate the next Verlet position:
        // curr_pos += new_vel + accel * (dt^2)
        // Note: dt is treated as 1, so accel * dt^2 is just accel itself.
        Vec3f new_position =  {(f32)0, (f32)0, (f32)0 };
        Math_Vec3f_Copy(&new_position, &target_limb->curr_pos);
        Math_Vec3f_Sum(&new_position, &new_velocity, &new_position);
        Math_Vec3f_Sum(&new_position, &accel, &new_position);

        // Save the calculated position as the target phys limb's new current position.
        Math_Vec3f_Copy(&target_limb->curr_pos, &new_position);
    }
}


/**
 * @brief Update phys limb's position in current substep (used in overcoat physics)
 * 
 * This function is essentially a substep version of Verlet_LimbUpdatePos(). Instead of assuming a change
 * in time as a full single frame, Verlet Integration is solved based on substeps within a single frame.
 * 
 * @param target_limb   Target phys limb to be moved in current substep
 * @param apply_force   Force to apply to phys limb to change its world position
 * @param apply_vel     Velocity to apply to phy slimb to change its world position
 * @param dt            Delta time (in substep)
 */
void Verlet_LimbUpdatePosSubstep(PhysLimb* target_limb, Vec3f* apply_force, Vec3f* apply_vel, f32 dt) {
    // Only apply this function to phys limbs that are unpinned.
    if (target_limb->pinned == 0) {
        // Calculate target phys limb's velocity, change in position from its previous position to its 
        // current position.
        Vec3f new_velocity = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_Diff(&target_limb->curr_pos, &target_limb->prev_pos, &new_velocity);

        // Calculate squared substep time (dt^2)
        f32 dt_squared = dt * dt;

        // Subtract the input velocity from the phys limb's velocity from movement. This lets the 
        // subsequent phys limbs below current phys limb to react opposite to the movement of their
        // parent phys limbs.
        Vec3f opposing_vel = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_ScaleAndStore(apply_vel, dt_squared, &opposing_vel);
        Math_Vec3f_Diff(&new_velocity, &opposing_vel, &new_velocity);
        // Store the resulting velocity as the phys limb's current velocity.
        Math_Vec3f_Copy(&target_limb->curr_vel, &new_velocity);

        // Save the phys limb's current position.
        Math_Vec3f_Copy(&target_limb->prev_pos, &target_limb->curr_pos);

        // Find acceleration based on force (accel = Force/mass)
        Vec3f accel = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_ScaleAndStore(apply_force, (1.f / target_limb->mass), &accel);

        // Scale acceleration based on substep time
        Math_Vec3f_ScaleAndStore(&accel, dt_squared, &accel);

        // New position with Verlet Integration
        // curr_pos += new_velocity + accel * (dt^2)
        Vec3f new_position = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_Copy(&new_position, &target_limb->curr_pos);
        Math_Vec3f_Sum(&new_position, &new_velocity, &new_position);
        Math_Vec3f_Sum(&new_position, &accel, &new_position);

        // Save the calculated position as the target phys limb's new current position.
        Math_Vec3f_Copy(&target_limb->curr_pos, &new_position);
    }
}


/**
 * @brief Set two phys limbs into target phys bone.
 * 
 * @param target_bone   Target phys bone that is to have the two input phys limbs
 * @param physLimb_A    Parent phys limb of target phys bone
 * @param physLimb_B    Child phys limb of target phsy bone
 */
void Verlet_InitBone(PhysBone* target_bone, PhysLimb* physLimb_A, PhysLimb* physLimb_b) {
    // Target phys bone's parent phys limb
    target_bone->limb_a = physLimb_A;

    // Target phys bone's child phys limb
    target_bone->limb_b = physLimb_b;

    // Set target phys bone's length
    target_bone->bone_length = Math_Vec3f_DistXYZ(&target_bone->limb_a->curr_pos, &target_bone->limb_b->curr_pos);
}


/**
 * @brief Apply distance constraint between the two phys limbs of target phys bone
 * 
 * This function reduces the distance between a target phys bone's two phys limbs based on the set
 * distance between the two phys limbs when the target phys bone was initialized.
 * 
 * @param target_bone   Target phys bone
 */
void Verlet_BoneConstraint(PhysBone* target_bone) {
    // Calculate current distance between target phys bone's two phys limbs
    f32 curr_dist = Math_Vec3f_DistXYZ(&target_bone->limb_a->curr_pos, &target_bone->limb_b->curr_pos);
    f32 diff = target_bone->bone_length - curr_dist;
    f32 percent = (diff/curr_dist);

    // Calculate offset to apply to phys limbs' position to retain phys bone's length
    // Direction Vector = Limb B's position - Limb A's position
    Vec3f direction_vec = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&target_bone->limb_b->curr_pos, &target_bone->limb_a->curr_pos, &direction_vec);
    
    Vec3f offset = { (f32)0, (f32)0, (f32)0 };
    
    // If target phys bone's both phys limbs are unpinned, percent is divided by 2 and both phys
    // limbs' positions get adjusted to retain phys bone's length.
    if (target_bone->limb_a->pinned == 0 && target_bone->limb_b->pinned == 0) {
        percent = percent/2.f;
        Math_Vec3f_ScaleAndStore(&direction_vec, percent, &offset);
        // Adjust parent phys limb's position
        Math_Vec3f_Diff(&target_bone->limb_a->curr_pos, &offset, &target_bone->limb_a->curr_pos);
        // Adjust child phys limb's position
        Math_Vec3f_Sum(&target_bone->limb_b->curr_pos, &offset, &target_bone->limb_b->curr_pos);
    }
    // Else if target phys bone's parent phys limb is pinned while the child phys limb is unpinned,
    // then only the unpinned child limb's position gets adjusted to retain phys bone's length.
    else if (target_bone->limb_a->pinned == 1 && target_bone->limb_b->pinned == 0) {
        percent = percent/1.f;
        Math_Vec3f_ScaleAndStore(&direction_vec, percent, &offset);
        // Adjust child phys limb's position only
        Math_Vec3f_Sum(&target_bone->limb_b->curr_pos, &offset, &target_bone->limb_b->curr_pos);
    }
    
}