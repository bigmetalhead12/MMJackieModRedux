/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Applying verlet integration to each limb of Jackie's belt strap hook
    model and then drawing them to Majora's Mask recomp

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
#define LIMB_MASS   0.3f
#define PINNED      1
#define NOT_PINNED  0
#define COLLISION_FACTOR    75
#define BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS   0.3f
#define LEFTTHIGH_COLLIDER_DEFAULT_RADIUS 4.65f
#define RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS 4.65f
#define TORSO_COLLIDER_DEFAULT_RADIUS 5.5f
#define GREATER_THAN_ZERO   0.000001f

/*
=================
Rendering Macros
=================
*/
#define MIN_CAMERA_DISTANCE 10.0f
#define FIRST_PERSON_VIEW_MODE 6

#include "z_beltStrapHook.h"
#include "verlet_physics.h"
#include "proxymm_custom_actor.h"
#include "physics_collision.h"
#include "math.h"
#include "customMath.h"

// For zero-ing player velocity when Jackie is opening a door
extern void Player_Action_36(Player* player, PlayState* play);

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));


#define FLAGS 0

/***********************************************************************

	Belt Strap Hook Physics

***********************************************************************/

/*
=================
Belt Strap Hook Limbs
=================
*/
// Enum of Belt Strap Hook limbs for jointTable
typedef enum BeltStrapHookLimbs {
    /*  0 */ BELTSTRAPHOOK_ROOT_POS,        // Belt strap hook's Root bone (position)
    /*  1 */ BELTSTRAPHOOK_ROOT_ROT,        // Belt strap hook's Root bone (rotation)
    /*  2 */ BELTSTRAPHOOK_LEFT_LIMB1,
    /*  3 */ BELTSTRAPHOOK_LEFT_LIMB2,
    /*  4 */ BELTSTRAPHOOK_LEFT_LIMB3,
    /*  5 */ BELTSTRAPHOOK_LEFT_LIMB4,
    /*  6 */ BELTSTRAPHOOK_LEFT_LIMB5,
    /*  7 */ BELTSTRAPHOOK_LEFT_LIMB6,
    /*  8 */ BELTSTRAPHOOK_LEFT_LIMB7,
    /*  9 */ BELTSTRAPHOOK_LEFT_LIMB8,
    /*  10*/ BELTSTRAPHOOK_LEFT_LIMB9,
    /*  11*/ BELTSTRAPHOOK_LEFT_LIMB10,
    /*  12*/ BELTSTRAPHOOK_LEFT_LIMB11,
    /*  13*/ BELTSTRAPHOOK_RIGHT_LIMB1,
    /*  14*/ BELTSTRAPHOOK_RIGHT_LIMB2,
    /*  15*/ BELTSTRAPHOOK_RIGHT_LIMB3,
    /*  16*/ BELTSTRAPHOOK_RIGHT_LIMB4,
    /*  17*/ BELTSTRAPHOOK_RIGHT_LIMB5,
    /*  18*/ BELTSTRAPHOOK_RIGHT_LIMB6,
    /*  19*/ BELTSTRAPHOOK_RIGHT_LIMB7,
    /*  20*/ BELTSTRAPHOOK_RIGHT_LIMB8,
    /*  21*/ BELTSTRAPHOOK_RIGHT_LIMB9,
    /*  22*/ BELTSTRAPHOOK_RIGHT_LIMB10,
    /*  22*/ BELTSTRAPHOOK_RIGHT_LIMB11
} BeltStrapHookLimbs;

/*
=================
Set Belt Strap Hook as Custom Actor
=================
*/
// Sets profile for belt strap hook before registering it as actor
ActorProfile BeltStrapHook_Profile = {
    ACTOR_ID_MAX,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(BeltStrapHook),
    BeltStrapHook_Init,
    BeltStrapHook_Destroy,
    BeltStrapHook_Update,
    BeltStrapHook_Draw,
};

s16 CUSTOM_ACTOR_BELTSTRAPHOOK = ACTOR_ID_MAX;


/**
 * @brief Register belt strap hook as custom actor
 */
RECOMP_CALLBACK("*", recomp_on_init) void BeltStrapHook_OnRecompInit() {
    CUSTOM_ACTOR_BELTSTRAPHOOK = CustomActor_Register(&BeltStrapHook_Profile);
}


/*
=================
Belt Strap Hook Init
=================
*/
BeltStrapHook* gPlayerBeltStrapHook = NULL;
PlayState* gPlayStateBeltStrapHook = NULL;


/**
 * @brief Create custom actor for belt strap hook on player_init
 * 
 * @param thisx     Actor pointer
 * @param play      Current playstate
 */
RECOMP_HOOK("Player_Init") void BeltStrapHook_on_player_init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_outfit") && gPlayerBeltStrapHook == NULL) {
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, CUSTOM_ACTOR_BELTSTRAPHOOK, 
                                    -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0, 
                                    0, 0);
    }
}


/**
 * @brief Initialize the position of the phys limbs of each chain in the belt strap hook
 * 
 * Take a chain of the belt strap hook (indicated by the starting and ending index values) and initialize
 * their respective phys limbs' starting positions
 * 
 * @param this		    BeltStrapHook actor
 * @param player	    Player actor
 * @param gPhysLimbs    Phys limbs for belt strap hook
 * @param chainStartIdx Starting index for chain of belt strap hook
 * @param chandEndIdx   Ending index for chain of belt strap hook
 */
void BeltStrapHook_InitChain(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], int chainStartIdx,
	int chainEndIdx) {
	Vec3f playerVel = player->actor.velocity;
	Vec3f zeroVel = { 0.f, 0.f, 0.f };
		
	for (int i = chainStartIdx; i <= chainEndIdx; i++) {
        PhysLimb* physLimb = gPhysLimbs[i];

        // Rotate the limb's default local offset into Jackie's world-facing direction.
        Vec3s rotatedOffset = { 0, 0, 0 };
        Vec3f transformVec = { 0.f, 0.f, 0.f };

        // Rotate the translation vector from parent limb to current limb with actor's rotation
        CustomMath_Vec3s_Rotate(&physLimb->default_jointPos, &player->actor.shape.rot, &rotatedOffset);

        // Scale the rotated translation vector
        CustomMath_Vec3s_Scale_ToVec3f(&rotatedOffset, this->actor.scale.x, &transformVec);

        // First limb of either chain of belt strap hook attaches directly to the root limb
        if (i == chainStartIdx) {
            Math_Vec3f_Sum(&this->bodyPartsPos[BELTSTRAPHOOK_BODYPART_ROOT], &transformVec, 
				&this->bodyPartsPos[i]);
        }
        // For rest of unpinned limbs, apply rotated translation vector to parent limb to find
        // current limb's position in world space
        else {
            Math_Vec3f_Sum(&this->bodyPartsPos[i - 1], &transformVec, &this->bodyPartsPos[i]);
        }

        // Use an index relative to the beginning of this chain.
        int chainLimbIdx = i - chainStartIdx + 1;
        f32 limbMass = LIMB_MASS * chainLimbIdx;

        // Initialize the phys limbs
        // First phys limb of chain of belt strap hook. This one directly inherits the player's
        // velocity
        if (i == chainStartIdx) {
            Verlet_InitLimb(physLimb, this->bodyPartsPos[i], playerVel, limbMass, PINNED,
                BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS);
        }
        // Rest of unpinned phys limbs of chain of belt strap hook. The rest do not inherit
        // the velocity 
        else {
            Verlet_InitLimb(physLimb, this->bodyPartsPos[i], zeroVel, limbMass, NOT_PINNED,
				BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS);
        }
    }
}


/**
 * @brief Initialize phys bones of each chain in the belt strap hook
 * 
 * Take the phys limbs and initialize the phys bones with them. This sets up every phys bone's limb A,
 * limb B, and length between them using Verlet_InitBone().
 * 
 * @param gPhysLimbs    Phys limbs for belt strap hook
 * @param gPhysBones    Phys bones for belt strap hook
 * @param chainStartIdx Starting index for chain of belt strap hook limbs
 * @param chandEndIdx   Ending index for chain of belt strap hook limbs
 * @param boneStartIdx  Starting index for bones in chain 
 */
void BeltStrapHook_InitChainBones(PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[], int chainStartIdx,
    int chainEndIdx, int boneStartIdx) {

    // Index for phys bones where the chain begins
    int boneIdx = boneStartIdx;

    // Iterate through phys limbs and match them with respective phys bone
    for (int limbBIdx = chainStartIdx; limbBIdx <= chainEndIdx; limbBIdx++, boneIdx++) {
        int limbAIdx = 0;

        // First bone connects the shared root to Limb1.
        if (limbBIdx == chainStartIdx) {
            limbAIdx = BELTSTRAPHOOK_BODYPART_ROOT;
        }
        // Limb A's index will always be 1 less than limb B's index
        else {
            limbAIdx = limbBIdx - 1;
        }

        // Initialize current phys bone, which assigns its limb A, limb B, and length
        Verlet_InitBone(gPhysBones[boneIdx], gPhysLimbs[limbAIdx], gPhysLimbs[limbBIdx]);
    }
}



/**
 * @brief Set up initial phys limbs and phys bones
 * 
 * Initialize all of the phys limbs' positions and velocity based on the player's rotation and velocity. Also, 
 * initialize all of the phys bones, which also initializes the distance between every phys limbs.
 * 
 * @param this          Belt strap hook actor
 * @param player        Player actor
 * @param gPhysLimbs    Phys limbs to represent positions for respective belt strap hook standard limbs after verlet
 */
void BeltStrapHook_SetDefaultBodyPartsPos(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[]) {
    // Set belt strap hook velocity
    Vec3f playerVelocity = player->actor.velocity;

    // Set position and rotation of belt strap Hook
    Math_Vec3f_Copy(&this->actor.world.pos, &player->bodyPartsPos[PLAYER_BODYPART_TORSO]);
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);
    Math_Vec3s_Copy(&this->actor.world.rot, &player->actor.world.rot);

    // Root limb's BodyPartsPos and gPhysLimb positions and velocity
    // Velocity only gets assigned to root limb
    // BodyPartsPos keeps track of global XYZ position of each limb
    Math_Vec3f_Copy(&this->bodyPartsPos[BELTSTRAPHOOK_BODYPART_ROOT], &player->bodyPartsPos[PLAYER_BODYPART_TORSO]);
    Verlet_InitLimb(gPhysLimbs[BELTSTRAPHOOK_BODYPART_ROOT], this->actor.world.pos, playerVelocity, 
        LIMB_MASS, PINNED, BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS);

    // Root limb's jointTable for position
    Vec3s rootPos_Vec3s = {0, 0, 0};
    Math_Vec3f_ToVec3s(&rootPos_Vec3s, &this->actor.world.pos);
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &rootPos_Vec3s);
    Math_Vec3s_Copy(&gPhysLimbs[BELTSTRAPHOOK_BODYPART_ROOT]->default_jointPos, &rootPos_Vec3s);

    // Root limb's jointTable for rotation
    Vec3s newRootJointRot = { 0, 0, 0};
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &newRootJointRot);

    // Initialize the phys limbs of the two chains of limbs in belt strpa hook
    // Initialize limbs in left belt strap hook
    BeltStrapHook_InitChain(this, player, gPhysLimbs, BELTSTRAPHOOK_BODYPART_LEFTLIMB1, 
        BELTSTRAPHOOK_BODYPART_LEFTLIMB11);

    // Initialize limbs in right belt strap Hook
    BeltStrapHook_InitChain(this, player, gPhysLimbs, BELTSTRAPHOOK_BODYPART_RIGHTLIMB1, 
        BELTSTRAPHOOK_BODYPART_RIGHTLIMB11);

    // Initialize phys bones by connecting the phys limbs together
    // Initialize phys bones in left belt strap hook
    BeltStrapHook_InitChainBones(gPhysLimbs, gPhysBones,
        BELTSTRAPHOOK_BODYPART_LEFTLIMB1, BELTSTRAPHOOK_BODYPART_LEFTLIMB11,
        BELTSTRAPHOOK_BONE_ROOT_LEFT_LIMB1);

    // Initialize phys bones in right belt strap hook
    BeltStrapHook_InitChainBones(gPhysLimbs, gPhysBones,
        BELTSTRAPHOOK_BODYPART_RIGHTLIMB1, BELTSTRAPHOOK_BODYPART_RIGHTLIMB11,
        BELTSTRAPHOOK_BONE_ROOT_RIGHT_LIMB1);
}


/**
 * @brief Initialize belt strap hook model and actor along with its limbs
 *
 * This function initializes the belt strap hook model and its custom actor. When doing this, the function also
 * takes the standardLimbs' positions of the beltStrapHook model (gBeltStrapHookSkel.c/h) generated via Fast 64. 
 * These positions are then stored into the phys limbs, which are used for verlet integration.
 * 
 * @param thisx		Actor
 * @param play		Current playstate
 */
void BeltStrapHook_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_outfit")) {
		// Initialize belt strap hook's states
        Player* player = GET_PLAYER(play);
        BeltStrapHook* this = (BeltStrapHook*)thisx;
        this->actor.room = -1;
        this->needsReset = 1;
        this->hideInFirstPerson = 0;

        gPlayerBeltStrapHook = this;
        gPlayStateBeltStrapHook = play;

        // Capture default jointPos
        // I can't think of any way aside from hardcoding, because the standardlimbs' jointPos values
        // keep getting updated based on its last jointPos values before map reloads.
        // Values are directly copied from the generated Fast64 model file (in my case, gBeltStrapHookSkel.c)
        Vec3s left_limb1_jointPos = { 174, -34, -364 };		// Skip the root limb and start with left limb 1
        Vec3s left_limb2_jointPos = { 0, 0, -230 };
        Vec3s left_limb3_jointPos = { 0, 0, -191 };
        Vec3s left_limb4_jointPos = { 0, 0, -234 };
        Vec3s left_limb5_jointPos = { 0, 0, -254 };
        Vec3s left_limb6_jointPos = { 0, 0, -254 };
        Vec3s left_limb7_jointPos = { 0, 0, -254 };
        Vec3s left_limb8_jointPos = { 0, 0, -254 };
        Vec3s left_limb9_jointPos = { 0, 0, -254 };
        Vec3s left_limb10_jointPos = { 0, 0, -254 };
        Vec3s left_limb11_jointPos = { 0, 0, -252 };
        Vec3s right_limb1_jointPos = { -174, -34, -364 };	// Right limb 1
        Vec3s right_limb2_jointPos = { 0, 0, -230 };
        Vec3s right_limb3_jointPos = { 0, 0, -191 };
        Vec3s right_limb4_jointPos = { 0, 0, -234 };
        Vec3s right_limb5_jointPos = { 0, 0, -254 };
        Vec3s right_limb6_jointPos = { 0, 0, -254 };
        Vec3s right_limb7_jointPos = { 0, 0, -254 };
        Vec3s right_limb8_jointPos = { 0, 0, -254 };
        Vec3s right_limb9_jointPos = { 0, 0, -254 };
        Vec3s right_limb10_jointPos = { 0, 0, -254 };
        Vec3s right_limb11_jointPos = { 0, 0, -252 };

        // default_jointPos values for every limb should NEVER change once assigned here
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[1]->default_jointPos, &left_limb1_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[2]->default_jointPos, &left_limb2_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[3]->default_jointPos, &left_limb3_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[4]->default_jointPos, &left_limb4_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[5]->default_jointPos, &left_limb5_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[6]->default_jointPos, &left_limb6_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[7]->default_jointPos, &left_limb7_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[8]->default_jointPos, &left_limb8_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[9]->default_jointPos, &left_limb9_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[10]->default_jointPos, &left_limb10_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[11]->default_jointPos, &left_limb11_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[12]->default_jointPos, &right_limb1_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[13]->default_jointPos, &right_limb2_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[14]->default_jointPos, &right_limb3_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[15]->default_jointPos, &right_limb4_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[16]->default_jointPos, &right_limb5_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[17]->default_jointPos, &right_limb6_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[18]->default_jointPos, &right_limb7_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[19]->default_jointPos, &right_limb8_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[20]->default_jointPos, &right_limb9_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[21]->default_jointPos, &right_limb10_jointPos);
        Math_Vec3s_Copy(&beltStrapHook_PhysLimbs[22]->default_jointPos, &right_limb11_jointPos);

        Actor_SetScale(&this->actor, 0.01f);
        SkelAnime_InitFlex(
            play,
            &this->skelAnime,
            &gBeltStrapHookSkel,
            NULL,
            this->jointTable,
            this->morphTable,
            GBELTSTRAPHOOKSKEL_NUM_LIMBS
        );

        // Verlet Integration starts here
        Verlet_InitPhysPlayer(&gBeltStrapHookPhysPlayer, player);
        BeltStrapHook_SetDefaultBodyPartsPos(this, player, beltStrapHook_PhysLimbs, beltStrapHook_PhysBones);
    }
}


/**
 * @brief Reset the belt strap hook's phys limbs' position when Jackie's door opening animation plays.
 */
RECOMP_HOOK("Player_Door_Knob") void BeltStrapHook_ResetOnDoorOpen(PlayState* play, Player* player,
    Actor* door) {
    // Ignore this if Jackie is not in human form and overcoat is not drawn.
    if (player != GET_PLAYER(play) || player->transformation != PLAYER_FORM_HUMAN ||
        gPlayerBeltStrapHook == NULL) {
        return;
    }

    // If Jackie is human and belt strap hook is being drawn, then make sure to reset belt strap hook shape.
    gPlayerBeltStrapHook->needsReset = 1;
}


/*
=================
Belt Strap Hook Destroy
=================
*/
/**
 * @brief Destroy belt strap hook model and actor
 * 
 * @param thisx		Actor pointer
 * @param play		Current playstate
 */
void BeltStrapHook_Destroy(Actor* thisx, PlayState* play) {
    gPlayerBeltStrapHook = NULL;
}


/*
=================
Belt Strap Hook Update
=================
*/

// Torso values for setting root for belt strap hooks model and torso sphere collider
Vec3f torso_globalPos = { (f32)0, (f32)0, (f32)0 };         // World position of player's torso limb
Vec3s torso_rotate = {0, 0, 0};                             // Rotation value for player's torso (with parent 
															// limbs' rotations included)

// Sphere colliders
PhysSphereCollider torsoSphereCollider = {
    { 0.0f, 0.0f, 0.0f },
    TORSO_COLLIDER_DEFAULT_RADIUS
};
PhysSphereCollider leftThighSphereCollider = {
    { 0.0f, 0.0f, 0.0f },
    LEFTTHIGH_COLLIDER_DEFAULT_RADIUS
};
PhysSphereCollider rightThighSphereCollider = {
    { 0.0f, 0.0f, 0.0f },
    RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS
};


/**
 * @brief Get Jackie's torso limb's world position and rotation and also set up collider center world 
 * positions.
 * 
 * This function looks through Jackie's limbs. There are 3 limbs in particular that this is used for: 
 * torso, left thigh limb, and right thigh limbs. The limbs are also observed to assign respective 
 * colliders for the belt strap hook.
 */
RECOMP_HOOK("Player_PostLimbDrawGameplay") void BeltStrapHook_on_Player_PostLimbDrawGameplay(PlayState* play, 
	s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    /********************************
     Torso Limb Positions and Rotations
    ********************************/
    if (limbIndex == PLAYER_LIMB_TORSO) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's Torso limb
        MtxF* mtx = Matrix_GetCurrent();

        // Get torso's world position directly from the current matrix
        torso_globalPos.x = mtx->xw;
        torso_globalPos.y = mtx->yw;
        torso_globalPos.z = mtx->zw;

        // Get torso rotation values (used for rotating the belt strap hook actor properly)
        Matrix_MtxFToYXZRot(mtx, &torso_rotate, 1);

        // Move the torso sphere collider
        Vec3f torsoColliderLocalOffset = {0.f, 300.f, 0.f};

        // Multiply the current matrix that has the torso position by the offset vector local to
        // torso to apply the center of the torso collider to the torso limb with offset applied.
        Matrix_MultVec3f(&torsoColliderLocalOffset, &torsoSphereCollider.center);

		// Close matrix
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }

    /********************************
     Left Thigh Limb Positions
    ********************************/
    if (limbIndex == PLAYER_LIMB_LEFT_THIGH) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's left thigh limb
        MtxF* mtx = Matrix_GetCurrent();

        // Local offset for the left thigh collider (100 units back)
        Vec3f leftThighColliderLocalOffset = {0.f, 100.f, 0.f};
        // Apply local offset to current world position of left thigh and apply that position as the left 
        // thigh collider.
        Matrix_MultVec3f(&leftThighColliderLocalOffset, &leftThighSphereCollider.center);

		// Pop matrix
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }

    // Right thigh
    if (limbIndex == PLAYER_LIMB_RIGHT_THIGH) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's right thigh limb
        MtxF* mtx = Matrix_GetCurrent();

        // Local offset for the right thigh collider (100 units back)
        Vec3f rightThighColliderLocalOffset = {0.f, 100.f, 0.f};
        // Apply local offset to current world position of right thigh and apply that position as the 
        // right thigh collider.
        Matrix_MultVec3f(&rightThighColliderLocalOffset, &rightThighSphereCollider.center);

		// Pop matrix
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }
}


/**
 * @brief Update the a given belt strap hook chain's respective phys limbs based on Verlet Integration
 *
 * This function takes one of the belt strap hook's chains based on the starting and ending index values.
 * Afterward, the function goes through every phys limb respective to the chain's actual limbs and updates
 * the phys limbs' position based on Verlet Integration.
 * 
 * It is important to note, however, that the pinned and unpinned limbs are handled differently.
 *
 * 1. The pinned limb is not supposed to be impacted by physics since it is pinned. So, it is only positioned
 * 	  and rotated in the world space properly by manually calculating the distance and rotation from its parent
 * 	  limb: the belt strap hook's root limb.
 *
 * 2. The unpinned limbs' positions are more freely updated via this codebase's Verlet Integration function, the
 * 	  Verlet_LimbUpdatePos() function.
 *
 * Keep note that this function only updates the position of phys limbs. The actual rotation of belt strap hook's
 * limbs are done outside of this function based on the phys limbs' positions calculated in this function.
 *
 * @param this			BeltStrapHook actor
 * @param player		Player actor
 * @param apply_force	Force that is to be acted upon the belt strap hook's chain's phys limbs.
 * @param gPhysLimbs	Phys limbs of belt strap hook. 
 */
void BeltStrapHook_UpdateChain(BeltStrapHook* this, Player* player, Vec3f* apply_force, PhysLimb* gPhysLimbs[], 
	int chainStartIdx, int chainEndIdx) {
	for (int i = chainStartIdx; i <= chainEndIdx; i++) {
		PhysLimb* physLimb = gPhysLimbs[i];
		
		// If phys limb is pinned
		if (physLimb->pinned) {
			// Save previous position and velocity of phys limb
			Math_Vec3f_Copy(&physLimb->prev_pos, &physLimb->curr_pos);
			Math_Vec3f_Copy(&physLimb->prev_vel, &physLimb->curr_vel);
			
			// Calculate pinned limb's local offset in world space
			Vec3f transformVec = { 0.f, 0.f, 0.f };
			Vec3s rootRotatedOffset = { 0, 0, 0 };
			Vec3s worldRotatedOffset = { 0, 0, 0 };
			
			// Take first phys limb's default joint position (which is just local translation from parent limb
			// during idle state) and rotate it by the current rotation of the root limb. This gives a rotated
			// translation vector.
			CustomMath_Vec3s_Rotate(&physLimb->default_jointPos, &this->skelAnime.jointTable[LIMB_ROOT_ROT],
				&rootRotatedOffset);
			// Take the rotated translation vec and rotate it by the player's actor's shape's rotation to make 
			// the phys limb face the direction of the player. Note that this essentially makes the phys limb's 
			// translation vec respective to world space rather than just local to its own skeleton
			CustomMath_Vec3s_Rotate(&rootRotatedOffset, &player->actor.shape.rot, &worldRotatedOffset);
			// Scale phys limb's rotated world space translation vec from root limb according to model
			CustomMath_Vec3s_Scale_ToVec3f(&worldRotatedOffset, this->actor.scale.x, &transformVec);
			
			// First limb in both left & right chains in belt strap hook starts from the same root, so first
			// limb in the chain will always take reference from the same belt strap hook root
			if (i == chainStartIdx) {
				// Use the translation vec from root limb to phys limb rotated according to world space to
				// find the new position of belt strap hook's respective actual limb
				Math_Vec3f_Sum(&this->bodyPartsPos[BELTSTRAPHOOK_BODYPART_ROOT], &transformVec,
					&this->bodyPartsPos[i]);
			}
			/*	// Untested. Also unused because only one limb is pinned in chain
			else {
                Math_Vec3f_Sum(&this->bodyPartsPos[i - 1], &transformVec, &this->bodyPartsPos[i]);
            }*/
			
			// Save the calculated new current position of the actual limb into respective phys limb
			Math_Vec3f_Copy(&physLimb->curr_pos, &this->bodyPartsPos[i]);
			// Save current velocity to phys limb also
            Math_Vec3f_Copy(&physLimb->curr_vel, &player->actor.velocity);
		}
		// For rest of unpinned limbs in chain
		else {
            // Apply Verlet integration using Jackie's horizontal velocity to find the new position of unpinned
			// phys limb
            Vec3f player_vel = player->actor.velocity;
            player_vel.y = 0.f;

            // Disable the opposing movement while riding Epona, climbing ladder/wall, or opening door.
            // Note: Physics for riding Epona was disabled due to some unwanted physics behavior. This may be 
            //       due to the codebase relying on player->actor.velocity for player's world velocity. This works 
            //       for most cases EXCEPT when player is riding on Epona, where player->actor.velocity might be 
            //       saving the player's last frame's world velocity. 
            // Riding Epona: PLAYER_STATE1_800000
            // Climbing Wall: PLAYER_STATE1_200000
            // Opening Door: Player_Action_36
            if (player->stateFlags1 & (PLAYER_STATE1_800000 | PLAYER_STATE1_200000) ||
                player->actionFunc == Player_Action_36) {
                player_vel = (Vec3f){0.f, 0.f, 0.f};
            }
            Verlet_LimbUpdatePos(physLimb, apply_force, &player_vel);
        }
	}
}


/**
 * @brief Update the positions of phys limbs based on verlet integration
 * 
 * This function directly modifies the position of every phys limb of belt strap hook using verlet integration. 
 * It uses the following to re-calculate the position of every phys limb for the frame:
 * 
 * 1. Player's velocity
 * 
 * 2. Force being applied to player
 * 
 * 3. Length of bone containing two phys limbs (endpoints)
 * 
 * 4. Collision with collider(s) assigned to limb position in this function
 * 
 * The function also calculates the belt strap hook's current size based on the player's actor's size
 * 
 */
void BeltStrapHook_UpdateBodyPartsPos(BeltStrapHook* this, Player* player, Vec3f apply_force, 
	PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]) {
    // Save Jackie's previous position as belt strap hook's previous position
    Math_Vec3f_Copy(&this->actor.prevPos, &player->actor.prevPos);

	/********************************
     Reset phys limbs after map change
    ********************************/
    // Rebase all phys limbs' positions after entering a new map so old physics state does not cause the 
    // belt strap hook to jump or move unpredictably
    if (this->needsReset) {
        this->needsReset = 0;

        // Reset belt strap hook root directly to player's current torso position
        Math_Vec3f_Copy(&gPhysLimbs[BELTSTRAPHOOK_BODYPART_ROOT]->curr_pos, &torso_globalPos);
        Math_Vec3f_Copy(&gPhysLimbs[BELTSTRAPHOOK_BODYPART_ROOT]->prev_pos, &torso_globalPos);

        // Collapse the rest of the belt strap hook limbs onto the torso temporarily
        for (int i = BELTSTRAPHOOK_BODYPART_ROOT; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_pos, &torso_globalPos);
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_pos, &torso_globalPos);

            // Remove velocity carried over from previous map
            Vec3f zero_velocity = { (f32)0, (f32)0, (f32)0 };
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_vel, &zero_velocity);      // current vel
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_vel, &zero_velocity);      // previous vel
        }
        return;
    }

	/********************************
     Sync belt strap hook actor with Jackie
    ********************************/
    // Match Jackie's scale so belt strap hook actor scales correctly with giant's mask transformation
    Math_Vec3f_Copy(&this->actor.scale, &player->actor.scale);

    // Assign belt strap hook actor's position to Jackie's torso limb's position
    Math_Vec3f_Copy(&this->actor.world.pos, &torso_globalPos);

    // Make belt strap hook actor face the same general direction as Jackie
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);

	/********************************
     Set root position
    ********************************/
    // Set belt strap hook's root limb's bodyPartsPos (world position) to torso's world position
    Math_Vec3f_Copy(&this->bodyPartsPos[BELTSTRAPHOOK_BODYPART_ROOT], &torso_globalPos);
	
	// Copy world position of belt strap hook's root limb to the root phys limb
    Math_Vec3f_Copy(&gPhysLimbs[BELTSTRAPHOOK_BODYPART_ROOT]->curr_pos, &this->actor.world.pos);

    // Set belt strap hook actor's jointTable root position
    Vec3s newRootJointPos = { 0, 0, 0};
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &newRootJointPos);
	
	/********************************
     Calculate root rotation
    ********************************/
    // Convert Jackie's animated torso rotation into belt strap hook root's coordinate system.
    Vec3s newRootJointRot = { 0, 0, 0};
    newRootJointRot.x = -16384 + torso_rotate.z;
    newRootJointRot.y = -16384 + torso_rotate.y - this->actor.shape.rot.y;
    newRootJointRot.z = -torso_rotate.x;

	// Store the final root rotation used by the belt strap hook skeleton
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &newRootJointRot);

	/********************************
     Update belt strap hook limb positions
    ********************************/
    // Set BodyPartsPos for left limbs
    BeltStrapHook_UpdateChain(this, player, &apply_force, gPhysLimbs, 
		BELTSTRAPHOOK_BODYPART_LEFTLIMB1, BELTSTRAPHOOK_BODYPART_LEFTLIMB11);

    // Set BodyPartsPos for right limbs
    BeltStrapHook_UpdateChain(this, player, &apply_force, gPhysLimbs,
		BELTSTRAPHOOK_BODYPART_RIGHTLIMB1, BELTSTRAPHOOK_BODYPART_RIGHTLIMB11);

	/********************************
     Solve bone lengths and collision
    ********************************/
	// 0.01f is Jackie's normal actor scale.
    f32 colliderScale = player->actor.scale.x / 0.01f;

	// Set collider size
    torsoSphereCollider.radius = TORSO_COLLIDER_DEFAULT_RADIUS * colliderScale;
    leftThighSphereCollider.radius = LEFTTHIGH_COLLIDER_DEFAULT_RADIUS * colliderScale;
    rightThighSphereCollider.radius = RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS * colliderScale;
	
    // Bone update with collision interleaved
    for (int i = 0; i < COLLISION_FACTOR; i++) {
        for (int j = 0; j < (int)BELTSTRAPHOOK_BONE_MAX; j++) {
            Verlet_BoneConstraint(gPhysBones[j]);
            
            // After each bone constraint, check collision on limb_b
            if (!gPhysBones[j]->limb_b->pinned) {
                PhysCol_SolveCollision(gPhysBones[j]->limb_b, &torsoSphereCollider);
                PhysCol_SolveCapsuleFromSpheres(gPhysBones[j]->limb_b, &leftThighSphereCollider, 
					&rightThighSphereCollider);
            }
        }
    }

    // Final cleanup pass
    for (int i = 0; i < (int)BELTSTRAPHOOK_BODYPART_MAX; i++) {
        PhysCol_SolveCollision(gPhysLimbs[i], &torsoSphereCollider);
        PhysCol_SolveCapsuleFromSpheres(gPhysLimbs[i], &leftThighSphereCollider, &rightThighSphereCollider);
    }
}


/**
 * @brief Rotate a single chain in belt strap hook
 *
 * This function takes the phys bones and the actual bones from a chain of the belt strap hook actor using index 
 * values that indicate the start and end of the chain of bones.
 *
 * @param this				BeltStrapHook actor
 * @param gPhysBones		List of phys bones
 * @param chainStartIdx		Index value that indicates the starting bone of the chain of belt strap hook
 * @param chainEndIdx		Index value that indicates the end bone of the chain of belt strap hook
 */
void BeltStrapHook_RotateChain(BeltStrapHook* this, PhysBone* gPhysBones[], int chainStartIdx, int chainEndIdx) {
    Vec3f zero = { 0.f, 0.f, 0.f };

	// Set up matrix for rotating chain in belt strap hook
	Matrix_Push();

	// Process each bone in the given chain
	for (int i = chainStartIdx; i <= chainEndIdx; i++) {
		PhysBone* physBone = gPhysBones[i];						// Take matching phys bone
		Vec3s* jointRot = &this->skelAnime.jointTable[i + 1];	// Joint's rotation in belt strap hook skeleton
		Vec3f worldDir = { 0.f, 0.f, 0.f };						// Direction of phys bone in respect to world
		Vec3f localDir = { 0.f, 0.f, 0.f };						// Direction of phys bone in respect to limb A
	
		// Find world direction of current phys bone by doing:   worldDir = limb_b position - limb_a position
		Math_Vec3f_Diff(&physBone->limb_b->curr_pos, &physBone->limb_a->curr_pos, &worldDir);
		
		// Find length^2 of current phys bone in respect to world
        f32 lengthSquared = Math3D_Vec3fMagnitudeSq(&worldDir);
		
		if (!(physBone->limb_a->pinned && physBone->limb_b->pinned) && lengthSquared > GREATER_THAN_ZERO) {
			// Get current matrix, which is currently rotated to match limb_a of phys bone
            MtxF* bone_limbA = Matrix_GetCurrent();
			
			// Convert phys bone's world direction to limb A's local space
            // localDir = transposed limb A's rotation * worldDir, which is also:
            // Dot product between each axis of limb A and world direction of phys bone
            // localDir.x = dot(limb A's X-axis, phys bone's world direction)
            // localDir.y = dot(limb A's Y-axis, phys bone's world direction)
            // localDir.z = dot(limb A's Z-axis, phys bone's world direction)
            CustomMath_Vec3f_InverseTransformDirection(bone_limbA, &worldDir, &localDir);
			
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
		
		// Rotate matrix based on this bone's limb A's calculated rotation for next bone's limb A
		// This ensures that the next bone's limb A would be inheriting its parent limbs' rotations
		// In other words, the matrix would always represent current bone's limb A's local space/frame
		Matrix_TranslateRotateZYX(&zero, jointRot);
	}
	
	// Pop matrix
	Matrix_Pop();
}


/**
 * @brief Rotate belt strap hooks' limbs (joints) based on physics bones' phys limbs' positions
 * 
 * This function takes the belt strap hook model and rotates its limbs so that it matches with the belt strap 
 * hook's physics bones. More specifically, the actual ponytail bones are rotated using a matrix so that the 
 * belt strap hooks's bones match with the rotation of the physics bones.
 * 
 * @param this          Ponytail actor
 * @param gPhysBones    Physics bones that have been impacted by verlet integration for this frame
 * @param player        Player actor
 */
void BeltStrapHook_RotateJoints(BeltStrapHook* this, PhysBone* gPhysBones[], Player* player) {
    Vec3f zero = {0.f, 0.f, 0.f};
	
	// Set up matrix at belt strap hook's root
	// Get Matrix
	Matrix_Push();
	// Rotate matrix to belt strap hook actor's rotation
	Matrix_SetTranslateRotateYXZ(0.f, 0.f, 0.f, &this->actor.shape.rot);
	// Rotate matrix again, this time to match belt strap hook's root rotation, which is in respect to its 
	// own actor's rotation in the world
	Matrix_TranslateRotateZYX(&zero, &this->skelAnime.jointTable[BELTSTRAPHOOK_ROOT_ROT]);
	
	// Rotate left chain
	BeltStrapHook_RotateChain(this, gPhysBones, BELTSTRAPHOOK_BONE_LEFT_LIMB1_LEFT_LIMB2,
	BELTSTRAPHOOK_BONE_LEFT_LIMB10_LEFT_LIMB11);
	
	// Rotate right chain
	BeltStrapHook_RotateChain(this, gPhysBones, BELTSTRAPHOOK_BONE_RIGHT_LIMB1_RIGHT_LIMB2,
	BELTSTRAPHOOK_BONE_RIGHT_LIMB10_RIGHT_LIMB11);

	// Pop matrix
    Matrix_Pop();
}


/**
 * @brief Update draw state of belt strap hook based on current state of player
 * 
 * This function updates the draw state of the belt strap hook based on the player's current state. In other 
 * words, it assigns a variable (hideInFirstPerson) indicating whether the belt strap hook should be drawn or 
 * not based on the following states:
 * 
 * 1. C-Up First Person View:
 * 
 *      - If the camera is in first person mode AND is less than a specific distance away from the player's 
 *        head, then hide the belt strap hook
 * 
 * 2. Bow/Hookshot First Person View
 * 
 *      - If the player is in first person view with bow or hookshot And the camera is less than a specific
 *        distance away from the player's head, then hide the belt strap hook
 * 
 * 3. Transformed into non-human
 *      - If the player transforms into a non-human form (Deku, Goron, Zora, FD), then hide the belt strap hook
 * 
 * @param thisx Belt strap hook actor
 * @param play  Current playstate
 */
void BeltStrapHook_Update(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    BeltStrapHook* this = (BeltStrapHook*)thisx;

    // Get reset positions of overcoats' phys limbs when transforming from non-human to human
    if (player->transformation != PLAYER_FORM_HUMAN) {
        this->needsReset = 1;
    }

    // Bool that indicates if belt strap hook should be drawn or not in main draw function
    this->hideInFirstPerson = 0;

    // Explaining some states:
    // - PLAYER_STATE1_100000 indicates first person mode for bow/hookshot
    // - Camera->mode helps indicate first person mode for C-Up
    // - There is no player state for the transition into regular C-Up first person,
    //   so distance between camera and head is also used

    // Camera information
    Camera* camera = GET_ACTIVE_CAM(play);
    f32 cameraDist = Math_Vec3f_DistXYZ(&play->view.eye, &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);

    // C-Up first person
    if (player->transformation == PLAYER_FORM_HUMAN && camera->mode == FIRST_PERSON_VIEW_MODE &&
        cameraDist < MIN_CAMERA_DISTANCE) {
        this->hideInFirstPerson = 1;
    }

    // Bow/hookshot first person
    if (player->transformation == PLAYER_FORM_HUMAN && (player->stateFlags1 & PLAYER_STATE1_100000) &&
        cameraDist < MIN_CAMERA_DISTANCE) {
        this->hideInFirstPerson = 1;
    }

    // Remove belt strap hook when player transforms from human to non-human
    if (player->transformation != PLAYER_FORM_HUMAN && gPlayerBeltStrapHook != NULL &&
        player->actor.draw == NULL) {
        Actor_Kill(&this->actor);
        gPlayerBeltStrapHook = NULL;
        return;
    }
}


/*
=================
Belt Strap Hook Draw
=================
*/

/**
 * @brief Renders belt strap hooks and operates its physics every frame
 * 
 * This function renders the belt strap hooks model and operates its physics every frame. This is done by using 
 * physics functions based on Verlet Integration.
 * 
 * To accomplish this, three objects are set up:
 *      
 *      a) Belt Strap Hook Actor
 *          
 *          - A belt strap hook actor with the belt strap hook model and its limbs generated via Fast64
 *      
 *      b) gBeltStrapHookPhysLimbs
 *          
 *          - A physics simulator object representing the belt strap hook model's limbs
 *      
 *      c) gBeltStrapHookPhysBones
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
 *    integration is used to calculate the belt strap hooks' limbs' new position. This information is stored in the 
 *    physics limbs and bones.
 * 
 * 3. Using the physics bones' limbs' positions, rotate the actual belt strap hooks' bones so that each bone's limbs 
 *    match with the physics bones' limbs' positions.
 * 
 */
RECOMP_HOOK_RETURN ("Player_Draw") void main_Draw_BeltStrapHook_with_Physics(void) {
    // Only draw when mod's config indicates that cadet outfit model needs to be drawn instead of 'default'
    if (!recomp_get_config_u32("change_outfit") || gPlayerBeltStrapHook == NULL || 
    gPlayStateBeltStrapHook == NULL) {
        return;
    }
    
    // Call player and belt strap hook's current state
    Player* player = GET_PLAYER(gPlayStateBeltStrapHook);
    PlayState* play = gPlayStateBeltStrapHook;

    // Only draw when human and not transforming into non-human form
    if (recomp_get_config_u32("change_outfit") && gPlayerBeltStrapHook != NULL && 
    player->transformation == PLAYER_FORM_HUMAN && player->actor.draw != NULL && 
    !(player->stateFlags2 & PLAYER_STATE2_20000000)) {
        BeltStrapHook* this = gPlayerBeltStrapHook;

        // Don't draw belt strap hooks while in first person view with bow or hookshot
        if (this->hideInFirstPerson) {
            this->needsReset = 1;
            return;
        }

        // Update belt strap hook physics only when game is not paused or in transition state into pause menu
        if (play->pauseCtx.state == 0) {
            // Calculate current net force using change in velocity (current vs previous frame) and gravity
            Vec3f net_force = { (f32)0, (f32)0, (f32)0 };    // Gravity + movement
            Verlet_UpdatePhysPlayerVelocity(&gBeltStrapHookPhysPlayer, player); 
            Verlet_CalcNetForce(&gBeltStrapHookPhysPlayer, (f32)GRAVITY, &net_force);

            // Update belt strap hook limbs' positions based on Verlet Integration, and save new positions of limbs in 
            // phys limbs/bones
            BeltStrapHook_UpdateBodyPartsPos(this, player, net_force, beltStrapHook_PhysLimbs, beltStrapHook_PhysBones);

            // Calculate rotations based on belt strap hook limbs' calculated positions
            BeltStrapHook_RotateJoints(this, beltStrapHook_PhysBones, player);
        }

        // Draw belt strap hooks
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 
            &this->actor.shape.rot);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        func_80122868(play, player);    // Draw blinking effect of belt strap hooks when player gets hit or jinxed
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
 * @brief Function unused; use main_Draw_BeltStrapHook_with_Physics() instead.
 * 
 * All drawing functionality has been moved to main_Draw_BeltStrapHook_with_Physics() due to this function not 
 * firing during the Song of Time, Inverted Song of Time, Double Song of Time cutscenes.
 * 
 * In other words, the belt strap hooks model does not get drawn for the frame during these cutscenes, removing 
 * them from the game during the cutscene.
 */
void BeltStrapHook_Draw(Actor* thisx, PlayState* play) {

}