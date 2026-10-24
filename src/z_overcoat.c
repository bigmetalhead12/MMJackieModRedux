/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Applying verlet integration to each limb of Jackie's overcoat model
    and then drawing them to Majora's Mask recomp

========================================================================
*/

#include "z_overcoat.h"
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
#define LIMB_MASS   0.2f
#define PINNED      1
#define NOT_PINNED  0
#define COLLISION_FACTOR    50

#define OVERCOAT_CHAIN_COUNT 21
#define OVERCOAT_NUM_BONES_WITH_NO_INWARD_ROT 3
#define OVERCOAT_EXIT_SEARCH_STEPS 10
#define OVERCOAT_SUBSTEPS 3
#define OVERCOAT_SUBSTEP_COLLIDER_COUNT 8
#define OVERCOAT_SIDE_BASE_SLACK  0.05f
#define OVERCOAT_SIDE_ROW_SLACK   0.15f


/*
=================
Collider Macros
=================
*/
// Permanent Colliders
// These colliders will run for all animations of Jackie
#define OVERCOAT_LIMB_COLLIDER_RADIUS   0.3f
#define LEFTTHIGH_COLLIDER_DEFAULT_RADIUS 3.9f
#define RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS 3.9f
#define LEFTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS 3.75f
#define RIGHTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS 3.75f
#define LEFTLEG_COLLIDER_DEFAULT_RADIUS 3.3f
#define RIGHTLEG_COLLIDER_DEFAULT_RADIUS 3.3f

// Temperorary Settings for Permanent Colliders
// These radii values are used for existing colliders
#define LEFTTHIGH_COLLIDER_FAST_RIDE_RADIUS 4.f
#define RIGHTTHIGH_COLLIDER_FAST_RIDE_RADIUS 4.f
#define LEFTLEG_COLLIDER_FAST_RIDE_RADIUS 2.f
#define RIGHTLEG_COLLIDER_FAST_RIDE_RADIUS 2.f

// Temporary Colliders
// These colliders only run for specific animations
#define TORSO_COLLIDER_DEFAULT_RADIUS 4.0f
#define TORSO_COLLIDER_SMALL_RADIUS 2.f
#define ROLL_COLLIDER_DEFAULT_RADIUS 4.5f

// Number of NULL pairs in capsule pairs 
#define OVERCOAT_RESERVED_COLLIDER_SLOTS 2


/*
=================
Reset Angle Macros
=================
*/
// Reset presets in degrees
// 0 is directly outward from the torso, positive is up, negative is down.

// Reset at start of animation
#define OVERCOAT_RESET_ANGLE_DEFAULT        -15.0f
#define OVERCOAT_RESET_ANGLE_EPONA_RIDING   30.0f
#define OVERCOAT_RESET_ANGLE_KAMARO         30.0f
#define OVERCOAT_RESET_ANGLE_TRANSFORM      -30.0f
#define OVERCOAT_RESET_ANGLE_SPIN_ATTACK    15.f
#define OVERCOAT_RESET_ANGLE_SPIN_ATTACK_CANCEL     0.f
#define OVERCOAT_RESET_ANGLE_TAKE_GIANT_MASK_OFF    -5.f

// Reset after end of animation
#define OVERCOAT_RESET_ANGLE_ROLL_END           -30.f
#define OVERCOAT_RESET_ANGLE_CLIMB_LEDGE_END    -15.0f


/*
=================
Rotation Angle Limit Macros
=================
*/
// Set the rotational limit of the overcoat chains

// Set rotational limit at start of animation
#define OVERCOAT_DEFAULT_ROT_LIMIT  0.f
#define OVERCOAT_KAMARO_UPPER_ROT_LIMIT 89.f
#define OVERCOAT_KAMARO_LOWER_ROT_LIMIT 89.f
#define OVERCOAT_EPONA_UPPER_ROT_LIMIT 0.f
#define OVERCOAT_EPONA_LOWER_ROT_LIMIT 0.f
#define OVERCOAT_SPIN_ATTACK_UPPER_ROT_LIMIT 89.f
#define OVERCOAT_SPIN_ATTACK_LOWER_ROT_LIMIT 89.f
#define OVERCOAT_TAKE_GIANT_MASK_OFF_UPPER_ROT_LIMIT 89.f
#define OVERCOAT_TAKE_GIANT_MASK_OFF_LOWER_ROT_LIMIT 89.f

// Set rotational limit after end of animation
#define OVERCOAT_ROLL_END_UPPER_ROT_LIMIT -30.f
#define OVERCOAT_ROLL_END_LOWER_ROT_LIMIT -30.f
#define OVERCOAT_CLIMB_LEDGE_END_UPPER_ROT_LIMIT -15.f
#define OVERCOAT_CLIMB_LEDGE_END_LOWER_ROT_LIMIT -15.f


/*
=================
Skip Physics Simulation at Specified Y-Rotation
=================
*/
// Jackie's turns larger than this per frame are partly carried rigidly instead of simulated.
#define OVERCOAT_MAX_YAW	75.f


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
// These macros are set up instead of an enum of overcoat limbs due to only two entries
// of jointtable being relevant, with only one of the two being actually used
#define OVERCOAT_ROOT_POS 0     // Overcoat's Root bone (position)
#define OVERCOAT_ROOT_ROT 1     // Overcoat's Root bone (rotation)


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

static u8 Overcoat_WasTakingGiantMaskOffInPrevFrame = 0;

// For detecting when Jackie does the Kamaro dance
extern LinkAnimationHeader gPlayerAnim_alink_dance_loop;

static u8 Overcoat_WasKamaroDancingInPrevFrame = 0;

// For detecting Jackie's specific animations while riding Epona
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_slowrun;
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_fastrun;

static u8 Overcoat_WasFastRidingInPrevFrame = 0;

// For rolling animation
extern LinkAnimationHeader gPlayerAnim_link_normal_landing_roll_free;
extern LinkAnimationHeader gPlayerAnim_link_normal_landing_roll;
extern LinkAnimationHeader gPlayerAnim_link_fighter_landing_roll_long;

static u8 Overcoat_WasRollingInPrevFrame = 0;   // For tracking if Jackie was rolling in prev frame

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

// For spin attack animation
extern LinkAnimationHeader gPlayerAnim_link_fighter_rolling_kiru;   // 1 handed sword
extern LinkAnimationHeader gPlayerAnim_link_fighter_Lrolling_kiru;  // 2 handed sword

// For spin attack cancel animation
extern LinkAnimationHeader gPlayerAnim_link_fighter_power_kiru_wait_end;    // 1h
extern LinkAnimationHeader gPlayerAnim_link_fighter_Lpower_kiru_wait_end;   // 2h

// For climbing up from ledge
extern LinkAnimationHeader gPlayerAnim_link_normal_fall_up;         // 1h sword in hand
extern LinkAnimationHeader gPlayerAnim_link_normal_fall_up_free;    // no sword in hand

static u8 Overcoat_WasClimbingInPrevFrame  = 0; // For tracking if Jackie was climbing in prev frame

// Epona riding animations
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_walk_muti;
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_slowrun_muti;
extern LinkAnimationHeader gPlayerAnim_link_uma_anim_fastrun_muti;
extern LinkAnimationHeader gPlayerAnim_link_uma_stop_muti;

/********************************
 For Handling Reset AFTER Animations
********************************/
// Set up flags specifically for handling post-animation resets.
// These are post rolling animation and post ledge climb animation.
typedef enum OvercoatPendingReset {
    OVERCOAT_PENDING_RESET_NONE,
    OVERCOAT_PENDING_RESET_ROLL_END,
    OVERCOAT_PENDING_RESET_CLIMB_LEDGE_END
} OvercoatPendingReset;

static OvercoatPendingReset overcoat_pendingResetRotLimitUpdate = OVERCOAT_PENDING_RESET_NONE;


/*
=================
Temp Offsets for Existing Colliders for Specific Anims
=================
*/

// Offsets for back thigh and knee colliders during fast-riding and Epnoa spurring anims
static Vec3f overcoat_fastRideLeftThighOffset = {-550.f, 200.f, -50.f};
static Vec3f overcoat_fastRideRightThighOffset = {-550.f, 200.f, 50.f};
static Vec3f overcoat_fastRideLeftLegOffset = {500.f, 800.f, -1000.f};
static Vec3f overcoat_fastRideRightLegOffset = {500.f, 800.f, 1000.f};


/*
=================
Set up Vertical Cloth Chains
=================
*/
// Define each vertical cloth chain by first phys limb (pinned), chain length (number of phys
// limbs in chain, from first pinned phys limb to last unpinned limb), and first phys bone
// (from root limb to first pinned limb of that chain)
static const OvercoatLimbChain cloth_chains[OVERCOAT_CHAIN_COUNT] = {
    {OVERCOAT_BODYPART_LEFTLIMB0_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB0_A},
    {OVERCOAT_BODYPART_LEFTLIMB1_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB1_A},
    {OVERCOAT_BODYPART_LEFTLIMB2_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB2_A},
    {OVERCOAT_BODYPART_LEFTLIMB3_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB3_A},
    {OVERCOAT_BODYPART_LEFTLIMB4_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB4_A},
    {OVERCOAT_BODYPART_LEFTLIMB5_A,  12, OVERCOAT_BONE_ROOT_LEFTLIMB5_A},
    {OVERCOAT_BODYPART_LEFTLIMB6_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB6_A},
    {OVERCOAT_BODYPART_LEFTLIMB7_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB7_A},
    {OVERCOAT_BODYPART_LEFTLIMB8_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB8_A},
    {OVERCOAT_BODYPART_LEFTLIMB9_A,   9, OVERCOAT_BONE_ROOT_LEFTLIMB9_A},

    {OVERCOAT_BODYPART_BACKLIMB_A,    9, OVERCOAT_BONE_ROOT_BACKLIMB_A},

    {OVERCOAT_BODYPART_RIGHTLIMB9_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB9_A},
    {OVERCOAT_BODYPART_RIGHTLIMB8_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB8_A},
    {OVERCOAT_BODYPART_RIGHTLIMB7_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB7_A},
    {OVERCOAT_BODYPART_RIGHTLIMB6_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB6_A},
    {OVERCOAT_BODYPART_RIGHTLIMB5_A, 12, OVERCOAT_BONE_ROOT_RIGHTLIMB5_A},
    {OVERCOAT_BODYPART_RIGHTLIMB4_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB4_A},
    {OVERCOAT_BODYPART_RIGHTLIMB3_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB3_A},
    {OVERCOAT_BODYPART_RIGHTLIMB2_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB2_A},
    {OVERCOAT_BODYPART_RIGHTLIMB1_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB1_A},
    {OVERCOAT_BODYPART_RIGHTLIMB0_A,  9, OVERCOAT_BONE_ROOT_RIGHTLIMB0_A}
};


/*
=================
Colliders
=================
*/
// Left thigh colliders
PhysSphereCollider overcoat_leftThighBackCollider = {
    {0.f, 0.f, 0.f}, 
    LEFTTHIGH_COLLIDER_DEFAULT_RADIUS
};
PhysSphereCollider overcoat_leftThighFrontCollider = {
    {0.f, 0.f, 0.f}, 
    LEFTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS
};

// Right thigh colliders
PhysSphereCollider overcoat_rightThighBackCollider = {
    {0.f, 0.f, 0.f}, 
    RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS
};
PhysSphereCollider overcoat_rightThighFrontCollider = {
    {0.f, 0.f, 0.f}, 
    RIGHTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS
};

// Left leg collider
PhysSphereCollider overcoat_leftLegCollider = {
    {0.f, 0.f, 0.f}, 
    LEFTLEG_COLLIDER_DEFAULT_RADIUS
};

// Right leg collider
PhysSphereCollider overcoat_rightLegCollider = {
    {0.f, 0.f, 0.f}, 
    RIGHTLEG_COLLIDER_DEFAULT_RADIUS
};

// Torso colliders
PhysSphereCollider overcoat_torsoCollider = {
    {0.f, 0.f, 0.f},
    TORSO_COLLIDER_DEFAULT_RADIUS
};
// Temporary collider only used for rolling or forward stab animation
PhysSphereCollider overcoat_rollCollider = {
    {0.f, 0.f, 0.f},
    ROLL_COLLIDER_DEFAULT_RADIUS
};


/*
=================
Positions and Directions
=================
*/
// Torso values for setting root for Overcoat's model and torso sphere collider
// World position of player's torso limb
Vec3f overcoat_torso_globalPos = { (f32)0, (f32)0, (f32)0 };    
// Rotation value for player's torso (with parent limbs' rotations included)     
Vec3s overcoat_torso_rotate = {0, 0, 0};


// Overcoat's current directional axes 
static Vec3f overcoat_forwardAxis = {0.f, 0.f, 0.f};
static Vec3f overcoat_leftAxis = {0.f, 0.f, 0.f};
static Vec3f overcoat_downAxis = {0.f, 0.f, 0.f};
static u8 overcoat_dirAxesReady = 0;


// Actual hip and knee positions, without collider offsets.
// Index 0 is left and 1 is right.
static Vec3f overcoat_thighJointPos[2];
static Vec3f overcoat_kneeJointPos[2];

// Identify a complete set of joint positions from the current frame.
static u32 overcoat_legJointFrame = 0;
static u8 overcoat_legJointMask = 0;

/*
=================
Positions and Directions
=================
*/
// Previous frame state values used for substep calculations
static u8 overcoat_stepHistoryReady = 0;
static PhysSphereCollider overcoat_prevFrameColliders[OVERCOAT_SUBSTEP_COLLIDER_COUNT];
static Vec3f overcoat_previousFrameForward;
static Vec3f overcoat_previousFrameDown;
static f32 overcoat_previousSubstepScale;

static s16 overcoat_previousFrameYaw = 0;


// Recomp utilities
RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));


/***********************************************************************

	Overcoat Physics

***********************************************************************/

/*
=================
Set Overcoat as Custom Actor
=================
*/
// Sets profile for overcoat before registering it as actor
ActorProfile Overcoat_Profile = {
    ACTOR_ID_MAX,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(Overcoat),
    Overcoat_Init,
    Overcoat_Destroy,
    Overcoat_Update,
    Overcoat_Draw,
};

s16 CUSTOM_ACTOR_OVERCOAT = ACTOR_ID_MAX;


/**
 * @brief Register overcoat as custom actor
 */
RECOMP_CALLBACK("*", recomp_on_init) void Overcoat_OnRecompInit() {
    CUSTOM_ACTOR_OVERCOAT = CustomActor_Register(&Overcoat_Profile);
}



/*
=================
Player Init
=================
*/

Overcoat* gPlayerOvercoat = NULL;
PlayState* gPlayStateOvercoat = NULL;

/**
 * @brief Create custom actor for overcoat on player_init
 * 
 * @param thisx     Actor pointer
 * @param play      Current playstate
 */
RECOMP_HOOK("Player_Init") void Overcoat_on_player_init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_outfit") && gPlayerOvercoat == NULL) {
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, CUSTOM_ACTOR_OVERCOAT, 
                                    -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0,  0, 0);
    }
}


/**
 * @brief Initialize one vertical cloth chain of phys limbs of overcoat
 * 
 * This function sets the starting world position and physics state of each phys limb in one vertical
 * chain of phys limbs of the overcoat.
 *
 * Keep note that the first phys limb (limbA of every chain) is connected to the same root limb of the
 * overcoat. This first phys limb and the root limb are pinned. The rest of the phys limbs hanging down
 * from the first phys limb are unpinned so that they can move via Verlet Integration.
 *
 * This function also initializes a phys bone using the current phys limb and its parent phys limb.
 *
 * @param this			Overcoat actor
 * @param player		Player actor used for its rotation and velocity
 * @param gPhysLimbs	Array of overcoat's phys limbs
 * @param gPhysBones	Array of overcoat's phys bones
 * @param startLimb		Index of starting limb of current vertical chain of phys limbs of overcoat
 * @param limbCount		Number of phys limbs in the current vertical chain
 * @param startBone		Index of first phys bone of this current vertical chain
 */
void Overcoat_InitChain(Overcoat* this, Player* player, PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[],
    int startLimb, int limbCount, int startBone) {
	// Set up zero velocity vector to apply to unpinned phys limbs
    Vec3f noVelocity = {0.f, 0.f, 0.f};
	
	// Iterate through each row of vertical phys limbs
    for (int physLimbRow = 0; physLimbRow < limbCount; physLimbRow++) {
		// Get current phyx limb index based on starting phys limb  index and current phys limb row
        int limbIdx = startLimb + physLimbRow;

		// Calculate where phys limb should be in respect to world space
        Vec3s rotatedOffset = {0, 0, 0};
        Vec3f transformVec = {0.f, 0.f, 0.f};
		
		// First rotate the distance/offset between current phys limb and parent limb and rotate it by
		// actor's rotation in world.
		// Note: default_jointPos describes the distance of current phys limb from its parent phys limb
        CustomMath_Vec3s_Rotate(&gPhysLimbs[limbIdx]->default_jointPos, &player->actor.shape.rot,
            &rotatedOffset);
		// Scale the rotated distance/offset from parent limb to curreent limb based on overcoat's
		// current scale.
        CustomMath_Vec3s_Scale_ToVec3f(&rotatedOffset, this->actor.scale.x, &transformVec);

        // LimbA (first row of phys limbs) starts from the shared root. Every other PhysLimb starts from 
		// the previous PhysLimb.
		int parentIdx;
		
		if (physLimbRow == 0) {
			// The first phys limb in the chain is limbA, so its parent is a root limb shared by other
			// pinned phys limbs
			parentIdx = OVERCOAT_BODYPART_ROOT;
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
				limbMass, PINNED, OVERCOAT_LIMB_COLLIDER_RADIUS);
		}
        // Every other phys limbs below the first row of phys limbs are unpinned and have no velocity
		else {
			Verlet_InitLimb(gPhysLimbs[limbIdx], this->bodyPartsPos[limbIdx], noVelocity,
				limbMass, NOT_PINNED, OVERCOAT_LIMB_COLLIDER_RADIUS);
		}

        // Create phys bone out of current phys limb and previous/parent phys limb
        Verlet_InitBone(gPhysBones[startBone + physLimbRow], gPhysLimbs[parentIdx], gPhysLimbs[limbIdx]);
    }
}


/**
 * @brief Initialize overcoat's actor, root, phys limbs, and phys bones.
 *
 * This function sets up the overcoat's starting physics state using Jackie's current torso position,
 * rotation, velocity, and actor scale.
 *
 * The function also sets up vertical cloth chains consisting of phys limbs and phys bones. Each cloth
 * chain begins with one pinned phys limb at row A, while the remaining phys limbs hanging below it
 * are unpinned and can move via Verlet Integration.
 *
 * This function runs when the overcoat actor is to be spawned and rendered, like when entering a-
 * map or transforming into human form.
 *
 * @param this			Overcoat actor
 * @param player		Player actor to get position, rotation, velocity, and scale
 * @param gPhysLimbs	Array of overcoat's phys limbs
 * @param gPhysBones	Array of overcoat's phys bones
 */
void Overcoat_SetDefaultBodyPartsPos(Overcoat* this, Player* player, PhysLimb* gPhysLimbs[], 
	PhysBone* gPhysBones[]) {
    // Set overcoat velocity based on player's velocity
    Vec3f playerVelocity = player->actor.velocity;

	/********************************
     Set up overcoat actor
    ********************************/
    // Assign Jackie's torso's world position to the overcoat's actor's world position
    Math_Vec3f_Copy(&this->actor.world.pos, &player->bodyPartsPos[PLAYER_BODYPART_TORSO]);
	// Assign Jackie's shape rotation to the overcoat's shape rotation
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);
	// Assign Jackie's world rotation to the overcoat's world's rotation
    Math_Vec3s_Copy(&this->actor.world.rot, &player->actor.world.rot);

	/********************************
     Set up root phys limb
    ********************************/
    // Root limb's BodyPartsPos and gPhysLimb positions and velocity
    // Velocity only gets assigned to root limb
    // BodyPartsPos keeps track of global XYZ position of each limb
    Math_Vec3f_Copy(&this->bodyPartsPos[OVERCOAT_BODYPART_ROOT], 
		&player->bodyPartsPos[PLAYER_BODYPART_TORSO]);
	
	// Initailize the root limb of overcoat as pinned phys limb
    Verlet_InitLimb(gPhysLimbs[OVERCOAT_BODYPART_ROOT], this->actor.world.pos, playerVelocity, 
		LIMB_MASS, PINNED, OVERCOAT_LIMB_COLLIDER_RADIUS);

	/********************************
     Set up root skeleton values
    ********************************/
    // Store root world position.
    Vec3s rootPos_Vec3s = {0, 0, 0};
    Math_Vec3f_ToVec3s(&rootPos_Vec3s, &this->actor.world.pos);
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &rootPos_Vec3s);
    Math_Vec3s_Copy(&gPhysLimbs[OVERCOAT_BODYPART_ROOT]->default_jointPos, &rootPos_Vec3s);

    // Start root rotation at zero.
    Vec3s newRootJointRot = { 0, 0, 0};
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_ROT], &newRootJointRot);

	// Initialize every vertical cloth chain and its corresponding phys bones.
    for (int i = 0; i < OVERCOAT_CHAIN_COUNT; i++) {
        Overcoat_InitChain(this, player, gPhysLimbs, gPhysBones, cloth_chains[i].startLimb,
            cloth_chains[i].limbCount, cloth_chains[i].startBone);
    }
}


/**
 * @brief Check if Jackie is riding Epona at fast speed or spurring Epona
 * 
 * This function checks Jackie's current animation while riding Epona and checks if any anim related
 * to riding Epona fast or spurring Epona is currently playing.
 * 
 * This function is primarily used to change the leg colliders' positions and sizes based on the
 * current animation of Jackie while on Epona. Because the adjusted positions and sizes of the colliders
 * are planned to be the same for both cases (fast riding and spurring), there is only one flag for
 * both cases.
 * 
 * This function is an example of marking the current playing animation with a flag to help codebase
 * changing the collider's position or radius when certain animation(s) is playing.
 * 
 * @param player    Current player instance
 */
u8 Overcoat_IsFastRidingOrSpurring(Player* player) {
    /********************************
     Detect fast riding
    ********************************/
    // If Jackie is not riding on Epona (PLAYER_STATE1_800000), then return 0 (false)
    if (!(player->stateFlags1 & PLAYER_STATE1_800000)) {
        return 0;
    }

    // If Jackie is riding Epona fast, then return 1 (true)
    if (player->skelAnime.animation == &gPlayerAnim_link_uma_anim_fastrun) {
        return 1;
    }


    /********************************
     Detect spurring
    ********************************/
    // Check if Jackie is spurring Epona currently.
    // This flag is mostly important due to the way the upper body's animation pointers work.
    // Normally, only the next if case would be necessary to catch upper body animations that involves
    // spurring Epona. However, I think the upper body animation pointer is still pointing at a spurring
    // animation even when the spurring has ended.
    // When riding Epona, Player_Action_52, which is the riding action, indicates spurring by setting
    // player->av1.actionVar1 to 1. However, it is set to 0 when spurring is not occuring. This is the
    // most accurate way to check if the spurring is occuring or not.
    if (player->av1.actionVar1 == 0) {
        return 0;
    }

    // Jackie is spurring Epona at this point. Check if any of these animations are playing while Jackie
    // is spurring Epona and return 1 (true) if this is happening.
    if (player->skelAnimeUpper.animation == &gPlayerAnim_link_uma_anim_walk_muti ||
        player->skelAnimeUpper.animation == &gPlayerAnim_link_uma_anim_slowrun_muti ||
        player->skelAnimeUpper.animation == &gPlayerAnim_link_uma_anim_fastrun_muti ||
        player->skelAnimeUpper.animation == &gPlayerAnim_link_uma_stop_muti) {
        return 1;
    }

    // If none of the animations are matches while Jackie is spurring, then return 0 (false)
    return 0;
}


/**
 * @brief Check whether Jackie is performing the Kamaro dance.
 * 
 * The function is primarily used to change a torso collider's size during the duration of the Kamaro
 * dance. This function is also used to change the reset and rotation limit of the overcoat chains
 * during the duration of the Kamaro dance.
 * 
 * @param player    Player instance
 */
u8 Overcoat_IsKamaroDancing(Player* player) {
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
 * This function is primarily used to indicate if the overcoat chains need to be reset.
 * 
 * @param play      Current playstate
 * @param player    Player instance
 */
u8 Overcoat_IsInFirstPerson(PlayState* play, Player* player) {
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
 * @brief Check whether Jackie is currently rolling.
 * 
 * This function is used to add a torso collider size during the rolling animation.
 * 
 * This function also is used to later reset the overcoat chains if the roll animation ends, which 
 * serves as an example for performing resets when an animation ends rather than when an animation 
 * begins.
 * 
 * @param player    Player instance
 */
u8 Overcoat_IsRolling(Player* player) {
    return (player->skelAnime.animation == &gPlayerAnim_link_normal_landing_roll_free || 
            player->skelAnime.animation == &gPlayerAnim_link_normal_landing_roll ||
            player->skelAnime.animation == &gPlayerAnim_link_fighter_landing_roll_long);
}


/**
 * @brief Check whether Jackie is currently climbing from ledge.
 * 
 * This function also is used to later reset the overcoat chains if the roll animation ends, which 
 * serves as an example for performing resets when an animation ends rather than when an animation 
 * begins.
 * 
 * @param player    Player instance
 */
u8 Overcoat_IsClimbingFromLedge(Player* player) {
    return (player->skelAnime.animation == &gPlayerAnim_link_normal_fall_up ||
            player->skelAnime.animation == &gPlayerAnim_link_normal_fall_up_free);
}


/**
 * @brief Check whether Jackie is currently doing a forward stab (for both 1h and 2h swords)
 * 
 * This function is used to add a torso collider size during the stab animation.
 * 
 * @param player    Player instance
 */
u8 Overcoat_IsForwardStabbing(Player* player) {
    static LinkAnimationHeader* const stabAnimations[] = {
        // One-handed stab and recovery.
        &gPlayerAnim_link_fighter_pierce_kiru,
        &gPlayerAnim_link_fighter_pierce_kiru_end,
        &gPlayerAnim_link_anchor_pierce_kiru_endR,

        // One-handed stab combo and recovery.
        &gPlayerAnim_link_fighter_pierce_kiru_finsh,
        &gPlayerAnim_link_fighter_pierce_kiru_finsh_end,
        &gPlayerAnim_link_anchor_pierce_kiru_finsh_endR,

        // Two-handed stab and recovery.
        &gPlayerAnim_link_fighter_Lpierce_kiru,
        &gPlayerAnim_link_fighter_Lpierce_kiru_end,
        &gPlayerAnim_link_anchor_Lpierce_kiru_endL,

        // Two-handed stab combo and recovery.
        &gPlayerAnim_link_fighter_Lpierce_kiru_finsh,
        &gPlayerAnim_link_fighter_Lpierce_kiru_finsh_end,
        &gPlayerAnim_link_anchor_Lpierce_kiru_finsh_endR
    };

    int animationCount = sizeof(stabAnimations) / sizeof(stabAnimations[0]);

    for (int i = 0; i < animationCount; i++) {
        if (player->skelAnime.animation == stabAnimations[i]) {
            return 1;
        }
    }

    return 0;
}


/**
 * @brief Check whether Jackie is currently doing a spin attack.
 * 
 * The function is primarily used to change a torso collider's size during the duration of the Kamaro
 * dance. This function is also used to change the reset and rotation limit of the overcoat chains 
 * during the duration of the Kamaro dance.
 * 
 * @param player    Player instance
 */
u8 Overcoat_IsSpinAttacking(Player* player) {
    return ((player->skelAnime.animation == &gPlayerAnim_link_fighter_rolling_kiru ||
             player->skelAnime.animation == &gPlayerAnim_link_fighter_Lrolling_kiru));
}


/**
 * @brief Check whether Jackie is currently taking off transformation mask.
 * 
 * This function resets the overcoat chains at the beginning part of the animation where Jackie
 * takes off the Giant's Mask, specifically, since the Giant's Mask is not considered a nonhuman-to-
 * human transformation.
 * 
 * @param player    Player instance
 */
u8 Overcoat_IsTakingGiantMaskOff(Player* player) {
    return (player->skelAnime.animation == &gPlayerAnim_cl_maskoff);
}


/**
 * @brief Check whether Jackie is currently canceling a spin attack and returning to idle.
 * 
 * The function is primarily used to change a torso collider's size during the duration of the Kamaro
 * dance. This function is also used to change the reset and rotation limit of the overcoat chains 
 * during the duration of the Kamaro dance.
 * 
 * @param player    Player instance
 */
u8 Overcoat_IsSpinAttackCanceling(Player* player) {
    return ((player->skelAnime.animation == &gPlayerAnim_link_fighter_power_kiru_wait_end ||
             player->skelAnime.animation == &gPlayerAnim_link_fighter_Lpower_kiru_wait_end));
}


/**
 * @brief Set the default rotational limit for how much upward overcoat's chains of phys limbs can go
 *
 * This function sets the default upward limit for upper and lower overcoat phys limbs in overcoat's 
 * chains of phys limbs.
 * 
 * @param this  Overcoat actor
 */
void Overcoat_SetDefaultRotLimitsAndReset(Overcoat* this) {
    // 0 is directly outward (horizontal) from the torso, positive is upward, negative is downward
    s16 defaultUpwardLimAngle = CustomMath_DegreeToBinaryAngle(OVERCOAT_DEFAULT_ROT_LIMIT);

    // Set rotation limits for the overcoat chains
    this->rotLimits = (OvercoatRotationSettings){
        // Set default reset angle. This rotation value  will be applied to overcoat chains when the
        // reset function is called.
        .resetAngle = OVERCOAT_RESET_ANGLE_DEFAULT,

        // Store the default rotation angle limit for both upper limbs and lower limbs of overcoat 
        // chain. Whenever current limit needs to be reset to default rotation limits, these default
        // values will be used.
        .defUpperChainLimAngle = defaultUpwardLimAngle,
        .defLowerChainLimAngle = defaultUpwardLimAngle,

        // Set the current rotation limit for both upper limbs and lower limbs of overcoat chain. 
        .currUpperChainLimAngle = defaultUpwardLimAngle,
        .currLowerChainLimAngle = defaultUpwardLimAngle,

        // Set the number of upper bones of the overcoat chains that will not have an inward 
        // rotation. Set to 0 if this feature is to not be used.
        .numTopBonesWithNoInwardRot = OVERCOAT_NUM_BONES_WITH_NO_INWARD_ROT
    };
}


/**
 * @brief Set reset angle for overcoat chains with a given rotation angle
 * 
 * @param this          Overcoat actor
 * @param reset_angle   Reset angle
 */
void Overcoat_SetResetAngle(Overcoat* this, f32 reset_angle) {
    this->rotLimits.resetAngle = reset_angle;
}


/**
 * @brief Set reset angle for overcoat chains with a given rotation angle
 * 
 * @param this              Overcoat actor
 * @param upper_chain_lim   Rotation limit for upper chains of phys limbs in overcoat actor
 * @param lower_chain_lim   Rotation limit for lower chains of phys limbs in overcoat actor
 */
void Overcoat_SetRotLimitAngles(Overcoat* this, f32 upper_chain_lim, f32 lower_chain_lim) {
    // Update rotation limit angles for upper and lower limbs of Overcoat chains.
		this->rotLimits.currUpperChainLimAngle = CustomMath_DegreeToBinaryAngle(upper_chain_lim);
        this->rotLimits.currLowerChainLimAngle = CustomMath_DegreeToBinaryAngle(lower_chain_lim);
}


/**
 * @brief Update the overcoat's chains of phys limbs' upward rotational limit and reset angle.
 *
 * This function checks the current animation and updates two components for the overcoat's chains of 
 * phys limbs: 
 *
 *	1. Upward rotational limit of overcoat's chains
 *
 *	2. Reset angle of overcoat's chains
 *
 * There are two ways this function checks the animation for resetting/changing rotation limits:
 * 
 *  a. At the start of a specific animation
 * 
 *  b. After the end of a specific animation.
 * 
 * @param this      Overcoat actor
 * @param player    Player instance
 */
void Overcoat_UpdateRotLimitsAndReset(Overcoat* this, Player* player) {
	// Restore normal limits and reset angle before applying animation-based changes to limit and reset.
    Overcoat_SetDefaultRotLimitsAndReset(this);
	
	// Check if player is human first.
	if (player->transformation != PLAYER_FORM_HUMAN) {
		return;
	}
	
    /********************************
     Reset and set new rotation limits at START of target animations
    ********************************/
	// Update limit of Jackie's overcoat chains during Kamaro dance.
	if (Overcoat_IsKamaroDancing(player)) {
        // Update reset angle for Epona fast ride. This only runs at first frame of animation.
        Overcoat_SetResetAngle(this, OVERCOAT_RESET_ANGLE_KAMARO);

		// Update rotation limit angles for upper and lower limbs of Overcoat chains.
        Overcoat_SetRotLimitAngles(this, 
            OVERCOAT_KAMARO_UPPER_ROT_LIMIT, OVERCOAT_KAMARO_LOWER_ROT_LIMIT);
	}

    // Update limit of Jackie's overcoat chains during spin attack.
    else if (Overcoat_IsSpinAttacking(player)) {
        // Update rotation limit angles for upper and lower limbs of Overcoat chains.
        Overcoat_SetRotLimitAngles(this,
            OVERCOAT_SPIN_ATTACK_UPPER_ROT_LIMIT, OVERCOAT_SPIN_ATTACK_LOWER_ROT_LIMIT);
    }

    // Update limit of Jackie's overcoat chains during spin attack cancel.
    else if (Overcoat_IsSpinAttackCanceling(player)) {
        // Update rotation limit angles for upper and lower limbs of Overcoat chains.
        Overcoat_SetRotLimitAngles(this,
            OVERCOAT_SPIN_ATTACK_UPPER_ROT_LIMIT, OVERCOAT_SPIN_ATTACK_LOWER_ROT_LIMIT);
    }
	
	// Update limit and reset of Jackie's overcoat chains during Epona fast ride.
	else if (player->skelAnime.animation == &gPlayerAnim_link_uma_anim_fastrun) {
		// Update reset angle for Epona fast ride. This only runs at first frame of animation.
        Overcoat_SetResetAngle(this, OVERCOAT_RESET_ANGLE_EPONA_RIDING);

		// Update rotation limit angles for upper and lower limbs of Overcoat chains.
        Overcoat_SetRotLimitAngles(this,
            OVERCOAT_EPONA_UPPER_ROT_LIMIT, OVERCOAT_EPONA_LOWER_ROT_LIMIT);
	}

    // Update limit and reset of Jackie's overcoat chain's when taking Giant's mask off.
    else if (Overcoat_IsTakingGiantMaskOff(player)) {
        // Update reset angle for taking Giant's Mask off. This only runs at first frame of animation.
        Overcoat_SetResetAngle(this, OVERCOAT_RESET_ANGLE_TAKE_GIANT_MASK_OFF);

        // Update rotation limit angles for upper and lower limbs of Overcoat chains.
        Overcoat_SetRotLimitAngles(this,
            OVERCOAT_TAKE_GIANT_MASK_OFF_LOWER_ROT_LIMIT, OVERCOAT_TAKE_GIANT_MASK_OFF_UPPER_ROT_LIMIT);
    }

    /********************************
     Reset and set new rotation limits after END of target animations
    ********************************/
    // Overcoat_Update() enables which reset is pending after its respective animation ends.
    // Update limit and reset of Jackie's overcoat chains AFTER roll animation ends.
    if (overcoat_pendingResetRotLimitUpdate == OVERCOAT_PENDING_RESET_ROLL_END) {
        // Update reset angle after roll ends.
        Overcoat_SetResetAngle(this, OVERCOAT_RESET_ANGLE_ROLL_END);

        // Update rotation limit angles for upper and lower limbs of Overcoat chains.
        Overcoat_SetRotLimitAngles(this,
            OVERCOAT_ROLL_END_UPPER_ROT_LIMIT, OVERCOAT_ROLL_END_LOWER_ROT_LIMIT);
    }
    // Update limit and reset of Jackie's overcoat chains AFTER ledge climb animation ends.
    else if (overcoat_pendingResetRotLimitUpdate == OVERCOAT_PENDING_RESET_CLIMB_LEDGE_END) {
        // Update reset angle after ledge climb animation ends.
        Overcoat_SetResetAngle(this, OVERCOAT_RESET_ANGLE_CLIMB_LEDGE_END);

        // Update rotation limit angles for upper and lower limbs of Overcoat chains.
        Overcoat_SetRotLimitAngles(this,
            OVERCOAT_CLIMB_LEDGE_END_UPPER_ROT_LIMIT, OVERCOAT_CLIMB_LEDGE_END_LOWER_ROT_LIMIT);
    }
}


/**
 * @brief Initialize overcoat model and actor along with its limbs
 * 
 * This function initializes the overcoat model and its custom actor. When doing this, the function also takes
 * the standardLimbs' positions of the overcoat model (gOvercoatSkel.c) generated via Fast64. These positions 
 * are then stored into the phys limbs, which are used for Verlet Integration.
 *
 * @param thisx		Actor
 * @param play		Current playstate
 */
void Overcoat_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_outfit")) {
		// Initialize overcoat's states
        Player* player = GET_PLAYER(play);
        Overcoat* this = (Overcoat*)thisx;
        this->actor.room = -1;
        this->needsReset = 1;
        this->wasTransformingIntoGiant = 0;
        this->wasGiant = 0;
        this->wasKamaroDancing = 0;

        // Global variable settings regarding resets or rotation limits
        Overcoat_WasTakingGiantMaskOffInPrevFrame = 0;
        Overcoat_WasKamaroDancingInPrevFrame = 0;
        Overcoat_WasFastRidingInPrevFrame = 0;
        Overcoat_WasRollingInPrevFrame = 0;
        Overcoat_WasClimbingInPrevFrame = 0;
        overcoat_pendingResetRotLimitUpdate = OVERCOAT_PENDING_RESET_NONE;

        // Initialize with default reset and rotation angle limits.
        Overcoat_UpdateRotLimitsAndReset(this, player);

        // Initialize pointer to overcoat actor and playstate.
        gPlayerOvercoat = this;
        gPlayStateOvercoat = play;

        // Capture default jointPos
        // I can't think of any way aside from hardcoding, because the standardlimbs' jointPos values
        // keep getting updated based on its last jointPos values before map reloads.
        // Values are directly copied from the generated Fast64 model file (in my case, gOvercoatSkel.c)
        Vec3s backlimb_a_jointPos = { 0, -32, -319 };
        Vec3s backlimb_b_jointPos = { 0, 0, -224 };
        Vec3s backlimb_c_jointPos = { 0, 0, -223 };
        Vec3s backlimb_d_jointPos = { 0, 0, -223 };
        Vec3s backlimb_e_jointPos = { 0, 0, -224 };
        Vec3s backlimb_f_jointPos = { 0, 0, -224 };
        Vec3s backlimb_g_jointPos = { 0, 0, -224 };
        Vec3s backlimb_h_jointPos = { 0, 0, -224 };
        Vec3s backlimb_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb0_a_jointPos = { -144, -32, 317 };
        Vec3s leftlimb0_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb0_c_jointPos = { 0, 0, -223 };
        Vec3s leftlimb0_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb0_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb0_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb0_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb0_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb0_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb1_a_jointPos = { 168, -31, 335 };
        Vec3s leftlimb1_b_jointPos = { 0, 0, -223 };
        Vec3s leftlimb1_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb1_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb1_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb1_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb1_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb1_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb1_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_a_jointPos = { 225, -32, 240 };
        Vec3s leftlimb2_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb2_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_a_jointPos = { 282, -32, 145 };
        Vec3s leftlimb3_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb3_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_a_jointPos = { 338, -32, 51 };
        Vec3s leftlimb4_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb4_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_a_jointPos = { 395, -32, -44 };
        Vec3s leftlimb5_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_j_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_k_jointPos = { 0, 0, -224 };
        Vec3s leftlimb5_l_jointPos = { 0, 0, -188 };
        Vec3s leftlimb6_a_jointPos = { 335, -32, -139 };
        Vec3s leftlimb6_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb6_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb6_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb6_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb6_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb6_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb6_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb6_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_a_jointPos = { 275, -32, -236 };
        Vec3s leftlimb7_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb7_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_a_jointPos = { 218, -32, -325 };
        Vec3s leftlimb8_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb8_i_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_a_jointPos = { 107, -31, -322 };
        Vec3s leftlimb9_b_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_c_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_d_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_e_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_f_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_g_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_h_jointPos = { 0, 0, -224 };
        Vec3s leftlimb9_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_a_jointPos = { 144, -31, 335 };
        Vec3s rightlimb0_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb0_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_a_jointPos = { -168, -31, 335 };
        Vec3s rightlimb1_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb1_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_a_jointPos = { -225, -32, 240 };
        Vec3s rightlimb2_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb2_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_a_jointPos = { -282, -32, 145 };
        Vec3s rightlimb3_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb3_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_a_jointPos = { -338, -32, 51 };
        Vec3s rightlimb4_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb4_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_a_jointPos = { -395, -32, -44 };
        Vec3s rightlimb5_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_j_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_k_jointPos = { 0, 0, -224 };
        Vec3s rightlimb5_l_jointPos = { 0, 0, -188 };
        Vec3s rightlimb6_a_jointPos = { -335, -32, -139 };
        Vec3s rightlimb6_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb6_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb6_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb6_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb6_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb6_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb6_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb6_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_a_jointPos = { -275, -32, -236 };
        Vec3s rightlimb7_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb7_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_a_jointPos = { -218, -32, -325 };
        Vec3s rightlimb8_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb8_i_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_a_jointPos = { -107, -31, -322 };
        Vec3s rightlimb9_b_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_c_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_d_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_e_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_f_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_g_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_h_jointPos = { 0, 0, -224 };
        Vec3s rightlimb9_i_jointPos = { 0, 0, -224 };
        
        // default_jointPos values for every limb should NEVER change once assigned here
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_A]->default_jointPos, &backlimb_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_B]->default_jointPos, &backlimb_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_C]->default_jointPos, &backlimb_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_D]->default_jointPos, &backlimb_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_E]->default_jointPos, &backlimb_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_F]->default_jointPos, &backlimb_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_G]->default_jointPos, &backlimb_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_H]->default_jointPos, &backlimb_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_BACKLIMB_I]->default_jointPos, &backlimb_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_A]->default_jointPos, &leftlimb0_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_B]->default_jointPos, &leftlimb0_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_C]->default_jointPos, &leftlimb0_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_D]->default_jointPos, &leftlimb0_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_E]->default_jointPos, &leftlimb0_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_F]->default_jointPos, &leftlimb0_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_G]->default_jointPos, &leftlimb0_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_H]->default_jointPos, &leftlimb0_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB0_I]->default_jointPos, &leftlimb0_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_A]->default_jointPos, &leftlimb1_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_B]->default_jointPos, &leftlimb1_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_C]->default_jointPos, &leftlimb1_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_D]->default_jointPos, &leftlimb1_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_E]->default_jointPos, &leftlimb1_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_F]->default_jointPos, &leftlimb1_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_G]->default_jointPos, &leftlimb1_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_H]->default_jointPos, &leftlimb1_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB1_I]->default_jointPos, &leftlimb1_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_A]->default_jointPos, &leftlimb2_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_B]->default_jointPos, &leftlimb2_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_C]->default_jointPos, &leftlimb2_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_D]->default_jointPos, &leftlimb2_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_E]->default_jointPos, &leftlimb2_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_F]->default_jointPos, &leftlimb2_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_G]->default_jointPos, &leftlimb2_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_H]->default_jointPos, &leftlimb2_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB2_I]->default_jointPos, &leftlimb2_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_A]->default_jointPos, &leftlimb3_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_B]->default_jointPos, &leftlimb3_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_C]->default_jointPos, &leftlimb3_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_D]->default_jointPos, &leftlimb3_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_E]->default_jointPos, &leftlimb3_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_F]->default_jointPos, &leftlimb3_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_G]->default_jointPos, &leftlimb3_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_H]->default_jointPos, &leftlimb3_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB3_I]->default_jointPos, &leftlimb3_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_A]->default_jointPos, &leftlimb4_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_B]->default_jointPos, &leftlimb4_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_C]->default_jointPos, &leftlimb4_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_D]->default_jointPos, &leftlimb4_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_E]->default_jointPos, &leftlimb4_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_F]->default_jointPos, &leftlimb4_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_G]->default_jointPos, &leftlimb4_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_H]->default_jointPos, &leftlimb4_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB4_I]->default_jointPos, &leftlimb4_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_A]->default_jointPos, &leftlimb5_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_B]->default_jointPos, &leftlimb5_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_C]->default_jointPos, &leftlimb5_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_D]->default_jointPos, &leftlimb5_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_E]->default_jointPos, &leftlimb5_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_F]->default_jointPos, &leftlimb5_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_G]->default_jointPos, &leftlimb5_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_H]->default_jointPos, &leftlimb5_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_I]->default_jointPos, &leftlimb5_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_J]->default_jointPos, &leftlimb5_j_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_K]->default_jointPos, &leftlimb5_k_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB5_L]->default_jointPos, &leftlimb5_l_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_A]->default_jointPos, &leftlimb6_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_B]->default_jointPos, &leftlimb6_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_C]->default_jointPos, &leftlimb6_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_D]->default_jointPos, &leftlimb6_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_E]->default_jointPos, &leftlimb6_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_F]->default_jointPos, &leftlimb6_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_G]->default_jointPos, &leftlimb6_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_H]->default_jointPos, &leftlimb6_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB6_I]->default_jointPos, &leftlimb6_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_A]->default_jointPos, &leftlimb7_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_B]->default_jointPos, &leftlimb7_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_C]->default_jointPos, &leftlimb7_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_D]->default_jointPos, &leftlimb7_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_E]->default_jointPos, &leftlimb7_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_F]->default_jointPos, &leftlimb7_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_G]->default_jointPos, &leftlimb7_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_H]->default_jointPos, &leftlimb7_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB7_I]->default_jointPos, &leftlimb7_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_A]->default_jointPos, &leftlimb8_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_B]->default_jointPos, &leftlimb8_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_C]->default_jointPos, &leftlimb8_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_D]->default_jointPos, &leftlimb8_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_E]->default_jointPos, &leftlimb8_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_F]->default_jointPos, &leftlimb8_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_G]->default_jointPos, &leftlimb8_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_H]->default_jointPos, &leftlimb8_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB8_I]->default_jointPos, &leftlimb8_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_A]->default_jointPos, &leftlimb9_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_B]->default_jointPos, &leftlimb9_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_C]->default_jointPos, &leftlimb9_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_D]->default_jointPos, &leftlimb9_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_E]->default_jointPos, &leftlimb9_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_F]->default_jointPos, &leftlimb9_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_G]->default_jointPos, &leftlimb9_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_H]->default_jointPos, &leftlimb9_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_LEFTLIMB9_I]->default_jointPos, &leftlimb9_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_A]->default_jointPos, &rightlimb0_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_B]->default_jointPos, &rightlimb0_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_C]->default_jointPos, &rightlimb0_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_D]->default_jointPos, &rightlimb0_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_E]->default_jointPos, &rightlimb0_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_F]->default_jointPos, &rightlimb0_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_G]->default_jointPos, &rightlimb0_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_H]->default_jointPos, &rightlimb0_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB0_I]->default_jointPos, &rightlimb0_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_A]->default_jointPos, &rightlimb1_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_B]->default_jointPos, &rightlimb1_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_C]->default_jointPos, &rightlimb1_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_D]->default_jointPos, &rightlimb1_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_E]->default_jointPos, &rightlimb1_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_F]->default_jointPos, &rightlimb1_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_G]->default_jointPos, &rightlimb1_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_H]->default_jointPos, &rightlimb1_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB1_I]->default_jointPos, &rightlimb1_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_A]->default_jointPos, &rightlimb2_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_B]->default_jointPos, &rightlimb2_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_C]->default_jointPos, &rightlimb2_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_D]->default_jointPos, &rightlimb2_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_E]->default_jointPos, &rightlimb2_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_F]->default_jointPos, &rightlimb2_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_G]->default_jointPos, &rightlimb2_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_H]->default_jointPos, &rightlimb2_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB2_I]->default_jointPos, &rightlimb2_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_A]->default_jointPos, &rightlimb3_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_B]->default_jointPos, &rightlimb3_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_C]->default_jointPos, &rightlimb3_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_D]->default_jointPos, &rightlimb3_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_E]->default_jointPos, &rightlimb3_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_F]->default_jointPos, &rightlimb3_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_G]->default_jointPos, &rightlimb3_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_H]->default_jointPos, &rightlimb3_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB3_I]->default_jointPos, &rightlimb3_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_A]->default_jointPos, &rightlimb4_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_B]->default_jointPos, &rightlimb4_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_C]->default_jointPos, &rightlimb4_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_D]->default_jointPos, &rightlimb4_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_E]->default_jointPos, &rightlimb4_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_F]->default_jointPos, &rightlimb4_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_G]->default_jointPos, &rightlimb4_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_H]->default_jointPos, &rightlimb4_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB4_I]->default_jointPos, &rightlimb4_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_A]->default_jointPos, &rightlimb5_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_B]->default_jointPos, &rightlimb5_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_C]->default_jointPos, &rightlimb5_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_D]->default_jointPos, &rightlimb5_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_E]->default_jointPos, &rightlimb5_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_F]->default_jointPos, &rightlimb5_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_G]->default_jointPos, &rightlimb5_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_H]->default_jointPos, &rightlimb5_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_I]->default_jointPos, &rightlimb5_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_J]->default_jointPos, &rightlimb5_j_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_K]->default_jointPos, &rightlimb5_k_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB5_L]->default_jointPos, &rightlimb5_l_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_A]->default_jointPos, &rightlimb6_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_B]->default_jointPos, &rightlimb6_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_C]->default_jointPos, &rightlimb6_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_D]->default_jointPos, &rightlimb6_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_E]->default_jointPos, &rightlimb6_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_F]->default_jointPos, &rightlimb6_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_G]->default_jointPos, &rightlimb6_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_H]->default_jointPos, &rightlimb6_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB6_I]->default_jointPos, &rightlimb6_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_A]->default_jointPos, &rightlimb7_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_B]->default_jointPos, &rightlimb7_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_C]->default_jointPos, &rightlimb7_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_D]->default_jointPos, &rightlimb7_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_E]->default_jointPos, &rightlimb7_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_F]->default_jointPos, &rightlimb7_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_G]->default_jointPos, &rightlimb7_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_H]->default_jointPos, &rightlimb7_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB7_I]->default_jointPos, &rightlimb7_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_A]->default_jointPos, &rightlimb8_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_B]->default_jointPos, &rightlimb8_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_C]->default_jointPos, &rightlimb8_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_D]->default_jointPos, &rightlimb8_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_E]->default_jointPos, &rightlimb8_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_F]->default_jointPos, &rightlimb8_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_G]->default_jointPos, &rightlimb8_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_H]->default_jointPos, &rightlimb8_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB8_I]->default_jointPos, &rightlimb8_i_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_A]->default_jointPos, &rightlimb9_a_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_B]->default_jointPos, &rightlimb9_b_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_C]->default_jointPos, &rightlimb9_c_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_D]->default_jointPos, &rightlimb9_d_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_E]->default_jointPos, &rightlimb9_e_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_F]->default_jointPos, &rightlimb9_f_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_G]->default_jointPos, &rightlimb9_g_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_H]->default_jointPos, &rightlimb9_h_jointPos);
        Math_Vec3s_Copy(&overcoat_PhysLimbs[OVERCOAT_BODYPART_RIGHTLIMB9_I]->default_jointPos, &rightlimb9_i_jointPos);

        Actor_SetScale(&this->actor, 0.01f);
        SkelAnime_InitFlex(
            play,
            &this->skelAnime,
            &gOvercoatSkel,
            NULL,
            this->jointTable,
            this->morphTable,
            GOVERCOATSKEL_NUM_LIMBS
        );

        // Verlet Integration starts here
        Verlet_InitPhysPlayer(&gOvercoatPhysPlayer, player);
        gOvercoatEpona = (PhysEponaMovement){0};
        Overcoat_SetDefaultBodyPartsPos(this, player, overcoat_PhysLimbs, overcoat_PhysBones);
    }
}


/**
 * @brief Request a coat reset when Kamaro's dance starts.
 */
RECOMP_HOOK("func_80839A10") void Overcoat_ResetOnKamaroDance(
    PlayState* play, Player* player) {
    if (player != GET_PLAYER(play) || player->transformation != PLAYER_FORM_HUMAN ||
        gPlayerOvercoat == NULL) {
        return;
    }

    // The game's function only starts the dance when grounded.
    if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        gPlayerOvercoat->needsReset = 1;
    }
}

/**
 * @brief Reset the overcoat's phys limbs' position when Jackie's door opening animation plays.
 */
RECOMP_HOOK("Player_Door_Knob") void Overcoat_ResetOnDoorOpen(PlayState* play, Player* player,
    Actor* door) {
    // Ignore this if Jackie is not in human form and overcoat is not drawn.
    if (player != GET_PLAYER(play) || player->transformation != PLAYER_FORM_HUMAN ||
        gPlayerOvercoat == NULL) {
        return;
    }

    // If Jackie is human and overcoat is being drawn, then make sure to reset overcoat shape.
    gPlayerOvercoat->needsReset = 1;
}


/*
=================
Overcoat Destroy
=================
*/
/**
 * @brief Destroy overcoat model and actor
 * 
 * @param thisx     Actor pointer
 * @param play      Current playstate
 */
void Overcoat_Destroy(Actor* thisx, PlayState* play) {
    gPlayerOvercoat = NULL;
}


/*
=================
Overcoat Update
=================
*/

/**
 * @brief Capture the directional axes of the torso and apply it to the overcoat
 * 
 * More specifically, this function extracts the directional axes of Jackie's torso and assign them to 
 * global variables that carry the directional axes of the overcoat.
 * 
 * @param mtx   Matrix that is currently carrying the positional data of the torso limb
 */
void Overcoat_CaptureLimitAxes(MtxF* mtx) {
    // Based on torso axes in matrix: local +Y = forward, local +Z = left.
    // Convert the matrix values into Vec3f directional axes relative to Jackie's torso limb
    overcoat_forwardAxis = (Vec3f){mtx->xy, mtx->yy, mtx->zy};
    overcoat_leftAxis = (Vec3f){mtx->xz, mtx->yz, mtx->zz};
    overcoat_downAxis = (Vec3f){-mtx->xx, -mtx->yx, -mtx->zx};

    // Calculate the lengths of the direcitonal axes vectors
    f32 forwardLength = sqrtf(Math3D_Vec3fMagnitudeSq(&overcoat_forwardAxis));
    f32 leftLength = sqrtf(Math3D_Vec3fMagnitudeSq(&overcoat_leftAxis));
    f32 downLength = sqrtf(Math3D_Vec3fMagnitudeSq(&overcoat_downAxis));

    // Flag variable that indicates that the directional axes for the overcoat
    overcoat_dirAxesReady = 0;

    // Check if the lengths of the directional axes vectors are greater than zero.
    if (forwardLength < GREATER_THAN_ZERO || leftLength < GREATER_THAN_ZERO || 
        downLength < GREATER_THAN_ZERO) {
        return;
    }

    // Normalize the directional axes of the overcoat.
    Math_Vec3f_Scale(&overcoat_forwardAxis, 1.f / forwardLength);
    Math_Vec3f_Scale(&overcoat_leftAxis, 1.f / leftLength);
    Math_Vec3f_Scale(&overcoat_downAxis, 1.f / downLength);

    // Mark flag variable that indicates that the directional axes for the overcoat has been set up now.
    overcoat_dirAxesReady = 1;
}


/**
 * @brief Limit the given phys bone's upward direction (and inward direction if indicated).
 * 
 * This function limits the upward direction and inward direction (if indicated in 'limitInward'
 * argument), of a given phys bone, indicated by its input 'parentPhysLimb' and 'childPhysLimb'. This
 * function is used so that the overcoat's phys bones do not exceed its horizontal limit to prevent
 * unwanted flapping behavior of the overcoat.
 * 
 * Regarding inward direction of the phys bone, whether the inward direction limit should be applied or
 * not is indicated with the 'limitInward' argument.  When this is '1', then the 'outwardDirVec' is used 
 * as reference to push the phys bone that is going inward towards Jackie's legs back outward.
 * 
 * This function is a manual fix/adjustment on the overcoat's physics that sometimes rotate or bend in 
 * extreme or unwanted directions during some animations that have severe movement of Jackie's limbs.
 * 
 * @param parentPhysLimb    Parent phys limb of the target phys bone
 * @param childPhysLimb     Child phys limb of the target phys bone
 * @param outwardDirVec     Outward direction axis of the target phys bone
 * @param limitInward       Flag value indicating if the phys bone has to be pushed outward
 * @param maxUpwardRotLim   Max upward rotation limit for phys bone in chain of phys limbs
 */
void Overcoat_ApplyBoneRotationLimit(PhysLimb* parentPhysLimb, PhysLimb* childPhysLimb, 
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
    // upward rotation limit of the overcoat's chains of phys limbs. This upward rotation limit is 
    // provided as an argument in the function 'maxUpwardRotLim'

    // Calculate the downward component of the phys bone's direction relative to overcoat.
    f32 boneDownwardComponent = CustomMath_Vec3f_Dot(&adjustedDirVec, &overcoat_downAxis);

    // If the downward amount of the direction is negative, then that means it's going upward. Remove 
    // the upward direction by subtracting the current direction of the phys bone by its downward 
    // direction scaled by its downward amount (which is negative, indicating that this is an upward 
    // direction).
    if (boneDownwardComponent < 0.f) {
        // Bone's vertical amount relative to overcoat expressed in world space
        Vec3f boneVerticalDir = {0.f, 0.f, 0.f};  

        // Bone's horizontal amount relative to overcoat expressed in world space
        Vec3f boneHorizontalDir = {0.f, 0.f, 0.f};
        
        // Calculate the component along the torso's down axis.
        Math_Vec3f_ScaleAndStore(&overcoat_downAxis, boneDownwardComponent, &boneVerticalDir);

        // Remove the vertical component, leaving the torso-relative horizontal direction.
        Math_Vec3f_Diff(&adjustedDirVec, &boneVerticalDir, &boneHorizontalDir);

        //  Calculate the horizontal component of the phys bone's direction relative to overcoat
        f32 boneHorizontalComponent = sqrtf(Math3D_Vec3fMagnitudeSq(&boneHorizontalDir));

        f32 allowedSin = Math_SinS(maxUpwardRotLim);
        f32 allowedCos = Math_CosS(maxUpwardRotLim);

        // tan(angle) gives the allowed upward amount.
        // All supplied angles here are below 90 degrees.
        f32 minDownLength = (-boneHorizontalComponent) * (allowedSin / allowedCos);

        // Check if the bone is pointing upward beyond the allowed upward angle.
        if (boneDownwardComponent < minDownLength) {
            // If the horizontal component of the phys bone's orientation relative to the overcoat is 
            // greater than 0, then preserve the horizontal component of the phys bone while replacing 
            // the vertical component of the phys bone's orientation relative to the overcoat with the 
            // allowed value.
            if (boneHorizontalComponent > GREATER_THAN_ZERO) {
                adjustedDirVec.x = boneHorizontalDir.x + overcoat_downAxis.x * minDownLength;
                adjustedDirVec.y = boneHorizontalDir.y + overcoat_downAxis.y * minDownLength;
                adjustedDirVec.z = boneHorizontalDir.z + overcoat_downAxis.z * minDownLength;
            }
            // Else, if the horizontal component of the phys bone's orientation relative to the overcoat 
            // is NOT greater than zero, but max upward rotation limb for the chain of phys limbs is 
            // greater than 0 degrees, then take the phys bone's chain's outward horizontal direction 
            // vector, tilt it by the max upward rotation limit, and match the phys bone's orientation to
            // match the chain's tilted orientation.
            else if (maxUpwardRotLim > 0) {
                // (outwardDirVec * allowedCos) supplies the horizontal portion.
                // (-overcoat_downAxis * allowedSin) supplies the upward portion.
                adjustedDirVec.x = (outwardDirVec->x * allowedCos) - (overcoat_downAxis.x * allowedSin);
                adjustedDirVec.y = (outwardDirVec->y * allowedCos) - (overcoat_downAxis.y * allowedSin);
                adjustedDirVec.z = (outwardDirVec->z * allowedCos) - (overcoat_downAxis.z * allowedSin);
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
        adjustedDirVec = overcoat_downAxis;
        allowedLengthSq = Math3D_Vec3fMagnitudeSq(&adjustedDirVec);
    }

    // Rescale the phys bone's length to its original length while preserving its direction.
    Math_Vec3f_Scale(&adjustedDirVec, sqrtf(originalLengthSq / allowedLengthSq));

    // Change the phys bone's child phys limb's position based on the new direction vector from the phys 
    // bone's parent phys limb.
    Math_Vec3f_Sum(&parentPhysLimb->curr_pos, &adjustedDirVec, &childPhysLimb->curr_pos);
}


/**
 * @brief Apply bend limits to every vertical overcoat cloth chain of phys limbs.
 *
 * This function goes through each vertical cloth chain and applies Overcoat_ApplyBoneRotationLimit()
 * to every bone segment in that chain.
 *
 * For each chain, it first determines which world-space direction counts as "outward" based on where 
 * that chain is located around the overcoat. After taht, the function walks down the chain one bone 
 * at a time, where the bones are prevented from bending upward relative to the torso. For the first 3 
 * (or other value defined by macro) bones, the phys limbs are also prevented from bending inward.
 *
 * @param gPhysLimbs   Phys limbs of the overcoat
 * @param chainStarts  Array containing the starting phys limb index for each chiain of phys limbs
 * @param chainCount   Number of chains in overcoat
 */
void Overcoat_ApplyChainRotationLimits(PhysLimb* gPhysLimbs[], const int chainStarts[],
    int chainCount, const OvercoatRotationSettings* limits) {
    // If directional axes of the overcoat is not established in the world space yet, then do not run 
    // this function.
    if (!overcoat_dirAxesReady) {
        return;
    }

    // For each vertical chain of physlimbs, keep its phys bones from bending too far upward or inward, 
    // depending on where this chain is located in within the overcoat and the limits set to the
    // chains beforehand.
    for (int currChain = 0; currChain < chainCount; currChain++) {
        // Get the current chain's starting limb's index
        int startLimbIdx = chainStarts[currChain];  // Index of first phys limb in current chain

        // By  default, set the current chain's outward direction as forward of overcoat.
        // If the current chain is one of the front chains of the overcoat, then the outward direction
        // will stay as forward.
        Vec3f outwardDir = {0.f, 0.f, 0.f};
        Math_Vec3f_Copy(&outwardDir, &overcoat_forwardAxis);

        // If the starting index of the chain of phys limbs is the first phys limb of the left most 
        // chain of phys limbs in the overcoat, then make the outward direction of this chain as left.
        if (startLimbIdx == OVERCOAT_BODYPART_LEFTLIMB5_A) {
            Math_Vec3f_Copy(&outwardDir, &overcoat_leftAxis);
        }
        // If the starting index of the chain of phys limbs is the first phys limb of the right most 
        // chain of phys limbs in the overcoat, then make the outward direction of this chain as right.
        else if (startLimbIdx == OVERCOAT_BODYPART_RIGHTLIMB5_A) {
            Math_Vec3f_ScaleAndStore(&overcoat_leftAxis, -1.f, &outwardDir);
        }
        // If the current chain of phys limbs is one of the back chains of phys limbs in the overcoat, 
        // then make the outward direction of this chain as back.
        // (Back, Left6-9, Right9-6)
        else if (startLimbIdx == OVERCOAT_BODYPART_BACKLIMB_A ||
                (startLimbIdx >= OVERCOAT_BODYPART_LEFTLIMB6_A &&
                startLimbIdx <= OVERCOAT_BODYPART_LEFTLIMB9_A) ||
                (startLimbIdx >= OVERCOAT_BODYPART_RIGHTLIMB6_A &&
                startLimbIdx <= OVERCOAT_BODYPART_RIGHTLIMB9_A)) {
            // Back is basically the opposite of forward.
            Math_Vec3f_ScaleAndStore(&overcoat_forwardAxis, -1.f, &outwardDir);
        }

        // Left/Right-most chains have A-L limbs, which is 11 phys bones, while the rest front and back 
        // chains have A-I limbs, which is 8 phys bones.
        int boneCount = cloth_chains[currChain].limbCount - 1;

        // Limit the current chain's direction so that it is not going upward or inward too much.
        // The directional axes of the overcoat are used to help with this operation.
        for (int currPhysLimb = 0; currPhysLimb < boneCount; currPhysLimb++) {
            // Limit the top bones' inward rotation, which is set beforehand in the overcoat's
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
            Overcoat_ApplyBoneRotationLimit(gPhysLimbs[startLimbIdx + currPhysLimb], 
                gPhysLimbs[startLimbIdx + currPhysLimb + 1], &outwardDir, limitInward, maxUpAngle);
        }
    }
}


/**
 * @brief Get Jackie's torso limb's world position and rotation and also set up collider center world 
 * positions.
 * 
 * This function looks through Jackie's limbs. There are 5 limbs in particular that this is used for: 
 * torso, left/right thigh limbs, and left/right shin limbs. 
 * 
 * The torso limb's world position and rotation values are extracted for assigning the overcoat actor's
 * position and rotation to the world. The overcoat's directional axes relative to the world are also 
 * captured for setting rotation limits to the phys limbs of the overcoat.
 * 
 * The other limbs are observed to assign respective colliders for the overcoat.
 */
RECOMP_HOOK("Player_PostLimbDrawGameplay") void Overcoat_on_Player_PostLimbDrawGameplay(
    PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    
    // For moving colliders while riding Epona...
    Player* player = GET_PLAYER(play);
    u8 fastRiding = Overcoat_IsFastRidingOrSpurring(player);

    /********************************
     Torso Limb Positions and Rotations
    ********************************/
    if (limbIndex == PLAYER_LIMB_TORSO) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's Torso limb
        MtxF* mtx = Matrix_GetCurrent();

        // Update torso-relative directions using the current torso matrix.
        Overcoat_CaptureLimitAxes(mtx);

        // Get torso's world position directly from the current matrix
        overcoat_torso_globalPos.x = mtx->xw;
        overcoat_torso_globalPos.y = mtx->yw;
        overcoat_torso_globalPos.z = mtx->zw;

        // Get torso rotation values (used for rotating the belt strap hook actor properly)
        Matrix_MtxFToYXZRot(mtx, &overcoat_torso_rotate, 1);

        // Position the torso collider directly at the torso limb.
        Math_Vec3f_Copy(&overcoat_torsoCollider.center, &overcoat_torso_globalPos);

        // Position the roll collider using its torso-local offset.
        Vec3f rollColliderLocalOffset = {-600.f, 0.f, 0.f};
        Matrix_MultVec3f(&rollColliderLocalOffset, &overcoat_rollCollider.center);

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

        // Get left thigh limb's world position directly from the current matrix
        overcoat_thighJointPos[0].x = mtx->xw;
        overcoat_thighJointPos[0].y = mtx->yw;
        overcoat_thighJointPos[0].z = mtx->zw;

        // Choose the left back thigh collider's local offset.
        Vec3f leftThighBackColliderLocalOffset = {0.f, 175.f, -40.f};

        // Apply different local offset for when Jackie is riding Epona at fast speed or spurring
        if (fastRiding) {
            Math_Vec3f_Copy(&leftThighBackColliderLocalOffset, &overcoat_fastRideLeftThighOffset);
        }

        // Convert the local offset into the collider's world position.
        Matrix_MultVec3f(&leftThighBackColliderLocalOffset, &overcoat_leftThighBackCollider.center);

        // Position the front thigh collider.
        Vec3f leftThighFrontColliderLocalOffset = {0.f, -100.f, -30.f};
        Matrix_MultVec3f(&leftThighFrontColliderLocalOffset, &overcoat_leftThighFrontCollider.center);

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

        // Get right thigh limb's world position directly from the current matrix
        overcoat_thighJointPos[1].x = mtx->xw;
        overcoat_thighJointPos[1].y = mtx->yw;
        overcoat_thighJointPos[1].z = mtx->zw;

        // Choose the right back thigh collider's local offset.
        Vec3f rightThighBackColliderLocalOffset = {0.f, 175.f, 40.f};

        // Apply different local offset for when Jackie is riding Epona at fast speed or spurring
        if (fastRiding) {
            Math_Vec3f_Copy(&rightThighBackColliderLocalOffset, &overcoat_fastRideRightThighOffset);
        }

        // Convert the local offset into the collider's world position.
        Matrix_MultVec3f(&rightThighBackColliderLocalOffset, &overcoat_rightThighBackCollider.center);

        // Position the front thigh collider.
        Vec3f rightThighFrontColliderLocalOffset = {0.f, -100.f, 30.f};
        Matrix_MultVec3f(&rightThighFrontColliderLocalOffset, &overcoat_rightThighFrontCollider.center);

        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }

    /********************************
     Left Leg Limb Positions
    ********************************/
    if (limbIndex == PLAYER_LIMB_LEFT_SHIN) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's left shin limb
        MtxF* mtx = Matrix_GetCurrent();

        // Get left shin limb's world position directly from the current matrix
        overcoat_kneeJointPos[0].x = mtx->xw;
        overcoat_kneeJointPos[0].y = mtx->yw;
        overcoat_kneeJointPos[0].z = mtx->zw;

        // Choose the left shin/leg collider's local offset.
        Vec3f leftLegColliderLocalOffset = {0.f, -70.f, 0.f};

        // Apply different local offset for when Jackie is riding Epona at fast speed or spurring
        if (fastRiding) {
            Math_Vec3f_Copy( &leftLegColliderLocalOffset, &overcoat_fastRideLeftLegOffset);
        }

        // Convert the local offset into the collider's world position.
        Matrix_MultVec3f(&leftLegColliderLocalOffset, &overcoat_leftLegCollider.center);

        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }

    /********************************
     Right Leg Limb Positions
    ********************************/
    if (limbIndex == PLAYER_LIMB_RIGHT_SHIN) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's right shin limb
        MtxF* mtx = Matrix_GetCurrent();

        // Get right shin limb's world position directly from the current matrix
        overcoat_kneeJointPos[1].x = mtx->xw;
        overcoat_kneeJointPos[1].y = mtx->yw;
        overcoat_kneeJointPos[1].z = mtx->zw;

        // Choose the right shin/leg collider's local offset.
        Vec3f rightLegColliderLocalOffset = {0.f, -70.f, 0.f};

        // Apply different local offset for when Jackie is riding Epona at fast speed or spurring
        if (fastRiding) {
            Math_Vec3f_Copy(&rightLegColliderLocalOffset, &overcoat_fastRideRightLegOffset);
        }

        // Convert the local offset into the collider's world position.
        Matrix_MultVec3f(&rightLegColliderLocalOffset, &overcoat_rightLegCollider.center);

        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }
}


/**
 * @brief Reduce the distance of two neighboring phys limbs down to the input distance limit
 * 
 * This function first checks to see if the distance between the two phys limbs are exceeding the 
 * input distance limit. If the distance between the two phys limbs does exceed the limit, then the
 * positions of the two phys limbs are re-calculated so that the distance is at the maximum distance.
 * 
 * @param physLimbA     First phys limb
 * @param physLimbB     Second phys limb
 * @param maxDistance   Max distance limit between the two neighboring phys limbs
 */
void Overcoat_LimitSidewaysDistance(PhysLimb* physLimbA, PhysLimb* physLimbB, f32 maxDistance) {
    // Only run this function if both phys limbs are unpinned and the distance limit is greater than 0.
    if (physLimbA->pinned || physLimbB->pinned || maxDistance < 0.f) {
        return;
    }

    // Find directional vector from phys limb A to phys limb B.
    Vec3f directionalVec = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&physLimbB->curr_pos, &physLimbA->curr_pos, &directionalVec);
    // Square the directional vector and check if it's valid
    f32 distanceSq = (directionalVec.x * directionalVec.x) + 
        (directionalVec.y * directionalVec.y) + (directionalVec.z * directionalVec.z);

    // If the distance from phys limb A and phys limb B are essentially not 0 and also greater than the max 
    // distance limit, then move forward with the function to reduce the distance.
    if (distanceSq <= maxDistance * maxDistance || distanceSq < GREATER_THAN_ZERO2) {
        return;
    }

    // Get the actual distance between the two neighboring phys limbs along that directional vector
    f32 distance = sqrtf(distanceSq);

    // Find the scale value needed to reduce the distance between the two neighboring phys limbs to the max 
    // distance limit. Note that this scale will be applied to both phys limbs.
    f32 correctionScale = 0.5f * (distance - maxDistance) / distance;

    // Scale the directional vector down by the scale value.
    Vec3f correctedDirVec = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&directionalVec, correctionScale, &correctedDirVec);

    // Apply the scaled directional vector to both phys limbs.
    Math_Vec3f_Sum(&physLimbA->curr_pos, &correctedDirVec, &physLimbA->curr_pos);
    Math_Vec3f_Diff(&physLimbB->curr_pos, &correctedDirVec, &physLimbB->curr_pos);
}


/**
 * @brief Normalize Vec3f direction used by overcoat during substep physics calculations
 * 
 * This function calculates the length of the target direction vector and scales it so that its final 
 * length becomes 1.
 * 
 * @param targetVec     Direction vector to normalize
 * 
 * @return true if the vector was successfully normalized
 * @return false if the vector was too small to safely normalize
 */
bool Overcoat_NormalizeSubstepDirectionVec(Vec3f* targetVec) {
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
Vec3f Overcoat_FindClosestPointOnCapsule(Vec3f targetPos, PhysSphereCollider* capsuleColStart, 
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
 * collider used by the overcoat.
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
f32 Overcoat_GroupContact(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* capsulePairs[][2], 
    int capsuleColCount, Vec3f* contactColCenter, f32* contactColRadius) {
    
    // Set up variable indicating deepest depth value of targetPos in any collider in overcoat
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
        Vec3f colCenter = Overcoat_FindClosestPointOnCapsule(targetPos, capsuleColStart, 
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
 * capsule colliders used by the overcoat.
 *
 * @param targetPos			Target world position being checked
 * @param pointRadius		Collision radius of target point/position
 * @param capsulePairs		Array of sphere collider pairs used as capsule colliders
 * @param capsuleColCount	Number of capsule colliders
 * 
 * @return true if target position is outside of all colliders, otherwise false
 */
bool Overcoat_GroupOutside(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* capsulePairs[][2], 
    int capsuleColCount) {
    Vec3f center = {0.f, 0.f, 0.f};		// Made only as function argument.
    f32 radius = 0.f;					// Made only as function argument.
	
	// Check if target position is inside colliders (torso and capsule) by calculating the
	// deepest depth the position is in any collider.
	f32 colliderDepth = Overcoat_GroupContact(targetPos, pointRadius, capsulePairs, 
		capsuleColCount, &center, &radius);
		
	// If target position is inside any collider, then return false
	bool posIsOutside = (colliderDepth <= 0.f);
	
    return posIsOutside;
}


/**
 * @brief Find safe exit position for phys limb from overcoat's colliders.
 * 
 * This function uses a given direction from a target phys limb's position to find a new position
 * that is outside the torso collider and all capsule colliders of the overcoat.
 *
 * The function first finds a distance that is guaranteed to extend past every collider in the
 * specified direction. It then checks if this position is actually outside all colliders.
 *
 * If the test exit position at mid point is outside all colliders, then that is the new farthest 
 * distance between position outside the colliders and the closest collider's boundary.
 *
 * This function was made for the overcoat because the overcoat uses many colliders that are 
 * overlapping one another around Jackie's lower body. Simply pushing out the phys limb from one 
 * collider can put the phys limb in another collider, creating unwanted clipping behavior of the
 * overcoat. In a way, this method is trying to treat the overlapping colliders as one big collider
 * entity.
 *
 * @param targetPhysLimbPos		Target phys limb's world position
 * @param physLimbDir			Direction along which a safe exit position for phys limb is searched
 * @param pointRadius			Collisionr radius of target phys limb
 * @param capsulePairs			Array of pairs of sphere colliders used for capsule colliders
 * @param capsuleColCount		Count of capsule colliders 
 * @param padding				Small extra distance added so exit doesn't sit on collider boundary
 * @param exitPosition			Output position containing safe exit position for phys limb
 */
bool Overcoat_GroupRayExit(Vec3f targetPhysLimbPos, Vec3f physLimbDir, f32 pointRadius,
    PhysSphereCollider* capsulePairs[][2], int capsuleColCount, f32 padding, Vec3f* exitPosition) {
    // Find a distance from phys limb, along the indicated search direction (physLimbDir), that is 
	// far enough that it should be past every collider
    f32 farDistance = 0.f;
	
	// Go through each collider in the array of capsule colliders and see how far the target phys limb 
    // should go to be outside all colliders (indicated by farDistance)
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
		
		// Check the two sphere collider endpoints of the current capsule collider.
		// The focus is going to be on the current capsule collider's sphere collider with the larger 
        // radius for finding a distance that is more certain to be big enough to push the phys limb out 
        // of the colliders.
        for (int capsuleEndIdx = 0; capsuleEndIdx < 2; capsuleEndIdx++) {
			Vec3f colliderCenter;
			if (capsuleEndIdx == 0) {
				colliderCenter = capsuleColStart->center;
			}
			else {
				colliderCenter = capsuleColEnd->center;
			}
			
			// Calculate a safe distance along the input direction that would move phys limb beyond capsule's 
            // sphere collider endpoint
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
	
	// Use the calculated farthest distance from any collider (farDistance) to find the point along the 
    // input phys limb direction where it would be outside all colliders. Update the exitPosition 
    // argument with this new position value
    Vec3f travelVec = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&physLimbDir, farDistance, &travelVec);
    Math_Vec3f_Sum(&targetPhysLimbPos, &travelVec, exitPosition);

	// If the newly calculated position is still inside any collider, return false and get out of function.
    if (!Overcoat_GroupOutside(*exitPosition, pointRadius, capsulePairs, capsuleColCount)) { 
		return false;
	}

	// Up to this point, a safe far distance was found that places the PhysLimb outside all colliders. 
	// However, this distance may be farther than necessary.
	//
	// Use binary search to find a closer safe exit position along input direction (physLimbDir).
	// nearDistance represents the inside side of the search, while farDistance represents  the known safe 
    // outside side. Each iteration cuts the gap between them in half.
	//
	// More iterations make exitPosition closer to the point where the PhysLimb first  becomes outside
	// all colliders along this direction.
    for (int iter = 0; iter < OVERCOAT_EXIT_SEARCH_STEPS; iter++) {
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
        bool testExitIsOutside = Overcoat_GroupOutside(testExitPos, pointRadius, capsulePairs, 
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
 * @brief Push the given overcoat phys limb out of Jackie's set colliders for overcoat.
 * 
 * This function checks whether a given phys limb is inside the currently set colliders for the
 * overcoat. All the colliders are capsule colliders.
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
void Overcoat_SolveColliderGroup(PhysLimb* physLimb, PhysSphereCollider* collider_pairs[][2], 
    int capsuleColCount) {
	// Find the collider where the phys limb is deepest in within the group of colliders.
    // First find the closest point along this capsule collider's centerline to the phys limb.
    Vec3f center = {0.f, 0.f, 0.f};     // Center of sphere collider in capsule collider
    f32 radius = 0.f;                   // Radius of sphere collider in capsule collider
	
	// If the phys limb is not inside any collider, no correction is needed.
    if (Overcoat_GroupContact(physLimb->curr_pos, physLimb->collision_radius,
		collider_pairs, capsuleColCount, &center, &radius) <= 0.f) {
		return;
	}

	// Find direction pointing from the collider center toward the phys limb's current position
    Vec3f direction = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&physLimb->curr_pos, &center, &direction);
		
	// Try to normalize the direction from the target collider center toward the phys limb.
    // If it is too short to normalize, try the previous position of phys limb instead.
    if (!Overcoat_NormalizeSubstepDirectionVec(&direction)) {
        // Use phys limb's previous position instead to create another possible outward direction
        Math_Vec3f_Diff(&physLimb->prev_pos, &center, &direction);
		// Try to normalize direction from collider center to phys limb's previous position If
		// this fails, then even the previous position of the phys limb is too close to the
		// collider's center.
        if (!Overcoat_NormalizeSubstepDirectionVec(&direction)) {
			// Use Jackie's forward direction as an alternative
            direction = overcoat_forwardAxis;
			
			// Try to normalize Jackie's forward direction. If even this fails, then just use
			// straight upward direction as final fallback direction.
            if (!Overcoat_NormalizeSubstepDirectionVec(&direction)) { 
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
	// Calculate the offset from the collider center to just outside its boundary.
    Vec3f outwardOffset = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&direction, radius + padding, &outwardOffset);

    // Calculate the candidate world position.
    Vec3f candidate = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(&center, &outwardOffset, &candidate);

    // Check whether phys limb would be outside all colliders if it were moved to this candidate
	// position. If the position is safe, then move the phys limb there and finish
    if (Overcoat_GroupOutside(candidate, physLimb->collision_radius, collider_pairs, capsuleColCount)) {
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
    bool outwardOK = Overcoat_GroupRayExit(physLimb->curr_pos, direction, physLimb->collision_radius, 
		collider_pairs, capsuleColCount, padding, &outwardExit);
	// Search in opposite direction for another valid exit that is outside all colliders
	// oppositeOK is a flag that indicates whether valid exit was found in opposite direction
	// oppositeExit stores the position of that exit.
    bool oppositeOK = Overcoat_GroupRayExit(physLimb->curr_pos, opposite, physLimb->collision_radius, 
		collider_pairs, capsuleColCount, padding, &oppositeExit);

    // If exit was found in normal outward direction, use it when either:
	//  1. no safe exit was found in the opposite direction OR
	//  2. outward exit is closer to the phys limb than the opposite exit
	// Otherwise, use the safe opposite exit
    bool outwardIsCloser = (Math_Vec3f_DistXYZ(&physLimb->curr_pos, &outwardExit) <=
        Math_Vec3f_DistXYZ(&physLimb->curr_pos, &oppositeExit));
    bool useOutwardExit = (outwardOK && (!oppositeOK || outwardIsCloser));

    if (useOutwardExit) {
        physLimb->curr_pos = outwardExit;
    } else if (oppositeOK) {
        physLimb->curr_pos = oppositeExit;
    }
}


/**
 * @brief Prevent the midpoint between two neighboring phys limbs from clipping through collider
 *
 * This function creates a temporary phys limb at the midpoint between two neighboring unpinnied phys
 * limbs. The midpoint is checked against the torso and capsule colliders just like phys limbs.
 *
 * If the midpoint is pushed out of a collider, the correction is applied to both neighboring phys
 * limbs. This helps keep the cloth surface between the two phys limbs from passing through Jackie's
 * body even when the phys limbs themselves are outside of the colliders.
 * 
 * @param physLimb_a		First phys limb
 * @param physLimb_b 		Second phys limb
 * @param collider_pairs	Array of pairs of colliders that are used for establishing capsule colliders
 * @param capsuleColCount	Number of capsule colliders
 */
void Overcoat_SolveMidpointGroup(PhysLimb* physLimb_a, PhysLimb* physLimb_b, 
	PhysSphereCollider* collider_pairs[][2], int capsuleColCount) {
	// Create temporary phys limb that represents midpoint between two neighboring phys limbs.
    PhysLimb midpoint = {0};
	
	// Calculate midpoint's current position by averaging out two phys limb's positions
	// Average current position
    Math_Vec3f_Sum(&physLimb_a->curr_pos, &physLimb_b->curr_pos, &midpoint.curr_pos);
    Math_Vec3f_Scale(&midpoint.curr_pos, 0.5f);

    // Average previous position
    Math_Vec3f_Sum(&physLimb_a->prev_pos, &physLimb_b->prev_pos, &midpoint.prev_pos);
    Math_Vec3f_Scale(&midpoint.prev_pos, 0.5f);
		
	// Calculate midpoint phys limb's averaged collision radius
    midpoint.collision_radius = (physLimb_a->collision_radius + physLimb_b->collision_radius) * 0.5f;
	
	// Save midpoint's position before collision correction constraint
    Vec3f midpointBeforeCollision = midpoint.curr_pos;
	
	// Apply collider correction on midpoint temp phys limb
	// This is basically the same way phys limbs get their collider correction applied
    Overcoat_SolveColliderGroup(&midpoint, collider_pairs, capsuleColCount);
	// Calculate how far midpoint was moved by the collision correction.
    Vec3f correction = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&midpoint.curr_pos, &midpointBeforeCollision, &correction);

	// Apply the midpoint's collision correction to both neighboring phys limbs
	Math_Vec3f_Sum(&physLimb_a->curr_pos, &correction, &physLimb_a->curr_pos);
    Math_Vec3f_Sum(&physLimb_b->curr_pos, &correction, &physLimb_b->curr_pos);
}


/**
 * @brief Reset the overcoat chains of phys limbs based on its assigned reset angle
 * 
 * This function resets the overcoat's chains of phys limbs at a previously assigned reset angle.
 * 
 * The following logic is followed to accomplish this:
 *
 *	1. Using the previously assigned reset angle, find the horizontal outward and downward direction
 *	   that the chains need to be going in when reset.
 *
 * 	2. Go through each chain of phys limbs. For each chain, determine the outward direction of the
 *	   chain based on the directional axes of the overcoat (based on Jackie's torso limb).
 *
 *	3. For each chain, apply the outward direction to the reset direction.
 *
 *	4. For each chain, use the chain's calculated outward reset direction to find the new world 
 *	   positions of the chain's phys limbs.
 *
 *	5. Remove all the phys limbs' velocity and previous position info.
 * 
 * @param this          Overcoat actor
 * @param gPhyslimbs    Phys limbs of overcoat
 * @param chains        Overcoat chains of phys limbs
 * @param chainCount	Number of all chains of overcoat
 */
void Overcoat_SetResetShape(Overcoat* this, PhysLimb* gPhysLimbs[], const OvercoatLimbChain chains[], 
    int chainCount) {
    // Torso-relative directions must be set up for overcoat before rebuilding.
    if (!overcoat_dirAxesReady) {
        return;
    }
	
	// Get overcoat's current actor scale.
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

    // Overcoat's downward direction, taken from Jackie's torso orientation.
    Vec3f overcoatDownDir = overcoat_downAxis;
    f32 overcoatDownDirLengthSq = Math3D_Vec3fMagnitudeSq(&overcoatDownDir);

    // If overcoat's down direction is basically just 0 (extremely slightly greater than 0), then
    // it has an invalid downward direction. Get out of function.
    if (overcoatDownDirLengthSq <= GREATER_THAN_ZERO) {
        return;
    }

    // Normalize the overcoat's downward direciton.
    Math_Vec3f_Scale(&overcoatDownDir, (1.f / sqrtf(overcoatDownDirLengthSq)));

	/********************************
     Reset each vertical cloth chain
    ********************************/
	// Go through each overcoat chain of phys limbs and reset each of them by calculating where 
	// each unpinned phys limb in the chain should be placed in 3D space relative to its parent.
    for (int currChain = 0; currChain < chainCount; currChain++) {
        int startLimb = chains[currChain].startLimb;	// Chain's phys limb A (pinned limb)
        int limbCount = chains[currChain].limbCount;	// Count of phys limbs in current chain

		
		// ======== Determine "outward direction" of current chain ========
		
		// Determine what direction the "outward direction" of current chain would be using the
		// torso's directional axes found before. Keep in mind that these axes describe the
		// local axes of the torso/overcoat in world space.
		// Note that "outward" for chains in the overcoat's front would be forward, chains in the 
		// left/right would be left/right (respectively), and chains in the back would be backward.
        Vec3f outwardDirection = {0.f, 0.f, 0.f};

        // Left chain extends to leftward direction of overcoat, which was defined via Jackie's 
		// torso limb's matrix (mtx) orientation in Overcoat_on_Player_PostLimbDrawGameplay.
        if (startLimb == OVERCOAT_BODYPART_LEFTLIMB5_A) {
            Math_Vec3f_Copy(&outwardDirection, &overcoat_leftAxis);
        }
		// Right chain extends to rightward direction of overcoat.
        else if (startLimb == OVERCOAT_BODYPART_RIGHTLIMB5_A) {
            Math_Vec3f_ScaleAndStore(&overcoat_leftAxis, -1.f, &outwardDirection);
        }
        // Front chains extend to forward direction of overcoat.
		// Basically, left limb 4 (leftmost front chain) to right limb 4 (rightmost front chain)
        else if ((startLimb >= OVERCOAT_BODYPART_LEFTLIMB0_A &&
				  startLimb <= OVERCOAT_BODYPART_LEFTLIMB4_A) ||
				  (startLimb >= OVERCOAT_BODYPART_RIGHTLIMB0_A &&
				  startLimb <= OVERCOAT_BODYPART_RIGHTLIMB4_A)) {
			Math_Vec3f_Copy(&outwardDirection, &overcoat_forwardAxis);
        }
        // Remaining (back) chains extends to backward direction of overcoat.
        else {
            Math_Vec3f_ScaleAndStore(&overcoat_forwardAxis, -1.f, &outwardDirection);
        }

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
		Math_Vec3f_ScaleAndStore(&overcoatDownDir, resetUpwardDirScale, &resetDownDir);
		
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
		// Start from the first UNPINNED phys limb (in this overcoat's case, 0 is the pinned phys
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
            f32 worldBoneLength = physBoneLength * actorScale;		// Scale to match overcoat size
			
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
     Reset velocity and prev position of all phys limbs of overcoat
    ********************************/
    // Remove velocity and previous position carried over from before the reset.
    for (int currLimbIdx = 0; currLimbIdx < OVERCOAT_BODYPART_MAX; currLimbIdx++) {
        PhysLimb* currLimb = gPhysLimbs[currLimbIdx];

        currLimb->prev_pos = currLimb->curr_pos;		// Reset previous position
        currLimb->curr_vel = (Vec3f){0.f, 0.f, 0.f};	// Reset current velocity
        currLimb->prev_vel = (Vec3f){0.f, 0.f, 0.f};	// Reset previous velocity
    }

    /********************************
     Disable all pending post-animation reset flags
    ********************************/
    // The post-animation reset angle has now been used to reset the phys limb positions.
    overcoat_pendingResetRotLimitUpdate = OVERCOAT_PENDING_RESET_NONE;
}


/**
 * @brief Rotate a given target point about the pivot point by a given angle along the y-axis.
 *
 * Typically, the target point is the current position of a phys limb, and the pivot point is the
 * overcoat root's world position.
 *
 * The 'targetPoint' param will get overwritten with a new, rotated position of old 'targetPoint'
 *
 * @param targetPoint		Point in the world position to rotate
 * @param pivotPoint		Point where the targetPoint should rotate around by the y-axis
 * @param rotAngle			Rotate targetPoint around pivotPoint by this value			
 */
void Overcoat_RotatePointAroundY(Vec3f* targetPoint, Vec3f* pivotPoint, s16 rotAngle) {
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
void Overcoat_InterpolatePosWithRot(Vec3f* prevFramePos, Vec3f* currFrameTargetPos,
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
 * @brief Calculate overcoat's world directional axes for current physics substep
 * 
 * This function interpolates the overcoat's previous frame's forward and down directions of the overcoat 
 * towards the current frame's target directions based on the current substep interpolation value.
 * 
 * After interpolation, the down direction is normalized. After that, the forward direction is corrected 
 * so that it is perpendicular to the down direction. Then, the forward direction is normalized also.
 * 
 * If either interpolated direction is unable to be normalized, then the functionf alls back to the 
 * current frame's target directions.
 * 
 * The overcoat's left direction is calculated using the cross product of the overcoat's forward and down 
 * directions. From this, we get the overcoat's forward, down, and left directions in world space for this 
 * current substep.
 * 
 * @param targetForward Current frame's target world-space forward direction of overcoat
 * @param targetDown    Current frame's target world-space down direction of overcoat.
 * @param t             Interpolation fraction value between previous and current frame
 */
void Overcoat_SetSubstepDirAxes(Vec3f targetForward, Vec3f targetDown, f32 t) {
	// Interpolate overcoat's down direction from previous frame toward current frame's target down
	// direction based on current substep.
    Vec3f downDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&overcoat_previousFrameDown, &targetDown, t, &downDirection);

    // Interpolate overcoat's forward direction from previous frame toward current frame's target forward 
    // direction based on the current substep.
    Vec3f forwardDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&overcoat_previousFrameForward, &targetForward, t, &forwardDirection);

    // Normalize the overcoat's interpolated down direction
    // If the down direction is too small to normalize, then use the final target down direction instead.
    if (!Overcoat_NormalizeSubstepDirectionVec(&downDirection)) {
        downDirection = targetDown;
    }

    // Using dot product, find projection to know how much of overcoat's forward direction points  in the 
    // same direction as overcoat's down direction.
    f32 projection = CustomMath_Vec3f_Dot(&forwardDirection, &downDirection);

    // Calculate the part of the forward direction that lies along the down direction.
    Vec3f forwardDownComponent = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&downDirection, projection, &forwardDownComponent);

    // Remove that component so forward becomes perpendicular to down.
    Math_Vec3f_Diff(&forwardDirection, &forwardDownComponent, &forwardDirection);

    // Normalize the overcoat's correct forward direction.
    // If the corrected forward direction is too small to normalize, then use the final target forward and 
    // down directions instead.
    if (!Overcoat_NormalizeSubstepDirectionVec(&forwardDirection)) {
        // Near-opposite poses have no unique linear interpolation.
        Math_Vec3f_Copy(&overcoat_forwardAxis, &targetForward);
        Math_Vec3f_Copy(&overcoat_downAxis, &targetDown);
    } 
    // Otherwise, store the interpolated and corrected substep directions of the overcoat
    else {
        Math_Vec3f_Copy(&overcoat_forwardAxis, &forwardDirection);
        Math_Vec3f_Copy(&overcoat_downAxis, &downDirection);
    }
    
    // Calculate the overcoat's left direction from the overcoat's forward and down directions.
    // This overcoat model's directional coordinate axes are now made: forward, down, and left
    Math3D_Vec3f_Cross(&overcoat_forwardAxis, &overcoat_downAxis, &overcoat_leftAxis);
}


/**
 * @brief Solve the overcoat's physics across multiple substeps within one rendered frame.
 * 
 * To better calculate frame-dependent values such as actor scale, collider states (world position and 
 * radius), directional references, phys limbs' positions, and phys bone lengths between previous frame
 * and current frame, this function divides a frame into multiple substeps.
 * 
 * A frame is divided into substeps to more accurately calculate physics-related operations to avoid
 * large value calculations between a previous frame and a current frame. This helps with reducing the
 * chances of unwanted physics behavior from the overcoat such as the overcoat clipping through Jackie's
 * legs, etc.
 * 
 * During each substep, unpinned phys limbs change in position via Verlet Integration. Afterward, phys bone
 * length constraint, horizontal distance constraint between neighboring phys limbs, directional 
 * reference constraints that are used for artificial physics restrictsion on the overcoat phys limbs,
 * and collision constraints are used in loops to stabilize the cloth as a whole.
 * 
 * Once all the substeps are complete (which means 1 frame is now complete), the current frame's exact
 * target values are restored and saved as previous frame values that is to be used for the next frame.
 * 
 * @param this                  Overcoat actor
 * @param player                Player actor that is used for movement and velocity info
 * @param apply_force           External net force applied to unpinned phys limbs during Verlet Integration
 * @param gPhysLimbs            Phys limbs of overcoat
 * @param gPhysBones            Phys bones of overcoat
 * @param limbsPrevPos          Previous frame's pinned phys limbs' starting position
 * @param chains                Array of starting phys limb indices of each vertical chains of overcoat
 * @param chainCount            Number of vertical cloth chains in the overcoat
 * @param sidewaysMaxDistance   Maximum allowed distance between matching phys limbs next to each other
 * @param capsulePairs          Pair of sphere colliders used as capsule colliders
 * @param capsuleCount          Number of pairs of sphere colliders contained in capsulePairs
 */
void Overcoat_SolvePhysAtSubsteps(Overcoat* this, Player* player, Vec3f apply_force, PhysLimb* gPhysLimbs[], 
	PhysBone* gPhysBones[], Vec3f limbsPrevPos[], const int chains[], int chainCount, 
    f32 sidewaysMaxDistance[][9], PhysSphereCollider* capsulePairs[][2], int capsuleCount) {
	/********************************
     Set up target positions of current frame
    ********************************/
    // Store current frame's target world position of each phys limb in current frame.
	// For pinned limbs, this becomes the target position they move toward across the substeps. This says
	// where each pinned limb should end up by the end of the frame.
    static Vec3f limbsCurrTargetPos[OVERCOAT_BODYPART_MAX];
	
	// Store target length of each bone for current frame, which says how long each bone should be by the
	// end of frame
    static f32 targetBoneLengths[OVERCOAT_BONE_MAX];
	
	// Store pointers to all sphere colliders used by overcoat's limbs
	// Most of these sphere colliders are to be used for capsule colliders
    PhysSphereCollider* colliderPointers[OVERCOAT_SUBSTEP_COLLIDER_COUNT] = {
        &overcoat_leftThighBackCollider, &overcoat_rightThighBackCollider,
        &overcoat_leftThighFrontCollider, &overcoat_rightThighFrontCollider,
        &overcoat_leftLegCollider, &overcoat_rightLegCollider,
        &overcoat_torsoCollider, &overcoat_rollCollider
    };
	
	// For current frame, store each collider's target position and radius for current frame.
	// Like the limbs, colliders' positions will change every substep.
	// Colliders' radii would change for capsule colliders with differently-sized sphere colliders.
    PhysSphereCollider overcoat_currFrameTargetColliders[OVERCOAT_SUBSTEP_COLLIDER_COUNT];
	
	// Set up Jackie's forward, down, and left directions in world space.
	// The overcoat uses these directions to keep its connected cloth chains moving correctly
	// relative to Jackie's body.
	// This system is designed for overcoat specifically because it has chains of limbs that are horizontally 
	// connected to each other. So, this directional system is set up to help with both vertical AND 
	// horizontal constraints, whereas belt strap hook and ponytail only has vertical constraints
    Vec3f targetForward = {0.f, 0.f, 0.f};
    Vec3f targetDown = {0.f, 0.f, 0.f};
    Vec3f targetLeft = {0.f, 0.f, 0.f};

    Math_Vec3f_Copy(&targetForward, &overcoat_forwardAxis);
    Math_Vec3f_Copy(&targetDown, &overcoat_downAxis);
    Math_Vec3f_Copy(&targetLeft, &overcoat_leftAxis);
	
	// Store Jackie's current actor scale as the target scale for this current frame.
    f32 targetScale = fabsf(this->actor.scale.x);
	
	// Divide one frame (delta time for Verlet Integration) into smaller substeps.
	// Each substep simulates a user-defined fraction of the time between the previous frame and the current
	// frame. This allows Verlet Integration, constraints, and collisions to be solved through smaller, more
	// gradual changes in position instead of one large frame-to-frame change.
	// This helps reduce the chances of the overcoat cloth clipping through the colliders, as change in 
	// position would be measured several times within a frame as opposed to only twice: at the previous frame 
	// and the current frame.
    f32 dt = 1.f / OVERCOAT_SUBSTEPS;
	
	// Store Jackie's horizontal movement in current frame so that the overcoat can react to the motion
    Vec3f opposingVelocity = {0.f, 0.f, 0.f};
    Math_Vec3f_Copy(&opposingVelocity, &player->actor.velocity);

    // While riding, use movement measured from world positions.
	// Riding Epona: PLAYER_STATE1_800000
    if (player->stateFlags1 & PLAYER_STATE1_800000) {
        opposingVelocity = gOvercoatEpona.world_vel;
    }

    opposingVelocity.y = 0.f;	// ignore vertical movement

    // Disable the opposing movement while climbing ladder/wall or opening door.
    // Climbing Wall: PLAYER_STATE1_200000
    // Opening Door: Player_Action_36
    if ((player->stateFlags1 & PLAYER_STATE1_200000) || player->actionFunc == Player_Action_36) {
        opposingVelocity = (Vec3f){0.f, 0.f, 0.f};
    }

	/********************************
     Save target colliders' state in current frame
    ********************************/
	// Save each collider's target position and radius for current frame.
	// During the substeps, the colliders will move from their position in the previous frame to their target
	// position in the current frame, substep by substep.
    for (int c = 0; c < OVERCOAT_SUBSTEP_COLLIDER_COUNT; c++) {
        overcoat_currFrameTargetColliders[c] = *colliderPointers[c];
    }
	
	/********************************
     Save phys limbs' target positions in current frame
    ********************************/
	// Save each phys limb's target world positions at the current frame.
	// Note: For pinned phys limbs, this position was already set by Overcoat_UpdatePinnedLimb()
    for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
        limbsCurrTargetPos[i] = gPhysLimbs[i]->curr_pos;
    }

	/********************************
     Initialize previous frame's history
    ********************************/
	// If there is no valid previous-frame physics history, use the current frame's target values as both the
	// previous frame and current frame values
	// This prevents the first set of substeps from interpolating from outdated or invalid state
    if (!overcoat_stepHistoryReady) {
		// Treat each collider's current frame target state as previous frame state
		// This makes collider start in the correct place instead of moving from unintended positions
        for (int currSphereCol = 0; currSphereCol < OVERCOAT_SUBSTEP_COLLIDER_COUNT; currSphereCol++) {
            overcoat_prevFrameColliders[currSphereCol] = overcoat_currFrameTargetColliders[currSphereCol];
        }
		
		// Initialize starting positions of phys limbs
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
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
        Math_Vec3f_Copy(&overcoat_previousFrameForward, &targetForward);
        Math_Vec3f_Copy(&overcoat_previousFrameDown, &targetDown);
        overcoat_previousSubstepScale = targetScale;

		// Save the current y-axis rotation along world space as previous frame's y-axis rotation
        overcoat_previousFrameYaw = this->actor.shape.rot.y;
    }


	/********************************
     Skip verlet for certain amount of yaw
    ********************************/
	// The turning animation of Jackie (Link, specifically) has her just snap in the direction the control
	// stick was heading to. This doesn't work well with Verlet integration, which specifically relies on 
	// actual movement/rotation of Jackie's body.
	// So, this part handles a situation where the body has to snap to a specific direction in one frame
	// rather than properly rotate in one direction. If the direction along the y-axis (rotating 
	// left/right) is greater than what is defined for OVERCOAT_MAX_YAW, then the verlet integration is 
	// skipped for this case.
	
	// Convert the max rotation along the y-axis (yaw) into s16 degrees.
	s16 maxYaw = CustomMath_DegreeToBinaryAngle(OVERCOAT_MAX_YAW);
		
    // Calculate the change in the overcoat's facing angle since the previous frame
    s16 yawDelta = (s16)(this->actor.shape.rot.y - overcoat_previousFrameYaw);

    // Check if Jackie's turn from previous frame to current frame was over the specified OVERCOAT_MAX_YAW
    // The current change in yaw will be subtracted by the yaw limit OVERCOAT_MAX_YAW to determine how
    // much of the rotation should forgo the verlet integration and just rotate along the overcoat.
    s16 yawToSkipVerlet = 0;
    // If current change in yaw is greater than OVERCOAT_MAX_YAW, then record how much of the yaw should 
    // skip the verlet integration.
    if (yawDelta > maxYaw) {
        yawToSkipVerlet = yawDelta - maxYaw;
    }
    // If current change in yaw is less than the negative of OVERCOAT_MAX_YAW, then record how much the
    // yaw should skip the verlet integration.
    else if (yawDelta < -maxYaw) {
        yawToSkipVerlet = yawDelta + maxYaw;
    }

	// If Jackie's yaw is greater than OVERCOAT_MAX_YAW, then show the verlet integration that would occur 
	// on overcoat up to specified OVERCOAT_MAX_YAW. Outside of the physics effect, the phys limbs along
    // with the colliders are then just rotated around the overcoat root limb.
    if (yawToSkipVerlet != 0) {
		// Set the overcoat root limb as the point where the phys limbs would rotate around.
        Vec3f limbsPivotPoint = limbsPrevPos[OVERCOAT_BODYPART_ROOT];

        // Rotate cloth and its previous position together, essentially skipping the application of verlet
        // integration on this amount of yaw rotation.
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
            // Pinned phys limb rotation
            if (gPhysLimbs[i]->pinned) {
                Overcoat_RotatePointAroundY(&limbsPrevPos[i], &limbsPivotPoint, yawToSkipVerlet);
            }
            // Unpinned phys limb rotation (previous position also has to be rotated alongside to avoid
            // verlet integration caused by change in position, which is also essentially velocity)
            else {
                Overcoat_RotatePointAroundY(&gPhysLimbs[i]->curr_pos, &limbsPivotPoint, yawToSkipVerlet);
                Overcoat_RotatePointAroundY(&gPhysLimbs[i]->prev_pos, &limbsPivotPoint, yawToSkipVerlet);
            }
        }

        // Rotate the previous frame's colliders positions and axes also by the same amount
        for (int currSphereCol = 0; currSphereCol < OVERCOAT_SUBSTEP_COLLIDER_COUNT; currSphereCol++) {
            Overcoat_RotatePointAroundY(&overcoat_prevFrameColliders[currSphereCol].center, 
				&limbsPivotPoint, yawToSkipVerlet);
        }

        // Rotate the forward and down directions of the overcoat by the same amount also
        CustomMath_Vec3f_RotateByY(&overcoat_previousFrameForward, yawToSkipVerlet, 
			&overcoat_previousFrameForward);
        CustomMath_Vec3f_RotateByY(&overcoat_previousFrameDown, yawToSkipVerlet, 
            &overcoat_previousFrameDown);
    }

    // Only this part of the turn is simulated across substeps
    s16 remainingYaw = (s16)(yawDelta - yawToSkipVerlet);

	/********************************
     Calculate Current Frame's Target Bone Lengths
    ********************************/
	// Calculate target world space length of every overcoat bone for the current frame.
	// Each target length is based on the bone's default local offset and Jackie's current target scale.
	// Keep note that while bone length is normally constant, the lengths would actually change if 
	// Jackie's actor scale changes (e.g. Giant's Mask transformation phase)
	
	// Note: Root-LimbA bones are technically irrelevant here and skipped later because they contain only 
	// pinned limbs.
    for (int j = 0; j < OVERCOAT_BONE_MAX; j++) {
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
	// overcoat.
	
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
    for (int currSubstep = 0; currSubstep < OVERCOAT_SUBSTEPS; currSubstep++) {
		// Calculate how far this substep is in the range between previous and current frame
		// For instance, if SUBSTEP is 4 (as in 4 substeps within a frame), then...
		// substep 0 = 25%, substep 1 = 50%, substep 2 = 75%, substep 3 = 100% 
        f32 t = (f32)(currSubstep + 1) / OVERCOAT_SUBSTEPS;
		
		/********************************
         1. Interpolate actor scale
        ********************************/
		// In case there is a change in Jackie's actor size between previous frame and current frame, 
		// interpolate Jackie's actor scale from the previous frame's scale toward the current frame's
		// target scale considering the current substep (t)
        f32 substepScale = overcoat_previousSubstepScale + 
			((targetScale - overcoat_previousSubstepScale) * t);		// based on interpolation formula
		
		// Calculate how much the current substep's scale differs from current frame's target scale.
		// This ratio is used to scale bone lengths and sideways constraint for this substep.
        f32 scaleRatio = 1.f;
		if (targetScale > GREATER_THAN_ZERO) {
			scaleRatio = substepScale / targetScale;
		}

		/********************************
         2. Interpolate colliders' position based on remaining yaw
        ********************************/
		// If Jackie's yaw was greater than the specified OVERCOAT_MAX_YAW, chances are, there is still
		// some yaw remaining that needs to be applied to the rotation of the overcoat.
		// However, keep note that by this point, the rotation that we did not want the verlet integration
		// to be applied to all phys limbs (both pinned and unpinned) has already been dealt with. At this
		// point, we want to properly rotate the colliders and pinned phys limbs by the rest of the 
		// remaining yaw (remainingYaw). That way, the verlet integration can be applied to the pinned
		// phys limbs via movement of the pinned phys limbs caused by the remaining yaw.
		
		// At this stage, move each collider to a new position between the previous frame's position and 
		// the current frame's target position for this current substep (t). When doing this, both
		// remaining yaw (remainingYaw) from prior and animation-based limb position change are used.
        for (int currSphereCol = 0; currSphereCol < OVERCOAT_SUBSTEP_COLLIDER_COUNT; currSphereCol++) {
			// Update colliders' world positions at current substep.
            Overcoat_InterpolatePosWithRot(&overcoat_prevFrameColliders[currSphereCol].center, 
				&overcoat_currFrameTargetColliders[currSphereCol].center, 
				&limbsPrevPos[OVERCOAT_BODYPART_ROOT], &limbsCurrTargetPos[OVERCOAT_BODYPART_ROOT], 
				remainingYaw, t, &colliderPointers[currSphereCol]->center);

			// Update the colliders' radii based on their change in position.
            colliderPointers[currSphereCol]->radius = overcoat_prevFrameColliders[currSphereCol].radius +
                (overcoat_currFrameTargetColliders[currSphereCol].radius - 
				overcoat_prevFrameColliders[currSphereCol].radius) * t;
        }
		
		/********************************
         3. Interpolate directional reference axes
        ********************************/
		// Set Jackie's forward and down directions (in world space) to the orientation they should have 
		// during the current substep. These directions are interpolated between the previous frame's
		// directional references and the current frame's target directional references of Jackie.
        if (overcoat_dirAxesReady) {
            Overcoat_SetSubstepDirAxes(targetForward, targetDown, t);
        }
		
		/********************************
         4. Update bone lengths based on current substep
        ********************************/
		// Set each bone's length to the properly scaled value for the current substep
        for (int j = 0; j < OVERCOAT_BONE_MAX; j++) {
            gPhysBones[j]->bone_length = targetBoneLengths[j] * scaleRatio;
        }
		
		/********************************
         5-6. Move pinned and unpinned phys limbs
        ********************************/
		// Update every phys limb of the overcoat for the current substep.
		// Pinned limbs are moved to their interpolated positions according to current substep.
		// Unpinned limbs are moved using Verlet Integration based on current substep's data.
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
            PhysLimb* currPhysLimb = gPhysLimbs[i];
			// For pinned phys limb...
            if (currPhysLimb->pinned) {
				// Move pinned phys limb to the interpolated position it should have during current substep. 
                // The root lerps normally since it's the pivot; every other pinned limb orbits around the 
                // root so large turns sweep instead of cutting through the torso.
                if (i == OVERCOAT_BODYPART_ROOT) {
                    CustomMath_Vec3f_Lerp(&limbsPrevPos[i], &limbsCurrTargetPos[i], t, 
                        &currPhysLimb->curr_pos);
                }
                else {
                    Overcoat_InterpolatePosWithRot(&limbsPrevPos[i], &limbsCurrTargetPos[i],
                        &limbsPrevPos[OVERCOAT_BODYPART_ROOT], &limbsCurrTargetPos[OVERCOAT_BODYPART_ROOT],
                        remainingYaw, t, &currPhysLimb->curr_pos);
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
        int iterations = COLLISION_FACTOR / OVERCOAT_SUBSTEPS;
		
		// Add an iteration if there is any remaining
        if (currSubstep < COLLISION_FACTOR % OVERCOAT_SUBSTEPS) {
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
		// Each iteration fixes bone lengths, limits sideways stretching between cloth chains 
		// horizontally, applies upper overcoat movement restrictions, and pushes horizontal midpoint
		// between cloth limb out of colliders
        for (int iteration = 0; iteration < iterations; iteration++) {

            // =========== Fix the phys bones' lengths to match its original length ==========

			// Correct every phys bone's length so that it matches the calculated respective target lengths 
            // for this substep
            for (int j = 0; j < OVERCOAT_BONE_MAX; j++) {
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

            // ============== Limit sideways distance of overcoat's phys limbs ==============
			
			// For each chain of phys limbs in overcoat, iterate through each phys limb in a given chain
			// and limit its horizontal distance from the next chain's respective phys limb so that the
			// overcoat mesh does not stretch too far apart sideways.
            for (int chain = 0; chain < chainCount - 1; chain++) {
				// Iterating through each phys limb in a current chain of phys limbs
                for (int row = 1; row < 9; row++) {
					// Limit the horizontal distance between current phys limb and next chain's phys limb
					// at the same row by the predetermined horizontal limit defined in sidewaysMaxDistance
                    Overcoat_LimitSidewaysDistance(
                        gPhysLimbs[chains[chain] + row], gPhysLimbs[chains[chain + 1] + row],
                        sidewaysMaxDistance[chain][row] * scaleRatio);
                }
				// Last chain is skipped since there is not supposed to be any max distance limit between
				// two front-most chains in the overcoat.
            }

            // ============ Limit the rotation of overcoat's chains of phys limbs ============
			
			// Limit the direction each upper overcoat bone can bend relative to Jackie's body.
			// In other words, this applies directional constraints to the phys bones to prevent unwanted
            // physics behavior. It is the most artificial element of this physics project. 
            // Check function for more details.
            Overcoat_ApplyChainRotationLimits(gPhysLimbs, chains, chainCount, &this->rotLimits);

            // ============= Push overcoat's unpinned phys limbs out of colliders =============
			
			// Apply collision correction to all the unpinned phys limbs. If a phys limb position is 
			// calculated to be inside a collider in this substep, push it back out.
            for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
				// Skip pinned phys limbs
                if (gPhysLimbs[i]->pinned) {
                    continue;
                }
				
				// Take all the colliders and correct the unpinned phys limb's position if they are inside
                // said colliders.
                Overcoat_SolveColliderGroup(gPhysLimbs[i], capsulePairs, capsuleCount);
            }

            // ======== Push midpoint of two horizontally neighboring phys limbs out of colliders ========
			
			// Check the midpoint between neighboring phys limbs in the same row from a current chain of
			// phys limbs and the next chain of phys limbs. If that midpoint is inside one of the colliders,
			// then push the midpoint area back out.
			// This is done to prevent the cloth model's two chains from clipping through the body.
            for (int chain = 0; chain < chainCount - 1; chain++) {
				// Check row of current chain and the next chain
                for (int row = 1; row < 9; row++) {
					// Take midpoint between current chain's phys limb and next chain's phys limb in current
					// row and push it out of colliders.
                    Overcoat_SolveMidpointGroup(
                        gPhysLimbs[chains[chain] + row], gPhysLimbs[chains[chain + 1] + row], 
                        capsulePairs, capsuleCount);
                }
            }
        }

        /********************************
         7c. Re-do Collider Corrections for current substep
        ********************************/
        for (int currPhysLimbIdx = 0; currPhysLimbIdx < OVERCOAT_BODYPART_MAX; currPhysLimbIdx++) {
            if (!gPhysLimbs[currPhysLimbIdx]->pinned) {
                Overcoat_SolveColliderGroup(gPhysLimbs[currPhysLimbIdx], capsulePairs, capsuleCount);
            }
        }

        /********************************
         8. Recalculate phys limb velocities from corrected positions
        ********************************/
        // Recalculate phys limb velocities from corrected positions in current substep vs previous substep.
        for (int currPhysLimbIdx = 0; currPhysLimbIdx < OVERCOAT_BODYPART_MAX; currPhysLimbIdx++) {
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
    for (int c = 0; c < OVERCOAT_SUBSTEP_COLLIDER_COUNT; c++) {
        *colliderPointers[c] = overcoat_currFrameTargetColliders[c];
        overcoat_prevFrameColliders[c] = overcoat_currFrameTargetColliders[c];
    }

    // Restore all pinned phys limbs to their current frame's target world positions
    for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
        if (gPhysLimbs[i]->pinned) {
            gPhysLimbs[i]->curr_pos = limbsCurrTargetPos[i];
        }
    }

    // Restore every bone to its exact target length for the current frame.
    for (int j = 0; j < OVERCOAT_BONE_MAX; j++) {
        gPhysBones[j]->bone_length = targetBoneLengths[j];
    }

    // Restore Jackie's directional references back to current frame's original directional references
    Math_Vec3f_Copy(&overcoat_forwardAxis, &targetForward);
    Math_Vec3f_Copy(&overcoat_downAxis, &targetDown);
    Math_Vec3f_Copy(&overcoat_leftAxis, &targetLeft);

    // Save current frame's directional references and scale as previous frame's values for next frame
    Math_Vec3f_Copy(&overcoat_previousFrameForward, &targetForward);
    Math_Vec3f_Copy(&overcoat_previousFrameDown, &targetDown);
    overcoat_previousSubstepScale = targetScale;
    overcoat_previousFrameYaw = this->actor.shape.rot.y;

    // Raise flag that substep are done running and previous frame's values are now fully established
    // for next frame to use for physics operations
    overcoat_stepHistoryReady = 1;
}


/**
 * @brief Update the a given pinned phys limb based on the overcoat's root limb
 *
 * This function takes a pinned phys limb from the overcoat using a given index value and updates its position
 * in the world space. This is done by taking the phys limb's default joint position, which describes the 
 * translation from the parent limb (root limb of overcoat) during overcoat's idle state (-Z). Afterward, this
 * translation vector is rotated based on the overcoat's actor's rotation in the world space.
 *
 * Keep note that this function only updates the position of the pinned phys limb.
 *
 * @param this			Overcoat actor
 * @param player		Player actor
 * @param gPhysLimbs	Phys limbs of belt strap hook. 
 * @param pinnedLimbIdx	Pinnned limb
 */
void Overcoat_UpdatePinnedLimb(Overcoat* this, Player* player, PhysLimb* gPhysLimbs[], int pinnedLimbIdx) {
	// Take pinned limb (the first limb of the chain, which is also after root limb)
	PhysLimb* physLimb = gPhysLimbs[pinnedLimbIdx];
	
	// Save pinned phys limb's previous position and velocity
	Math_Vec3f_Copy(&physLimb->prev_pos, &physLimb->curr_pos);
	Math_Vec3f_Copy(&physLimb->prev_vel, &physLimb->curr_vel);
	
	// Calculate pinned limb's offset from the overcoat root
	Vec3f transformVec = { 0.f, 0.f, 0.f };
	Vec3s rootRotatedOffset = { 0, 0, 0 };
	Vec3s worldRotatedOffset = { 0, 0, 0 };
		
	// Take pinned phys limb's default joint position (which is just local translation from parent limb
	// during idle state) and rotate it by the current rotation of the root limb. This gives a rotated
	// translation vector.
	CustomMath_Vec3s_Rotate(&physLimb->default_jointPos, &this->skelAnime.jointTable[OVERCOAT_ROOT_ROT],
        &rootRotatedOffset);
	// Take the rotated translation vec and rotate it by the player's actor's shape's rotation to make 
	// the phys limb face the direction of the player. Note that this essentially makes the phys limb's 
	// translation vec respective to world space rather than just local to its own skeleton
    CustomMath_Vec3s_Rotate(&rootRotatedOffset, &player->actor.shape.rot, &worldRotatedOffset);
	// Scale phys limb's rotated world space translation vec from root limb according to model
    CustomMath_Vec3s_Scale_ToVec3f(&worldRotatedOffset, this->actor.scale.x, &transformVec);

	// Apply rotated translation vector to root limb's position to find pinned limb's position
	Math_Vec3f_Sum(&this->bodyPartsPos[OVERCOAT_BODYPART_ROOT], &transformVec, 
        &this->bodyPartsPos[pinnedLimbIdx]);

	// Save the calculated new current position of the actual limb into respective phys limb
	Math_Vec3f_Copy(&physLimb->curr_pos, &this->bodyPartsPos[pinnedLimbIdx]);
	// Save current velocity to phys limb also
	Math_Vec3f_Copy(&physLimb->curr_vel, &player->actor.velocity);
}


/**
 * @brief Update the colliders' radii based on the current action/state of the player.
 *
 * This function uses the pre-made colliders and their pre-defined radii values. The appropriate
 * radius values are selected based on the current state/action of the player.
 *
 * @param player	Player instance
 */
void Overcoat_UpdateColliderRadii(Player* player) {
	// Convert the selected radii into current player's actor size.
    f32 colliderScale = player->actor.scale.x / 0.01f;

	// Initialize colliders radii with preset radii values.
	f32 torsoRadius = TORSO_COLLIDER_DEFAULT_RADIUS;
    f32 leftThighBackRadius = LEFTTHIGH_COLLIDER_DEFAULT_RADIUS;
    f32 rightThighBackRadius = RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS;
	f32 leftThighFrontRadius = LEFTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS;
	f32 rightThighFrontRadius = RIGHTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS;
    f32 leftLegRadius = LEFTLEG_COLLIDER_DEFAULT_RADIUS;
    f32 rightLegRadius = RIGHTLEG_COLLIDER_DEFAULT_RADIUS;
	f32 rollTorsoRadius = ROLL_COLLIDER_DEFAULT_RADIUS;

	// Change torso collider size to smaller preset radius value if Jackie is Kamaro dancing.
    if (Overcoat_IsKamaroDancing(player)) {
        torsoRadius = TORSO_COLLIDER_SMALL_RADIUS;
    }

    // Change torso collider size to smaller preset radius value if Jackie is doing a Spin Attack.
    if (Overcoat_IsSpinAttacking(player)) {
        torsoRadius = TORSO_COLLIDER_SMALL_RADIUS;
    }
	
	// Change back thigh collider sizes and knee collider sizes if Jackie is on Epona going fast or 
	// spurring.
	if (Overcoat_IsFastRidingOrSpurring(player)) {
        leftThighBackRadius = LEFTTHIGH_COLLIDER_FAST_RIDE_RADIUS;
        rightThighBackRadius = RIGHTTHIGH_COLLIDER_FAST_RIDE_RADIUS;
        leftLegRadius = LEFTLEG_COLLIDER_FAST_RIDE_RADIUS;
        rightLegRadius = RIGHTLEG_COLLIDER_FAST_RIDE_RADIUS;
    }
	
	// Update colliders with set radii values and input actor scale.
	overcoat_torsoCollider.radius = torsoRadius * colliderScale;
    overcoat_leftThighBackCollider.radius = leftThighBackRadius * colliderScale;
    overcoat_rightThighBackCollider.radius = rightThighBackRadius * colliderScale;
    overcoat_leftThighFrontCollider.radius = leftThighFrontRadius * colliderScale;
    overcoat_rightThighFrontCollider.radius = rightThighFrontRadius * colliderScale;
    overcoat_leftLegCollider.radius = leftLegRadius * colliderScale;
    overcoat_rightLegCollider.radius = rightLegRadius * colliderScale;
    overcoat_rollCollider.radius = rollTorsoRadius * colliderScale;
}


/**
 * @brief Update the positions of phys limbs of overcoat based on verlet integration
 * 
 * This function directly modifies the position of every phys limb of the overcoat using verlet integration. 
 * It uses the following to calculate the position of every phys limb for the frame:
 * 
 * 1. Player's current position, rotation, velocity, and actor scale
 * 
 * 2. Current and previous positions of overcoat's phys limbs
 * 
 * 3. Pinned phys limb positins based on the overcoat's root limb
 * 
 * 4. Length of phys bones containing two phys limbs as endpoints
 * 
 * 5. Maximum sideways horizontal distance between neighboring cloth chains of phys limbs
 * 
 * 6. Collision with colliders assigned to Jackie for overcoat
 * 
 * 7. Artificial directional constraints that restrict how the overcoat can move relative to player
 * 
 * The function also calculates the overcoat's current size based on the player's actor's size every substep, 
 * which are basically smaller frames divided from a single frame.
 * 
 * @param this          Overcoat actor
 * @param player        Player actor
 * @param apply_force   Net force applied to overcoat which causes change in position via Verlet Integration
 * @param gPhysLimbs    Phys limbs of the overcoat
 * @param gPhysBones    Phys bones of the overcoat
 */
void Overcoat_UpdateBodyPartsPos(Overcoat* this, Player* player, Vec3f apply_force, PhysLimb* gPhysLimbs[], 
	PhysBone* gPhysBones[]) {
    // Apply rotation limit and reset before doing anything else.
    //Overcoat_UpdateRotLimitsAndReset(this, player);

    // Save Jackie's previous position as overcoat's previous position
    Math_Vec3f_Copy(&this->actor.prevPos, &player->actor.prevPos);

    /********************************
     Update Epona movement
    ********************************/
    // Measure Jackie's world movement while riding Epona.
    // Restart the movement measurement when the overcoat physics resets.
    Verlet_UpdatePhysEponaMovement(&gOvercoatEpona, player, this->needsReset);

    /********************************
     Reset phys limbs after map change
    ********************************/
    // Rebase all phys limbs' positions after entering a new map so old physics state does not cause the 
    // overcoat to jump or move unpredictably
    if (this->needsReset) {
        this->needsReset = 0;
        overcoat_stepHistoryReady = 0;

        // Reset root of overcoat to current torso position
        Math_Vec3f_Copy(&gPhysLimbs[OVERCOAT_BODYPART_ROOT]->curr_pos, &overcoat_torso_globalPos);
        Math_Vec3f_Copy(&gPhysLimbs[OVERCOAT_BODYPART_ROOT]->prev_pos, &overcoat_torso_globalPos);

        // Reset rest of overcoat's limbs to current torso position
        for (int i = OVERCOAT_BODYPART_ROOT; i < OVERCOAT_BODYPART_MAX; i++) {
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_pos, &overcoat_torso_globalPos);
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_pos, &overcoat_torso_globalPos);

            // Clear current and previous velocity values so no movement from previous map is carried over
            Vec3f zero_velocity = { (f32)0, (f32)0, (f32)0 };
            Math_Vec3f_Copy(&gPhysLimbs[i]->curr_vel, &zero_velocity);      // current vel
            Math_Vec3f_Copy(&gPhysLimbs[i]->prev_vel, &zero_velocity);      // previous vel
        }
        //return;
    }

    // Save all of overcoat phys limbs' previous frame's world positions.
    // This is necessary because the overcoat interpolates pinned phys limbs' positions across multiple 
    // substeps instead of moving them directly from one frame position to the next.
    static Vec3f pinnedLimbsPrevPos[OVERCOAT_BODYPART_MAX];
    for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
        pinnedLimbsPrevPos[i] = gPhysLimbs[i]->curr_pos;
    }

    /********************************
     Sync overcoat actor with Jackie
    ********************************/
    // Match Jackie's scale so overcoat actor scales correctly with giant's mask transformation
    Math_Vec3f_Copy(&this->actor.scale, &player->actor.scale);

    // Assign overcoat actor's position to Jackie's torso limb's position
    Math_Vec3f_Copy(&this->actor.world.pos, &overcoat_torso_globalPos);

    // Make overcoat actor face the same general direction as Jackie
    Math_Vec3s_Copy(&this->actor.shape.rot, &player->actor.shape.rot);

    /********************************
     Set root position
    ********************************/
    // Set overcoat's root limb's bodyPartsPos (world position) to torso's world position
    Math_Vec3f_Copy(&this->bodyPartsPos[OVERCOAT_BODYPART_ROOT], &overcoat_torso_globalPos);

    // Copy world position of overcoat's root limb to the root phys limb
    Math_Vec3f_Copy(&gPhysLimbs[OVERCOAT_BODYPART_ROOT]->curr_pos, &this->actor.world.pos);

    // Set overcoat actor's jointTable root position
    Vec3s newRootJointPos = { 0, 0, 0};
    Math_Vec3s_Copy(&this->skelAnime.jointTable[LIMB_ROOT_POS], &newRootJointPos);

    /********************************
     Calculate root rotation
    ********************************/
    // Convert Jackie's animated torso rotation into overcoat root's coordinate system.
    Vec3s newRootJointRot = { 0, 0, 0 };
    newRootJointRot.x = -16384 + overcoat_torso_rotate.z;
    newRootJointRot.y = -16384 + overcoat_torso_rotate.y - this->actor.shape.rot.y;
    newRootJointRot.z = -overcoat_torso_rotate.x;

    // Store the final root rotation used by the belt strap hook skeleton
    Math_Vec3s_Copy(&this->skelAnime.jointTable[OVERCOAT_ROOT_ROT], &newRootJointRot);

    /********************************
     Set up pinned chain anchors
    ********************************/
    // Update every pinned phys limb's world position so each cloth chain starts from its current
    // world position based on the overcoat root limb's position
	for (int chain = 0; chain < OVERCOAT_CHAIN_COUNT; chain++) {
		Overcoat_UpdatePinnedLimb(this, player, gPhysLimbs, cloth_chains[chain].startLimb);
	}

    /********************************
     Update body colliders' size
    ********************************/
    Overcoat_UpdateColliderRadii(player);

    /********************************
     Set up sideways cloth connections for horizontal distance limits
    ********************************/
    // Neighboring phys limbs are connected while the front opening (right limb 0 and left limb 0)
	// stays unconnected.
	int sidewaysChains[OVERCOAT_CHAIN_COUNT];
	int sidewaysChainCount = OVERCOAT_CHAIN_COUNT;

	// Connect the neighboring first phys limbs together.
	for (int chain = 0; chain < sidewaysChainCount; chain++) {
		sidewaysChains[chain] = cloth_chains[chain].startLimb;
	}

    /********************************
     Set up capsule colliders with sphere collider pairs
    ********************************/
    // Two sphere colliders are paired together to create a capsule collider
    // Capsule colliders essentially cover the hips and legs to prevent the overcoat from clipping
    // through Jackie's body between individual sphere colliders.
    PhysSphereCollider* capsulePairs[][2] = {
        // Permanent thigh and leg capsules.
        {&overcoat_leftThighBackCollider, &overcoat_rightThighBackCollider},
        {&overcoat_leftThighFrontCollider, &overcoat_rightThighFrontCollider},
        {&overcoat_leftThighFrontCollider, &overcoat_leftLegCollider},
        {&overcoat_leftThighBackCollider, &overcoat_leftLegCollider},
        {&overcoat_rightThighFrontCollider, &overcoat_rightLegCollider},
        {&overcoat_rightThighBackCollider, &overcoat_rightLegCollider},
        {&overcoat_rightThighFrontCollider, &overcoat_leftLegCollider},
        {&overcoat_leftThighFrontCollider, &overcoat_rightLegCollider},

        // Space for up to two optional spheres.
        {NULL, NULL},
        {NULL, NULL}
    };

    // Calculate number of capsule colliders
    // Start with only the permanent thigh and leg pairs.
    // Exclude the two reserved slots until they are filled.
    int capsuleCount = sizeof(capsulePairs) / sizeof(capsulePairs[0]) - OVERCOAT_RESERVED_COLLIDER_SLOTS;

    // Always include the torso sphere.
    // Its radius was selected above based on the animation.
    capsulePairs[capsuleCount][0] = &overcoat_torsoCollider;
    capsulePairs[capsuleCount][1] = &overcoat_torsoCollider;
    capsuleCount++;

    // Include the extra sphere collider during rolling or forward stabbing.
    if (Overcoat_IsRolling(player) || Overcoat_IsForwardStabbing(player)) {
        capsulePairs[capsuleCount][0] = &overcoat_rollCollider;
        capsulePairs[capsuleCount][1] = &overcoat_rollCollider;
        capsuleCount++;
    }

    // Exclude capsule colliders with the front thigh sphere collider during fast riding.
    // Compact the active pairs so the collision loops encounter no empty slots.
    if (Overcoat_IsFastRidingOrSpurring(player)) {
        // Index that helps overwrite capsule colliders with front thigh collider in capsulePairs array
        int keptCapsuleCount = 0;

        // Look through each capsule and check if that capsule includes a front thigh sphere collider
        for (int currCapsule = 0; currCapsule < capsuleCount; currCapsule++) {
            // Check if either the start or the end sphere collider of the current capsule is a
            // front thigh collider.
            PhysSphereCollider* start = capsulePairs[currCapsule][0];
            PhysSphereCollider* end = capsulePairs[currCapsule][1];

            u8 usesFrontThigh =(start == &overcoat_leftThighFrontCollider ||
                                start == &overcoat_rightThighFrontCollider ||
                                end == &overcoat_leftThighFrontCollider ||
                                end == &overcoat_rightThighFrontCollider);

            // Skip if one of the end sphere collider 
            if (usesFrontThigh) {
                continue;
            }

            // Overwrite capsule collider with front thigh collider in capsulePairs array.
            capsulePairs[keptCapsuleCount][0] = start;
            capsulePairs[keptCapsuleCount][1] = end;
            keptCapsuleCount++;
        }

        // Use the count only including capsule colliders without front thigh collider.
        capsuleCount = keptCapsuleCount;
    }

    // Create 2D array that will store maximum allowed sideways distance between neighboring cloth chains
    // 21 chains form 20 sideways connections.
    // Each connection contains matching rows A through I (9 rows total)
    f32 sidewaysMaxDistance[20][9];

    /********************************
     Calculate horizontal distance limits
    ********************************/
    // Calculate maximum allowed horizontal distance between matching phys limbs in neighboring vertical
    // cloth chains. Use each physlimb's default joint offset (idle) to find these distance limits.

    // Loop through every neighboring pair of vertical cloth chains
    for (int chain = 0; chain < sidewaysChainCount - 1; chain++) {
        // Store accumulated root limb-relative position of current row for each of the two
        // neighboring chains
        Vec3f defaultRootOffsetA = {0.f, 0.f, 0.f};
        Vec3f defaultRootOffsetB = {0.f, 0.f, 0.f};

        // Compare matching phys limbs from rows A to I
        for (int row = 0; row < 9; row++) {
            PhysLimb* physLimbA = gPhysLimbs[sidewaysChains[chain] + row];
            PhysLimb* physLimbB = gPhysLimbs[sidewaysChains[chain + 1] + row];

            // Add current phys limb's default offset from its parent phys limb to figure out where
            // this current phys limb is positioned relative to root limb of overcoat in idle pose
            Vec3f jointOffsetA = {physLimbA->default_jointPos.x, physLimbA->default_jointPos.y,
                physLimbA->default_jointPos.z};
            Vec3f jointOffsetB = {physLimbB->default_jointPos.x, physLimbB->default_jointPos.y,
                physLimbB->default_jointPos.z};

            // Accumulate each PhysLimb's rest-pose position relative to the overcoat root
            Math_Vec3f_Sum(&defaultRootOffsetA, &jointOffsetA, &defaultRootOffsetA);
            Math_Vec3f_Sum(&defaultRootOffsetB, &jointOffsetB, &defaultRootOffsetB);

            // Calculate scaled rest-pose distance between neighboring PhysLimbs
            f32 restDistance = Math_Vec3f_DistXYZ(&defaultRootOffsetA, &defaultRootOffsetB) * 
                fabsf(this->actor.scale.x);

            // Allow more sideways stretching toward the bottom of the coat
            f32 allowedStretch = 1.f + OVERCOAT_SIDE_BASE_SLACK + row * OVERCOAT_SIDE_ROW_SLACK;

            sidewaysMaxDistance[chain][row] = restDistance * allowedStretch;
        }
    }

    /********************************
     Reset overcoat phys limbs' positions after physics reset
    ********************************/
    // If there is no valid previous frame physics values, then reset the unpinned phys limbs into
    // valid pre-set cloth shape before starting the substep simulation
    if (!overcoat_stepHistoryReady) {
        // Wait until Jackie's torso direction references are set, since they are needed to
        // reset the overcoat shape
        if (!overcoat_dirAxesReady) {
            return;
        }
        
        // Reset the overcoat's chains of phys limbs based on previously assigned reset angle.
        Overcoat_SetResetShape(this, gPhysLimbs, cloth_chains, OVERCOAT_CHAIN_COUNT);

        // Use the reset phys limb positions as starting positions for the upcoming substep
        // physics operations
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
            pinnedLimbsPrevPos[i] = gPhysLimbs[i]->curr_pos;
        }
    }

    /********************************
     Physics Operations with Substeps
    ********************************/
    // Run the physics operations (e.g. position changes, constraints, etc.) across multiple
    // substeps.
    Overcoat_SolvePhysAtSubsteps(this, player, apply_force, gPhysLimbs, gPhysBones, pinnedLimbsPrevPos, 
        sidewaysChains, sidewaysChainCount, sidewaysMaxDistance,capsulePairs, capsuleCount);
}


/**
 * @brief Rotate bone with pinned parent limb while using rotation stabilizer
 * 
 * Rotate actual overcoat bone in the direction of the matching phys bone. To do this, find a stable 
 * shortest rotation possible for the actual bone. This is done by using a quaternion, which helps with:
 *
 * 1. finding a 3D axis on which the overcoat bone's limb A should rotate around
 *
 * 2. finding how far the overcoat bone's limb A should rotate on this axis
 *
 * This function has more rotation behaviors to limit the free rotation of pinned limb in bone.
 *
 * @param direction	Target direction from phys bone
 * @param rotation 	Rotation for actual bone of overcoat
 */
void Overcoat_RotateFromPinned(Vec3f* direction, Vec3s* rotation) {
    // Find length of direction vector of phys bone
    f32 lengthSquared = Math3D_Vec3fMagnitudeSq(direction);

    if (lengthSquared < GREATER_THAN_ZERO) {
        return;
    }

    // Normalize the direction vector, which says what direction limb A should be pointing to in this bone
	// This would describe the phys bone's direction {dx, dy, dz}, which should be where the actual overcoat bone
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
    f32 qz = 0.f;

    // Find rotation amount on axis {qx, qy, qz}
	// Dot product of current bone direction * target phys bone direction
    f32 qw = 1.f - dz;

    // Length of quaternion ^ 2
    f32 quatLengthSquared = (qx * qx) + (qy * qy) + (qz * qz) + (qw * qw);

    // If the target phys bone direction points towards +Z, then quaternion length becomes very nearly 0
    if (quatLengthSquared < GREATER_THAN_ZERO) {
        *rotation = (Vec3s){ -32768, 0, 0 };
        return;
    }

    // Normalize quaternion {qx, qy, qz, qw}
    f32 inverseQuatLength = 1.f / sqrtf(quatLengthSquared);
    qx *= inverseQuatLength;
    qy *= inverseQuatLength;
    qz *= inverseQuatLength;    // irrelevant since it's 0
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
    f32 zz = 1.f - (2.f * (qx * qx)) - (2.f * (qy * qy));

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
 * @brief Rotate given overcoat's chain's limbs (joints) based on phys bones' phys limbs' positions
 * 
 * This function takes a given overcoat chain and rotates its limbs so that it matches with the overcoat's phys
 * bones. More specifically, the actual overcoat's given chain's bones are rotated using a matrix so that the 
 * overcoat's bones match with the rotation of the phys bones.
 * 
 * @param this          Overcoat actor
 * @param gPhysBones    Phys bones that have been impacted by verlet integration for this frame
 * @param rootBoneIdx   Given overcoat chain's index indicating root bone, which has root limb and pinned limb
 * @param chainLength   Length of the given overcoat chain
 */
void Overcoat_RotateChain(Overcoat* this, PhysBone* gPhysBones[], int rootBoneIdx, int chainLength) {
    Vec3f zero = { 0.f, 0.f, 0.f };

    // Skip bone with root and pinned limbs
    s16 firstBone = rootBoneIdx + 1;            // First bone in chain, with limb A pinned and limb B unpinned
    s16 endBone = rootBoneIdx + chainLength;

    // Set up matrix for rotation operations on chain limbs
    Matrix_Push();

    // Rotate every phys bone that mathces respective bone in overcoat's chain
    for (s16 i = firstBone; i < endBone; i++) {
        PhysBone* physBone = gPhysBones[i];                     // Take matching phys bone
        Vec3s* jointRot = &this->skelAnime.jointTable[i + 1];   // Joint's rotation in overcoat skeleton
        Vec3f worldDir = { 0.f, 0.f, 0.f };                     // Direction of phys bone in respect to world
        Vec3f localDir = { 0.f, 0.f, 0.f };                     // Direction of phys bone in respect to limb A

        // Find world direction of current phys bone by doing:   worldDir = limb_b position - limb_a position
        Math_Vec3f_Diff(&physBone->limb_b->curr_pos, &physBone->limb_a->curr_pos, &worldDir);

        // Find length^2 of current phys bone in respect to world
        f32 lengthSquared = Math3D_Vec3fMagnitudeSq(&worldDir);

        // If length of target phys bone is not 0, then translate world direction of phys bone into local 
        // direction in respect to target phys bone's limb A.
        if (!(physBone->limb_a->pinned && physBone->limb_b->pinned) &&
            lengthSquared > GREATER_THAN_ZERO) {
            // Get current matrix, which is currently oriented in respect to limb_a of phys bone
            MtxF* boneLimbA = Matrix_GetCurrent();

            // Convert phys bone's world direction limb A's local space
            // localDir = transposed limb A's rotation * worldDir, which is also:
            // Dot product between each axis of limb A and world direction of phys bone
            // localDir.x = dot(limb A's X-axis, phys bone's world direction)
            // localDir.y = dot(limb A's Y-axis, phys bone's world direction)
            // localDir.z = dot(limb A's Z-axis, phys bone's world direction)
            CustomMath_Vec3f_InverseTransformDirection(boneLimbA, &worldDir, &localDir);

            // For bone where limb A is pinned and limb B is unpinned, calculate rotation with more limits 
            // to rotations. Limitations are set for bone with pinned limb A to avoid rotation that visually 
            // looks incorrect.
            if (i == firstBone) {
                Overcoat_RotateFromPinned(&localDir, jointRot);
            }
            // For every other bone where both limb A and limb B are unpinned, calculate rotation with more 
            // freedom.
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
        Matrix_TranslateRotateZYX(&zero, jointRot);
    }

    // Pop matrix
    Matrix_Pop();
}


/**
 * @brief Rotate overcoat' phys limbs (joints) based on the phys bones' phys limbs' current world positions.
 * 
 * This function takes the overcoat and rotates its limbs so that it matches with the overcoat's phys bones. 
 * This is done by rotating each chain of limbs in the overcoat at a time using the Overcoat_RotateChain() 
 * function.
 * 
 * @param this          Overcoat actor
 * @param gPhysBones    Physics bones that have been impacted by verlet integration for this frame
 * @param player        Player actor
 */
void Overcoat_RotateJoints(Overcoat* this, PhysBone* gPhysBones[], Player* player) {
    Vec3f zero = {0.f, 0.f, 0.f};

    /********************************
     Set shared root orientation
    ********************************/
    // Set up matrix to face in the direction of overcoat
    Matrix_Push();
    
    // Make matrix face overcoat actor's rotation (which should also be player actor's rotation)
    // Y-axis primarily defines actor's normal facing/yaw direction, so rotate by Y-axis first.
    Matrix_SetTranslateRotateYXZ(0.f, 0.f, 0.f, &this->actor.shape.rot);

    // Rotate matrix again, this time by the overcoat's root rotation (player's skeleton head limb)
    Matrix_TranslateRotateZYX(&zero, &this->skelAnime.jointTable[OVERCOAT_ROOT_ROT]);

    /********************************
     Rotate each overcoat chain
    ********************************/
    // Iterate through every chain of phys limbs in the overcoat to rotate its limbs one by one
    for (int currChainIdx = 0; currChainIdx < OVERCOAT_CHAIN_COUNT; currChainIdx++) {
        Overcoat_RotateChain(this, gPhysBones, cloth_chains[currChainIdx].startBone, 
            cloth_chains[currChainIdx].limbCount);
    }

    Matrix_Pop();
}



/**
 * @brief Update draw state of overcoat based on current state of player
 * 
 * This function updates the draw state of the overcoat based on the player's current state. In other 
 * words, it assigns a variable (hideInFirstPerson) indicating whether the overcoat should be drawn or 
 * not based on the following states:
 * 
 * 1. C-Up First Person View:
 * 
 *      - If the camera is in first person mode AND is less than a specific distance away from the player's 
 *        head, then hide the overcoat
 * 
 * 2. Bow/Hookshot First Person View
 * 
 *      - If the player is in first person view with bow or hookshot And the camera is less than a specific
 *        distance away from the player's head, then hide the overcoat
 * 
 * 3. Transformed into non-human
 *      - If the player transforms into a non-human form (Deku, Goron, Zora, FD), then hide the overcoat
 * 
 * @param thisx Overcoat actor
 * @param play  Current playstate
 */
void Overcoat_Update(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Overcoat* this = (Overcoat*)thisx;

    /********************************
     Request reset after target animations end
    ********************************/
    // Raise pending reset flag for resets that need to occur after specified animation ends.
    // By default, ensure that there are no pending resets based on end of animation.
    overcoat_pendingResetRotLimitUpdate = OVERCOAT_PENDING_RESET_NONE;

    // Request reset for after roll animation ends.
    u8 currRolling = Overcoat_IsRolling(player);

    if (Overcoat_WasRollingInPrevFrame && !currRolling) {
        overcoat_pendingResetRotLimitUpdate = OVERCOAT_PENDING_RESET_ROLL_END;
        this->needsReset = 1;
    }

    Overcoat_WasRollingInPrevFrame = currRolling;   // Save current roll anim state to prev frame

    // Request the ledge-climb reset once when climbing stops.
    u8 currClimbingFromLedge = Overcoat_IsClimbingFromLedge(player);
    
    if (Overcoat_WasClimbingInPrevFrame && !currClimbingFromLedge) {
        overcoat_pendingResetRotLimitUpdate = OVERCOAT_PENDING_RESET_CLIMB_LEDGE_END;
        this->needsReset = 1;
    }
    
    Overcoat_WasClimbingInPrevFrame = currClimbingFromLedge; // Save current climb anim state to prev frame

    /********************************
     Kamaro Dance Start Reset
    ********************************/
    // Reset when Kamaro Dance animation starts.
    // This is determined by if anim wasn't being played in the previous frame but is being played now.
    if (!Overcoat_WasKamaroDancingInPrevFrame && Overcoat_IsKamaroDancing(player)) {
        this->needsReset = 1;
    }

    Overcoat_WasKamaroDancingInPrevFrame = Overcoat_IsKamaroDancing(player);

    /********************************
     Kamaro Dance End Reset
    ********************************/
    // Restore the default reset shape when the dance ends.
    if (this->wasKamaroDancing && !Overcoat_IsKamaroDancing(player)) {
        this->needsReset = 1;
    }

    this->wasKamaroDancing = Overcoat_IsKamaroDancing(player);

    /********************************
     Epona Fast Ride Start Reset
    ********************************/
    // Reset when fast ride animation is enabled.
    // This is determined by if anim wasn't being played in the previous frame but is being played now.
    if (!Overcoat_WasFastRidingInPrevFrame && 
        (player->skelAnime.animation == &gPlayerAnim_link_uma_anim_fastrun)) {
        this->needsReset = 1;
    }

    Overcoat_WasFastRidingInPrevFrame = 
        (player->skelAnime.animation == &gPlayerAnim_link_uma_anim_fastrun);

    /********************************
     Giant's Mask Mask Off Start Reset
    ********************************/
    // Reset when Jackie takes the Giant's Mask off.
    // This is determined by if anim wasn't being played in the previous frame but is being played now.
    if (!Overcoat_WasTakingGiantMaskOffInPrevFrame && Overcoat_IsTakingGiantMaskOff(player)) {
        this->needsReset = 1;
    }

    Overcoat_WasTakingGiantMaskOffInPrevFrame = Overcoat_IsTakingGiantMaskOff(player);

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
    // Get reset positions of overcoats' phys limbs when transforming from non-human to human
    if (player->transformation != PLAYER_FORM_HUMAN) {
        this->needsReset = 1;
    }

    /********************************
     First-Person Camera
    ********************************/
    // Keep a reset pending while Jackie is in first person.
    if (Overcoat_IsInFirstPerson(play, player)) {
        this->needsReset = 1;
    }

    /********************************
     Transformation into Non-Human
    ********************************/
    // Remove overcoat when player transforms from human to non-human
    if (player->transformation != PLAYER_FORM_HUMAN && gPlayerOvercoat != NULL &&
        player->actor.draw == NULL) {
        Actor_Kill(&this->actor);
        gPlayerOvercoat = NULL;
        return;
    }


    /********************************
     Apply Reset and Rotation Limits
    ********************************/
    Overcoat_UpdateRotLimitsAndReset(this, player);
}


/*
=================
Overcoat Draw
=================
*/

/**
 * @brief Renders overcoat and operates its physics every frame
 * 
 * This function renders the overcoat model and operates its physics every frame. This is done by using 
 * physics functions based on Verlet Integration.
 * 
 * To accomplish this, three objects are set up:
 *      
 *      a) Overcoat Actor
 *          
 *          - A overcoat actor with the overcoat model and its limbs generated via Fast64
 *      
 *      b) gOvercoatPhysLimbs
 *          
 *          - A physics simulator object representing the overcoat model's limbs
 *      
 *      c) gOvercoatPhysBones
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
 *    integration is used to calculate the overcoat's limbs' new position. This information is stored in the 
 *    physics limbs and bones.
 * 
 * 3. Using the physics bones' limbs' positions, rotate the actual overcoat's bones so that each bone's limbs 
 *    match with the physics bones' limbs' positions.
 * 
 */
RECOMP_HOOK_RETURN ("Player_Draw") void main_Draw_Overcoat_with_Physics(void) {
    // Only draw when mod's config indicates that cadet outfit model needs to be drawn instead of 'default'
    if (!recomp_get_config_u32("change_outfit") || gPlayerOvercoat == NULL || gPlayStateOvercoat == NULL) {
        return;
    }
    
    Player* player = GET_PLAYER(gPlayStateOvercoat);
    PlayState* play = gPlayStateOvercoat;

    // Only draw when human and not transforming into non-human form
    if (recomp_get_config_u32("change_outfit") && gPlayerOvercoat != NULL && 
    player->transformation == PLAYER_FORM_HUMAN && player->actor.draw != NULL && 
    !(player->stateFlags2 & PLAYER_STATE2_20000000)) {
        Overcoat* this = gPlayerOvercoat;

        // Don't draw overcoat while in first person view with bow or hookshot
        if (Overcoat_IsInFirstPerson(play, player)) {
            this->needsReset = 1;
            return;
        }

        // Update overcoat physics only when game is not paused or in transition state into pause menu
        if (play->pauseCtx.state == 0) {
            // Calculate current net force using change in velocity (current vs previous frame) and gravity
            Vec3f net_force = { (f32)0, (f32)0, (f32)0 };    // Gravity + movement
            Verlet_UpdatePhysPlayerVelocity(&gOvercoatPhysPlayer, player); 
            Verlet_CalcNetForce(&gOvercoatPhysPlayer, (f32)GRAVITY, &net_force);

            // Update overcoat's positions based on Verlet Integration, and save new positions of limbs in 
            // phys limbs/bones
            Overcoat_UpdateBodyPartsPos(this, player, net_force, overcoat_PhysLimbs, overcoat_PhysBones);

            // Wait until the reset shape and substep history have been rebuilt.
            if (!overcoat_stepHistoryReady) {
                return;
            }

            // Calculate rotations based on overcoat limbs' calculated positions
            Overcoat_RotateJoints(this, overcoat_PhysBones, player);
        }

        // Draw overcoat with all the physics-based rotations applied on the actual overcoat phys limbs
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 
            &this->actor.shape.rot);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        func_80122868(play, player);    // Draw blinking effect of overcoat when player gets hit or jinxed
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
 * @brief Function unused; use main_Draw_Overcoat_with_Physics() instead.
 * 
 * All drawing functionality has been moved to main_Draw_Overcoat_with_Physics() due to this function not 
 * firing during the Song of Time, Inverted Song of Time, Double Song of Time cutscenes.
 * 
 * In other words, the overcoat model does not get drawn for the frame during these cutscenes, removing 
 * them from the game during the cutscene.
 */
void Overcoat_Draw(Actor* thisx, PlayState* play) {

}