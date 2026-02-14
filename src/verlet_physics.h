/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

#ifndef Z_VERLET_H
#define Z_VERLET_H

#include "math.h"
#include "ultra64.h"
#include "global.h"

/***********************************************************************

	Verlet Integration

***********************************************************************/
typedef struct {
    Vec3f   curr_vel;
    Vec3f   prev_vel;
    Vec3s   rot;
} PhysPlayer;

typedef struct {
    Vec3f   curr_pos;
    Vec3f   prev_pos;
    Vec3f   alt_curr_pos;
    Vec3f   alt_prev_pos;
    Vec3f   curr_vel;
    Vec3f   prev_vel;
    Vec3s   default_jointPos;
    f32     mass;
    u8      pinned;
} PhysLimb;

typedef struct {
    PhysLimb*   limb_a;
    PhysLimb*   limb_b;
    f32         bone_length;
} PhysBone;

// Initialize player velocity and rotation
void Verlet_InitPhysPlayer(PhysPlayer* target_player, Player* player);

// Update player velocity
void Verlet_UpdatePhysPlayerVelocity(PhysPlayer* target_player, Player* player);

// Calculate net force based on gravity and movement acceleration
void Verlet_CalcNetForce(PhysPlayer* target_player, f32 grav_force, Vec3f* net_force);

// Initialize PhysLimb with input position
void Verlet_InitLimb(PhysLimb* target_limb, Vec3f pos, Vec3f vel, f32 limb_mass, u8 pin_status);

// Update position of limb
void Verlet_LimbUpdatePos(PhysLimb* target_limb, Vec3f* apply_force, Vec3f* apply_vel);

// Set two PhysLimbs into target PhysBone
void Verlet_InitBone(PhysBone* target_bone, PhysLimb* limb_a, PhysLimb* limb_b);

// Update bone based on constraint
void Verlet_BoneConstraint(PhysBone* target_bone);

#endif