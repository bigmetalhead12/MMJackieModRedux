// TODO

/*
    BLENDER
    1.  Set the root bone of the ponytail to be in the same location as the head 
        bone of Jackie.
    2.  Make other bones to hang directly downward like in Riesbyfe's skirt. 
        Make each joint to be at each horizontal line of the vertices
    3.  Clean up bones with script
    4.  Export w/ Fast64

    CODE(pain)
    1.  Set up the old draw, update, destroy functions
    2.  Assign root bone of ponytail to have same rotation and position as Jackie's
        head bone.
    3.  Code physics

    OR (I'll probably do this)
    1.  Set up the functions
    2.  For testing purposes, attach ponytail to Jackie's root bone and position it
        away from body for easy viewing
    3.  Code physics (use Jackie's actor's position and rotation)
        - Find velocity and time (delta T)

    Now I have to:
    1. With the current position values, find a way to find net force (gravity + velocity) [Done]
    2. Find a way to connect limbs to my point/stick [Done]
    3. Find a way to impact points [Done]
    4. Find a way to impact stick [Done]
        -May have to start finding way to have stick impact jointTable also

    1. Find a way to have rotation be part of the equation [Done]

    1. Find a way to have player model be part of the equation [Current Task]
*/


/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Applying verlet integration to each limb of Jackie's ponytail model
    and then drawing them to Majora's Mask recomp

========================================================================
*/

// MACROS
#define GRAVITY     -5.f
#define LIMB_MASS   1.f
#define PINNED      1
#define NOT_PINNED  0

#define PITCH_ADUSTER 0     // Adjust x-axis rotation based on ponytail skeleton's rest form
#define YAW_LIMIT   4096*4

#include "z_ponytail.h"
#include "verlet_physics.h"
#include "proxymm_custom_actor.h"
#include "math.h"
#include "customMath.h"

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

#define FLAGS 0


/***********************************************************************

	Ponytail Physics

***********************************************************************/

void Ponytail_Init(Actor* thisx, PlayState* play);      // When actor is spawned, this function 
                                                        // gets called called once
void Ponytail_Destroy(Actor* thisx, PlayState* play);   // Get rid of Ponytail
void Ponytail_Update(Actor* thisx, PlayState* play);    // Called every single frame while actor is 
                                                        // alive Logic stuff (like movement, 
                                                        // behavior, etc.)
void Ponytail_Draw(Actor* thisx, PlayState* play);      // Draw displaylist
                                                        // Separate from Update

/*
=================
Ponytail Profile
=================
*/
ActorProfile Ponytail_Profile = {
    ACTOR_ID_MAX,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(Ponytail),
    Ponytail_Init,
    Ponytail_Destroy,
    Ponytail_Update,
    Ponytail_Draw,
};

/*
=================
Ponytail Bones
=================
*/
typedef enum PonytailBones {
    PONYTAIL_ROOT_POS,     // Ponytail's Root bone (position)
    PONYTAIL_ROOT_ROT,     // Ponytail's Root bone (rotation)
    PONYTAIL_UC,
    PONYTAIL_HEAD,
    PONYTAIL_LIMB1,
    PONYTAIL_LIMB2,
    PONYTAIL_LIMB3,
    PONYTAIL_LIMB4
} PonytailBones;

/*
=================
Set Ponytail as Custom Actor
=================
*/
s16 CUSTOM_ACTOR_PONYTAIL = ACTOR_ID_MAX;

RECOMP_CALLBACK("*", recomp_on_init) void Ponytail_OnRecompInit() {
    CUSTOM_ACTOR_PONYTAIL = CustomActor_Register(&Ponytail_Profile);
}

/*
=================
Player Init
=================
*/
RECOMP_HOOK("Player_Init") void on_player_init(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (recomp_get_config_u32("change_hairstyle")) {
        s32 pad;
        Player* this = (Player*)thisx;
        s8 objectSlot;
        s32 respawnFlag;
        s32 var_a1;
        PlayerStartMode startMode;
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, CUSTOM_ACTOR_PONYTAIL, 
                                    -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0, 
                                    0, 0);
    }
}

/*
=================
Ponytail Init
=================
*/
Ponytail* gPlayerPonytail;
PlayState* gPlayStatePonytail;

void Ponytail_SetDefaultBodyPartsPos(Ponytail* this, Player* player, StandardLimb* gLimbs[], PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]) {
    // Set Ponytail velocity
    Vec3f playerVelocity = player->actor.velocity;

    // Set position and rotation of Ponytail
    Math_Vec3f_Copy(&this->actor.world.pos, &player->actor.world.pos);
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);
    Math_Vec3s_Copy(&this->actor.world.rot, &player->actor.world.rot);

    // Root limb's BodyPartsPos and gPhysLimb positions and velocity
    // Velocity only gets assigned to root limb
    // BodyPartsPos keeps track of global XYZ position of each limb
    Vec3s newRootJointPos = { 0, 0, 0 };
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &newRootJointPos);
    Verlet_InitLimb(gPhysLimbs[PONYTAIL_BODYPART_ROOT], this->actor.world.pos, playerVelocity, LIMB_MASS, PINNED);

    // Root limb's jointTable for rotation
    Vec3s rootPos_Vec3s = {0, 0, 0};
    Math_Vec3f_ToVec3s(&rootPos_Vec3s, &this->actor.world.pos);
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &rootPos_Vec3s);
    Math_Vec3s_Copy(&gPhysLimbs[PONYTAIL_BODYPART_ROOT]->default_jointPos, &rootPos_Vec3s);

    // Root limb's jointTable for rotation initialized
    Vec3s newRootJointRot = { 0, 0, 0};
    newRootJointRot.y = -32768;
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &newRootJointRot);
    
    // BodyPartsPos and gPhysLimbs' pos and vel for Rest of the limbs
    for (int i = (int)PONYTAIL_BODYPART_UPPERCONTROL; i < (int)PONYTAIL_BODYPART_MAX; i++) {
        // Find global position of current limb based on offset from parent limb's position
        Vec3f transformVec3f = {0.f, 0.f, 0.f};
        // Apply ponytail root limb's rotation here
        Vec3s rotatedOffset = {0, 0, 0};

        CustomMath_Vec3s_Rotate(&gPhysLimbs[i]->default_jointPos, &player->actor.shape.rot, &rotatedOffset);
        CustomMath_Vec3s_Scale_ToVec3f(&rotatedOffset, this->actor.scale.x, &transformVec3f);
        Math_Vec3f_Sum(&this->bodyPartsPos[i-1], &transformVec3f, &this->bodyPartsPos[i]);

        // If limb shouldn't have any verlet physics, pin it
        if (i <= (int)PONYTAIL_BODYPART_LIMB1) {
            Verlet_InitLimb(gPhysLimbs[i], this->bodyPartsPos[i], playerVelocity, (LIMB_MASS), PINNED);
        }
        else {
            Vec3f no_velocity = {(f32)0, (f32)0, (f32)0};
            Verlet_InitLimb(gPhysLimbs[i], this->bodyPartsPos[i], no_velocity, (LIMB_MASS), NOT_PINNED);
        }
    }

    // Set matching gPhysLimbs to appropriate gPhysBones
    for (int i = 0; i < (int)PONYTAIL_BONE_MAX; i++) {
        Verlet_InitBone(gPhysBones[i], gPhysLimbs[i], gPhysLimbs[i+1]);
    }
}


void Ponytail_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_hairstyle")) {
        Player* player = GET_PLAYER(play);
        Ponytail* this = (Ponytail*)thisx;
        this->actor.room = -1;

        gPlayerPonytail = this;
        gPlayStatePonytail = play;

        // Capture default jointPos
        // I can't think of any way aside from hardcoding, because the standardlimbs' jointPos values
        // keep getting updated based on its last jointPos values before map reloads.
        // Values are directly copied from the generated Fast64 model file (in my case, gPonytailSkel.c)
        Vec3s uc_jointPos       = { 0, 21, -7 };
        Vec3s head_jointPos     = { 1392, -259, 0 };
        Vec3s limb1_jointPos    = { -320, -195, 0 };
        Vec3s limb2_jointPos    = { 0, 0, -206 };
        Vec3s limb3_jointPos    = { 0, 0, -191 };
        Vec3s limb4_jointPos    = { 0, 0, -197 };
        Vec3s limb5_jointPos    = { 0, 0, -158 };

        Math_Vec3s_Copy(&ponytailPhysLimbs[1]->default_jointPos, &uc_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[2]->default_jointPos, &head_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[3]->default_jointPos, &limb1_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[4]->default_jointPos, &limb2_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[5]->default_jointPos, &limb3_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[6]->default_jointPos, &limb4_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[7]->default_jointPos, &limb5_jointPos);

        Actor_SetScale(&this->actor, 0.01f);
        SkelAnime_InitFlex(
            play,
            &this->skelAnime,
            &gPonytailSkel,
            NULL,
            this->jointTable,
            this->morphTable,
            GPONYTAILSKEL_NUM_LIMBS
        );

        // Verlet integration starts here
        Verlet_InitPhysPlayer(&gJackiePhysPlayer, player);
        Ponytail_SetDefaultBodyPartsPos(this, player, gPonytailLimbs, ponytailPhysLimbs, ponytailPhysBones);   
    }
}

/*
=================
Ponytail Destroy
=================
*/
void Ponytail_Destroy(Actor* thisx, PlayState* play) {
}

/*
=================
Ponytail Update
=================
*/

void Ponytail_UpdateBodyPartsPos(Ponytail* this, Player* player, Vec3f apply_force, StandardLimb* gLimbs[], PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]) {
    // Set Ponytail's previous position
    Math_Vec3f_Copy(&this->actor.prevPos, &player->actor.prevPos);

    // Set Ponytail's position and rotations
    Math_Vec3f_Copy(&this->actor.world.pos, &player->bodyPartsPos[PLAYER_BODYPART_WAIST]);
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);

    // Root Limb
    // bodyPartsPos (ponytail actor & gPhysLimbs)
    Math_Vec3f_Copy(&this->bodyPartsPos[PONYTAIL_BODYPART_ROOT], &player->bodyPartsPos[PLAYER_BODYPART_WAIST]);
    Math_Vec3f_Copy(&gPhysLimbs[PONYTAIL_BODYPART_ROOT]->curr_pos, &player->bodyPartsPos[PLAYER_BODYPART_WAIST]);
    // jointTable (pos and rotation)
    Vec3s newRootJointPos = { 0, 0, 0}; // Ponytail's root already set to Player's waist (same pos as player's root)
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &newRootJointPos);  
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &player->skelAnime.jointTable[PLAYER_LIMB_ROOT]);

    // UC Limb
    // bodyPartsPos (ponytail actor & gPhysLimbs)
    Math_Vec3f_Copy(&this->bodyPartsPos[PONYTAIL_BODYPART_UPPERCONTROL], &player->bodyPartsPos[PLAYER_BODYPART_TORSO]); // Torso position same as Upper Control 
    Math_Vec3f_Copy(&gPhysLimbs[PONYTAIL_BODYPART_UPPERCONTROL]->curr_pos, &player->bodyPartsPos[PLAYER_BODYPART_TORSO]);
    // jointTable (rotation only)
    Math_Vec3s_Copy(&this->skelAnime.jointTable[PONYTAIL_UC], &player->skelAnime.jointTable[PLAYER_LIMB_UPPER_ROOT]);

    // Head Limb
    // bodyPartsPos (ponytail actor & gPhysLimbs)
    Math_Vec3f_Copy(&this->bodyPartsPos[PONYTAIL_BODYPART_HEAD], &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);
    Math_Vec3f_Copy(&gPhysLimbs[PONYTAIL_BODYPART_HEAD]->curr_pos, &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);
    // jointTable (rotation only)
    Math_Vec3s_Copy(&this->skelAnime.jointTable[PONYTAIL_HEAD], &player->skelAnime.jointTable[PLAYER_LIMB_HEAD]);

    // BodyPartsPos Rest of the limbs
    for (int i = (int)PONYTAIL_BODYPART_LIMB1; i < (int)PONYTAIL_BODYPART_MAX; i++) {
        if (gPhysLimbs[i]->pinned == 1) {
            // Set previous values for current gPhysLimb
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_pos, &gPhysLimbs[i]->curr_pos);    // Previous Position
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_vel, &gPhysLimbs[i]->curr_vel);    // Previous Velocity

            // Find current global position of current limb based on offset from parent limb's position
            Vec3f transformVec3f = {0.f, 0.f, 0.f};
            Vec3s rotatedOffset = { 0, 0, 0};
            CustomMath_Vec3s_Rotate(&gPhysLimbs[i]->default_jointPos, &this->actor.shape.rot, &rotatedOffset);
            CustomMath_Vec3s_Scale_ToVec3f(&rotatedOffset, this->actor.scale.x, &transformVec3f);

            // Set Current values for current gPhysLimb
            Math_Vec3f_Sum(&this->bodyPartsPos[i-1], &transformVec3f, &this->bodyPartsPos[i]);
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_pos, &this->bodyPartsPos[i]);      // Current position
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_vel, &player->actor.velocity);     // Current Velocity
        }
        else {
            // Apply Verlet Integration
            Vec3f player_vel = { (f32)0, (f32)0, (f32)0 };
            Math_Vec3f_Copy(&player_vel, &player->actor.velocity);

            u8 linkOnGround = (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND);
            if (linkOnGround) {
                player_vel.y = 0.f;
            }

            Verlet_LimbUpdatePos(gPhysLimbs[i], &apply_force, &player_vel);
        }
    }

    // Bone update
    for (int i = 0; i < 1; i++) {
        for (int i = 0; i < (int)PONYTAIL_BONE_MAX; i++) {
            Verlet_BoneConstraint(gPhysBones[i]);
        }
    }

}

void Ponytail_RotateJoints(Ponytail* this, Player* player, PhysBone* gPhysBones[]) {
    // Keep track of all limbs' rotation for subsequent limbs' rotations
    Vec3s offset_rotation = { (s16)0, (s16)0, (s16)0 };

    // Go through every bone
    for (int i = (int)PONYTAIL_BONE_ROOT_UPPERCONTROL; i < (int)PONYTAIL_BONE_MAX; i++) {
        // Skip to next bone if both limbs in current bone are pinned
        if (gPhysBones[i]->limb_a->pinned == 1 && gPhysBones[i]->limb_b->pinned == 1) {
            CustomMath_Vec3s_Sum(&offset_rotation, &this->skelAnime.jointTable[i + 1], &offset_rotation);
            continue;
        }
        else {
            recomp_printf("[%d] offset: %d, %d, %d\n", i, offset_rotation.x, offset_rotation.y, offset_rotation.z);
            // Find limb_a's pitch and roll values based on direction of bone
            s16 curr_rotate_x = CustomMath_Vec3f_Pitch(&gPhysBones[i]->limb_b->curr_pos, &gPhysBones[i]->limb_a->curr_pos);
            s16 curr_rotate_z = CustomMath_Vec3f_Roll(&gPhysBones[i]->limb_a->curr_pos, &gPhysBones[i]->limb_b->curr_pos);

            // PONYTAIL_BODYPART_LIMB1 is a pinned bone
            if (i == (int)PONYTAIL_BODYPART_LIMB1) {
                // Get global direction vector from current bone's limb_a to limb_b
                Vec3f bone_direction = {(f32)0.f, (f32)0.f, (f32)0.f};
                Math_Vec3f_Diff(&gPhysBones[i]->limb_b->curr_pos, &gPhysBones[i]->limb_a->curr_pos, &bone_direction);

                // Transform direction into local space by removing actor rot & model offset
                Vec3s actorRotWithOffset = {0, 0, 0};
                Math_Vec3s_Copy(&actorRotWithOffset, &this->actor.shape.rot);
                CustomMath_Vec3f_InverseRotate(&bone_direction, &actorRotWithOffset, &bone_direction);

                // Now compute pitch/roll from the local-space direction
                Vec3f origin = {0, 0, 0};
                s16 local_pitch = CustomMath_Vec3f_Pitch(&bone_direction, &origin) + 16384;
                s16 local_roll = CustomMath_Vec3f_Roll(&origin, &bone_direction);
                Vec3s curr_rotate = { 0, (s16)0, 0 };

                Vec3s new_rotate = { 0, 0, 0};
                curr_rotate.x = -offset_rotation.x + local_roll;
                curr_rotate.y = -offset_rotation.y;
                curr_rotate.z = -offset_rotation.z - local_pitch;

                new_rotate.x = curr_rotate.x;
                new_rotate.y = curr_rotate.y;
                new_rotate.z = curr_rotate.z;
                // Apply pitch and roll to current limb's jointTable
                this->skelAnime.jointTable[i + 1] = new_rotate;

                // Keep note of offset for next limb
                CustomMath_Vec3s_Sum(&offset_rotation, &new_rotate, &offset_rotation);
            }
            else {
                // Get global direction vector from current bone's limb_a to limb_b
                Vec3f bone_direction = {(f32)0.f, (f32)0.f, (f32)0.f};
                Math_Vec3f_Diff(&gPhysBones[i]->limb_b->curr_pos, &gPhysBones[i]->limb_a->curr_pos, &bone_direction);

                // Transform direction into local space by removing actor rot & model offset
                Vec3s actorRotWithOffset = {0, 0, 0};
                Math_Vec3s_Copy(&actorRotWithOffset, &this->actor.shape.rot);
                CustomMath_Vec3f_InverseRotate(&bone_direction, &actorRotWithOffset, &bone_direction);
                
                // Now compute pitch/roll from the local-space direction
                Vec3f origin = {0, 0, 0}; 
                s16 local_pitch = CustomMath_Vec3f_Pitch(&bone_direction, &origin)  + 16384;
                s16 local_roll = CustomMath_Vec3f_Roll(&origin, &bone_direction);
                Vec3s curr_rotate = { 0, (s16)0, 0 };

                Vec3s new_rotate = { 0, 0, 0};
                curr_rotate.x = -offset_rotation.x + local_roll;
                curr_rotate.y = -offset_rotation.y;
                curr_rotate.z = -offset_rotation.z - local_pitch;

                new_rotate.x = curr_rotate.x;
                new_rotate.y = curr_rotate.y;
                new_rotate.z = curr_rotate.z;

                // Subtract parent's accumulated rotation to get relative rotation
                
                //CustomMath_Vec3s_Diff(&curr_rotate, &offset_rotation, &apply_rot);

                // Apply pitch and roll to current limb's jointTable
                this->skelAnime.jointTable[i + 1] = new_rotate;

                // Keep note of offset for next limb
                CustomMath_Vec3s_Sum(&offset_rotation, &new_rotate, &offset_rotation);
            }
        }
    }
}

void Ponytail_Update(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (recomp_get_config_u32("change_hairstyle")) {
        /*
        Ponytail* this = (Ponytail*)thisx;

        Player* player = GET_PLAYER(play);

        Vec3f net_force = { (f32)0, (f32)0, (f32)0 };    // Gravity + movement

        Verlet_UpdatePhysPlayerVelocity(&gJackiePhysPlayer, player);
        Verlet_CalcNetForce(&gJackiePhysPlayer, (f32)GRAVITY, &net_force);
        
        // Update ponytail's limbs' global positions based on pin status and verlet integration calculations
        Ponytail_UpdateBodyPartsPos(this, player, net_force, gPonytailLimbs, ponytailPhysLimbs, ponytailPhysBones);

        // Uew calculated global positions in ponytailPhysLimbs in ponytailPhysBones to find rotations for jointTable
        Ponytail_RotateJoints(this, ponytailPhysBones);
        */
    }
}

/*
=================
Player Draw
=================
*/
RECOMP_HOOK_RETURN ("Player_Draw") void return_Ponytail_Player_Draw(void) {
    Player* player = GET_PLAYER(gPlayStatePonytail);
    if (recomp_get_config_u32("change_hairstyle")) {
        Ponytail* this = gPlayerPonytail;

        Vec3f net_force = { (f32)0, (f32)0, (f32)0 };    // Gravity + movement

        Verlet_UpdatePhysPlayerVelocity(&gJackiePhysPlayer, player);
        Verlet_CalcNetForce(&gJackiePhysPlayer, (f32)GRAVITY, &net_force);
        
        // Update ponytail's limbs' global positions based on pin status and verlet integration calculations
        Ponytail_UpdateBodyPartsPos(this, player, net_force, gPonytailLimbs, ponytailPhysLimbs, ponytailPhysBones);

        // Uew calculated global positions in ponytailPhysLimbs in ponytailPhysBones to find rotations for jointTable
        Ponytail_RotateJoints(this, player, ponytailPhysBones);
    }
    
    

}

/*
=================
Ponytail Draw
=================
*/
void Ponytail_Draw(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (recomp_get_config_u32("change_hairstyle")) {
        Ponytail* this = (Ponytail*)thisx;

        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        SkelAnime_DrawFlexOpa(
            play,
            this->skelAnime.skeleton,
            this->skelAnime.jointTable,
            this->skelAnime.dListCount,
            NULL,
            NULL,
            &this->actor
        );
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

