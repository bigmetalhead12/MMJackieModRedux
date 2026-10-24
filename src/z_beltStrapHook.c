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

#include "z_beltStrapHook.h"
#include "proxymm_custom_actor.h"
#include "math.h"
#include "customMath.h"


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
#define COLLISION_FACTOR    50

#define BELTSTRAPHOOK_CHAIN_COUNT 2
#define BELTSTRAPHOOK_NUM_BONES_WITH_NO_INWARD_ROT  0
#define BELTSTRAPHOOK_EXIT_SEARCH_STEPS 12
#define BELTSTRAPHOOK_SUBSTEPS  1        // One full frame with no substeps
#define BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT 0


/*
=================
Collider Macros
=================
*/
// Permanent Colliders
// These colliders will run for all animations of Jackie
#define BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS   0.3f
#define LEFTTHIGH_COLLIDER_DEFAULT_RADIUS 4.85f
#define RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS 4.85f
#define TORSO_COLLIDER_DEFAULT_RADIUS 5.5f

// Number of NULL pairs in capsule pairs 
#define BELTSTRAPHOOK_RESERVED_COLLIDER_SLOTS 0


/*
=================
Reset Angle Macros
=================
*/
#define BELTSTRAPHOOK_RESET_ANGLE_DEFAULT       -60.f
#define BELTSTRAPHOOK_RESET_ANGLE_EPONA_RIDING  30.0f
#define BELTSTRAPHOOK_RESET_ANGLE_KAMARO        30.0f
#define BELTSTRAPHOOK_RESET_ANGLE_TRANSFORM     -60.0f


/*
=================
Rotation Angle Limit Macros
=================
*/
// Set the rotational limit of the belt strap hook chains
#define BELTSTRAPHOOK_DEFAULT_ROT_LIMIT 89.f
#define BELTSTRAPHOOK_KAMARO_UPPER_ROT_LIMIT 89.f
#define BELTSTRAPHOOK_KAMARO_LOWER_ROT_LIMIT 89.f
#define BELTSTRAPHOOK_EPONA_UPPER_ROT_LIMIT 0.f
#define BELTSTRAPHOOK_EPONA_LOWER_ROT_LIMIT 0.f


/*
=================
Skip Physics Simulation at Specified Y-Rotation
=================
*/
// Jackie's turns larger than this per frame are partly carried rigidly instead of simulated.
#define BELTSTRAPHOOK_MAX_YAW	75.f


/*
=================
Rendering Macros
=================
*/
#define MIN_CAMERA_DISTANCE 10.0f
#define FIRST_PERSON_VIEW_MODE 6
#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED


/*
=================
Zero Macros
=================
*/
// These macros were set up for position-calculation operations to avoid using pure zero values
// Note: NEED TO MAKE THEM MORE DESCRIPTIVE
#define GREATER_THAN_ZERO   0.000001f
#define GREATER_THAN_ZERO2  0.00000001f
#define GREATER_THAN_ZERO3  0.0001f


/*
=================
JointTable Macros
=================
*/
// These macros are set up instead of an enum of belt strap hook limbs due to only two entries
// of jointtable being relevant, with only one of the two being actually used
#define BELTSTRAPHOOK_ROOT_POS 0    // BeltStrapHook's Root bone (position)
#define BELTSTRAPHOOK_ROOT_ROT 1    // BeltStrapHook's Root bone (rotation)


/*
=================
Externing Animations
=================
*/

// For zero-ing player velocity when Jackie is opening a door
extern void Player_Action_36(Player* player, PlayState* play);

// For keeping track of Giant's Mask transformation status
extern LinkAnimationHeader gPlayerAnim_cl_setmask;
extern LinkAnimationHeader gPlayerAnim_cl_setmaskend;
extern LinkAnimationHeader gPlayerAnim_cl_maskoff;

// For detecting when Jackie does the Kamaro dance
extern LinkAnimationHeader gPlayerAnim_alink_dance_loop;

// For detecting Jackie's specific animations while riding Epona
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_slowrun;
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_fastrun;

// For crouch shielding
extern LinkAnimationHeader gPlayerAnim_link_normal_defense_free;
extern LinkAnimationHeader gPlayerAnim_link_normal_defense;
extern LinkAnimationHeader gPlayerAnim_link_bow_defense;
extern LinkAnimationHeader gPlayerAnim_link_normal_defense_wait_free;
extern LinkAnimationHeader gPlayerAnim_link_normal_defense_wait;
extern LinkAnimationHeader gPlayerAnim_link_bow_defense_wait;

// For rolling animation
extern LinkAnimationHeader gPlayerAnim_link_normal_landing_roll_free;
extern LinkAnimationHeader gPlayerAnim_link_normal_landing_roll;
extern LinkAnimationHeader gPlayerAnim_link_fighter_landing_roll_long;

// For thrust/piercing animation
// 1 handed sword
extern LinkAnimationHeader gPlayerAnim_link_fighter_pierce_kiru;
extern LinkAnimationHeader gPlayerAnim_link_fighter_pierce_kiru_end;
extern LinkAnimationHeader gPlayerAnim_link_anchor_pierce_kiru_endR;
extern LinkAnimationHeader gPlayerAnim_link_fighter_pierce_kiru_finsh;
extern LinkAnimationHeader gPlayerAnim_link_fighter_pierce_kiru_finsh_end;
extern LinkAnimationHeader gPlayerAnim_link_anchor_pierce_kiru_finsh_endR;
// 2 handed sword
extern LinkAnimationHeader gPlayerAnim_link_fighter_Lpierce_kiru;
extern LinkAnimationHeader gPlayerAnim_link_fighter_Lpierce_kiru_end;
extern LinkAnimationHeader gPlayerAnim_link_anchor_Lpierce_kiru_endL;
extern LinkAnimationHeader gPlayerAnim_link_fighter_Lpierce_kiru_finsh;
extern LinkAnimationHeader gPlayerAnim_link_fighter_Lpierce_kiru_finsh_end;
extern LinkAnimationHeader gPlayerAnim_link_anchor_Lpierce_kiru_finsh_endR;

// Epona riding animations
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_walk_muti;
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_slowrun_muti;
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_fastrun_muti;
extern LinkAnimationHeader gPlayerAnim_link_uma_stop_muti;


/*
=================
Set up Vertical Cloth Chains
=================
*/
// Define each vertical cloth chain by first phys limb (pinned), chain length (number of phys
// limbs in chain, from first pinned phys limb to last unpinned limb), and first phys bone
// (from root limb to first pinned limb of that chain)
static const BeltStrapHookLimbChain belt_chains[BELTSTRAPHOOK_CHAIN_COUNT] = {
    {BELTSTRAPHOOK_BODYPART_LEFTLIMB1, 11, BELTSTRAPHOOK_BONE_ROOT_LEFT_LIMB1},
    {BELTSTRAPHOOK_BODYPART_RIGHTLIMB1, 11, BELTSTRAPHOOK_BONE_ROOT_RIGHT_LIMB1}
};


/*
=================
Colliders
=================
*/
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


/*
=================
Positions and Directions
=================
*/
// Torso values for setting root for belt strap hooks model and torso sphere collider
Vec3f torso_globalPos = { (f32)0, (f32)0, (f32)0 };         // World position of player's torso limb
Vec3s torso_rotate = {0, 0, 0};                             // Rotation value for player's torso (with parent 
															// limbs' rotations included)

// Belt strap hook's current directional axes 
static Vec3f beltStrapHook_forwardAxis = {0.f, 0.f, 0.f};
static Vec3f beltStrapHook_leftAxis = {0.f, 0.f, 0.f};
static Vec3f beltStrapHook_downAxis = {0.f, 0.f, 0.f};
static u8 beltStrapHook_dirAxesReady = 0;


/*
=================
Frame Update Helpers
=================
*/
// Previous frame state values used for substep calculations
// NOTE: This segment of code was only added for consistency sake with the belt strap hook, which actually does 
//       use substeps. For belt strap hook, these variables will have no impact on the functionality regarding
//       physics as a result.
static u8 beltStrapHook_stepHistoryReady = 0;       // This will always be 0 with no substeps enabled
static PhysSphereCollider beltStrapHook_prevFrameColliders[BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT];    // Same
static Vec3f beltStrapHook_previousFrameForward;
static Vec3f beltStrapHook_previousFrameDown;
static f32 beltStrapHook_previousSubstepScale;

static s16 beltStrapHook_previousFrameYaw = 0;




RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));


/***********************************************************************

	Belt Strap Hook Physics

***********************************************************************/

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
                                    -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0, 0, 0);
    }
}


/**
 * @brief Initialize one vertical cloth chain of phys limbs of the belt strap hook
 * 
 * This function sets the starting world position and physics state of each phys limb in one vertical
 * chain of phys limbs of the belt strap hook.
 *
 * Keep note that the first phys limb (limbA of every chain) is connected to the same root limb of the
 * elt strap hook. This first phys limb and the root limb are pinned. The rest of the phys limbs hanging 
 * down from the first phys limb are unpinned so that they can move via Verlet Integration.
 *
 * This function also initializes a phys bone using the current phys limb and its parent phys limb.
 *
 * @param this			BeltStrapHook actor
 * @param player		Player actor used for its rotation and velocity
 * @param gPhysLimbs	Array of elt strap hook's phys limbs
 * @param gPhysBones	Array of elt strap hook's phys bones
 * @param startLimb		Index of starting limb of current vertical chain of phys limbs of elt strap hook
 * @param limbCount		Number of phys limbs in the current vertical chain
 * @param startBone		Index of first phys bone of this current vertical chain
 */
void BeltStrapHook_InitChain(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[], int startLimb, int limbCount, int startBone) {
    // Set up zero velocity vector to apply to unpinned phys limbs
    Vec3f noVelocity = {0.f, 0.f, 0.f};

    // Iterate through each row of vertical phys limbs
    for (int physLimbRow = 0; physLimbRow < limbCount; physLimbRow++) {
		// Get current phyx limb index based on starting phys limb index and current phys limb row
        int limbIdx = startLimb + physLimbRow;

		// Calculate where phys limb should be in respect to world space
        Vec3s rotatedOffset = {0, 0, 0};
        Vec3f transformVec = {0.f, 0.f, 0.f};
		
		// First rotate the distance/offset between current phys limb and parent limb and rotate it by
		// actor's rotation in world.
		// Note: default_jointPos describes the distance of current phys limb from its parent phys limb
        CustomMath_Vec3s_Rotate(&gPhysLimbs[limbIdx]->default_jointPos, &player->actor.shape.rot,
            &rotatedOffset);
		// Scale the rotated distance/offset from parent limb to curreent limb based on belt strap hook's
		// current scale.
        CustomMath_Vec3s_Scale_ToVec3f(&rotatedOffset, this->actor.scale.x, &transformVec);

        // LimbA (first row of phys limbs) starts from the shared root. Every other PhysLimb starts from 
		// the previous PhysLimb.
		int parentIdx;
		
		if (physLimbRow == 0) {
			// The first phys limb in the chain is limbA, so its parent is a root limb shared by other
			// pinned phys limbs
			parentIdx = BELTSTRAPHOOK_BODYPART_ROOT;
		}
		else {
			// Every phys limb after LimbA uses the phys limb directly above it as its parent
			parentIdx = limbIdx - 1;
		}

		// Calculate current phys limb's world position
        Math_Vec3f_Sum(&this->bodyPartsPos[parentIdx], &transformVec, &this->bodyPartsPos[limbIdx]);
		
		// Initialize the phys limb with verlet integration
		f32 limbMass = LIMB_MASS * (physLimbRow + 1);		// current phys limb's mass

        // LimbA is the pinned phys limb at the top of this cloth chain of phys limbs.
        // Assign Jackie's current velocity and mark this phys limb as pinned.
		if (physLimbRow == 0) {
			Verlet_InitLimb(gPhysLimbs[limbIdx], this->bodyPartsPos[limbIdx], player->actor.velocity,
				limbMass, PINNED, BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS);
		}
        // Every other phys limbs below the first row of phys limbs are unpinned and have no velocity
		else {
			Verlet_InitLimb(gPhysLimbs[limbIdx], this->bodyPartsPos[limbIdx], noVelocity,
				limbMass, NOT_PINNED, BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS);
		}

        // Create phys bone out of current phys limb and previous/parent phys limb
        Verlet_InitBone(gPhysBones[startBone + physLimbRow], gPhysLimbs[parentIdx], gPhysLimbs[limbIdx]);
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
 * @param gPhysLimbs    Phys limbs to represent positions for respective belt strap hook standard limbs after 
 *                      verlet
 */
void BeltStrapHook_SetDefaultBodyPartsPos(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[]) {
    // Set belt strap hook velocity
    Vec3f playerVelocity = player->actor.velocity;

    /********************************
     Set up belt strap hook actor
    ********************************/
    // Assign Jackie's torso's world position to the belt strap hook's actor's world position
    Math_Vec3f_Copy(&this->actor.world.pos, &player->bodyPartsPos[PLAYER_BODYPART_TORSO]);
    // Assign Jackie's shape rotation to the belt strap hook's shape rotation
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);
    // Assign Jackie's world rotation to the belt strap hook's world's rotation
    Math_Vec3s_Copy(&this->actor.world.rot, &player->actor.world.rot);

    /********************************
     Set up root phys limb
    ********************************/
    // Root limb's BodyPartsPos and gPhysLimb positions and velocity
    // Velocity only gets assigned to root limb
    // BodyPartsPos keeps track of global XYZ position of each limb
    Math_Vec3f_Copy(&this->bodyPartsPos[BELTSTRAPHOOK_BODYPART_ROOT], 
        &player->bodyPartsPos[PLAYER_BODYPART_TORSO]);

    // Initailize the root limb of belt strap hook as pinned phys limb
    Verlet_InitLimb(gPhysLimbs[BELTSTRAPHOOK_BODYPART_ROOT], this->actor.world.pos, playerVelocity, 
        LIMB_MASS, PINNED, BELTSTRAPHOOK_LIMB_COLLIDER_RADIUS);

    /********************************
     Set up root skeleton values
    ********************************/
    // Store root world position.
    Vec3s rootPos_Vec3s = {0, 0, 0};
    Math_Vec3f_ToVec3s(&rootPos_Vec3s, &this->actor.world.pos);
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &rootPos_Vec3s);
    Math_Vec3s_Copy(&gPhysLimbs[BELTSTRAPHOOK_BODYPART_ROOT]->default_jointPos, &rootPos_Vec3s);

    // Start root rotation at zero.
    Vec3s newRootJointRot = { 0, 0, 0};
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &newRootJointRot);

    // Initialize every vertical belt chain and its corresponding phys bones.
    for (int i = 0; i < BELTSTRAPHOOK_CHAIN_COUNT; i++) {
        BeltStrapHook_InitChain(this, player, gPhysLimbs, gPhysBones, belt_chains[i].startLimb, 
            belt_chains[i].limbCount, belt_chains[i].startBone);
    }
}


/**
 * @brief Check whether Jackie is performing the Kamaro dance.
 * 
 * The function is primarily used to change a torso collider's size during the duration of the Kamaro
 * dance. This function is also used to change the reset and rotation limit of the belt strap hook 
 * chains during the duration of the Kamaro dance.
 * 
 * @param player    Player instance
 */
u8 BeltStrapHook_IsKamaroDancing(Player* player) {
    return (player->skelAnime.animation == &gPlayerAnim_alink_dance_loop);
}


/**
 * @brief Check whether Jackie is in first person view.
 * 
 * There are several different types of first person views.
 * 
 *  - PLAYER_STATE1_100000 indicates first person mode for bow/hookshot
 * 
 *  - Camera->mode helps indicate first person mode for C-Up
 * 
 *  - There is no player state for the transition into regular C-Up first person, so distance between 
 *    camera and head is also used
 * 
 * This function is primarily used to indicate if the belt strap hook chains need to be reset.
 * 
 * @param play      Current playstate
 * @param player    Player instance
 */
u8 BeltStrapHook_IsInFirstPerson(PlayState* play, Player* player) {
    // Only consider case when Jackie is in human form.
    if (player->transformation != PLAYER_FORM_HUMAN) {
        return 0;
    }

    // Get camera's distance from Jackie's head's position.
    Camera* camera = GET_ACTIVE_CAM(play);
    f32 cameraDist = Math_Vec3f_DistXYZ(&play->view.eye, &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);

    // If the camera is beyond the preset max distance between the camera and Jackie's head, then
    // consider the current state to not be in first person view.
    // This is done because Camera->mode becomes FIRST_PERSON_VIEW_MODE the moment C-Up is pressed,
    // even before the camera actually reaches Jackie's head during the camera transition phase.
    if (cameraDist >= MIN_CAMERA_DISTANCE) {
        return 0;
    }

    // At this point, the camera is very close to Jackie's head.
    // C-Up first person
    if (camera->mode == FIRST_PERSON_VIEW_MODE) {
        return 1;
    }

    // Bow or hookshot first person
    if (player->stateFlags1 & PLAYER_STATE1_100000) {
        return 1;
    }

    return 0;
}


/**
 * @brief Set the default rotational limit for how much upward belt strap hook's chains of phys limbs 
 * can go
 *
 * This function sets the default upward limit for upper and lower belt strap hook phys limbs in 
 * belt strap hook's chains of phys limbs.
 * 
 * @param this  BeltStrapHook actor
 */
void BeltStrapHook_SetDefaultRotLimitsAndReset(BeltStrapHook* this) {
    // 0 is directly outward (horizontal) from the torso, positive is upward, negative is downward
    s16 defaultUpwardLimAngle = CustomMath_DegreeToBinaryAngle(BELTSTRAPHOOK_DEFAULT_ROT_LIMIT);

    // Set rotation limits for the belt strap hook chains
    this->rotLimits = (BeltStrapHookRotationSettings){
        // Set default reset angle. This rotation value  will be applied to belt strap hook chains 
        // when the reset function is called.
        .resetAngle = BELTSTRAPHOOK_RESET_ANGLE_DEFAULT,

        // Store the default rotation angle limit for both upper limbs and lower limbs of belt strap 
        // hook chain. Whenever current limit needs to be reset to default rotation limits, these 
        // default values will be used.
        .defUpperChainLimAngle = defaultUpwardLimAngle,
        .defLowerChainLimAngle = defaultUpwardLimAngle,

        // Set the current rotation limit for both upper limbs and lower limbs of belt strap hook chain. 
        .currUpperChainLimAngle = defaultUpwardLimAngle,
        .currLowerChainLimAngle = defaultUpwardLimAngle,

        // Set the number of upper bones of the belt strap hook chains that will not have an inward 
        // rotation. Set to 0 if this feature is to not be used.
        .numTopBonesWithNoInwardRot = BELTSTRAPHOOK_NUM_BONES_WITH_NO_INWARD_ROT
    };
}


/**
 * @brief Set reset angle for belt strap hook chains with a given rotation angle
 * 
 * @param this          BeltStrapHook actor
 * @param reset_angle   Reset angle
 */
void BeltStrapHook_SetResetAngle(BeltStrapHook* this, f32 reset_angle) {
    this->rotLimits.resetAngle = reset_angle;
}


/**
 * @brief Set reset angle for belt strap hook chains with a given rotation angle
 * 
 * @param this              BeltStrapHook actor
 * @param upper_chain_lim   Rotation limit for upper chains of phys limbs in belt strap hook actor
 * @param lower_chain_lim   Rotation limit for lower chains of phys limbs in belt strap hook actor
 */
void BeltStrapHook_SetRotLimitAngles(BeltStrapHook* this, f32 upper_chain_lim, f32 lower_chain_lim) {
    // Update rotation limit angles for upper and lower limbs of BeltStrapHook chains.
		this->rotLimits.currUpperChainLimAngle = CustomMath_DegreeToBinaryAngle(upper_chain_lim);
        this->rotLimits.currLowerChainLimAngle = CustomMath_DegreeToBinaryAngle(lower_chain_lim);
}


/**
 * @brief Update the belt strap hook's chains of phys limbs' upward rotational limit and reset angle.
 *
 * This function checks the current animation and updates two components for the belt strap hook's chains 
 * of phys limbs: 
 *
 *	1. Upward rotational limit of belt strap hook's chains
 *
 *	2. Reset angle of belt strap hook's chains
 * 
 * @param this      BeltStrapHook actor
 * @param player    Player instance
 */
void BeltStrapHook_UpdateRotLimitsAndReset(BeltStrapHook* this, Player* player) {
	// Restore normal limits and reset angle before applying animation-based changes to limit and reset.
    BeltStrapHook_SetDefaultRotLimitsAndReset(this);
	
	// Check if player is human first.
	if (player->transformation != PLAYER_FORM_HUMAN) {
		return;
	}
	
	// Update limit and reset of Jackie's belt strap hook chains during Kamaro dance.
	if (BeltStrapHook_IsKamaroDancing(player)) {
		// Update reset angle for Kamaro dance.
        BeltStrapHook_SetResetAngle(this, BELTSTRAPHOOK_RESET_ANGLE_KAMARO);
		
		// Update rotation limit angles for upper and lower limbs of BeltStrapHook chains.
        BeltStrapHook_SetRotLimitAngles(this,
            BELTSTRAPHOOK_KAMARO_UPPER_ROT_LIMIT, BELTSTRAPHOOK_KAMARO_LOWER_ROT_LIMIT);
	}
	
	// Update limit and reset of Jackie's belt strap hook chains during Epona fast ride.
	else if (player->skelAnime.animation == &gPlayerAnim_link_uma_anim_fastrun) {
		// Update reset angle for Epona fast ride.
        BeltStrapHook_SetResetAngle(this, BELTSTRAPHOOK_RESET_ANGLE_EPONA_RIDING);

		// Update rotation limit angles for upper and lower limbs of BeltStrapHook chains.
        BeltStrapHook_SetRotLimitAngles(this,
            BELTSTRAPHOOK_EPONA_UPPER_ROT_LIMIT, BELTSTRAPHOOK_EPONA_LOWER_ROT_LIMIT);
	}
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
        BeltStrapHook_SetDefaultRotLimitsAndReset(this);
        BeltStrapHook_UpdateRotLimitsAndReset(this, player);
        this->wasTransformingIntoGiant = 0;
        this->wasGiant = 0;
        this->wasKamaroDancing = 0;

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
    // Ignore this if Jackie is not in human form and belt strap hook is not drawn.
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

/**
 * @brief Capture the directional axes of the torso and apply it to the belt strap hook
 * 
 * More specifically, this function extracts the directional axes of Jackie's torso and assign them to 
 * global variables that carry the directional axes of the belt strap hook.
 * 
 * @param mtx   Matrix that is currently carrying the positional data of the torso limb
 */
void BeltStrapHook_CaptureLimitAxes(MtxF* mtx) {
    // Based on torso axes in matrix: local +Y = forward, local +Z = left.
    // Convert the matrix values into Vec3f directional axes relative to Jackie's torso limb
    beltStrapHook_forwardAxis = (Vec3f){mtx->xy, mtx->yy, mtx->zy};
    beltStrapHook_leftAxis = (Vec3f){mtx->xz, mtx->yz, mtx->zz};
    beltStrapHook_downAxis = (Vec3f){-mtx->xx, -mtx->yx, -mtx->zx};

    // Calculate the lengths of the direcitonal axes vectors
    f32 forwardLength = sqrtf(Math3D_Vec3fMagnitudeSq(&beltStrapHook_forwardAxis));
    f32 leftLength = sqrtf(Math3D_Vec3fMagnitudeSq(&beltStrapHook_leftAxis));
    f32 downLength = sqrtf(Math3D_Vec3fMagnitudeSq(&beltStrapHook_downAxis));

    // Flag variable that indicates that the directional axes for the belt strap hook
    beltStrapHook_dirAxesReady = 0;

    // Check if the lengths of the directional axes vectors are greater than zero.
    if (forwardLength < GREATER_THAN_ZERO || leftLength < GREATER_THAN_ZERO || 
        downLength < GREATER_THAN_ZERO) {
        return;
    }

    // Normalize the directional axes of the belt strap hook.
    Math_Vec3f_Scale(&beltStrapHook_forwardAxis, 1.f / forwardLength);
    Math_Vec3f_Scale(&beltStrapHook_leftAxis, 1.f / leftLength);
    Math_Vec3f_Scale(&beltStrapHook_downAxis, 1.f / downLength);

    // Mark flag variable that indicates that the directional axes for the belt strap hook has been set 
    // up now.
    beltStrapHook_dirAxesReady = 1;
}



/**
 * @brief Limit the given phys bone's upward direction (and inward direction if indicated).
 * 
 * This function limits the upward direction and inward direction (if indicated in 'limitInward'
 * argument), of a given phys bone, indicated by its input 'parentPhysLimb' and 'childPhysLimb'. This
 * function is used so that the belt strap hook's phys bones do not exceed its horizontal limit to prevent
 * unwanted flapping behavior of the belt strap hook.
 * 
 * Regarding inward direction of the phys bone, whether the inward direction limit should be applied or
 * not is indicated with the 'limitInward' argument.  When this is '1', then the 'outwardDirVec' is used 
 * as reference to push the phys bone that is going inward towards Jackie's legs back outward.
 * 
 * This function is a manual fix/adjustment on the belt strap hook's physics that sometimes rotate or bend 
 * in extreme or unwanted directions during some animations that have severe movement of Jackie's limbs.
 * 
 * @param parentPhysLimb    Parent phys limb of the target phys bone
 * @param childPhysLimb     Child phys limb of the target phys bone
 * @param outwardDirVec     Outward direction axis of the target phys bone
 * @param limitInward       Flag value indicating if the phys bone has to be pushed outward
 * @param maxUpwardRotLim   Max upward rotation limit for phys bone in chain of phys limbs
 */
void BeltStrapHook_ApplyBoneRotationLimit(PhysLimb* parentPhysLimb, PhysLimb* childPhysLimb, 
    Vec3f* outwardDirVec, u8 limitInward, s16 maxUpwardRotLim) {
    // Commented out for now; May need this for future work where API allows for pinned phys limbs beyond 
    // the first phys limb of a current chain this function is being used for.
    /*
    if (childPhysLimb->pinned) {
        return;
    }*/

    // Calculate the direction vector from parent phys limb to child phys limb.
    Vec3f directionVec = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&childPhysLimb->curr_pos, &parentPhysLimb->curr_pos, &directionVec);

    // Check if the direction vector is valid by checking if it does not have a length of zero.
    f32 originalLengthSq = Math3D_Vec3fMagnitudeSq(&directionVec);
    if (originalLengthSq < GREATER_THAN_ZERO2) {
        return;
    }

    // This vector will carry the new direction of the phys limb after the limits have been applied.
    Vec3f adjustedDirVec = {0.f, 0.f, 0.f};
    Math_Vec3f_Copy(&adjustedDirVec, &directionVec);
    
    u8 physBoneAdjusted = 0;        // Flag that indicates if the direction of the phys bone was changed

    /********************************
     Remove Inward Rotation of Phys Bone
    ********************************/
    // This part of the code only runs when indicated via boolean argument limitInward.
    // To limit the inward motions of the current phys bone (indicated by child phys limb and parent phys
    // limb), remove the inward rotation from the current direction of the phys bone.
    // Note: This part of the function is designed specifically for the "upper phys bones" of the current
    //       chain of phys limbs. There may be use for it for lower phys bones eventually, however, so
    //       keep that in mind while moving forward.
    if (limitInward) {
        // Calculate just how much in the specified outward direction the bone is currently going in.
        f32 outwardRotAmount = CustomMath_Vec3f_Dot(&adjustedDirVec, outwardDirVec);

        // If the outward rotation amount is negative, that means it's going inward.
        // Remove the inward direction by subtracting the current direction of the phys bone by its 
        // outward direction scaled by the outward amount (which is negative, meaning that this is an 
        // inward direction).
        if (outwardRotAmount < 0.f) {
            Vec3f inwardRotAmount = { 0.f, 0.f, 0.f };
            Math_Vec3f_ScaleAndStore(outwardDirVec, outwardRotAmount, &inwardRotAmount);
            Math_Vec3f_Diff(&directionVec, &inwardRotAmount, &adjustedDirVec);
            physBoneAdjusted = 1;
        }
    }

    /********************************
     Remove Upward Rotation of Phys Bone
    ********************************/
    // This part of the code runs all the time.

    // This part of the code essentially prevents the current phys bone from rotating beyond the set
    // upward rotation limit of the belt strap hook's chains of phys limbs. This upward rotation limit is 
    // provided as an argument in the function 'maxUpwardRotLim'

    // Calculate the downward component of the phys bone's direction relative to belt strap hook.
    f32 boneDownwardComponent = CustomMath_Vec3f_Dot(&adjustedDirVec, &beltStrapHook_downAxis);

    // If the downward amount of the direction is negative, then that means it's going upward. Remove 
    // the upward direction by subtracting the current direction of the phys bone by its downward 
    // direction scaled by its downward amount (which is negative, indicating that this is an upward 
    // direction).
    if (boneDownwardComponent < 0.f) {
        // Bone's vertical amount relative to belt strap hook expressed in world space
        Vec3f boneVerticalDir = {0.f, 0.f, 0.f};  

        // Bone's horizontal amount relative to belt strap hook expressed in world space
        Vec3f boneHorizontalDir = {0.f, 0.f, 0.f};
        
        // Calculate the component along the torso's down axis.
        Math_Vec3f_ScaleAndStore(&beltStrapHook_downAxis, boneDownwardComponent, &boneVerticalDir);

        // Remove the vertical component, leaving the torso-relative horizontal direction.
        Math_Vec3f_Diff(&adjustedDirVec, &boneVerticalDir, &boneHorizontalDir);

        //  Calculate the horizontal component of the phys bone's direction relative to belt strap hook
        f32 boneHorizontalComponent = sqrtf(Math3D_Vec3fMagnitudeSq(&boneHorizontalDir));

        f32 allowedSin = Math_SinS(maxUpwardRotLim);
        f32 allowedCos = Math_CosS(maxUpwardRotLim);

        // tan(angle) gives the allowed upward amount.
        // All supplied angles here are below 90 degrees.
        f32 minDownLength = (-boneHorizontalComponent) * (allowedSin / allowedCos);

        // Check if the bone is pointing upward beyond the allowed upward angle.
        if (boneDownwardComponent < minDownLength) {
            // If the horizontal component of the phys bone's orientation relative to the belt strap hook 
            // is greater than 0, then preserve the horizontal component of the phys bone while replacing 
            // the vertical component of the phys bone's orientation relative to the belt strap hook with 
            // the allowed value.
            if (boneHorizontalComponent > GREATER_THAN_ZERO) {
                adjustedDirVec.x = boneHorizontalDir.x + beltStrapHook_downAxis.x * minDownLength;
                adjustedDirVec.y = boneHorizontalDir.y + beltStrapHook_downAxis.y * minDownLength;
                adjustedDirVec.z = boneHorizontalDir.z + beltStrapHook_downAxis.z * minDownLength;
            }
            // Else, if the horizontal component of the phys bone's orientation relative to the belt strap 
            // hook is NOT greater than zero, but max upward rotation limb for the chain of phys limbs is 
            // greater than 0 degrees, then take the phys bone's chain's outward horizontal direction 
            // vector, tilt it by the max upward rotation limit, and match the phys bone's orientation to
            // match the chain's tilted orientation.
            else if (maxUpwardRotLim > 0) {
                // (outwardDirVec * allowedCos) supplies the horizontal portion.
                // (-beltStrapHook_downAxis * allowedSin) supplies the upward portion.
                adjustedDirVec.x = (outwardDirVec->x * allowedCos) - (beltStrapHook_downAxis.x * allowedSin);
                adjustedDirVec.y = (outwardDirVec->y * allowedCos) - (beltStrapHook_downAxis.y * allowedSin);
                adjustedDirVec.z = (outwardDirVec->z * allowedCos) - (beltStrapHook_downAxis.z * allowedSin);
            }
            // Else, move on with the horizontal direction of the bone.
            else {
                Math_Vec3f_Copy(&adjustedDirVec, &boneHorizontalDir);
            }

            // Indicate in flag that phys bone was adjusted.
            physBoneAdjusted = 1;
        }
    }

    // Leave the function if the phys bone's direction was not adjusted.
    if (!physBoneAdjusted) {
        return;
    }

    // If the phys bone direction was adjusted, then verify if the direction of the phys bone is valid 
    // (as in, not the length of 0). 
    f32 allowedLengthSq = Math3D_Vec3fMagnitudeSq(&adjustedDirVec);

    // If the recalculated phys bone direction has the length of zero, then set the phys bone's direction 
    // as completely downward based on its down direction axis.
    if (allowedLengthSq < GREATER_THAN_ZERO) {
        adjustedDirVec = beltStrapHook_downAxis;
        allowedLengthSq = Math3D_Vec3fMagnitudeSq(&adjustedDirVec);
    }

    // Rescale the phys bone's length to its original length while preserving its direction.
    Math_Vec3f_Scale(&adjustedDirVec, sqrtf(originalLengthSq / allowedLengthSq));

    // Change the phys bone's child phys limb's position based on the new direction vector from the phys 
    // bone's parent phys limb.
    Math_Vec3f_Sum(&parentPhysLimb->curr_pos, &adjustedDirVec, &childPhysLimb->curr_pos);
}


/**
 * @brief Apply bend limits to every vertical belt strap hook cloth chain of phys limbs.
 *
 * This function goes through each vertical cloth chain and applies 
 * BeltStrapHook_ApplyBoneRotationLimit() to every bone segment in that chain.
 *
 * For each chain, it first determines which world-space direction counts as "outward" based on where 
 * that chain is located around the belt strap hook. After taht, the function walks down the chain one bone 
 * at a time, where the bones are prevented from bending upward relative to the torso. For the first 3 
 * (or other value defined by macro) bones, the phys limbs are also prevented from bending inward.
 *
 * @param gPhysLimbs   Phys limbs of the belt strap hook
 * @param chainStarts  Array containing the starting phys limb index for each chiain of phys limbs
 * @param chainCount   Number of chains in belt strap hook
 */
void BeltStrapHook_ApplyChainRotationLimits(PhysLimb* gPhysLimbs[], const BeltStrapHookLimbChain chainStarts[],
    int chainCount, const BeltStrapHookRotationSettings* limits) {
    // If directional axes of the belt strap hook is not established in the world space yet, then do not run 
    // this function.
    if (!beltStrapHook_dirAxesReady) {
        return;
    }

    // For each vertical chain of physlimbs, keep its phys bones from bending too far upward or inward, 
    // depending on where this chain is located in within the belt strap hook and the limits set to the
    // chains beforehand.
    for (int currChain = 0; currChain < chainCount; currChain++) {
        // Get the current chain's starting limb's index
        int startLimbIdx = chainStarts[currChain].startLimb; // Index of first phys limb in current chain

        // By  default, set the current chain's outward direction as forward of belt strap hook.
        // If the current chain is one of the front chains of the belt strap hook, then the outward 
        // direction will stay as forward.
        Vec3f outwardDir = {0.f, 0.f, 0.f};
        Math_Vec3f_ScaleAndStore(&beltStrapHook_forwardAxis, -1.f, &outwardDir);

        // Belt chains of phys limbs
        int boneCount = belt_chains[currChain].limbCount - 1;

        // Limit the current chain's direction so that it is not going upward or inward too much.
        // The directional axes of the belt strap hook are used to help with this operation.
        for (int currPhysLimb = 0; currPhysLimb < boneCount; currPhysLimb++) {
            // Limit the top bones' inward rotation, which is set beforehand in the belt strap hook's
            // rotation setting's numTopBonesWithNoInwardRot.
            // Check if this is currently one of the top bones of the current chain.
            u8 limitInward = (currPhysLimb < limits->numTopBonesWithNoInwardRot);
            s16 maxUpAngle;
            
            // If this is one of the top bones in the current chain, apply the upper chain rotation 
            // limit.
            if (limitInward) {
                maxUpAngle = limits->currUpperChainLimAngle;
            }

            // Else, if this is NOT one of the top bones, then apply the lower chain rotation limit.
            else {
                maxUpAngle = limits->currLowerChainLimAngle;
            }

            // Apply the rotation limit
            BeltStrapHook_ApplyBoneRotationLimit(gPhysLimbs[startLimbIdx + currPhysLimb], 
                gPhysLimbs[startLimbIdx + currPhysLimb + 1], &outwardDir, limitInward, maxUpAngle);
        }
    }
}


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

        // Update torso-relative directions using the current torso matrix.
        BeltStrapHook_CaptureLimitAxes(mtx);

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

    /********************************
     Right Thigh Limb Positions
    ********************************/
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
 * @brief Normalize Vec3f direction used by belt strap hook during substep physics calculations
 * 
 * This function calculates the length of the target direction vector and scales it so that its final 
 * length becomes 1.
 * 
 * @param targetVec     Direction vector to normalize
 * 
 * @return true if the vector was successfully normalized
 * @return false if the vector was too small to safely normalize
 */
bool BeltStrapHook_NormalizeSubstepDirectionVec(Vec3f* targetVec) {
    // Find length of target direction vector with dot product of itself (basically squaring the length 
    // of the vector).
    f32 lengthSq = Math3D_Vec3fMagnitudeSq(targetVec);

    // Treat extremely small values that's very close to 0 as just 0.
    // Return "false" if the length of the target vector is basically 0.
    if (lengthSq < GREATER_THAN_ZERO2) {
        return false;
    }

    // Otherwise, scale the vector by 1/length of vector so that its final length becomes 1.
    Math_Vec3f_Scale(targetVec, 1.f / sqrtf(lengthSq));
    return true;
}


/**
 * @brief Find the closest point along a given capsule collider to a target world position
 * 
 * The capsule is made of two sphere colliders. This function projects the target position onto the
 * line segment between the two sphere colliders' centers and clamps the result so the closest point
 * remains between the two sphere colliders.
 * 
 * The function also interpolates the capsule's radius at that closest point. This allows the capsule
 * with differently sized endpoint sphere colliders to be handled. 
 * 
 * @param targetPos         Target world position 
 * @param capsuleColStart   Sphere collider that's the starting endpoint of the capsule collider
 * @param capsuleColEnd     Sphere collider that's the ending endpoint of the capsule collider
 * @param colRadius         Output radius of the sphere collider at the closest point in capsule
 * 
 * @return  World position of the closest point to targetPos along the capsule collider's center line
 */
Vec3f BeltStrapHook_FindClosestPointOnCapsule(Vec3f targetPos, PhysSphereCollider* capsuleColStart, 
    PhysSphereCollider* capsuleColEnd, f32* colRadius) {
    // Create vector pointing from the starting sphere collider's center to the end sphere collider's
    // center.
    Vec3f capsuleAxis = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&capsuleColEnd->center, &capsuleColStart->center, &capsuleAxis);

    // Calculate the squared length of capsule's center line
    f32 lengthSq = Math3D_Vec3fMagnitudeSq(&capsuleAxis);
    
    // Set up variable (t) that indicates where the point closest to targetPos is between the two
    // capsule endpoint sphere colliders
    f32 t = 0.f;

    // Only project onto the capsule's center line if the two sphere centers are actually apart 
    // instead of just being a sphere collider
    if (lengthSq >= GREATER_THAN_ZERO3) {
        Vec3f startToTarget = {0.f, 0.f, 0.f};
        Math_Vec3f_Diff(&targetPos, &capsuleColStart->center, &startToTarget);
        t = CustomMath_Vec3f_Dot(&startToTarget, &capsuleAxis) / lengthSq;

        // Keep closest point within the positions of the start and end colliders of capsule
        if (t < 0.f) {
            t = 0.f;
        }

        if (t > 1.f) {
            t = 1.f;
        }
    }

    // Interpolate the capsule radius at the closest point.
    *colRadius = capsuleColStart->radius + (capsuleColEnd->radius - capsuleColStart->radius) * t;

    // Calculate the world position of the closest point along the capsule's center line.
    Vec3f scaledCapsuleAxis = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&capsuleAxis, t, &scaledCapsuleAxis);
    Vec3f closestPoint = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(&capsuleColStart->center, &scaledCapsuleAxis, &closestPoint);

    return closestPoint;
}


/**
 * @brief Calculate the deepest depth between a target position and any of the colliders.
 *
 * This function checks a target position against the torso sphere collider and every capsule
 * collider used by the belt strap hook.
 *
 * For each collider, the function finds a point along the collider that is closest to the target 
 * position and calculates the collider radius at that point. It then calculates how deep the target
 * position is inside the collider. This includes the target's own collision radius. 
 *
 * @param targetPos          Target world position being checked
 * @param pointRadius        Collision radius of target point/position
 * @param capsulePairs       Array of sphere collider pairs used as capsule colliders
 * @param capsuleColCount    Number of capsule colliders
 * @param contactColCenter   Output center position of the deepest collider contact.
 * @param contactColRadius   Output combined collider and target radius of the deepest contact.
 *
 * @return Deepest overlap depth found among all body colliders.
 */
f32 BeltStrapHook_GroupContact(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* capsulePairs[][2], 
    int capsuleColCount, Vec3f* contactColCenter, f32* contactColRadius) {
    // Set up variable indicating deepest depth value of targetPos in any collider in belt strap hook
    f32 deepest = 0.f;

	// Note: This is crudely coded because I did not handle list of colliders cleanly. The array
	//		 for colliders is only consisted of capsule colliders.
    for (int capsuleIdx = 0; capsuleIdx < capsuleColCount; capsuleIdx++) {
        // Use the two sphere colliders that make up this capsule collider
        PhysSphereCollider* capsuleColStart = capsulePairs[capsuleIdx][0];
        PhysSphereCollider* capsuleColEnd = capsulePairs[capsuleIdx][1];

        // Find what part of the current capsule collider is closest to target position
        // Set up variable for radius of sphere collider that would be located in closest point in 
        // capsule. This is important in case capsule has differently sized endpoint sphere colliders,
        // in which case the radius would be interpolated based on its position in capsule.
        f32 colRadius;
        // Find center position of sphere collider at a point in capsule collider that would be located
        // closest to the target position.
        Vec3f colCenter = BeltStrapHook_FindClosestPointOnCapsule(targetPos, capsuleColStart, 
            capsuleColEnd, &colRadius);
        // Find the depth, or just how deep the target position is located inside current collider
        f32 currDepth = colRadius + pointRadius - Math_Vec3f_DistXYZ(&targetPos, &colCenter);
        // If the calculated depth is deeper than the currently recorded deepest depth so far, then
        // replace that variable's value with this newly calculated current depth. Also keep note of the
        // collider's center position and radius.
        if (currDepth > deepest) {
            deepest = currDepth;
            *contactColCenter = colCenter;
            *contactColRadius = colRadius + pointRadius;
        }
    }

    // Return the deepest depth recorded in any of collider.
    return deepest;
}


/**
 * @brief Check whether target position is outside all of colliders
 *
 * This function checks if the target position is outside the torso sphere collider and other
 * capsule colliders used by the belt strap hook.
 *
 * @param targetPos			Target world position being checked
 * @param pointRadius		Collision radius of target point/position
 * @param capsulePairs		Array of sphere collider pairs used as capsule colliders
 * @param capsuleColCount	Number of capsule colliders
 * 
 * @return true if target position is outside of all colliders, otherwise false
 */
bool BeltStrapHook_GroupOutside(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* capsulePairs[][2], 
    int capsuleColCount) {
    Vec3f center = {0.f, 0.f, 0.f};		// Made only as function argument.
    f32 radius = 0.f;					// Made only as function argument.
	
	// Check if target position is inside colliders (torso and capsule) by calculating the
	// deepest depth the position is in any collider.
	f32 colliderDepth = BeltStrapHook_GroupContact(targetPos, pointRadius, capsulePairs, 
		capsuleColCount, &center, &radius);
		
	// If target position is inside any collider, then return false
	bool posIsOutside = colliderDepth <= 0.f;
	
    return posIsOutside;
}


/**
 * @brief Find safe exit position for phys limb from belt strap hook's colliders.
 * 
 * This function uses a given direction from a target phys limb's position to find a new position
 * that is outside the torso collider and all capsule colliders of the belt strap hook.
 *
 * The function first finds a distance that is guaranteed to extend past every collider in the
 * specified direction. It then checks if this position is actually outside all colliders.
 *
 * If the test exit position at mid point is outside all colliders, then that is the new farthest 
 * distance between position outside the colliders and the closest collider's boundary.
 *
 * This function was made for the belt strap hook because the belt strap hook uses many colliders that 
 * are overlapping one another around Jackie's lower body. Simply pushing out the phys limb from one 
 * collider can put the phys limb in another collider, creating unwanted clipping behavior of the
 * belt strap hook. 
 *
 * @param targetPhysLimbPos		Target phys limb's world position
 * @param physLimbDir			Direction along which a safe exit position for phys limb is searched
 * @param pointRadius			Collisionr radius of target phys limb
 * @param capsulePairs			Array of pairs of sphere colliders used for capsule colliders
 * @param capsuleColCount		Count of capsule colliders 
 * @param padding				Small extra distance added so exit doesn't sit on collider boundary
 * @param exitPosition			Output position containing safe exit position for phys limb
 */
bool BeltStrapHook_GroupRayExit(Vec3f targetPhysLimbPos, Vec3f physLimbDir, f32 pointRadius,
    PhysSphereCollider* capsulePairs[][2], int capsuleColCount, f32 padding, Vec3f* exitPosition) {
	/********************************
     Find a guaranteed outside distance
    ********************************/
    // Find a distance from phys limb, along the indicated search direction (physLimbDir), that is 
	// far enough that it should be past every collider
    f32 farDistance = 0.f;
	
	// Go through each collider (including both torso collider and other colliders) and see how far
	// the target phys limb should go to be outside all colliders (indicated by farDistance)
	// Note: This is crudely coded because I did not handle list of colliders cleanly. The array
	//		 for colliders is only consisted of capsule colliders.
    for (int capsuleIdx = 0; capsuleIdx < capsuleColCount; capsuleIdx++) {
        // Use the two sphere colliders that make up this capsule collider
        PhysSphereCollider* capsuleColStart = capsulePairs[capsuleIdx][0];	// Capsule collider endpoint 1
		PhysSphereCollider* capsuleColEnd = capsulePairs[capsuleIdx][1];    // Capsule collider endpoint 2
        
		// Get the larger radius of the two endpoints of the capsule collider 
		f32 colRadius;
		if (capsuleColStart->radius > capsuleColEnd->radius) {
			colRadius = capsuleColStart->radius;
		}
		else {
			colRadius = capsuleColEnd->radius;
		}
		
		// Check the two sphere collider endpoints of the current capsule collider
		// The focus is going to be on the current capsule collider's sphere collider 
		// with the larger radius for finding a distance that is more certain to be big
		// enough to push the phys limb out of the colliders.
        for (int capsuleEndIdx = 0; capsuleEndIdx < 2; capsuleEndIdx++) {
			Vec3f colliderCenter;
			if (capsuleEndIdx == 0) {
				colliderCenter = capsuleColStart->center;
			}
			else {
				colliderCenter = capsuleColEnd->center;
			}
			
			// Calculate a safe distance along the input direction that would move phys limb 
			// beyond capsule's sphere collider endpoint
            Vec3f physLimbToCollider = {0.f, 0.f, 0.f};
            Math_Vec3f_Diff(&colliderCenter, &targetPhysLimbPos, &physLimbToCollider);
            f32 projectedDistance = CustomMath_Vec3f_Dot(&physLimbToCollider, &physLimbDir);
            f32 currDistance = projectedDistance + colRadius + pointRadius;
				
			// If the calculated distance is greater than the current farthest distance from 
			// collider boundary to phys limb, then keep track of that new farthest distance
            if (currDistance > farDistance) {
				farDistance = currDistance;
			}
        }
    }
	
	// Ensure that the farthest point is not precisely on the collider boundary by adding padding
    farDistance += padding;
    f32 nearDistance = 0.f;	// Distance from the target PhysLimb's current position
	
	// Use the calculated farthest distance from any collider (farDistance) to find the point along
	// the input phys limb direction where it would be outside all colliders
	// Update the exitPosition argument with this new position value
    Vec3f travelVec = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&physLimbDir, farDistance, &travelVec);
    Math_Vec3f_Sum(&targetPhysLimbPos, &travelVec, exitPosition);

	// If the newly calculated position is still inside any collider, return false and get out of
	// function.
    if (!BeltStrapHook_GroupOutside(*exitPosition, pointRadius, capsulePairs, capsuleColCount)) { 
		return false;
	}

	// Up to this point, a safe far distance was found that places the PhysLimb outside all colliders. 
	// However, this distance may be farther than necessary.
	//
	// Use binary search to find a closer safe exit position along input direction (physLimbDir).
	// nearDistance represents the inside side of the search, while farDistance represents
	// the known safe outside side. Each iteration cuts the gap between them in half.
	//
	// More iterations make exitPosition closer to the point where the PhysLimb first  becomes outside
	// all colliders along this direction.
    for (int iter = 0; iter < BELTSTRAPHOOK_EXIT_SEARCH_STEPS; iter++) {
		// Find midpoint between nearDistance and farDistance
        f32 midpointDist = (nearDistance + farDistance) * 0.5f;
		
		// Check where the phys limb would end up by moving it along the input direction to the
		// midpoint distance
        Vec3f midpointTravelVec = {0.f, 0.f, 0.f};
        Math_Vec3f_ScaleAndStore(&physLimbDir, midpointDist, &midpointTravelVec);
        Vec3f testExitPos = {0.f, 0.f, 0.f};
        Math_Vec3f_Sum(&targetPhysLimbPos, &midpointTravelVec, &testExitPos);
		// If the test exit position at mid point is outside all colliders, then that is the new 
		// farthest distance between position outside the colliders and the closest collider's
		// boundary.
        bool testExitIsOutside = BeltStrapHook_GroupOutside(testExitPos, pointRadius, capsulePairs, 
			capsuleColCount);
        if (testExitIsOutside) {
            farDistance = midpointDist;
            *exitPosition = testExitPos;
        } 
		// If the text exit position at mid point is inside a collider still, then it closest
		// position inside the collider is assigned that midpoint distance
		else {
            nearDistance = midpointDist;
        }
		
		// Continue to run this to get closer to boundary of collider
    }
    return true;
}


/**
 * @brief Push the given belt strap hook phys limb out of Jackie's set colliders for belt strap hook.
 * 
 * This function checks whether a given phys limb is inside the currently set colliders for the
 * belt strap hook. All the colliders are capsule colliders.
 *
 * If the phys limb is inside a collider, the function calculates an outward direction from the collider 
 * and tries to move the phys limbs in that direction out of the collider.
 * 
 * If the initial calculated new world position for the phys limb being pushed out of the collider still
 * overlaps the collider group, search from the phys limb's original world current position in the 
 * outward and opposite directions. Use the closer successful exit, preferring outward direction when
 * distances are equal.
 * 
 * If neither exits succeed, then leave the position unchanged.
 *
 * @param physLimb			Phys limb that is being checked and corrected
 * @param collider_pairs	Array of pairs of sphere colliders used to form capsule colliders
 * @param capsuleColCount	Number of capsule collider pairs
 */
void BeltStrapHook_SolveColliderGroup(PhysLimb* physLimb, PhysSphereCollider* collider_pairs[][2], 
    int capsuleColCount) {
	// Find the collider where the phys limb is deepest in within the group of colliders.
    // First find the closest point along this capsule collider's centerline to the phys limb.
    Vec3f center = {0.f, 0.f, 0.f};     // Center of sphere collider in capsule collider
    f32 radius = 0.f;                   // Radius of sphere collider in capsule collider
	
	// If the phys limb is not inside any collider, no correction is needed.
    if (BeltStrapHook_GroupContact(physLimb->curr_pos, physLimb->collision_radius,
		collider_pairs, capsuleColCount, &center, &radius) <= 0.f) {
		return;
	}

	// Find direction pointing from the collider center toward the phys limb's current position
    Vec3f direction = {(physLimb->curr_pos.x - center.x), (physLimb->curr_pos.y - center.y), 
		(physLimb->curr_pos.z - center.z)};
		
	// Try to normalize the direction from the target collider center toward the phys limb.
    // If it is too short to normalize, try the previous position of phys limb instead.
    if (!BeltStrapHook_NormalizeSubstepDirectionVec(&direction)) {
        // Use phys limb's previous position instead to create another possible outward direction
        direction = (Vec3f){(physLimb->prev_pos.x - center.x), (physLimb->prev_pos.y - center.y), 
			(physLimb->prev_pos.z - center.z)};
		// Try to normalize direction from collider center to phys limb's previous position If
		// this fails, then even the previous position of the phys limb is too close to the
		// collider's center.
        if (!BeltStrapHook_NormalizeSubstepDirectionVec(&direction)) {
			// Use Jackie's forward direction as an alternative
            direction = beltStrapHook_forwardAxis;
			
			// Try to normalize Jackie's forward direction. If even this fails, then just use
			// straight upward direction as final fallback direction.
            if (!BeltStrapHook_NormalizeSubstepDirectionVec(&direction)) { 
				direction = (Vec3f){0.f, 1.f, 0.f};
			}
        }
    }
    
	// Create very small padding gap between collider and phys limb so that phys limb won't end
	// up exactly on the collider's boundary. This is because if the phys limb ends up exactly
	// on the collider's boundary, then the phys limb would still be considered colliding against
	// the collider.
	f32 padding = radius * GREATER_THAN_ZERO3;
	// Fail safe for when padding somehow ends up being 0 or less
    if (padding < GREATER_THAN_ZERO3) {
		padding = GREATER_THAN_ZERO3;
	}
	// Calculate a possible new position just outside the collider, which includes the padding gap 
	// so the phys limb does not sit directly on the boundary.
    Vec3f candidate = {(center.x + direction.x * (radius + padding)), 
		(center.y + direction.y * (radius + padding)), (center.z + direction.z * (radius + padding))};

    // Check whether phys limb would be outside all colliders if it were moved to this candidate
	// position. If the position is safe, then move the phys limb there and finish
    if (BeltStrapHook_GroupOutside(candidate, physLimb->collision_radius, collider_pairs, 
		capsuleColCount)) {
        physLimb->curr_pos = candidate;
        return;
    }

	// Store possible safe exit positions in the normal outward direction (collider center to phys limb)
	// and in the opposite direction.
    Vec3f outwardExit = {0.f, 0.f, 0.f};	// exit position that goes from collider center to phys limb
    Vec3f oppositeExit = {0.f, 0.f, 0.f};	// opposite exit position that points to collider center and
                                            // beyond to the other side of the sphere collider.
	
	// Create direction that points exactly opposite from the normal outward direction.
    Vec3f opposite = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&direction, -1.f, &opposite);
	
	// Search outward from phys limb for a position that is outside all colliders
	// outwardOK is a flag that indicates whether valid exit was found in normal direction.
	// outwardExit stores the position of that exit.
    bool outwardOK = BeltStrapHook_GroupRayExit(physLimb->curr_pos, direction, physLimb->collision_radius, 
		collider_pairs, capsuleColCount, padding, &outwardExit);
	// Search in opposite direction for another valid exit that is outside all colliders
	// oppositeOK is a flag that indicates whether valid exit was found in opposite direction
	// oppositeExit stores the position of that exit.
    bool oppositeOK = BeltStrapHook_GroupRayExit(physLimb->curr_pos, opposite, physLimb->collision_radius, 
		collider_pairs, capsuleColCount, padding, &oppositeExit);

    // If exit was found in normal outward direction, use it when either:
	//  1. no safe exit was found in the opposite direction OR
	//  2. outward exit is closer to the phys limb than the opposite exit
	// Otherwise, use the safe opposite exit
    bool outwardIsCloser = (Math_Vec3f_DistXYZ(&physLimb->curr_pos, &outwardExit) <=
        Math_Vec3f_DistXYZ(&physLimb->curr_pos, &oppositeExit));
    bool useOutwardExit = outwardOK && (!oppositeOK || outwardIsCloser);

    if (useOutwardExit) {
        physLimb->curr_pos = outwardExit;
    } else if (oppositeOK) {
        physLimb->curr_pos = oppositeExit;
    }
}


/**
 * @brief Reset the belt strap hook chains of phys limbs based on its assigned reset angle
 * 
 * This function resets the belt strap hook's chains of phys limbs at a previously assigned reset angle.
 * 
 * The following logic is followed to accomplish this:
 *
 *	1. Using the previously assigned reset angle, find the horizontal outward and downward direction
 *	   that the chains need to be going in when reset.
 *
 * 	2. Go through each chain of phys limbs. For each chain, determine the outward direction of the
 *	   chain based on the directional axes of the belt strap hook (based on Jackie's torso limb).
 *
 *	3. For each chain, apply the outward direction to the reset direction.
 *
 *	4. For each chain, use the chain's calculated outward reset direction to find the new world 
 *	   positions of the chain's phys limbs.
 *
 *	5. Remove all the phys limbs' velocity and previous position info.
 * 
 * @param this          Belt Strap Hook actor
 * @param gPhyslimbs    Phys limbs of belt strap hook
 * @param chains        Belt strap hook chains of phys limbs
 * @param chainCount	Number of all chains of belt strap hook
 */
void BeltStrapHook_SetResetShape(BeltStrapHook* this, PhysLimb* gPhysLimbs[], 
    const BeltStrapHookLimbChain chains[], int chainCount) {
    // Torso-relative directions must be set up for belt strap hook before rebuilding.
    if (!beltStrapHook_dirAxesReady) {
        return;
    }
	
	// Get belt strap hook's current actor scale.
    f32 actorScale = fabsf(this->actor.scale.x);

	/********************************
     Calculate downward and outward directions of current reset angle
    ********************************/
    // Use the current settings unless a specific reset angle was requested.
    f32 resetAngle = this->rotLimits.resetAngle;
    s16 resetBinaryAngle = CustomMath_DegreeToBinaryAngle(resetAngle);      // Convert to s16 angle

    // Calculate how much horizontal direction and upward direction there is in current reset angle.
    f32 resetHorizontalDirScale = Math_CosS(resetBinaryAngle);
    f32 resetUpwardDirScale  = Math_SinS(resetBinaryAngle);

    // Belt strap hook's downward direction, taken from Jackie's torso orientation.
    Vec3f beltStrapHookDownDir = beltStrapHook_downAxis;
    f32 beltStrapHookDownDirLengthSq = Math3D_Vec3fMagnitudeSq(&beltStrapHookDownDir);

    // If belt strap hook's down direction is basically just 0 (extremely slightly greater than 0), then
    // it has an invalid downward direction. Get out of function.
    if (beltStrapHookDownDirLengthSq <= GREATER_THAN_ZERO) {
        return;
    }

    // Normalize the belt strap hook's downward direciton.
    Math_Vec3f_Scale(&beltStrapHookDownDir, (1.f / sqrtf(beltStrapHookDownDirLengthSq)));

	/********************************
     Reset each vertical cloth chain
    ********************************/
	// Go through each belt strap hook chain of phys limbs and reset each of them by calculating where 
	// each unpinned phys limb in the chain should be placed in 3D space relative to its parent.
    for (int currChain = 0; currChain < chainCount; currChain++) {
        int startLimb = chains[currChain].startLimb;	// Chain's phys limb A (pinned limb)
        int limbCount = chains[currChain].limbCount;	// Count of phys limbs in current chain

		
		// ======== Determine "outward direction" of current chain ========
		
		// Determine what direction the "outward direction" of current chain would be using the
		// torso's directional axes found before. Keep in mind that these axes describe the
		// local axes of the torso/belt strap hook in world space.
		// Note that "outward" for chains in the belt strap hook's front would be backward.
        Vec3f outwardDirection = {0.f, 0.f, 0.f};
        Math_Vec3f_ScaleAndStore(&beltStrapHook_forwardAxis, -1.f, &outwardDirection);

        // Validate legitimacy of current outward direction by checking if it's not zero.
        f32 outwardLengthSquared =Math3D_Vec3fMagnitudeSq(&outwardDirection);
        if (outwardLengthSquared <= GREATER_THAN_ZERO) {
            continue;
        }

        // If valid, normalize the calculated outward direction of the current chain of
		// phys limbs.
        Math_Vec3f_Scale(&outwardDirection, 1.f / sqrtf(outwardLengthSquared));

		
		// ======== Calculate reset direction of chain in world space ========
		
		// With the current chain's outward direction determined, now rotate that chain
		// by its currently assigned reset angle.
		Vec3f resetHorizontalDir = { 0.f, 0.f, 0.f };	// Horizontal outward reset of chain
		Vec3f resetDownDir = { 0.f, 0.f, 0.f };			// Vertical reset of chain
		Vec3f resetDirection = { 0.f, 0.f, 0.f };		// Resulting reset direction of chain
		
		// Scale the chain's horizontal outward direction based on cosine of reset angle
		Math_Vec3f_ScaleAndStore(&outwardDirection, resetHorizontalDirScale, &resetHorizontalDir);
		
		// Scale the chain's downward direction based on sine of reset angle.
		Math_Vec3f_ScaleAndStore(&beltStrapHookDownDir, resetUpwardDirScale, &resetDownDir);
		
		// Subtract horizontal outward direction by downward direction of chain
		// Note that positive reset angles point upward and negative reset angles point downward.
		Math_Vec3f_Diff(&resetHorizontalDir, &resetDownDir, &resetDirection);
		
		// Validate current reset value by checking if it's not zero.
        f32 resetLengthSquared = Math3D_Vec3fMagnitudeSq(&resetDirection);
        if (resetLengthSquared <= GREATER_THAN_ZERO) {
            continue;
        }

		// If valid, normalize the calculated reset direction of the current chain of
		// phys limbs.
        Math_Vec3f_Scale(&resetDirection, (1.f / sqrtf(resetLengthSquared)));


		// ====== Reset all the phys limbs' position in world space based on reset direction ======
		
        // Go through each phys limb in the current chain and reset each limb's world position
		// based on the calculated reset direction.
		// Start from the first UNPINNED phys limb (in this belt strap hook's case, 0 is the pinned phys
		// limb, so skip that).
        for (int currLimbInChain = 1; currLimbInChain < limbCount; currLimbInChain++) {
			// Get the current limb's actual index in the entire gPhysLimbs set
            int currLimbIdx = startLimb + currLimbInChain;

			// Get the parent phys limb and current phys limb from gPhysLimbs
            PhysLimb* parentPhysLimb = gPhysLimbs[currLimbIdx - 1];
            PhysLimb* currPhysLimb = gPhysLimbs[currLimbIdx];
			
			// Commented out for now; may need this for future work involving pinned limb in
			// middle of chain.
			/*
            if (currPhysLimb->pinned) {
                continue;
            }*/

			// Get the default offset of current phys limb from parent phys limb and convert it
			// to Vec3f value (for calculating its length).
            Vec3f defaultJointOffset = {currPhysLimb->default_jointPos.x, 
				currPhysLimb->default_jointPos.y, currPhysLimb->default_jointPos.z};

			// Calculate the exact length value of current phys limb's default offset from parent
			// phys limb.
            f32 physBoneLength =sqrtf(Math3D_Vec3fMagnitudeSq(&defaultJointOffset));
            f32 worldBoneLength = physBoneLength * actorScale;		// Scale to match belt strap hook size
			
			// Calculate offset from parent phys limb to current phys limb
            Vec3f parentToCurrLimbOffset = {0.f, 0.f, 0.f};
			// Scale the reset direction to the phys bone length. This value should tell how far and in
			// what direction in the 3D world space the current phys limb should be in from parent phys
			// limb.
            Math_Vec3f_ScaleAndStore(&resetDirection, worldBoneLength, &parentToCurrLimbOffset);
			// Apply that direction value to the parent phys limb's position in world space.
            Math_Vec3f_Sum(&parentPhysLimb->curr_pos, &parentToCurrLimbOffset, &currPhysLimb->curr_pos);

			// Store that newly calculated current position of current phys limb to bodyPartsPos.
            this->bodyPartsPos[currLimbIdx] = currPhysLimb->curr_pos;
        }
    }

	/********************************
     Reset velocity and prev position of all phys limbs of belt strap hook
    ********************************/
    // Remove velocity and previous position carried over from before the reset.
    for (int currLimbIdx = 0; currLimbIdx < BELTSTRAPHOOK_BODYPART_MAX; currLimbIdx++) {
        PhysLimb* currLimb = gPhysLimbs[currLimbIdx];

        currLimb->prev_pos = currLimb->curr_pos;		// Reset previous position
        currLimb->curr_vel = (Vec3f){0.f, 0.f, 0.f};	// Reset current velocity
        currLimb->prev_vel = (Vec3f){0.f, 0.f, 0.f};	// Reset previous velocity
    }
}


/**
 * @brief Rotate a given target point about the pivot point by a given angle along the y-axis.
 *
 * Typically, the target point is the current position of a phys limb, and the pivot point is the
 * belt strap hook root's world position.
 *
 * The 'targetPoint' param will get overwritten with a new, rotated position of old 'targetPoint'
 *
 * @param targetPoint		Point in the world position to rotate
 * @param pivotPoint		Point where the targetPoint should rotate around by the y-axis
 * @param rotAngle			Rotate targetPoint around pivotPoint by this value			
 */
void BeltStrapHook_RotatePointAroundY(Vec3f* targetPoint, Vec3f* pivotPoint, s16 rotAngle) {
	// Find the line between targetPoint and pivotPoint
    Vec3f offset = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(targetPoint, pivotPoint, &offset);
	
	// Rotate that line by the input angle
    CustomMath_Vec3f_RotateByY(&offset, rotAngle, &offset);
	
	// Add that rotated line on to pivotPoint to get new targetPoint (saved over old targetPoint)
    Math_Vec3f_Sum(pivotPoint, &offset, targetPoint);
}


/**
 * @brief Calculate target phys collider/limb's new position in current substep based on yaw
 *
 * This function uses the previous frame and the current frame to calculate the target collider/phys 
 * limb's current positions in the current substep (indicated by t) without applying verlet integration. 
 * This function was designed to consider Jackie's torso limb's yaw rotation AND other animation-based
 * change in position when calculating the colliders' positions in the current substep.
 *
 * Note that this function is specifically for rotating colliders and phys limbs that are to skip verlet 
 * integration (i.e. pinned phys limbs). This function is not meant to be used for unpinned phys limbs.
 * 
 * @param prevFramePos			Target phys object's world position in previous frame
 * @param currFrameTargetPos	Target phys object's target world position in current frame
 * @param prevFramePivotPoint	Point where target phys object should rotate around in previous frame
 * @param currFramePivotPoint	Point where target phys object is expected to rotate around in current
 *								frame
 * @param yawRotation			How much the rotation by y-axis that needs to be applied to calculating
 *								position in current substep
 * @param t						Current substep described as fraction (current substep / # of substeps)
 * @param currSubstepNewPos		Output position of target phys object
 */
void BeltStrapHook_InterpolatePosWithRot(Vec3f* prevFramePos, Vec3f* currFrameTargetPos,
	Vec3f* prevFramePivotPoint, Vec3f* currFramePivotPoint, s16 yawRotation, f32 t, 
	Vec3f* currSubstepNewPos) {
	/********************************
     Calculate Target Phys Object's Position at Current Substep Due to Yaw
    ********************************/
	// Find the current substep's pivot point.
    Vec3f substepPivotPoint = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(prevFramePivotPoint, currFramePivotPoint, t, &substepPivotPoint);

	// In previous frame, find the line segment between target phys object's world position and the
	// pivot point.
    Vec3f prevFrameOffset = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(prevFramePos, prevFramePivotPoint, &prevFrameOffset);

	// Rotate that line segment by the given yaw rotation scaled down to current substep (t)
    Vec3f prevFrameOffsetRotated = prevFrameOffset;
    CustomMath_Vec3f_RotateByY(&prevFrameOffset, (s16)(yawRotation * t), &prevFrameOffsetRotated);
	
	/********************************
     Calculate Target Phys Object's Position at Current Substep Due to Other Movements
    ********************************/
	// Movements outside of yaw like crouching may impact position of target phys object at substep.
	
	// Calculate the full target rotation at the end of substeps between prev and current frame (t = 1)
    Vec3f currFrameTargetOffsetRotated = prevFrameOffset;
    CustomMath_Vec3f_RotateByY(&prevFrameOffset, yawRotation * 1, &currFrameTargetOffsetRotated);
	
	// Using the target rotation by end of substeps, calculate where the target phys object is supposed 
	// to end up at current frame (t = 1).
    Vec3f predictedEndPosFromYaw = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(currFramePivotPoint, &currFrameTargetOffsetRotated, &predictedEndPosFromYaw);

	// Take into account of other animation (bending, crouching, etc.) in current frame.
	// Calculate the difference from current frame's target position impacted by current animation
	// and where the current frame's target position would have been just from Jackie's yaw rotation.
    Vec3f residual = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(currFrameTargetPos, &predictedEndPosFromYaw, &residual);
	
	// Scale that difference by the current substep's value.
    Math_Vec3f_Scale(&residual, t);
	
	/********************************
     Combine the Change in Position in Current Substep based on the two
    ********************************/
	// From the current substep's pivot point world position, find the point where the phys object is
	// currently ending up in based on the yaw alone.
    Math_Vec3f_Sum(&substepPivotPoint, &prevFrameOffsetRotated, currSubstepNewPos);
	
	// From that position resulted just from the yaw, find the world position where the phys object is
	// in the current substep that is caused by the other animations.
    Math_Vec3f_Sum(currSubstepNewPos, &residual, currSubstepNewPos);
}


/**
 * @brief Calculate belt strap hook's world directional axes for current physics substep
 * 
 * This function interpolates the belt strap hook's previous frame's forward and down directions of the belt 
 * strap hook towards the current frame's target directions based on the current substep interpolation value.
 * 
 * After interpolation, the down direction is normalized. After that, the forward direction is corrected 
 * so that it is perpendicular to the down direction. Then, the forward direction is normalized also.
 * 
 * If either interpolated direction is unable to be normalized, then the functionf alls back to the 
 * current frame's target directions.
 * 
 * The belt strap hook's left direction is calculated using the cross product of the belt strap hook's 
 * forward and down directions. From this, we get the belt strap hook's forward, down, and left directions 
 * in world space for this current substep.
 * 
 * @param targetForward Current frame's target world-space forward direction of belt strap hook
 * @param targetDown    Current frame's target world-space down direction of belt strap hook.
 * @param t             Interpolation fraction value between previous and current frame
 */
void BeltStrapHook_SetSubstepDirAxes(Vec3f targetForward, Vec3f targetDown, f32 t) {
	// Interpolate belt strap hook's down direction from previous frame toward current frame's target down
	// direction based on current substep.
    Vec3f downDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&beltStrapHook_previousFrameDown, &targetDown, t, &downDirection);

    // Interpolate belt strap hook's forward direction from previous frame toward current frame's target forward 
    // direction based on the current substep.
    Vec3f forwardDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&beltStrapHook_previousFrameForward, &targetForward, t, &forwardDirection);

    // Normalize the belt strap hook's interpolated down direction
    // If the down direction is too small to normalize, then use the final target down direction instead.
    if (!BeltStrapHook_NormalizeSubstepDirectionVec(&downDirection)) {
        downDirection = targetDown;
    }

    // Using dot product, find projection to know how much of belt strap hook's forward direction points  in the 
    // same direction as belt strap hook's down direction.
    f32 projection = CustomMath_Vec3f_Dot(&forwardDirection, &downDirection);

    // Calculate the part of the forward direction that lies along the down direction.
    Vec3f forwardDownComponent = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&downDirection, projection, &forwardDownComponent);

    // Remove that component so forward becomes perpendicular to down.
    Math_Vec3f_Diff(&forwardDirection, &forwardDownComponent, &forwardDirection);
    
    // Normalize the belt strap hook's correct forward direction.
    // If the corrected forward direction is too small to normalize, then use the final target forward and 
    // down directions instead.
    if (!BeltStrapHook_NormalizeSubstepDirectionVec(&forwardDirection)) {
        // Near-opposite poses have no unique linear interpolation.
        Math_Vec3f_Copy(&beltStrapHook_forwardAxis, &targetForward);
        Math_Vec3f_Copy(&beltStrapHook_downAxis, &targetDown);
    } 
    // Otherwise, store the interpolated and corrected substep directions of the belt strap hook
    else {
        Math_Vec3f_Copy(&beltStrapHook_forwardAxis, &forwardDirection);
        Math_Vec3f_Copy(&beltStrapHook_downAxis, &downDirection);
    }
    
    // Calculate the belt strap hook's left direction from the belt strap hook's forward and down 
    // directions. This belt strap hook model's directional coordinate axes are now made: forward, down, and 
    // left.
    Math3D_Vec3f_Cross(&beltStrapHook_forwardAxis, &beltStrapHook_downAxis, &beltStrapHook_leftAxis);
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
 * @brief Update the a given pinned phys limb based on the belt strap hook's root limb
 *
 * This function takes a pinned phys limb from the belt strap hook using a given index value and 
 * updates its position in the world space. This is done by taking the phys limb's default joint position, 
 * which describes the translation from the parent limb (root limb of belt strap hook) during belt strap 
 * hook's idle state (-Z). Afterward, this translation vector is rotated based on the belt strap hook's 
 * actor's rotation in the world space.
 *
 * Keep note that this function only updates the position of the pinned phys limb.
 *
 * @param this			Belt strap hook actor
 * @param player		Player actor
 * @param gPhysLimbs	Phys limbs of belt strap hook. 
 * @param pinnedLimbIdx	Pinnned limb
 */
void BeltStrapHook_UpdatePinnedLimb(BeltStrapHook* this, Player* player, PhysLimb* gPhysLimbs[], 
    int pinnedLimbIdx) {
    // Take pinned limb (the first limb of the chain, which is also after root limb).
    PhysLimb* physLimb = gPhysLimbs[pinnedLimbIdx];

    // Save pinned phys limb's previous position and velocity.
    Math_Vec3f_Copy(&physLimb->prev_pos, &physLimb->curr_pos);
    Math_Vec3f_Copy(&physLimb->prev_vel, &physLimb->curr_vel);

    // Calculate pinned limb's offset from the belt strap hook's root.
    Vec3f transformVec = {0.f, 0.f, 0.f};
    Vec3s rootRotatedOffset = {0, 0, 0};
    Vec3s worldRotatedOffset = {0, 0, 0};

    // Take pinned phys limb's default joint position (which is just local translation from parent limb
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

    // Apply rotated translation vector to root limb's position to find pinned limb's position
    Math_Vec3f_Sum(&this->bodyPartsPos[BELTSTRAPHOOK_BODYPART_ROOT],
        &transformVec, &this->bodyPartsPos[pinnedLimbIdx]);
    
    // Save the calculated new current position of the actual limb into respective phys limb
    Math_Vec3f_Copy(&physLimb->curr_pos, &this->bodyPartsPos[pinnedLimbIdx]);
    // Save current velocity to phys limb also
    Math_Vec3f_Copy(&physLimb->curr_vel, &player->actor.velocity);
}


/**
 * @brief Update the radii for the colliders set up for belt strap hook.
 * 
 * Note that this function may need to receive some form of array connecting colliders and their
 * radius values together as arguments.
 * 
 * @param player    Player instance
 */
void BeltStrapHook_UpdateColliderRadii(Player* player) {
    // Convert the selected radii into current player's actor size.
    f32 colliderScale = player->actor.scale.x / 0.01f;

    // Initialize colliders radii with preset radii values.
    f32 torsoRadius = TORSO_COLLIDER_DEFAULT_RADIUS;
    f32 leftThighBackRadius = LEFTTHIGH_COLLIDER_DEFAULT_RADIUS;
    f32 rightThighBackRadius = RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS;

	// Update colliders with set radii values and input actor scale. (For belt strap hook, these
    // colliders' sizes would not change during any animation.)
    torsoSphereCollider.radius = torsoRadius * colliderScale;
    leftThighSphereCollider.radius = leftThighBackRadius * colliderScale;
    rightThighSphereCollider.radius = rightThighBackRadius * colliderScale;
}


/**
 * @brief Solve the belt strap hook's physics across multiple substeps within one rendered frame.
 * 
 * Note, however, that the number of substeps for belt strap hook is 1, so there is no substep
 * between two frames in this case.
 *
 * @param this          Belt strap hook actor
 * @param player        Player instance
 * @param apply_force   Force being applied onto belt strap hook for verlet integration
 * @param gPhysLimbs    All the phys limbs of the belt strap hook
 * @param gPhysBones    All the phys bones of the belt strap hook
 * @param limbsPrevPos  Previous frame's pinned phys limbs' starting position
 * @param chains        Array of starting phys limb indices of each vertical chains of belt strap hook
 * @param chainCount    Number of vertical cloth chains in the belt strap hook
 * @param substepCount  Number of substeps to use for interpolation
 */
void BeltStrapHook_SolvePhysAtSubsteps(BeltStrapHook* this, Player* player, Vec3f apply_force, 
    PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[], Vec3f limbsPrevPos[], 
    const BeltStrapHookLimbChain chains[], int chainCount, PhysSphereCollider* capsulePairs[][2], 
    int capsuleCount, int substepCount) {
    /********************************
     Set number of substeps for solving physics
    ********************************/
    int numSubsteps = substepCount;
    
    // If number of substeps is inputted to be less than 1, fix it to 1.
    if (numSubsteps < 1) {
        numSubsteps = 1;
    }

    // Set up change in time, where 1 frame is 1 full unit of time.
    f32 dt = 1.f / numSubsteps;

    /********************************
     Set up target positions of current frame
    ********************************/
    // Store current frame's target world position of each phys limb in current frame.
	// For pinned limbs, this becomes the target position they move toward across the substeps. This says
	// where each pinned limb should end up by the end of the frame.
    Vec3f limbsCurrTargetPos[BELTSTRAPHOOK_BODYPART_MAX];

    // Store target length of each bone for current frame, which says how long each bone should be by the
	// end of frame
    static f32 targetBoneLengths[BELTSTRAPHOOK_BONE_MAX];

    // Store pointers to all sphere colliders used by belt strap hook's limbs
	// Most of these sphere colliders are to be used for capsule colliders
    // NOTE: No substeps here, so this is only put here for maintaining design consistency with belt strap hook.
    PhysSphereCollider* colliderPointers[BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT] = {
        // Nothing here due to no substeps, but this should normally be where the addresses to the 
        // left/right thigh colliders and torso collider should be.
    };
	
	// For current frame, store each collider's target position and radius for current frame.
	// Like the limbs, colliders' positions will change every substep.
	// Colliders' radii would change for capsule colliders with differently-sized sphere colliders.
    // NOTE: No substeps here, so this is only put here for maintaining design consistency with belt strap hook.
    PhysSphereCollider beltStrapHook_currFrameTargetColliders[BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT];

    // Set up Jackie's forward, down, and left directions in world space.
	// The belt strap hook uses these directions to keep its connected cloth chains moving correctly
	// relative to Jackie's body.
	// This system is designed for belt strap hook specifically because it has chains of limbs that are horizontally 
	// connected to each other. So, this directional system is set up to help with both vertical AND 
	// horizontal constraints, whereas belt strap hook and ponytail only has vertical constraints
    Vec3f targetForward = {0.f, 0.f, 0.f};
    Vec3f targetDown = {0.f, 0.f, 0.f};
    Vec3f targetLeft = {0.f, 0.f, 0.f};

    Math_Vec3f_Copy(&targetForward, &beltStrapHook_forwardAxis);
    Math_Vec3f_Copy(&targetDown, &beltStrapHook_downAxis);
    Math_Vec3f_Copy(&targetLeft, &beltStrapHook_leftAxis);

    // Store Jackie's current actor scale as the target scale for this current frame.
    f32 targetScale = fabsf(this->actor.scale.x);

    /********************************
     Set up opposing movement
    ********************************/
    // Store Jackie's horizontal movement in current frame so that the belt strap hook can react to 
    // the motion.
    Vec3f opposingVelocity = {0.f, 0.f, 0.f};
    Math_Vec3f_Copy(&opposingVelocity, &player->actor.velocity);

    // While riding, use movement measured from world positions.
	// Riding Epona: PLAYER_STATE1_800000
    if (player->stateFlags1 & PLAYER_STATE1_800000) {
        Math_Vec3f_Copy(&opposingVelocity, &gBeltStrapHookEpona.world_vel);
    }
    opposingVelocity.y = 0.f;   // ignore vertical movement

    // Disable the opposing movement while climbing ladder/wall or opening door.
    // Climbing Wall: PLAYER_STATE1_200000
    // Opening Door: Player_Action_36
    if ((player->stateFlags1 & PLAYER_STATE1_200000) || player->actionFunc == Player_Action_36) {
        opposingVelocity = (Vec3f){0.f, 0.f, 0.f};
    }

    /********************************
     Save target colliders' state in current frame
    ********************************/
    // Collider stuff are not conducted here due to the fact that none of the colliders are changing
    // positions or sizes or require any interpolation-related changes.

    /********************************
     Save phys limbs' target positions in current frame
    ********************************/
	// Save each phys limb's target world positions at the current frame.
	// Note: For pinned phys limbs, this position was already set by BeltStrapHook_UpdatePinnedLimb()
    for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
        if (gPhysLimbs[i]->pinned) {
            Math_Vec3f_Copy(&limbsCurrTargetPos[i], &gPhysLimbs[i]->curr_pos);
        }
    }

    /********************************
     Initialize previous frame's history
    ********************************/
	// If there is no valid previous-frame physics history, use the current frame's target values as both the
	// previous frame and current frame values
	// This prevents the first set of substeps from interpolating from outdated or invalid state
    if (!beltStrapHook_stepHistoryReady) {
		// Treat each collider's current frame target state as previous frame state
		// This makes collider start in the correct place instead of moving from unintended positions
        for (int currSphereCol = 0; currSphereCol < BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT; currSphereCol++) {
            beltStrapHook_prevFrameColliders[currSphereCol] = beltStrapHook_currFrameTargetColliders[currSphereCol];
        }
		
		// Initialize starting positions of phys limbs
        for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
			// If the phys limb is pinned and it has no valid previous frame, start this phys limb at its
			// current frame's target position
            if (gPhysLimbs[i]->pinned) {
                limbsPrevPos[i] = limbsCurrTargetPos[i];
            }
			// Remove any old Verlet Integration movement from the phys limb by assigning current frame's 
			// target position as the phys previous position
            gPhysLimbs[i]->prev_pos = gPhysLimbs[i]->curr_pos;
        }
		
		// Use current frame's target directions (described in world space) and scale as previous frame's
		// values. This is only done when a valid previous frame does not exist, so the first substeps
		// do not interpolate from outdated directions or actor scale.
        Math_Vec3f_Copy(&beltStrapHook_previousFrameForward, &targetForward);
        Math_Vec3f_Copy(&beltStrapHook_previousFrameDown, &targetDown);
        beltStrapHook_previousSubstepScale = targetScale;

		// Save the current y-axis rotation along world space as previous frame's y-axis rotation
        beltStrapHook_previousFrameYaw = this->actor.shape.rot.y;
    }

    /********************************
     Skip verlet for certain amount of yaw
    ********************************/
	// The turning animation of Jackie (Link, specifically) has her just snap in the direction the control
	// stick was heading to. This doesn't work well with Verlet integration, which specifically relies on 
	// actual movement/rotation of Jackie's body.
	// So, this part handles a situation where the body has to snap to a specific direction in one frame
	// rather than properly rotate in one direction. If the direction along the y-axis (rotating 
	// left/right) is greater than what is defined for BELTSTRAPHOOK_MAX_YAW, then the verlet integration is 
	// skipped for this case.
	
	// Convert the max rotation along the y-axis (yaw) into s16 degrees.
	s16 maxYaw = CustomMath_DegreeToBinaryAngle(BELTSTRAPHOOK_MAX_YAW);
		
    // Calculate the change in the belt strap hook's facing angle since the previous frame
    s16 yawDelta = (s16)(this->actor.shape.rot.y - beltStrapHook_previousFrameYaw);

    // Check if Jackie's turn from previous frame to current frame was over the specified 
    // BELTSTRAPHOOK_MAX_YAW The current change in yaw will be subtracted by the yaw limit 
    // BELTSTRAPHOOK_MAX_YAW to determine how much of the rotation should forgo the verlet integration and 
    // just rotate along the belt strap hook.
    s16 yawToSkipVerlet = 0;
    // If current change in yaw is greater than BELTSTRAPHOOK_MAX_YAW, then record how much of the yaw should 
    // skip the verlet integration.
    if (yawDelta > maxYaw) {
        yawToSkipVerlet = yawDelta - maxYaw;
    }
    // If current change in yaw is less than the negative of BELTSTRAPHOOK_MAX_YAW, then record how much the
    // yaw should skip the verlet integration.
    else if (yawDelta < -maxYaw) {
        yawToSkipVerlet = yawDelta + maxYaw;
    }

	// If Jackie's yaw is greater than BELTSTRAPHOOK_MAX_YAW, then show the verlet integration that would 
    // occur on belt strap hook up to specified BELTSTRAPHOOK_MAX_YAW. Outside of the physics effect, the phys 
    // limbs along with the colliders are then just rotated around the belt strap hook root limb.
    if (yawToSkipVerlet != 0) {
		// Set the belt strap hook root limb as the point where the phys limbs would rotate around.
        Vec3f limbsPivotPoint = limbsPrevPos[BELTSTRAPHOOK_BODYPART_ROOT];

        // Rotate cloth and its previous position together, essentially skipping the application of verlet
        // integration on this amount of yaw rotation.
        for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
            // Pinned phys limb rotation
            if (gPhysLimbs[i]->pinned) {
                BeltStrapHook_RotatePointAroundY(&limbsPrevPos[i], &limbsPivotPoint, yawToSkipVerlet);
            }
            // Unpinned phys limb rotation (previous position also has to be rotated alongside to avoid
            // verlet integration caused by change in position, which is also essentially velocity)
            else {
                BeltStrapHook_RotatePointAroundY(&gPhysLimbs[i]->curr_pos, &limbsPivotPoint, yawToSkipVerlet);
                BeltStrapHook_RotatePointAroundY(&gPhysLimbs[i]->prev_pos, &limbsPivotPoint, yawToSkipVerlet);
            }
        }

        // Rotate the previous frame's colliders positions and axes also by the same amount
        for (int currSphereCol = 0; currSphereCol < BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT; currSphereCol++) {
            BeltStrapHook_RotatePointAroundY(&beltStrapHook_prevFrameColliders[currSphereCol].center, 
				&limbsPivotPoint, yawToSkipVerlet);
        }

        // Rotate the forward and down directions of the belt strap hook by the same amount also
        CustomMath_Vec3f_RotateByY(&beltStrapHook_previousFrameForward, yawToSkipVerlet, 
			&beltStrapHook_previousFrameForward);
        CustomMath_Vec3f_RotateByY(&beltStrapHook_previousFrameDown, yawToSkipVerlet, 
            &beltStrapHook_previousFrameDown);
    }

    // Only this part of the turn is simulated across substeps
    s16 remainingYaw = (s16)(yawDelta - yawToSkipVerlet);

    /********************************
     Calculate Current Frame's Target Bone Lengths
    ********************************/
	// Calculate target world space length of every belt strap hook bone for the current frame.
	// Each target length is based on the bone's default local offset and Jackie's current target scale.
	// Keep note that while bone length is normally constant, the lengths would actually change if 
	// Jackie's actor scale changes (e.g. Giant's Mask transformation phase)
	
	// Note: Root-LimbA bones are technically irrelevant here and skipped later because they contain only 
	// pinned limbs.
    for (int j = 0; j < BELTSTRAPHOOK_BONE_MAX; j++) {
        Vec3s offset = gPhysBones[j]->limb_b->default_jointPos;
        f32 x = offset.x;
		f32 y = offset.y; 
		f32 z = offset.z;
        targetBoneLengths[j] = sqrtf(x * x + y * y + z * z) * targetScale;
    }

    /********************************
     Solve Verlet Integration Across Substeps (within 1 frame)
    ********************************/
	// Instead of solving the verlet integration of limbs from 1 previous frame to current frame, divide
	// the time between previous frame and current frame into substeps (this is why dt was calculated).
	// Solving the verlet integration for every substep would allow for a more gradual 
	// movement/calculation of position changes, helps reduce unwanted clipping or other behavior for the 
	// belt strap hook.
	
	// For each substep:
	// 1. Interpolate Jackie's actor scale between previous and current frames.
	// 2. Interpolate collider positions and radii between previous and current frames.
	// 3. Interpolate the direction references of Jackie.
	// 4. Update bone lengths to match current substep's interpolated scale.
	// 5. Move pinned phys limbs toward their current substep target positions (not current frame)
	// 6. Move unpinned phys limbs using Verlet Integration in substep
	// 7. Repeatedly solve bone, sideways, movement limit, and collision constraints in current substep.
	// 8. Recalculate limb velocities from corrected positions in current substep vs previous substep.
	
	// Split the current frame into several smaller frames 
    // Final substep represents the current frame's target state.
    for (int currSubstep = 0; currSubstep < BELTSTRAPHOOK_SUBSTEPS; currSubstep++) {
		// Calculate how far this substep is in the range between previous and current frame
		// For instance, if SUBSTEP is 4 (as in 4 substeps within a frame), then...
		// substep 0 = 25%, substep 1 = 50%, substep 2 = 75%, substep 3 = 100% 
        f32 t = (f32)(currSubstep + 1) / BELTSTRAPHOOK_SUBSTEPS;
		
		/********************************
         1. Interpolate actor scale
        ********************************/
		// In case there is a change in Jackie's actor size between previous frame and current frame, 
		// interpolate Jackie's actor scale from the previous frame's scale toward the current frame's
		// target scale considering the current substep (t)
        f32 substepScale = beltStrapHook_previousSubstepScale + 
			((targetScale - beltStrapHook_previousSubstepScale) * t);		// based on interpolation formula
		
		// Calculate how much the current substep's scale differs from current frame's target scale.
		// This ratio is used to scale bone lengths and sideways constraint for this substep.
        f32 scaleRatio = 1.f;
		if (targetScale > GREATER_THAN_ZERO) {
			scaleRatio = substepScale / targetScale;
		}

		/********************************
         2. Interpolate colliders' position based on remaining yaw
        ********************************/
		// If Jackie's yaw was greater than the specified BELTSTRAPHOOK_MAX_YAW, chances are, there is still
		// some yaw remaining that needs to be applied to the rotation of the belt strap hook.
		// However, keep note that by this point, the rotation that we did not want the verlet integration
		// to be applied to all phys limbs (both pinned and unpinned) has already been dealt with. At this
		// point, we want to properly rotate the colliders and pinned phys limbs by the rest of the 
		// remaining yaw (remainingYaw). That way, the verlet integration can be applied to the pinned
		// phys limbs via movement of the pinned phys limbs caused by the remaining yaw.
		
		// At this stage, move each collider to a new position between the previous frame's position and 
		// the current frame's target position for this current substep (t). When doing this, both
		// remaining yaw (remainingYaw) from prior and animation-based limb position change are used.
        for (int currSphereCol = 0; currSphereCol < BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT; currSphereCol++) {
			// Update colliders' world positions at current substep.
            BeltStrapHook_InterpolatePosWithRot(&beltStrapHook_prevFrameColliders[currSphereCol].center, 
				&beltStrapHook_currFrameTargetColliders[currSphereCol].center, 
				&limbsPrevPos[BELTSTRAPHOOK_BODYPART_ROOT], &limbsCurrTargetPos[BELTSTRAPHOOK_BODYPART_ROOT], 
				remainingYaw, t, &colliderPointers[currSphereCol]->center);

			// Update the colliders' radii based on their change in position.
            colliderPointers[currSphereCol]->radius = beltStrapHook_prevFrameColliders[currSphereCol].radius +
                (beltStrapHook_currFrameTargetColliders[currSphereCol].radius - 
				beltStrapHook_prevFrameColliders[currSphereCol].radius) * t;
        }
		
		/********************************
         3. Interpolate directional reference axes
        ********************************/
		// Set Jackie's forward and down directions (in world space) to the orientation they should have 
		// during the current substep. These directions are interpolated between the previous frame's
		// directional references and the current frame's target directional references of Jackie.
        if (beltStrapHook_dirAxesReady) {
            BeltStrapHook_SetSubstepDirAxes(targetForward, targetDown, t);
        }
		
		/********************************
         4. Update bone lengths based on current substep
        ********************************/
		// Set each bone's length to the properly scaled value for the current substep
        for (int j = 0; j < BELTSTRAPHOOK_BONE_MAX; j++) {
            gPhysBones[j]->bone_length = targetBoneLengths[j] * scaleRatio;
        }
		
		/********************************
         5-6. Move pinned and unpinned phys limbs
        ********************************/
		// Update every phys limb of the belt strap hook for the current substep.
		// Pinned limbs are moved to their interpolated positions according to current substep.
		// Unpinned limbs are moved using Verlet Integration based on current substep's data.
        for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
            PhysLimb* currPhysLimb = gPhysLimbs[i];
			// For pinned phys limb...
            if (currPhysLimb->pinned) {
				// Move pinned phys limb to the interpolated position it should have during current substep. 
                // The root lerps normally since it's the pivot; every other pinned limb orbits around the 
                // root so large turns sweep instead of cutting through the torso.
                if (i == BELTSTRAPHOOK_BODYPART_ROOT) {
                    CustomMath_Vec3f_Lerp(&limbsPrevPos[i], &limbsCurrTargetPos[i], t, 
                        &currPhysLimb->curr_pos);
                }
                else {
                    BeltStrapHook_InterpolatePosWithRot(&limbsPrevPos[i], &limbsCurrTargetPos[i],
                        &limbsPrevPos[BELTSTRAPHOOK_BODYPART_ROOT], 
                        &limbsCurrTargetPos[BELTSTRAPHOOK_BODYPART_ROOT], remainingYaw, t,
                        &currPhysLimb->curr_pos);
                }
            } 
			// For unpinned limbs...
			else {
				// Move unpinned limbs using Verlet integration within this substep
                Verlet_LimbUpdatePosSubstep(currPhysLimb, &apply_force, &opposingVelocity, dt);
            }
        }
		
		/********************************
         7a. Distribute constraint iterations across substeps
        ********************************/
		// Divide total number of constraint-solving iterations for one frame (e.g. collider collisions and 
        // bone length constraints) across all substeps. 
		// For instance, if COLLISION_FACTOR is 75, which means there is to be 75 constraint-solving
		// iterations, the 75 total iterations per frame is distibuted across every each substep 
		// (19 or 18 iterations per substep if number of substeps per frame is 4).
        int iterations = COLLISION_FACTOR / BELTSTRAPHOOK_SUBSTEPS;
		
		// Add an iteration if there is any remaining
        if (currSubstep < COLLISION_FACTOR % BELTSTRAPHOOK_SUBSTEPS) {
            iterations++;
        }
		
		// Make sure every substep gets at least one constraint-solving iteration
        if (iterations < 1) {
            iterations = 1;
        }
		
		/********************************
         7b. Solve cloth constraints with calculated iterations for this substep
        ********************************/
        // Repeat these corrections 'iterations' number of times for the current substep so the cloth can 
		// settle into a more stable position.
		// Each iteration fixes bone lengths, limits sideways stretching between cloth chains horizontally, 
        // applies upper belt strap hook movement restrictions, and pushes horizontal midpoint between 
        // cloth limb out of colliders
        for (int iteration = 0; iteration < iterations; iteration++) {

            // =========== Fix the phys bones' lengths to match its original length ==========

			// Correct every phys bone's length so that it matches the calculated respective target lengths 
            // for this substep
            for (int j = 0; j < BELTSTRAPHOOK_BONE_MAX; j++) {
                PhysBone* bone = gPhysBones[j];
				// Skip phys bones where both phys limbs are pinned
                if (bone->limb_a->pinned && bone->limb_b->pinned) {
                    continue;
                }
                // Only solve bone length constraint if two phys limbs in current phys bone are far 
				// apart beyond calculated length
                if (Math_Vec3f_DistXYZ(&bone->limb_a->curr_pos, &bone->limb_b->curr_pos) > 
					GREATER_THAN_ZERO) {
                    Verlet_BoneConstraint(bone);
                }
            }

            // ============== Limit sideways distance of belt strap hook's phys limbs ==============
			
			// For each chain of phys limbs in belt strap hook, iterate through each phys limb in a given chain
			// and limit its horizontal distance from the next chain's respective phys limb so that the
			// belt strap hook mesh does not stretch too far apart sideways.

            // NOTE: There is no sideways limit for belt strap hook, so this is disabled.

            /*
            for (int chain = 0; chain < chainCount - 1; chain++) {
				// Iterating through each phys limb in a current chain of phys limbs
                for (int row = 1; row < 9; row++) {
					// Limit the horizontal distance between current phys limb and next chain's phys limb
					// at the same row by the predetermined horizontal limit defined in sidewaysMaxDistance
                    BeltStrapHook_LimitSidewaysDistance(
                        gPhysLimbs[chains[chain] + row], gPhysLimbs[chains[chain + 1] + row],
                        sidewaysMaxDistance[chain][row] * scaleRatio);
                }
				// Last chain is skipped since there is not supposed to be any max distance limit between
				// two front-most chains in the belt strap hook.
            }*/

            // ============ Limit the rotation of belt strap hook's chains of phys limbs ============
			
			// Limit the direction each upper belt strap hook bone can bend relative to Jackie's body.
			// In other words, this applies directional constraints to the phys bones to prevent unwanted
            // physics behavior. It is the most artificial element of this physics project. 
            // Check function for more details.
            BeltStrapHook_ApplyChainRotationLimits(gPhysLimbs, chains, chainCount, &this->rotLimits);

            // ============= Push belt strap hook's unpinned phys limbs out of colliders =============
			
			// Apply collision correction to all the unpinned phys limbs. If a phys limb position is 
			// calculated to be inside a collider in this substep, push it back out.
            for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
				// Skip pinned phys limbs
                if (gPhysLimbs[i]->pinned) {
                    continue;
                }
				
				// Take all the colliders and correct the unpinned phys limb's position if they are inside
                // said colliders.
                BeltStrapHook_SolveColliderGroup(gPhysLimbs[i], capsulePairs, capsuleCount);
            }

            // ======== Push midpoint of two horizontally neighboring phys limbs out of colliders ========
			
			// Check the midpoint between neighboring phys limbs in the same row from a current chain of
			// phys limbs and the next chain of phys limbs. If that midpoint is inside one of the colliders,
			// then push the midpoint area back out.
			// This is done to prevent the cloth model's two chains from clipping through the body.

            // NOTE: There is no need to solve for midpoint for belt strap hook.

            /*
            for (int chain = 0; chain < chainCount - 1; chain++) {
				// Check row of current chain and the next chain
                for (int row = 1; row < 9; row++) {
					// Take midpoint between current chain's phys limb and next chain's phys limb in current
					// row and push it out of colliders.
                    BeltStrapHook_SolveMidpointGroup(
                        gPhysLimbs[chains[chain] + row], gPhysLimbs[chains[chain + 1] + row], 
                        capsulePairs, capsuleCount);
                }
            }*/
        }

        /********************************
         7c. Re-do Collider Corrections for current substep
        ********************************/
        for (int currPhysLimbIdx = 0; currPhysLimbIdx < BELTSTRAPHOOK_BODYPART_MAX; currPhysLimbIdx++) {
            if (!gPhysLimbs[currPhysLimbIdx]->pinned) {
                BeltStrapHook_SolveColliderGroup(gPhysLimbs[currPhysLimbIdx], capsulePairs, capsuleCount);
            }
        }

        /********************************
         8. Recalculate phys limb velocities from corrected positions
        ********************************/
        // Recalculate phys limb velocities from corrected positions in current substep vs previous substep.
        for (int currPhysLimbIdx = 0; currPhysLimbIdx < BELTSTRAPHOOK_BODYPART_MAX; currPhysLimbIdx++) {
            PhysLimb* currPhysLimb = gPhysLimbs[currPhysLimbIdx];

            // Current velocity of current substep now becomes previous velocity for next substep
            Math_Vec3f_Copy(&currPhysLimb->prev_vel, &currPhysLimb->curr_vel);    

            // Calculate new velocity for the next upcoming substep.
            Math_Vec3f_Diff(&currPhysLimb->curr_pos, &currPhysLimb->prev_pos, &currPhysLimb->curr_vel);
            Math_Vec3f_Scale(&currPhysLimb->curr_vel, 1.f / dt);
        }
    }

    /********************************
     Restore current frame physics state (after all substeps)
    ********************************/
    // After all substeps are finished running, restore the exact target values for current frame.
    // Afterward, save the current frame's collider, directional references, and scale values as previous
    // frame data that will be used when interpolating for next frame.

    // Restore all colliders to their exact current frame target state then save that state as previous
    // frame collider state for when the next upcoming frame becomes the current frame.
    for (int c = 0; c < BELTSTRAPHOOK_SUBSTEP_COLLIDER_COUNT; c++) {
        *colliderPointers[c] = beltStrapHook_currFrameTargetColliders[c];
        beltStrapHook_prevFrameColliders[c] = beltStrapHook_currFrameTargetColliders[c];
    }

    // Restore all pinned phys limbs to their current frame's target world positions
    for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
        if (gPhysLimbs[i]->pinned) {
            gPhysLimbs[i]->curr_pos = limbsCurrTargetPos[i];
        }
    }

    // Restore every bone to its exact target length for the current frame.
    for (int j = 0; j < BELTSTRAPHOOK_BONE_MAX; j++) {
        gPhysBones[j]->bone_length = targetBoneLengths[j];
    }

    // Restore Jackie's directional references back to current frame's original directional references
    Math_Vec3f_Copy(&beltStrapHook_forwardAxis, &targetForward);
    Math_Vec3f_Copy(&beltStrapHook_downAxis, &targetDown);
    Math_Vec3f_Copy(&beltStrapHook_leftAxis, &targetLeft);

    // Save current frame's directional references and scale as previous frame's values for next frame
    Math_Vec3f_Copy(&beltStrapHook_previousFrameForward, &targetForward);
    Math_Vec3f_Copy(&beltStrapHook_previousFrameDown, &targetDown);
    beltStrapHook_previousSubstepScale = targetScale;
    beltStrapHook_previousFrameYaw = this->actor.shape.rot.y;

    // Raise flag that substep are done running and previous frame's values are now fully established
    // for next frame to use for physics operations
    beltStrapHook_stepHistoryReady = 1;
}


/**
 * @brief Update the positions of phys limbs of belt strap hook based on verlet integration
 * 
 * This function directly modifies the position of every phys limb of the belt strap hook using verlet 
 * integration. It uses the following to calculate the position of every phys limb for the frame:
 * 
 * 1. Player's current position, rotation, velocity, and actor scale
 * 
 * 2. Current and previous positions of belt strap hook's phys limbs
 * 
 * 3. Pinned phys limb positins based on the belt strap hook's root limb
 * 
 * 4. Length of phys bones containing two phys limbs as endpoints
 * 
 * 5. Maximum sideways horizontal distance between neighboring cloth chains of phys limbs
 * 
 * 6. Collision with colliders assigned to Jackie for belt strap hook
 * 
 * 7. Artificial directional constraints that restrict how the belt strap hook can move relative to player
 * 
 * The function also calculates the belt strap hook's current size based on the player's actor's size 
 * every substep, which are basically smaller frames divided from a single frame.
 * 
 * @param this          BeltStrapHook actor
 * @param player        Player actor
 * @param apply_force   Net force applied to belt strap hook which causes change in position via Verlet 
 *                      Integration
 * @param gPhysLimbs    Phys limbs of the belt strap hook
 * @param gPhysBones    Phys bones of the belt strap hook
 */
void BeltStrapHook_UpdateBodyPartsPos(BeltStrapHook* this, Player* player, Vec3f apply_force, 
	PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]) {
    // Apply rotation limit and reset before doing anything else.
    BeltStrapHook_UpdateRotLimitsAndReset(this, player);

    // Save Jackie's previous position as belt strap hook's previous position
    Math_Vec3f_Copy(&this->actor.prevPos, &player->actor.prevPos);

    /********************************
     Update Epona movement
    ********************************/
    // Measure Jackie's world movement while riding Epona.
    // Restart the movement measurement when the belt strap hook physics resets.
    Verlet_UpdatePhysEponaMovement(&gBeltStrapHookEpona, player, this->needsReset);

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

    // Save all of belt strap hook phys limbs' previous frame's world positions.
    // This is necessary because the belt strap hook interpolates pinned phys limbs' positions across 
    // multiple substeps instead of moving them directly from one frame position to the next.
    static Vec3f pinnedLimbsPrevPos[BELTSTRAPHOOK_BODYPART_MAX];
    for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
        pinnedLimbsPrevPos[i] = gPhysLimbs[i]->curr_pos;
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
     Set up pinned chain anchors
    ********************************/
    for (int currChain = 0; currChain < BELTSTRAPHOOK_CHAIN_COUNT; currChain++) {
        BeltStrapHook_UpdatePinnedLimb(this, player, gPhysLimbs, belt_chains[currChain].startLimb);
    }

    /********************************
     Update body colliders' size
    ********************************/
    BeltStrapHook_UpdateColliderRadii(player);

    /********************************
     Set up sideways cloth connections for horizontal distance limits
    ********************************/
    // There are no sideways limits to consider for belt strap hook, as this only cosists of
    // two chains of phys limbs that do not need to be a specific distance from one another.

    /********************************
     Set up capsule colliders with sphere collider pairs
    ********************************/
    // Two sphere colliders are paired together to create a capsule collider
    // Capsule colliders essentially cover the hips and legs to prevent the belt strap hook from clipping
    // through Jackie's body between individual sphere colliders.
    PhysSphereCollider* capsulePairs[][2] = {
        {&torsoSphereCollider, &torsoSphereCollider},
        {&leftThighSphereCollider, &rightThighSphereCollider}
    };

    // Calculate number of capsule colliders
    // Start with only the permanent thigh and leg pairs.
    // Exclude the two reserved slots until they are filled.
    int capsuleCount = sizeof(capsulePairs) / sizeof(capsulePairs[0]) - 
        BELTSTRAPHOOK_RESERVED_COLLIDER_SLOTS;

    /********************************
     Calculate horizontal distance limits
    ********************************/
    // There are no sideways limits to consider for belt strap hook, as this only cosists of
    // two chains of phys limbs that do not need to be a specific distance from one another.

    /********************************
     Reset belt strap hook's phys limbs' positions after physics reset
    ********************************/
    // If there is no valid previous frame physics values, then reset the unpinned phys limbs into
    // valid pre-set cloth shape before starting the substep simulation
    // If there is no valid previous frame physics values, then reset the unpinned phys limbs into
    // valid pre-set cloth shape before starting the substep simulation
    if (!beltStrapHook_stepHistoryReady) {
        // Wait until Jackie's torso direction references are set, since they are needed to
        // reset the belt strap hook's shape
        if (!beltStrapHook_dirAxesReady) {
            return;
        }
        
        // Reset the belt strap hook's chains of phys limbs based on previously assigned reset angle.
        BeltStrapHook_SetResetShape(this, gPhysLimbs, belt_chains, BELTSTRAPHOOK_CHAIN_COUNT);

        // Use the reset phys limb positions as starting positions for the upcoming substep
        // physics operations
        for (int i = 0; i < BELTSTRAPHOOK_BODYPART_MAX; i++) {
            pinnedLimbsPrevPos[i] = gPhysLimbs[i]->curr_pos;
        }
    }

    /********************************
     Physics Operations with Substeps
    ********************************/
    // Run the physics operations (e.g. position changes, constraints, etc.) across multiple
    // substeps.
    BeltStrapHook_SolvePhysAtSubsteps(this, player, apply_force, gPhysLimbs, gPhysBones, 
        pinnedLimbsPrevPos, belt_chains, BELTSTRAPHOOK_CHAIN_COUNT, capsulePairs, capsuleCount,
        BELTSTRAPHOOK_SUBSTEPS);
}


/**
 * @brief Rotate a single chain in belt strap hook
 *
 * This function takes the phys bones and the actual bones from a chain of the belt strap hook actor using index 
 * values that indicate the start and end of the chain of bones.
 * 
 * Note that this function is unique for belt strap hook in that it makes the belt strap hooks behave like
 * straps rather than something that is being directly extended from the belt strap hook.
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
 * hook's physics bones. More specifically, the actual belt strap hook bones are rotated using a matrix so 
 * that the belt strap hooks's bones match with the rotation of the physics bones.
 * 
 * @param this          BeltStrapHook actor
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
	
	// Rotate every belt strap hook chain.
    for (int i = 0; i < BELTSTRAPHOOK_CHAIN_COUNT; i++) {
        // Skip the bone connecting the root and pinned limb.
        int firstBoneIdx = belt_chains[i].startBone + 1;

        // RotateChain() uses an inclusive ending bone index.
        int lastBoneIdx = belt_chains[i].startBone + belt_chains[i].limbCount - 1;

        BeltStrapHook_RotateChain(this, gPhysBones, firstBoneIdx, lastBoneIdx);
    }

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

    BeltStrapHook_UpdateRotLimitsAndReset(this, player);

    /********************************
     Kamaro Dance Reset
    ********************************/

    // Restore the ordinary reset shape when the dance ends.
    if (this->wasKamaroDancing && !BeltStrapHook_IsKamaroDancing(player)) {
        this->needsReset = 1;
    }

    this->wasKamaroDancing = BeltStrapHook_IsKamaroDancing(player);

    /********************************
     Giant's Mask Transformation Reset
    ********************************/
    // Transforming into Giant
    // Track the Giant's mask tranformation animation phase
    // gPlayerAnim_cl_setmask and gPlayerAnim_cl_setmaskend indicate Jackie is transforming
    // while beginning to wear the Giant's mask
    // Credit: Neirn
    u8 playingMaskAnimation = (player->skelAnime.animation == &gPlayerAnim_cl_setmask ||
        player->skelAnime.animation == &gPlayerAnim_cl_setmaskend);

    // Reset when the mask animation sequence ends with the Giant's Mask equipped.

    // Keep track of the Giant's mask transformation animation phase ending when:
    // - Jackie was in the transformation animation phase in the previous frame
    // - Jackie is not in the transformation animation phase in the current frame
    // - Jackie is currently wearing the Giant's mask
    if (this->wasTransformingIntoGiant && !playingMaskAnimation && 
        player->currentMask == PLAYER_MASK_GIANT) {
        this->needsReset = 1;
    }

    // Keep track of the current frame's state with the transformation animation phase for next frame
    this->wasTransformingIntoGiant = playingMaskAnimation;

    // Transforming from Giant to Normal (Removing Giant's Mask)
    // Keep note of Jackie's current scale (If she's a Giant, it should be greater than 0.01f)
    f32 giantScale = player->actor.scale.x;

    // If Jackie's actor scale is greater than her normal scale (0.01f), then remember for next frame that
    // she's a Giant in the current frame.
    if (giantScale > 0.01f) {
        this->wasGiant = 1;
    }

    // If Jackie was a giant in the previous frame but her scale in the current frame is normal size (0.01f),
    // then she's no longer a Giant in the current frame.
    if (this->wasGiant && giantScale == 0.01f) {
        this->wasGiant = 0;

        // Make another check on if she's human form and she's not wearing any mask right now.
        if (player->transformation == PLAYER_FORM_HUMAN && player->currentMask == PLAYER_MASK_NONE) {
            this->needsReset = 1;
        }
    }

    /********************************
     Transformation from Non-Human to Human
    ********************************/
    // Get reset positions of belt strap hook's phys limbs when transforming from non-human to human
    if (player->transformation != PLAYER_FORM_HUMAN) {
        this->needsReset = 1;
    }

    /********************************
     First-Person Camera
    ********************************/
    // Keep a reset pending while Jackie is in first person.
    if (BeltStrapHook_IsInFirstPerson(play, player)) {
        this->needsReset = 1;
    }

    /********************************
     Transformation into Non-Human
    ********************************/
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
        if (BeltStrapHook_IsInFirstPerson(play, player)) {
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
            BeltStrapHook_UpdateBodyPartsPos(this, player, net_force, beltStrapHook_PhysLimbs, 
                beltStrapHook_PhysBones);

            // Wait for valid torso axes and a completed shape reset.
            if (this->needsReset) {
                return;
            }

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