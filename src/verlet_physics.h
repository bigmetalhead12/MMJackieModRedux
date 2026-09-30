/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Verlet integration designed for Majora's Mask recomp

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
    f32     collision_radius;
} PhysLimb;

typedef struct {
    PhysLimb*   limb_a;
    PhysLimb*   limb_b;
    f32         bone_length;
} PhysBone;


void Verlet_InitPhysPlayer(PhysPlayer* target_player, Player* player);
void Verlet_UpdatePhysPlayerVelocity(PhysPlayer* target_player, Player* player);
void Verlet_CalcNetForce(PhysPlayer* target_player, f32 grav_force, Vec3f* net_force);
void Verlet_InitLimb(PhysLimb* target_limb, Vec3f pos, Vec3f vel, f32 limb_mass, u8 pin_status, f32 sphere_collider_radius);
void Verlet_LimbUpdatePos(PhysLimb* target_limb, Vec3f* apply_force, Vec3f* apply_vel);
void Verlet_LimbUpdatePosSubstep(PhysLimb* target_limb, Vec3f* apply_force, Vec3f* apply_vel, f32 dt);
void Verlet_InitBone(PhysBone* target_bone, PhysLimb* limb_a, PhysLimb* limb_b);
void Verlet_BoneConstraint(PhysBone* target_bone);

#endif