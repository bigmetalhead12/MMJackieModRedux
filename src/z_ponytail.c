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
    1. With the current position values, find a way to find net force (gravity + velocity)
    2. Find a way to connect limbs to my point/stick
    3. Find a way to impact points
    4. Find a way to impact stick
        -May have to start finding way to have stick impact jointTable also

    1. Find a way to have rotation be part of the equation

    1. Find a way to have player model be part of the equation
*/


/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

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

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

#define FLAGS 0

// Designed to calculate bone x-axis rotations beyond -90/+90 degrees from origin
s16 CustomMath_Vec3f_Pitch(Vec3f* b, Vec3f* a) {
    // If vector b's z position is >= vector a's z position, proceed as is
    if (b->z >= a->z) {
        return Math_Atan2S_XY( b->z - a->z, b->y - a->y);
        //return Math_Vec3f_Pitch(b, a);
    }
    // Else, if b's z position is < a's z position, then apply offset
    else {
        s16 low_offset = -32768;    // -180 degrees
        //return (low_offset - Math_Vec3f_Pitch(b, a));
        return Math_Atan2S_XY( b->z - a->z, b->y - a->y);
    }
}

// Designed to calculate bone y-axis rotations beyond -90/+90 degrees from origin
s16 CustomMath_Vec3f_Yaw(Vec3f* a, Vec3f* b) {
    // If vector b's z position is >= vector a's z position, proceed as is
    if (b->z >= a->z) {
        return Math_Vec3f_Yaw(a, b);
    }
    // Else, if b's z position is > a's z position, then apply offset
    else {
        s16 low_offset = -32768;    // -180 degrees
        return (low_offset + Math_Vec3f_Yaw(a, b));
    }
}

// Designed to calculate bone z-axis rotations 
s16 CustomMath_Vec3f_Roll(Vec3f* a, Vec3f* b) {
    f32 x_dist = b->x - a->x;
    f32 y_dist = b->y - a->y;
    if (y_dist <= 0) {
        return 32768+Math_Atan2S_XY(y_dist, x_dist);
    }
    else {
        return (Math_Atan2S_XY(y_dist, x_dist));
    }
}
// Vec3s version for Vector Sum
void CustomMath_Vec3s_Sum(Vec3s* l, Vec3s* r, Vec3s* dest) {
    dest->x = l->x + r->x;
    dest->y = l->y + r->y;
    dest->z = l->z + r->z;
}

// Vec3s version for Vector Difference
void CustomMath_Vec3s_Diff(Vec3s* l, Vec3s* r, Vec3s* dest) {
    dest->x = l->x - r->x;
    dest->y = l->y - r->y;
    dest->z = l->z - r->z;
}

// Scale target Vec3s by input f32 value and put it into Vec3f dest
void CustomMath_Vec3s_Scale_ToVec3f(Vec3s* target, f32 scale, Vec3f* dest) {
    Vec3s transformVec3s = {0,0,0};
    Vec3f transformVec3f = {0.f, 0.f, 0.f};

    Math_Vec3s_Copy(&transformVec3s, target);
    Math_Vec3s_ToVec3f(&transformVec3f, &transformVec3s);
    Math_Vec3f_Scale(&transformVec3f, scale);

    Math_Vec3f_Copy(dest, &transformVec3f);
}

// Rotate target Vec3s based on given s16 angle measure
// By X axis
void CustomMath_Vec3s_RotateByX(Vec3s* target, s16 angle, Vec3s* dest) {

}

// By Y axis
void CustomMath_Vec3s_RotateByY(Vec3s* target, s16 angle, Vec3s* dest) {

}

// By Z axis
void CustomMath_Vec3s_RotateByZ(Vec3s* target, s16 angle, Vec3s* dest) {

}

void CustomMath_Vec3s_Rotate(Vec3s* target, Vec3s* rotate, Vec3s* dest) {
    CustomMath_Vec3s_RotateByX(target, rotate->x, dest);
    CustomMath_Vec3s_RotateByY(target, rotate->y, dest);
    CustomMath_Vec3s_RotateByZ(target, rotate->z, dest);
}


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
    /*  0 */ PONYTAIL_ROOT_POS,     // Ponytail's Root bone (position)
    /*  1 */ PONYTAIL_ROOT_ROT,     // Ponytail's Root bone (rotation)
    /*  2 */ PONYTAIL_LIMB1,
    /*  3 */ PONYTAIL_LIMB2,
    /*  4 */ PONYTAIL_LIMB3,
    /*  5 */ PONYTAIL_LIMB4
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
    Vec3f transform;

    Vec3f playerVelocity = player->actor.velocity;

    // Change this eventually to 
    // Math_Vec3f_Copy(&this->actor.world.pos, &rootPos);
    transform.x = player->actor.world.pos.x;
    transform.y = player->actor.world.pos.y + 100.f;
    transform.z = player->actor.world.pos.z;

    Math_Vec3f_Copy(&this->actor.world.pos, &transform);

    // Rotation for ponytail's actor
    Math_Vec3s_Copy(&this->actor.world.rot, &player->actor.world.rot);

    // BodyPartsPos and gPhysLimbs' positions and velocity
    // Velocity only gets assigned to root bone
    Math_Vec3f_Copy(&this->bodyPartsPos[PONYTAIL_BODYPART_ROOT], &this->actor.world.pos);
    
    Verlet_InitLimb(gPhysLimbs[PONYTAIL_BODYPART_ROOT], this->actor.world.pos, playerVelocity, LIMB_MASS, PINNED);

    // jointTable for Root Position
    Vec3s newRootJointPos = { 0, 0, 0};
    
    Math_Vec3f_ToVec3s(&newRootJointPos, &transform);
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &newRootJointPos);

    // jointTable rotation
    // NOTE: Eventually, I'll have to find Jackie's head limb's rotation relative to actor rotation and apply here
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &player->actor.world.rot);

    // BodyPartsPos and gPhysLimbs' pos and vel for Rest of the limbs
    for (int i = 1; i < (int)PONYTAIL_BODYPART_MAX; i++) {
        // Find global position of current limb based on offset from parent limb's position
        Vec3f transformVec3f = {0.f, 0.f, 0.f};
        
        // Apply ponytail root limb's rotation here
        // NOTE: APPLY PARENT'S ROTATION TO TRANSLATION VECTOR FOR POSITION RELATIVE TO ROTATION
        // gLimbs[i]->jointPos vs new JointPos based on rotation; make new function for this for new Vec3s
        // Make target Vec3s vector also to carry new jointPos values
        CustomMath_Vec3s_Scale_ToVec3f(&gLimbs[i]->jointPos, this->actor.scale.x, &transformVec3f);
        Math_Vec3f_Sum(&this->bodyPartsPos[i-1], &transformVec3f, &this->bodyPartsPos[i]);

        if (i == (int)PONYTAIL_BODYPART_LIMB1) {
            Verlet_InitLimb(gPhysLimbs[i], this->bodyPartsPos[i], playerVelocity, (LIMB_MASS), PINNED);
        }
        else {
            Vec3f no_velocity = {(f32)0, (f32)0, (f32)0};
            Verlet_InitLimb(gPhysLimbs[i], this->bodyPartsPos[i], no_velocity, (LIMB_MASS), NOT_PINNED);
            // TEST: Comment when testing rotation verlet and un-comment above
            //Verlet_InitLimb(gPhysLimbs[i], this->bodyPartsPos[i], playerVelocity, (LIMB_MASS), PINNED);
        }
    }

    // Set matching gPhysLimbs to appropriate gPhysBones
    for (int i = 0; i < (int)PONYTAIL_BONE_MAX; i++) {
        Verlet_InitBone(gPhysBones[i], gPhysLimbs[i], gPhysLimbs[i+1]);
        recomp_printf("BONE [%d", i);
        recomp_printf("] length: %f\n", gPhysBones[i]->bone_length);
    }
}

void Ponytail_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_hairstyle")) {
        Player* player = GET_PLAYER(play);
        Ponytail* this = (Ponytail*)thisx;
        this->actor.room = -1;

        gPlayerPonytail = this;
        gPlayStatePonytail = play;

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
    // Change this eventually to 
    // Math_Vec3f_Copy(&this->actor.world.pos, &rootPos);
    // This kind of hard mapping of limbs should be based on if said limbs are pinned or not
    Math_Vec3f_Copy(&this->actor.prevPos, &player->actor.prevPos);
    Math_Vec3f_Copy(&this->actor.world.pos, &player->actor.world.pos);

    // Rotation
    //Math_Vec3s_Copy(&this->actor.world.rot, &player->actor.world.rot);

    // BE SURE TO REMOVE THIS AFTER TESTING
    Vec3f offsetVec3f = { (f32)0, (f32)70, (f32)0 };
    Math_Vec3f_Sum(&this->actor.world.pos, &offsetVec3f, &this->actor.world.pos);

    // Set Root limb's bodyPartsPos
    Math_Vec3f_Copy(&this->bodyPartsPos[PONYTAIL_BODYPART_ROOT], &this->actor.world.pos);
    Math_Vec3f_Copy(&gPhysLimbs[PONYTAIL_BODYPART_ROOT]->curr_pos, &this->actor.world.pos);

    // jointTable Root Position
    Vec3s newRootJointPos = { 0, 0, 0};
    Math_Vec3f_ToVec3s(&newRootJointPos, &this->actor.world.pos);
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &newRootJointPos);

    // jointTable Root rotation
    //Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &player->actor.world.rot);

    // BodyPartsPos Rest of the limbs
    for (int i = 1; i < (int)PONYTAIL_BODYPART_MAX; i++) {
        if (gPhysLimbs[i]->pinned == 1) {
            // Set previous values for current gPhysLimb
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_pos, &gPhysLimbs[i]->curr_pos);    // Previous Position
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_vel, &gPhysLimbs[i]->curr_vel);    // Previous Velocity

            // Find current global position of current limb based on offset from parent limb's position
            Vec3f transformVec3f = {0.f, 0.f, 0.f};
            CustomMath_Vec3s_Scale_ToVec3f(&gLimbs[i]->jointPos, this->actor.scale.x, &transformVec3f);

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
            recomp_printf("\n[%d]: \n", i);
            Verlet_BoneConstraint(gPhysBones[i]);
            recomp_printf("\n");
            
        }
    }

}

void Ponytail_RotateJoints(Ponytail* this, PhysBone* gPhysBones[]) {
    // Note: Rotation values in jointTable are relative to its parent's rotation

    // I may have to initially add the player's actor's rotation on to this when considering
    // initial rotation of player. Keep this in mind
    Vec3s offset_rotation = { (s16)0, (s16)0, (s16)0 };

    for (int i = (int)PONYTAIL_BONE_ROOT_LIMB1; i < (int)PONYTAIL_BONE_MAX; i++) {
        // Skip to next bone if both limbs in current bone are pinned
        if (gPhysBones[i]->limb_a->pinned == 1 && gPhysBones[i]->limb_b->pinned == 1) {
            continue;
        }
        else {
            recomp_printf("%d: ", i);
            if (i == 1) {
                recomp_printf("limb_b->curr_pos: %.16f, ", gPhysBones[i]->limb_b->curr_pos.x);
                recomp_printf("%.16f, ", gPhysBones[i]->limb_b->curr_pos.y);
                recomp_printf("%.16f\n", gPhysBones[i]->limb_b->curr_pos.z);
                recomp_printf("limb_a->curr_pos: %.16f, ", gPhysBones[i]->limb_a->curr_pos.x);
                recomp_printf("%.16f, ", gPhysBones[i]->limb_a->curr_pos.y);
                recomp_printf("%.16f\n", gPhysBones[i]->limb_a->curr_pos.z);
                recomp_printf("t  e  s  t  :  %.16f, ", gPhysBones[i]->limb_b->curr_pos.x - gPhysBones[i]->limb_a->curr_pos.x);
                recomp_printf("%.16f, ", gPhysBones[i]->limb_b->curr_pos.y - gPhysBones[i]->limb_a->curr_pos.y);
                recomp_printf("%.16f\n", gPhysBones[i]->limb_b->curr_pos.z - gPhysBones[i]->limb_a->curr_pos.z);
            }
            
            s16 curr_rotate_x = CustomMath_Vec3f_Pitch(&gPhysBones[i]->limb_b->curr_pos, &gPhysBones[i]->limb_a->curr_pos);
            
            if (i == 1) {
                recomp_printf("CURR PITCH ROTATE X: %d\n", curr_rotate_x);
            }
            //s16 curr_rotate_y = CustomMath_Vec3f_Yaw(&gPhysBones[i]->limb_a->curr_pos, &gPhysBones[i]->limb_b->curr_pos);
            s16 curr_rotate_z = CustomMath_Vec3f_Roll(&gPhysBones[i]->limb_a->curr_pos, &gPhysBones[i]->limb_b->curr_pos);
            //s16 curr_rotate_z = 0;

            if (i == 1) {
                //recomp_printf("CURR YAW ROTATE Y: %d\n", curr_rotate_y);
                recomp_printf("CURR ROLL ROTATE Z: %d\n", curr_rotate_z);
            }
            //s16 curr_rotate_y = -8192;
            /*
            if (curr_rotate_y > YAW_LIMIT * i) {
                curr_rotate_y = YAW_LIMIT * i;
            }
            else if (curr_rotate_y < -YAW_LIMIT * i) {
                curr_rotate_y = -YAW_LIMIT * i;
            }*/

            if (i == (int)PONYTAIL_BODYPART_LIMB1) {
                //Vec3s curr_rotate = { curr_rotate_x, curr_rotate_y, (s16)0 };
                Vec3s curr_rotate = { curr_rotate_x, (s16)0, curr_rotate_z };
                this->skelAnime.jointTable[i + 1] = curr_rotate;
                CustomMath_Vec3s_Sum(&offset_rotation, &curr_rotate, &offset_rotation);
            }
            else {
                //Vec3s curr_rotate = { curr_rotate_x, curr_rotate_y, (s16)0 };
                Vec3s curr_rotate = { curr_rotate_x, (s16)0, curr_rotate_z };
                Vec3s apply_rot = { (s16)0, (s16)0, (s16)0 };

                // apply = offset - curr
                CustomMath_Vec3s_Diff(&curr_rotate, &offset_rotation, &apply_rot);

                this->skelAnime.jointTable[i + 1] = apply_rot;
                CustomMath_Vec3s_Sum(&offset_rotation, &apply_rot, &offset_rotation);
            }
        }
    }
}

void Ponytail_Update(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (recomp_get_config_u32("change_hairstyle")) {
        Ponytail* this = (Ponytail*)thisx;

        Player* player = GET_PLAYER(play);

        Vec3f net_force = { (f32)0, (f32)0, (f32)0 };    // Gravity + movement

        recomp_printf("PREVIOUS VELOCITY: %f, ", gJackiePhysPlayer.curr_vel.x);
        recomp_printf("%f, ", gJackiePhysPlayer.curr_vel.y);
        recomp_printf("%f\n", gJackiePhysPlayer.curr_vel.z);

        Verlet_UpdatePhysPlayerVelocity(&gJackiePhysPlayer, player);
        Verlet_CalcNetForce(&gJackiePhysPlayer, (f32)GRAVITY, &net_force);
        recomp_printf("CURRENT VELOCITY: %f, ", gJackiePhysPlayer.curr_vel.x);
        recomp_printf("%f, ", gJackiePhysPlayer.curr_vel.y);
        recomp_printf("%f\n", gJackiePhysPlayer.curr_vel.z);

        recomp_printf("ACCELERATION: %f, ", net_force.x);
        recomp_printf("%f, ", net_force.y);
        recomp_printf("%f\n", net_force.z);

        // Update Ponytail's roobone and other pinned bone's position based on player's position
        // Note: this may have to be updated to handle both pinned and unpinned bones
        // where pinned bones get updated to be with player's position, and
        // unpinned bones get updated with Verlet integration specifically, since verlet
        // takes into account of positions of limbs directly. It might not be heplful to
        // separate the two from each other, but I dunno. Anyway, net force needs to be found
        // on its own anyway, so start from that.
        Ponytail_UpdateBodyPartsPos(this, player, net_force, gPonytailLimbs, ponytailPhysLimbs, ponytailPhysBones);
        //Ponytail_UpdateBodyPartsPos(this, player, net_force, gPonytailLimbs, ponytailPhysLimbs, ponytailPhysBones);

        // Uew calculated global positions in ponytailPhysLimbs in ponytailPhysBones to find rotations for jointTable
        Ponytail_RotateJoints(this, ponytailPhysBones);




        // After this, figure out how to apply this to ponytail when it's rotating based on link actor
        // After this, figure out how to properly attach this to Link's head bodyPartPos
    }
}

/*
=================
Player Draw
=================
*/
RECOMP_HOOK_RETURN ("Player_Draw") void return_Skirt_Player_Draw(void) {
    if (!recomp_get_config_u32("change_hairstyle")) {
        return;
    }
    
    Player* player = GET_PLAYER(gPlayStatePonytail);

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

