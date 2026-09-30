/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Applying verlet integration to each limb of Jackie's ponytail model
    and then drawing them to Majora's Mask recomp

    This implementation is not complete, as there are two unaddressed
    parts to this:
        1. No collision system designed (with floor and player's body
           parts)
        2. Offset values to ponytail's limbs (not including root limb) 
           to ensure they hang directly down in the world regardless of
           player's head limb's rotation not properly calculated.   

========================================================================
*/

/***********************************************************************

	Macros

***********************************************************************/

/*
=================
Physics-Related Macros
=================
*/
#define GRAVITY     -5.f
#define LIMB_MASS   1.f
#define PINNED      1
#define NOT_PINNED  0
#define NECK_COLLIDER_RADIUS 2.f
#define COLLISION_FACTOR    30
#define PONYTAIL_LIMB_COLLIDER_RADIUS  0.3f
#define GREATER_THAN_ZERO   0.000001f

/*
=================
Rendering Macros
=================
*/
#define MIN_CAMERA_DISTANCE 10.0f
#define FIRST_PERSON_VIEW_MODE 6

#include "z_ponytail.h"
#include "verlet_physics.h"
#include "proxymm_custom_actor.h"
#include "physics_collision.h"
#include "math.h"
#include "customMath.h"

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

// For zero-ing player velocity when Jackie is opening a door
extern void Player_Action_36(Player* player, PlayState* play);


#define FLAGS 0


/***********************************************************************

	Ponytail Physics

***********************************************************************/

/*
=================
Ponytail Limbs
=================
*/
// Enum of ponytail limbs for jointTable
typedef enum PonytailLimbs {
    /*  0 */ PONYTAIL_ROOT_POS,     // Ponytail's Root bone (position)
    /*  1 */ PONYTAIL_ROOT_ROT,     // Ponytail's Root bone (rotation)
    /*  2 */ PONYTAIL_LIMB1,
    /*  3 */ PONYTAIL_LIMB2,
    /*  4 */ PONYTAIL_LIMB3,
    /*  5 */ PONYTAIL_LIMB4
} PonytailLimbs;


/*
=================
Set Ponytail as Custom Actor
=================
*/
// Sets profile for ponytail before registering it as actor
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

s16 CUSTOM_ACTOR_PONYTAIL = ACTOR_ID_MAX;

/**
 * @brief Register ponytail as custom actor
 */
RECOMP_CALLBACK("*", recomp_on_init) void Ponytail_OnRecompInit() {
    CUSTOM_ACTOR_PONYTAIL = CustomActor_Register(&Ponytail_Profile);
}


/*
=================
Ponytail Init
=================
*/
Ponytail* gPlayerPonytail = NULL;
PlayState* gPlayStatePonytail = NULL;


/**
 * @brief Create custom actor for ponytail on player_init
 * 
 * @param thisx     Actor pointer
 * @param play      Current playstate
 */
RECOMP_HOOK("Player_Init") void Ponytail_on_player_init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_hairstyle") && gPlayerPonytail == NULL) {
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, CUSTOM_ACTOR_PONYTAIL, 
                                    -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0, 
                                    0, 0);
    }
}


/**
 * @brief Set up initial phys limbs and phys bones
 * 
 * Initialize all of the phys limbs' positions and velocity based on the player's rotation and velocity. Also, 
 * initialize all of the phys bones, which also initializes the distance between every phys limbs.
 * 
 * @param this          Ponytail actor
 * @param player        Player actor
 * @param gPhysLimbs    Phys limbs to represent the positions for respective ponytail standard limbs after verlet
 */
void Ponytail_SetDefaultBodyPartsPos(Ponytail* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[]) {
    // Set Ponytail velocity
    Vec3f playerVelocity = player->actor.velocity;

    // Set position and rotation of Ponytail
    Math_Vec3f_Copy(&this->actor.world.pos, &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);
    Math_Vec3s_Copy(&this->actor.world.rot, &player->actor.world.rot);

    // Root limb's BodyPartsPos and gPhysLimb positions and velocity
    // Velocity only gets assigned to root limb
    // BodyPartsPos keeps track of global XYZ position of each limb
    Math_Vec3f_Copy(&this->bodyPartsPos[PONYTAIL_BODYPART_ROOT], &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);
    Verlet_InitLimb(gPhysLimbs[PONYTAIL_BODYPART_ROOT], this->actor.world.pos, playerVelocity, LIMB_MASS, PINNED, 
        PONYTAIL_LIMB_COLLIDER_RADIUS);

    // Root limb's jointTable for position
    Vec3s rootPos_Vec3s = {0, 0, 0};
    Math_Vec3f_ToVec3s(&rootPos_Vec3s, &this->actor.world.pos);
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &rootPos_Vec3s);
    Math_Vec3s_Copy(&gPhysLimbs[PONYTAIL_BODYPART_ROOT]->default_jointPos, &rootPos_Vec3s);

    // Root limb's jointTable for rotation
    Vec3s newRootJointRot = { 0, 0, 0};
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &newRootJointRot);

    // BodyPartsPos and gPhysLimbs' pos and vel for rest of the limbs
    for (int i = (int)PONYTAIL_BODYPART_LIMB1; i < (int)PONYTAIL_BODYPART_MAX; i++) {
        // Find global position of current limb based on offset from parent limb's position
        Vec3f transformVec3f = {0.f, 0.f, 0.f};

        // Apply ponytail root limb's rotation here
        Vec3s rotatedOffset = {0, 0, 0};
        CustomMath_Vec3s_Rotate(&gPhysLimbs[i]->default_jointPos, &player->actor.shape.rot, &rotatedOffset);
        CustomMath_Vec3s_Scale_ToVec3f(&rotatedOffset, this->actor.scale.x, &transformVec3f);
        Math_Vec3f_Sum(&this->bodyPartsPos[i-1], &transformVec3f, &this->bodyPartsPos[i]);

        // If limb shouldn't have any verlet physics, pin it
        if (i == (int)PONYTAIL_BODYPART_LIMB1) {
            Verlet_InitLimb(gPhysLimbs[i], this->bodyPartsPos[i], playerVelocity, (LIMB_MASS*(i)), PINNED, 
            PONYTAIL_LIMB_COLLIDER_RADIUS);
        }
        else {
            Vec3f no_velocity = {(f32)0, (f32)0, (f32)0};
            Verlet_InitLimb(gPhysLimbs[i], this->bodyPartsPos[i], no_velocity, (LIMB_MASS*(i)), NOT_PINNED, 
            PONYTAIL_LIMB_COLLIDER_RADIUS);
        }
    }

    // Set matching gPhysLimbs to appropriate gPhysBones
    for (int i = 0; i < (int)PONYTAIL_BONE_MAX; i++) {
        Verlet_InitBone(gPhysBones[i], gPhysLimbs[i], gPhysLimbs[i+1]);
    }
}


/**
 * @brief Initialize ponytail model and actor along with its limbs
 * 
 * This function initializes the ponytail model and its custom actor. When doing this, the function also takes the 
 * standardLimbs' positions of the ponytail model (gPonytailSkel.c/h) generated via Fast 64. These positions are then
 * stored into the phys limbs, which are used for verlet integration.
 * 
 * @param thisx     Actor
 * @param play      Current playstate
 */
void Ponytail_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_hairstyle")) {
        // Initialize ponytail's states
        Player* player = GET_PLAYER(play);
        Ponytail* this = (Ponytail*)thisx;
        this->actor.room = -1;
        this->needsReset = 1;
        this->needsResetShape = 0;
        this->hideInFirstPerson = 0;

        gPlayerPonytail = this;
        gPlayStatePonytail = play;

        // Capture default jointPos for phys limbs
        // I can't think of any way aside from hardcoding, because the standardlimbs' jointPos values
        // keep getting updated based on its last jointPos values before map reloads.
        // Values are directly copied from the generated Fast64 model file (in my case, gPonytailSkel.c)
        Vec3s limb1_jointPos = { 0, 157, -340 };    // Start from gPonytailSkelLimb_001
        Vec3s limb2_jointPos = { 0, 0, -206 };
        Vec3s limb3_jointPos = { 0, 0, -191 };
        Vec3s limb4_jointPos = { 0, 0, -197 };
        Vec3s limb5_jointPos = { 0, 0, -158 };

        // default_jointPos values for every limb should NEVER change once assigned here
        Math_Vec3s_Copy(&ponytailPhysLimbs[1]->default_jointPos, &limb1_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[2]->default_jointPos, &limb2_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[3]->default_jointPos, &limb3_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[4]->default_jointPos, &limb4_jointPos);
        Math_Vec3s_Copy(&ponytailPhysLimbs[5]->default_jointPos, &limb5_jointPos);

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
        Ponytail_SetDefaultBodyPartsPos(this, player, ponytailPhysLimbs, ponytailPhysBones);
    }
}


/**
 * @brief Reset the ponytail's phys limbs' position when Jackie's door opening animation plays.
 */
RECOMP_HOOK("Player_Door_Knob") void Ponytail_ResetOnDoorOpen(PlayState* play, Player* player,
    Actor* door) {
    // Ignore this if Jackie is not in human form and overcoat is not drawn.
    if (player != GET_PLAYER(play) || player->transformation != PLAYER_FORM_HUMAN ||
        gPlayerPonytail == NULL) {
        return;
    }

    // If Jackie is human and ponytail is being drawn, then make sure to reset ponytail shape.
    gPlayerPonytail->needsReset = 1;
}


/*
=================
Ponytail Destroy
=================
*/
/**
 * @brief Destroy ponytail model and actor
 * 
 * @param thisx     Actor pointer
 * @param play      Current playstate
 */
void Ponytail_Destroy(Actor* thisx, PlayState* play) {
    gPlayerPonytail = NULL;
}


/*
=================
Ponytail Update
=================
*/

Vec3f head_globalPos = { (f32)0, (f32)0, (f32)0 };      // World position of player's head limb
Vec3s head_rotate = {0, 0, 0};              // Rotation value for player's head (with parent limbs' rotations included)

PhysSphereCollider neckSphereCollider = {
    { 0.0f, 0.0f, 0.0f },                   // World space position
    NECK_COLLIDER_RADIUS                    // Sphere collider radius
};


/**
 * @brief Get player head limb's position and rotation values using matrix
 * 
 * @param play      Current playstate
 * @param limbIndex Current limb in player skeleton
 */
RECOMP_HOOK("Player_PostLimbDrawGameplay") void Ponytail_on_Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, 
    Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    if (limbIndex == PLAYER_LIMB_HEAD) {
        // Get matrix of player's head limb
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push(); 
        MtxF* mtx = Matrix_GetCurrent();

        // Get world position of head limb
        head_globalPos.x = mtx->xw;
        head_globalPos.y = mtx->yw;
        head_globalPos.z = mtx->zw;

        // Get rotation values of head limb
        Matrix_MtxFToYXZRot(mtx, &head_rotate, 1);

        // Close matrix
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }
}


/**
 * @brief Reset the ponytail's limbs into valid positions after a physics reset
 *
 * When changing map, ponytail limbs are initially collapse donto Jackie's head model. This function repositions
 * the ponytail's unpinned limbs into simple outward shape. All previous movement-related data are also
 * cleared so that verlet integration can properly work with moving the ponytail limbs without making the limbs
 * fling around unpredictably.
 * 
 * @param this      Ponytail actor
 * @param limbs     Phys limbs of the ponytail
 */
void Ponytail_SetResetShape(Ponytail* this, PhysLimb* limbs[]) {
    PhysLimb* rootLimb = limbs[PONYTAIL_BODYPART_ROOT];
    PhysLimb* pinnedLimb = limbs[PONYTAIL_BODYPART_LIMB1];

    /********************************
     Find reset direction
    ********************************/
    // Find horizontal direction from root limb to pinned limb (Y is ignored because it's only purely horizontal)
    Vec3f outwardDir = {(pinnedLimb->curr_pos.x - rootLimb->curr_pos.x), 0.f, 
        (pinnedLimb->curr_pos.z - rootLimb->curr_pos.z)};
    f32 horizontalLength = sqrtf((outwardDir.x * outwardDir.x) + (outwardDir.z * outwardDir.z));
    Vec3f limbDirection = {0.f, -1.f, 0.f};

    if (horizontalLength > GREATER_THAN_ZERO) {
        // Calculate direction of 45 degrees from rest
        limbDirection.x = outwardDir.x / horizontalLength;
        limbDirection.z = outwardDir.z / horizontalLength;

        // Scale by sin(45 degrees) (or 1 / sqrt(2))
        Math_Vec3f_Scale(&limbDirection, (1/sqrtf(2.f)));
    }


    /********************************
     Reset unpinned ponytail limbs
    ********************************/
    // Scale ponytail to match Jackie's current model size
    f32 scale = fabsf(this->actor.scale.x);

    // Pinned limb is already positioned properly
    // Reset every unpinned limb after it using the previous limb as its parent.
    for (int i = PONYTAIL_BODYPART_LIMB2; i < PONYTAIL_BODYPART_MAX; i++) {
        PhysLimb* parentLimb = limbs[i - 1];
        PhysLimb* childLimb = limbs[i];

        if (childLimb->pinned) {
            continue;
        }

        // Calculate this limb's default bone length and account for actor scale
        Vec3s offset = childLimb->default_jointPos;
        f32 x = offset.x, y = offset.y, z = offset.z;
        f32 length = sqrtf((x * x) + (y * y) + (z * z)) * scale;

        // Scale the reset direction to this bone's length
        Vec3f limbOffset = limbDirection;
        Math_Vec3f_Scale(&limbOffset, length);

        // Place the child limb one bone-length away from its parent.
        Math_Vec3f_Sum(&parentLimb->curr_pos, &limbOffset, &childLimb->curr_pos);

        // Save reset world positions of unpinned limbs
        this->bodyPartsPos[i] = childLimb->curr_pos;
    }

    // Start stationary without carrying velocity from the previous map
    for (int i = 0; i < PONYTAIL_BODYPART_MAX; i++) {
        limbs[i]->prev_pos = limbs[i]->curr_pos;
        limbs[i]->curr_vel = (Vec3f){0.f, 0.f, 0.f};
        limbs[i]->prev_vel = (Vec3f){0.f, 0.f, 0.f};
    }

    this->needsResetShape = 0;
}


/**
 * @brief Update the positions of phys limbs based on verlet integration
 * 
 * This function directly modifies the position of every phys limb using verlet integration. It uses the following
 * to re-calculate the position of every phys limb for the frame:
 * 
 * 1. Player's velocity
 * 
 * 2. Force being applied to player
 * 
 * 3. Length of bone containing two phys limbs (endpoints)
 * 
 * 4. Collision with collider(s) assigned to limb position in this function
 * 
 * The function also calculates the ponytail's current size based on the player's actor's size
 * 
 */
void Ponytail_UpdateBodyPartsPos(Ponytail* this, Player* player, Vec3f apply_force, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[]) {
    // Save Jackie's previous position as ponytail's previous position
    Math_Vec3f_Copy(&this->actor.prevPos, &player->actor.prevPos);

    /********************************
     Reset phys limbs after map change
    ********************************/
    // Rebase all phys limbs' positions after entering a new map so old physics state does not cause the 
    // ponytail to jump or move unpredictably
    if (this->needsReset) {
        this->needsReset = 0;
        this->needsResetShape = 1;

        // Reset  ponytail root directly to player's current head position
        Math_Vec3f_Copy(&gPhysLimbs[PONYTAIL_BODYPART_ROOT]->curr_pos, &head_globalPos);
        Math_Vec3f_Copy(&gPhysLimbs[PONYTAIL_BODYPART_ROOT]->prev_pos, &head_globalPos);

        // Collapse the rest of the ponytail limbs onto the head temporarily
        for (int i = 1; i < (int)PONYTAIL_BODYPART_MAX; i++) {
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_pos, &head_globalPos);
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_pos, &head_globalPos);

            // Remove velocity carried over from previous map
            Vec3f zero_velocity = { (f32)0, (f32)0, (f32)0 };
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_vel, &zero_velocity);
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_vel, &zero_velocity);
        }

        return;
    }

    /********************************
     Sync ponytail actor with Jackie
    ********************************/
    // Match Jackie's scale so ponytail actor scales correctly with giant's mask transformation
    Math_Vec3f_Copy(&this->actor.scale, &player->actor.scale);

    // Assign ponytail actor's position to Jackie's head limb's position
    Math_Vec3f_Copy(&this->actor.world.pos, &head_globalPos);

    // Assign neck collision sphere position to Jackie's head limb position
    Math_Vec3f_Copy(&neckSphereCollider.center, &head_globalPos);

    // Match ponytail actor's direction to Jackie's general facing direction
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);

    // Ponytail model faces opposite direction of Jackie's head limb by default, so rotate it 180 degrees
    this->actor.shape.rot.y += -32768;

    /********************************
     Set root position
    ********************************/
    // Ponytail's bodyPartsPos root starts at position of Jackie's head limb
    Math_Vec3f_Copy(&this->bodyPartsPos[PONYTAIL_BODYPART_ROOT], &head_globalPos);

    // Assign the root physics limb's current position at ponytail actor's current world position.
    Math_Vec3f_Copy(&gPhysLimbs[PONYTAIL_BODYPART_ROOT]->curr_pos, &this->actor.world.pos);

    // Ponytail's skeleton root has no additional local translation
    Vec3s newRootJointPos = { 0, 0, 0 };
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &newRootJointPos);

    /********************************
     Calculate root rotation
    ********************************/
    // Convert Jackie's animated head rotation into ponytail root's coordinate system.
    Vec3s newRootJointRot = { 0, 0, 0 };
    newRootJointRot.x = 32768 + head_rotate.z;
    newRootJointRot.y = -16384 + head_rotate.y - this->actor.shape.rot.y;
    newRootJointRot.z = head_rotate.x;

    // Store the final root rotation used by the ponytail skeleton
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &newRootJointRot);

    /********************************
     Update ponytail limb positions
    ********************************/
    for (int i = (int)PONYTAIL_BODYPART_LIMB1; i < (int)PONYTAIL_BODYPART_MAX; i++) {
        // Pinned limb
        if (gPhysLimbs[i]->pinned == 1) {
            // Preserve pinned limb's previous and current positions and velocities before moving it
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_pos, &gPhysLimbs[i]->curr_pos);
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_vel, &gPhysLimbs[i]->curr_vel);

            // Pinned limb's original position relative to its parent limb (Ponytail's root limb)
            Vec3f localOffset = {gPhysLimbs[i]->default_jointPos.x, gPhysLimbs[i]->default_jointPos.y, 
                gPhysLimbs[i]->default_jointPos.z};
            Vec3f zeroF = { 0.f, 0.f, 0.f };

            // Use matrix
            Matrix_Push();

            // Create matrix representing ponytail's actor's world space position and overall rotation
            // Use YXZ for actor rotation
            Matrix_SetTranslateRotateYXZ( this->actor.world.pos.x, this->actor.world.pos.y, 
                this->actor.world.pos.z, &this->actor.shape.rot);

            // Apply Jackie's current scale
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

            // Pinned limb inherits the parent limb's (ponytail root limb's) local rotation
            Matrix_TranslateRotateZYX(&zeroF, &this->skelAnime.jointTable[PONYTAIL_ROOT_ROT]);

            // Transform pinned limb's local default offset into world space
            Matrix_MultVec3f(&localOffset, &this->bodyPartsPos[i]);

            // Stop using matrix
            Matrix_Pop();

            // Save pinned limb's current world position 
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_pos, &this->bodyPartsPos[i]);

            // Pinned limb follows Jackie's movement
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_vel, &player->actor.velocity);
        }

        // Unpinned limbs
        else if (!this->needsResetShape) {
            // Use Jackie's horizontal movement as inherited motion for the ponytail
            Vec3f player_vel = player->actor.velocity;
            player_vel.y = 0.f;

            // Disable the opposing movement while riding Epona, climbing ladder/wall, or opening door.
            // Riding Epona: PLAYER_STATE1_800000
            // Climbing Wall: PLAYER_STATE1_200000
            // Opening Door: Player_Action_36
            // Note: Physics for riding Epona was disabled due to some unwanted physics behavior. This may be 
            //       due to the codebase relying on player->actor.velocity for player's world velocity. This works 
            //       for most cases EXCEPT when player is riding on Epona, where player->actor.velocity might be 
            //       saving the player's last frame's world velocity. 
            if (player->stateFlags1 & (PLAYER_STATE1_800000 | PLAYER_STATE1_200000) ||
                player->actionFunc == Player_Action_36) {
                player_vel = (Vec3f){0.f, 0.f, 0.f};
            }

            // Apply Verlet integration using external forces and Jackie's movement
            Verlet_LimbUpdatePos(gPhysLimbs[i], &apply_force, &player_vel);
        }
    }

    // If Jackie is entering new map, then reset ponytail limbs' positions to prevent ponytail limbs'
    // unpredictable movement 
    if (this->needsResetShape) {
        Ponytail_SetResetShape(this, gPhysLimbs);
        return;
    }

    /********************************
     Solve bone lengths and collision
    ********************************/
    // Bone update based on bone length and collision with collider(s)
    for (int i = 0; i < COLLISION_FACTOR; i++) {
        for (int j = 0; j < (int)PONYTAIL_BONE_MAX; j++) {
            Verlet_BoneConstraint(gPhysBones[j]);
            
            if (!gPhysBones[j]->limb_b->pinned) {
                PhysCol_SolveCollision(gPhysBones[j]->limb_b, &neckSphereCollider);
            }
        }
    }
    
    // Final cleanup of phys limb positions based on collision with colliders
    for (int i = 0; i < (int)PONYTAIL_BODYPART_MAX; i++) {
        PhysCol_SolveCollision(gPhysLimbs[i], &neckSphereCollider);
    }
}


/**
 * @brief Rotate bone with pinned parent limb while using rotation stabilizer
 * 
 * Rotate actual ponytail bone in the direction of the matching phys bone. To do this, find a stable shortest rotation
 * possible for the actual bone. This is done by using a quaternion, which helps with:
 *
 * 1. finding a 3D axis on which the ponytail bone's limb A should rotate around
 *
 * 2. finding how far the ponytail bone's limb A should rotate on this axis
 *
 * This function has more rotation behaviors to limit the free rotation of pinned limb in bone.
 *
 * @param direction	Target direction from phys bone
 * @param rotation 	Rotation for actual bone of ponytail
 */
void Ponytail_RotateFromPinned(Vec3f* direction, Vec3s* rotation) {
	// Find length of direction vector of phys bone
    f32 lengthSquared = (direction->x * direction->x) + (direction->y * direction->y) + (direction->z * direction->z);

    if (lengthSquared < GREATER_THAN_ZERO) {
        return;
    }

	// Normalize the direction vector, which says what direction limb A should be pointing to in this bone
	// This would describe the phys bone's direction {dx, dy, dz}, which should be where the actual ponytail bone
	// should point to (from limb A)
    f32 inverseLength = 1.f / sqrtf(lengthSquared);
    f32 dx = direction->x * inverseLength;
    f32 dy = direction->y * inverseLength;
    f32 dz = direction->z * inverseLength;
	
	// In idle, this bone rests in a pure -Z direction (essentially {0, 0, -1})
	// A quaternion needs to be used to find axis in 3D space on limb A that allows current direction of bone 
	// from limb A to be rotated to the direction of the phys bone.
	// This is done with cross product: 
	// axis = current bone direction {0, 0, -1} 	X	target phys bone direction {dx, dy, dz}
	// | i	j	k |
	// | 0	0	-1|	= (dy, -dx, 0)
	// |dx	dy	dz|
    f32 qx = dy;
    f32 qy = -dx;
	f32 qz = 0;
	
	// Find rotation amount on axis {qx, qy, qz}
	// Dot product of current bone direction * target phys bone direction
    f32 qw = 1.f - dz;
	
	// Length of quaternion ^ 2
    f32 quatLengthSquared = (qx * qx) + (qy * qy) + (qz * qz) + (qw * qw);

	// If the target phys bone direction points towards +Z, then quaternion length becomes very nearly 0
    if (quatLengthSquared < GREATER_THAN_ZERO) {
        // -180 degree rotation around X-axis
        *rotation = (Vec3s){-32768, 0, 0};
        return;
    }

	// Normalize quaternion {qx, qy, qz, qw}
    f32 inverseQuatLength = 1.f / sqrtf(quatLengthSquared);
    qx *= inverseQuatLength;
    qy *= inverseQuatLength;
	qz *= inverseQuatLength;		// irrelevant since it's 0
    qw *= inverseQuatLength;

    // Convert normalized quaternion into rotation matrix
	//	| 1 - 2y^2 - 2z^2			2xy - 2wz			2xz + 2wy	| 
	//	|		2xy + 2wz		1- 2x^2 - 2z^2			2yz - 2wz	|
	//	| 		2xz - 2wy			2yz + 2wx		1 - 2x^2 - 2y^2 |
    f32 xx = 1.f - (2.f * (qy * qy));
    f32 yx = 2.f * (qx * qy);
    f32 zx = -2.f * (qy * qw);

    f32 yy = 1.f - (2.f * (qx * qx));
    f32 zy = 2.f * (qx * qw);

    f32 yz = -2.f * (qx * qw);
    f32 zz = 1.f - (2.f * (qx * qx)) - (2 * (qy * qy));
	
	// In this rotation matrix...
	// 1st column: xx, yx, and zx describe where limb A's x-axis would point in limb A's idle 3D grid (-Z) after rotation
	// 2nd column: xy, yy, and zy describe where limb A's y-axis would point in limb A's idle 3D grid (-Z) after rotation
	// 3rd column: xz, yz, and zz describe where limb A's z-axis would point in limb A's idle 3D grid (-Z) after rotation

    // Convert to the regular ZYX angles expected by the joint table of ponytail
	// Find length of rotated x-axis on XY plane only
    f32 horizontal = sqrtf((xx * xx) + (yx * yx));

	// Find rotation by y-axis based on rotation of x-axis on 3d grid relative to ponytail limb A's idle rotation (-Z)
    rotation->y = Math_Atan2S_XY(horizontal, -zx);

	// Normal case, where rotated x-axis' length is greater than 0 on XY plane
    if (horizontal > GREATER_THAN_ZERO) {
		// Find rotation by x-axis using y-axis' z position (sin(x)) and z-axis' z position (cos(x))
		// This is possible because zy = cos(y) * sin(x) and zz = cos(y) * cos(x)
        rotation->x = Math_Atan2S_XY(zz, zy);
		// Find rotation by z-axis using x-axis' rotation 
        rotation->z = Math_Atan2S_XY(xx, yx);
    }
	// Edge case, where rotated x-axis' length is essentially 0 on XY plane
    else {
		// Find rotation by x-axis using y-axis' y position and z-axis' -y position
        rotation->x = Math_Atan2S_XY(yy, -yz);
		// Find rotation by z-axis using x-axis' rotation; it's 0 because there is essentially no line on XY plane
        rotation->z = 0;
    }
}


/**
 * @brief Rotate ponytails' limbs (joints) based on phys bones' phys limbs' positions
 * 
 * This function takes the ponytail and rotates its limbs so that it matches with the ponytail's physics
 * bones. More specifically, the actual ponytail bones are rotated using a matrix so that the ponytail's 
 * bones match with the rotation of the physics bones.
 * 
 * @param this          Ponytail actor
 * @param gPhysBones    Physics bones that have been impacted by verlet integration for this frame
 * @param player        Player actor
 */
void Ponytail_RotateJoints(Ponytail* this, PhysBone* gPhysBones[], Player* player) {
    Vec3f zero_vec = {0.f, 0.f, 0.f};

    // Set up matrix to face in the direction of ponytail
    Matrix_Push();
    // Make matrix face ponytail actor's rotation (which should also be player actor's rotation)
    // Y-axis primarily defines actor's normal facing/yaw direction, so rotate by Y-axis first
    Matrix_SetTranslateRotateYXZ(0.f, 0.f, 0.f, &this->actor.shape.rot);    
    // Rotate matrix again, this time by the ponytail's root rotation (player's skeleton head limb)
    Matrix_TranslateRotateZYX(&zero_vec, &this->skelAnime.jointTable[PONYTAIL_ROOT_ROT]);

    // Rotate every bone in ponytail according to matching phys bone (except phys bone with 2 pinned limbs)
    for (int i = PONYTAIL_BONE_LIMB1_LIMB2; i < PONYTAIL_BONE_MAX; i++) {
        PhysBone* physBone = gPhysBones[i];                     // Take matching phys bone
        Vec3s* jointRot = &this->skelAnime.jointTable[i + 1];   // Joint's rotation in ponytail skeleton
        Vec3f worldDir = {0.f, 0.f, 0.f};                       // Direction of phys bone in respect to world
        Vec3f localDir = {0.f, 0.f, 0.f};                       // Direction of phys bone in respect to limb A

        // Find world direction of current phys bone by doing:   worldDir = limb_b position - limb_a position
        Math_Vec3f_Diff(&physBone->limb_b->curr_pos, &physBone->limb_a->curr_pos, &worldDir);

        // Find length^2 of current phys bone in respect to world
        f32 lengthSquared = Math3D_Vec3fMagnitudeSq(&worldDir);

        // If length of target phys bone is not 0, then translate world direction of phys bone into local direction 
        // in respect to target phys bone's limb A 
        if (!(physBone->limb_a->pinned && physBone->limb_b->pinned) && lengthSquared > GREATER_THAN_ZERO) {
            // Get current matrix, which is currently oriented in respect to limb_a of phys bone
            MtxF* bone_limbA = Matrix_GetCurrent();

            // Convert phys bone's world direction limb A's local space
            // localDir = transposed limb A's rotation * worldDir, which is also:
            // Dot product between each axis of limb A and world direction of phys bone
            // localDir.x = dot(limb A's X-axis, phys bone's world direction)
            // localDir.y = dot(limb A's Y-axis, phys bone's world direction)
            // localDir.z = dot(limb A's Z-axis, phys bone's world direction)
            CustomMath_Vec3f_InverseTransformDirection(bone_limbA, &worldDir, &localDir);

            // For bone where limb A is pinned and limb B is unpinned, rotate with shortest possible rotation.
			// Limitations are set for bone with pinned limb A to avoid rotation that visually looks incorrect
            if (i == PONYTAIL_BONE_LIMB1_LIMB2) {
                Ponytail_RotateFromPinned(&localDir, jointRot);
            }
            // For every other bone where both limb A and limb B are unpinned, calculate rotation with more freedom
            else {
                // Find length of localDir on horizontal (XZ) plane
                f32 horizontal = sqrtf((localDir.x * localDir.x) + (localDir.z * localDir.z));

                // X-axis rotation of limb A = Atan(horizontal local direction, vertical local direction)
                jointRot->x = Math_Atan2S_XY(horizontal, localDir.y);

                // Y-axis rotation of limb A = Atan(z local direction, x local direction)
                if (horizontal > GREATER_THAN_ZERO) {
                    jointRot->y = Math_Atan2S_XY(-localDir.z, -localDir.x);
                }

                // Z-axis rotation of limb A = 0 (not used)
                jointRot->z = 0;
            }
        }

        // Include this joint's rotation when calculating its child.
        Matrix_TranslateRotateZYX(&zero_vec, jointRot);
    }

    // Pop matrix
    Matrix_Pop();
}


/**
 * @brief Update draw state of ponytail based on current state of player
 * 
 * This function updates the draw state of the ponytail based on the player's current state. In other words,
 * it assigns a variable (hideInFirstPerson) indicating whether the ponytail should be drawn or not based 
 * on the following states:
 * 
 * 1. C-Up First Person View:
 * 
 *      - If the camera is in first person mode AND is less than a specific distance away from the player's 
 *        head, then hide the ponytail
 * 
 * 2. Bow/Hookshot First Person View
 * 
 *      - If the player is in first person view with bow or hookshot And the camera is less than a specific
 *        distance away from the player's head, then hide the ponytail
 * 
 * 3. Transformed into non-human
 *      - If the player transforms into a non-human form (Deku, Goron, Zora, FD), then hide the ponytail
 * 
 * @param thisx Ponytail actor
 * @param play  Current playstate
 */
void Ponytail_Update(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Ponytail* this = (Ponytail*)thisx;

    // Bool that indicates if ponytail should be drawn or not in main draw function
    this->hideInFirstPerson = 0;

    // Get reset positions of overcoats' phys limbs when transforming from non-human to human
    if (player->transformation != PLAYER_FORM_HUMAN) {
        this->needsReset = 1;
    }

    // Explaining some states:
    // - PLAYER_STATE1_100000 indicates first person mode for bow/hookshot first person
    // - Camera->mode helps indicate first person mode (6) for C-Up
    // - There is no player state for when view is transitioning from third person to regular first person view, 
    //   so distance between camera and head is used

    // Camera information
    Camera* camera = GET_ACTIVE_CAM(play);
    f32 cameraDist = Math_Vec3f_DistXYZ(&play->view.eye, &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);

    // C-up first person
    if (player->transformation == PLAYER_FORM_HUMAN && this != NULL &&
        camera->mode == FIRST_PERSON_VIEW_MODE && cameraDist < MIN_CAMERA_DISTANCE) {
        this->hideInFirstPerson = 1;
    }

    // Bow/hookshot first person
    if (player->transformation == PLAYER_FORM_HUMAN && this != NULL &&
        (player->stateFlags1 & PLAYER_STATE1_100000) && 
        cameraDist < MIN_CAMERA_DISTANCE) {
        this->hideInFirstPerson = 1;
    }

    // Transform from human to non-human form
    if (player->transformation != PLAYER_FORM_HUMAN && gPlayerPonytail != NULL && player->actor.draw == NULL) {
        this->hideInFirstPerson = 1;
    }
}


//=================
// Ponytail Draw
//=================

/**
 * @brief Renders ponytail and operates its physics every frame
 * 
 * This function renders the ponytail and operates its physics every frame. This is done by using physics 
 * functions based on Verlet Integration.
 * 
 * To accomplish this, three objects are set up:
 *      
 *      a) Ponytail actor
 *          
 *          - A ponytail actor with the ponytail model and its limbs generated via Fast64
 *      
 *      b) gPonytailPhysLimbs
 *          
 *          - A physics simulator object representing the ponytail model's limbs
 *      
 *      c) gPonytailPhysBones
 *          
 *          - An object containing bones of the phys limbs, where a bone contains 2 phys limbs (endpoints)
 *            and the bone's defined length
 * 
 * The process of this function goes as follows:
 * 
 * 1. Using player's current velocity and previous velocity (as in, velocity from previous frame), the net force
 *    being applied to the player is found.
 * 
 * 2. With the net force, player's velocity information, and the phys limbs' position information, verlet 
 *    integration is used to calculate the ponytail limbs' new position. This information is stored in the 
 *    physics limbs and bones.
 * 
 * 3. Using the physics bones' limbs' positions, rotate the actual ponytail bones so that each bone's limbs 
 *    match with the physics bones' limbs' positions.
 * 
 */
RECOMP_HOOK_RETURN("Player_Draw") void main_Draw_Ponytail_with_Physics(void) {
    // Only draw when mod's config indicates that ponytail needs to be drawn instead of 'default'
    if (!recomp_get_config_u32("change_hairstyle") || gPlayerPonytail == NULL || gPlayStatePonytail == NULL) {
        return;
    }
    
    // Call player and ponytail's current state
    Player* player = GET_PLAYER(gPlayStatePonytail);
    PlayState* play = gPlayStatePonytail;
    
    // Only draw when human and not transforming into non-human form
    if (recomp_get_config_u32("change_hairstyle") && player->transformation == PLAYER_FORM_HUMAN && 
    player->actor.draw != NULL && !(player->stateFlags2 & PLAYER_STATE2_20000000)) {
        Ponytail* this = gPlayerPonytail;

        // Don't draw ponytail while in first person view with bow or hookshot
        if (this->hideInFirstPerson) {
            this->needsReset = 1;
            return;
        }

        // Update ponytail physics only when game is not paused or in transition state into pause menu
        if (play->pauseCtx.state == 0) {
            // Calculate current net force using change in velocity (current vs previous frame) and gravity
            Vec3f net_force = { 0.f, 0.f, 0.f };
            Verlet_UpdatePhysPlayerVelocity(&gJackiePhysPlayer, player);
            Verlet_CalcNetForce(&gJackiePhysPlayer, (f32)GRAVITY, &net_force);

            // Update Ponytail's positions based on Verlet Integration, and save new positions of limbs in 
            // phys limbs/bones
            Ponytail_UpdateBodyPartsPos(this, player, net_force, ponytailPhysLimbs, ponytailPhysBones);

            // Calculate rotations based on ponytail's calculated positions
            Ponytail_RotateJoints(this, ponytailPhysBones, player);
        }

        // Draw Ponytail with all the physics-based rotations applied on the actual ponytail limbs
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 
            &this->actor.shape.rot);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        func_80122868(play, player);    // Draw blinking effect of ponytail when player gets hit or jinxed
        SkelAnime_DrawFlexOpa(
            play,
            this->skelAnime.skeleton,
            this->skelAnime.jointTable,
            this->skelAnime.dListCount,
            NULL,
            NULL,
            &this->actor
        );

        // Reset fog to scene's default values after drawing so that other actors don't get affected
        if (player->invincibilityTimer > 0 || gSaveContext.jinxTimer != 0) {
            POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}


/**
 * @brief Function unused; use main_Draw_Ponytail_with_Physics() instead.
 * 
 * All drawing functionality has been moved to main_Draw_Ponytail_with_Physics() due to this function not 
 * firing during the Song of Time, Inverted Song of Time, Double Song of Time cutscenes.
 * 
 * In other words, the ponytail does not get drawn for the frame during these cutscenes, removing them from 
 * the game during the cutscene.
 */
void Ponytail_Draw(Actor* thisx, PlayState* play) {
    
}