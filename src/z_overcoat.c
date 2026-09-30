/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

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
#define LIMB_MASS   0.2f
#define PINNED      1
#define NOT_PINNED  0
#define COLLISION_FACTOR    75

#define OVERCOAT_LIMB_COLLIDER_RADIUS   0.3f
#define LEFTTHIGH_COLLIDER_DEFAULT_RADIUS 3.9f
#define RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS 3.9f
#define LEFTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS 3.45f
#define RIGHTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS 3.45f
#define LEFTLEG_COLLIDER_DEFAULT_RADIUS 2.8f
#define RIGHTLEG_COLLIDER_DEFAULT_RADIUS 2.8f
#define TORSO_COLLIDER_DEFAULT_RADIUS 4.f

#define OVERCOAT_CHAIN_COUNT 21
#define OVERCOAT_LIMBS_WITH_INWARD_LIMIT 3
#define OVERCOAT_EXIT_SEARCH_STEPS 12
#define OVERCOAT_SUBSTEPS 4
#define OVERCOAT_SUBSTEP_COLLIDER_COUNT 7
#define OVERCOAT_SIDE_BASE_SLACK  0.05f
#define OVERCOAT_SIDE_ROW_SLACK   0.15f

/*
=================
Rendering Macros
=================
*/
#define MIN_CAMERA_DISTANCE 10.0f
#define FIRST_PERSON_VIEW_MODE 6

#include "z_overcoat.h"
#include "proxymm_custom_actor.h"
#include "math.h"
#include "customMath.h"

/*
=================
Zero Macros
=================
*/
// These macros were set up for position-calculation operations to avoid using pure zero values
#define GREATER_THAN_ZERO   0.000001f
#define GREATER_THAN_ZERO2  0.00000001f
#define GREATER_THAN_ZERO3  0.0001f

// For zero-ing player velocity when Jackie is opening a door
extern void Player_Action_36(Player* player, PlayState* play);

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

#define FLAGS 0


/***********************************************************************

	Overcoat Physics

***********************************************************************/

/*
=================
Overcoat Limbs
=================
*/
// Enum of Overcoat limbs for jointTable
typedef enum OvercoatLimbs {
    OVERCOAT_ROOT_POS,        // Overcoat's Root bone (position)
    OVERCOAT_ROOT_ROT,        // Overcoat's Root bone (rotation)
    OVERCOAT_BACKLIMB_A,
    OVERCOAT_BACKLIMB_B,
    OVERCOAT_BACKLIMB_C,
    OVERCOAT_BACKLIMB_D,
    OVERCOAT_BACKLIMB_E,
    OVERCOAT_BACKLIMB_F,
    OVERCOAT_BACKLIMB_G,
    OVERCOAT_BACKLIMB_H,
    OVERCOAT_BACKLIMB_I,
    OVERCOAT_LEFTLIMB0_A,
    OVERCOAT_LEFTLIMB0_B,
    OVERCOAT_LEFTLIMB0_C,
    OVERCOAT_LEFTLIMB0_D,
    OVERCOAT_LEFTLIMB0_E,
    OVERCOAT_LEFTLIMB0_F,
    OVERCOAT_LEFTLIMB0_G,
    OVERCOAT_LEFTLIMB0_H,
    OVERCOAT_LEFTLIMB0_I,
    OVERCOAT_LEFTLIMB1_A,
    OVERCOAT_LEFTLIMB1_B,
    OVERCOAT_LEFTLIMB1_C,
    OVERCOAT_LEFTLIMB1_D,
    OVERCOAT_LEFTLIMB1_E,
    OVERCOAT_LEFTLIMB1_F,
    OVERCOAT_LEFTLIMB1_G,
    OVERCOAT_LEFTLIMB1_H,
    OVERCOAT_LEFTLIMB1_I,
    OVERCOAT_LEFTLIMB2_A,
    OVERCOAT_LEFTLIMB2_B,
    OVERCOAT_LEFTLIMB2_C,
    OVERCOAT_LEFTLIMB2_D,
    OVERCOAT_LEFTLIMB2_E,
    OVERCOAT_LEFTLIMB2_F,
    OVERCOAT_LEFTLIMB2_G,
    OVERCOAT_LEFTLIMB2_H,
    OVERCOAT_LEFTLIMB2_I,
    OVERCOAT_LEFTLIMB3_A,
    OVERCOAT_LEFTLIMB3_B,
    OVERCOAT_LEFTLIMB3_C,
    OVERCOAT_LEFTLIMB3_D,
    OVERCOAT_LEFTLIMB3_E,
    OVERCOAT_LEFTLIMB3_F,
    OVERCOAT_LEFTLIMB3_G,
    OVERCOAT_LEFTLIMB3_H,
    OVERCOAT_LEFTLIMB3_I,
    OVERCOAT_LEFTLIMB4_A,
    OVERCOAT_LEFTLIMB4_B,
    OVERCOAT_LEFTLIMB4_C,
    OVERCOAT_LEFTLIMB4_D,
    OVERCOAT_LEFTLIMB4_E,
    OVERCOAT_LEFTLIMB4_F,
    OVERCOAT_LEFTLIMB4_G,
    OVERCOAT_LEFTLIMB4_H,
    OVERCOAT_LEFTLIMB4_I,
    OVERCOAT_LEFTLIMB5_A,
    OVERCOAT_LEFTLIMB5_B,
    OVERCOAT_LEFTLIMB5_C,
    OVERCOAT_LEFTLIMB5_D,
    OVERCOAT_LEFTLIMB5_E,
    OVERCOAT_LEFTLIMB5_F,
    OVERCOAT_LEFTLIMB5_G,
    OVERCOAT_LEFTLIMB5_H,
    OVERCOAT_LEFTLIMB5_I,
    OVERCOAT_LEFTLIMB5_J,
    OVERCOAT_LEFTLIMB5_K,
    OVERCOAT_LEFTLIMB5_L,
    OVERCOAT_LEFTLIMB6_A,
    OVERCOAT_LEFTLIMB6_B,
    OVERCOAT_LEFTLIMB6_C,
    OVERCOAT_LEFTLIMB6_D,
    OVERCOAT_LEFTLIMB6_E,
    OVERCOAT_LEFTLIMB6_F,
    OVERCOAT_LEFTLIMB6_G,
    OVERCOAT_LEFTLIMB6_H,
    OVERCOAT_LEFTLIMB6_I,
    OVERCOAT_LEFTLIMB7_A,
    OVERCOAT_LEFTLIMB7_B,
    OVERCOAT_LEFTLIMB7_C,
    OVERCOAT_LEFTLIMB7_D,
    OVERCOAT_LEFTLIMB7_E,
    OVERCOAT_LEFTLIMB7_F,
    OVERCOAT_LEFTLIMB7_G,
    OVERCOAT_LEFTLIMB7_H,
    OVERCOAT_LEFTLIMB7_I,
    OVERCOAT_LEFTLIMB8_A,
    OVERCOAT_LEFTLIMB8_B,
    OVERCOAT_LEFTLIMB8_C,
    OVERCOAT_LEFTLIMB8_D,
    OVERCOAT_LEFTLIMB8_E,
    OVERCOAT_LEFTLIMB8_F,
    OVERCOAT_LEFTLIMB8_G,
    OVERCOAT_LEFTLIMB8_H,
    OVERCOAT_LEFTLIMB8_I,
    OVERCOAT_LEFTLIMB9_A,
    OVERCOAT_LEFTLIMB9_B,
    OVERCOAT_LEFTLIMB9_C,
    OVERCOAT_LEFTLIMB9_D,
    OVERCOAT_LEFTLIMB9_E,
    OVERCOAT_LEFTLIMB9_F,
    OVERCOAT_LEFTLIMB9_G,
    OVERCOAT_LEFTLIMB9_H,
    OVERCOAT_LEFTLIMB9_I,
    OVERCOAT_RIGHTLIMB0_A,
    OVERCOAT_RIGHTLIMB0_B,
    OVERCOAT_RIGHTLIMB0_C,
    OVERCOAT_RIGHTLIMB0_D,
    OVERCOAT_RIGHTLIMB0_E,
    OVERCOAT_RIGHTLIMB0_F,
    OVERCOAT_RIGHTLIMB0_G,
    OVERCOAT_RIGHTLIMB0_H,
    OVERCOAT_RIGHTLIMB0_I,
    OVERCOAT_RIGHTLIMB1_A,
    OVERCOAT_RIGHTLIMB1_B,
    OVERCOAT_RIGHTLIMB1_C,
    OVERCOAT_RIGHTLIMB1_D,
    OVERCOAT_RIGHTLIMB1_E,
    OVERCOAT_RIGHTLIMB1_F,
    OVERCOAT_RIGHTLIMB1_G,
    OVERCOAT_RIGHTLIMB1_H,
    OVERCOAT_RIGHTLIMB1_I,
    OVERCOAT_RIGHTLIMB2_A,
    OVERCOAT_RIGHTLIMB2_B,
    OVERCOAT_RIGHTLIMB2_C,
    OVERCOAT_RIGHTLIMB2_D,
    OVERCOAT_RIGHTLIMB2_E,
    OVERCOAT_RIGHTLIMB2_F,
    OVERCOAT_RIGHTLIMB2_G,
    OVERCOAT_RIGHTLIMB2_H,
    OVERCOAT_RIGHTLIMB2_I,
    OVERCOAT_RIGHTLIMB3_A,
    OVERCOAT_RIGHTLIMB3_B,
    OVERCOAT_RIGHTLIMB3_C,
    OVERCOAT_RIGHTLIMB3_D,
    OVERCOAT_RIGHTLIMB3_E,
    OVERCOAT_RIGHTLIMB3_F,
    OVERCOAT_RIGHTLIMB3_G,
    OVERCOAT_RIGHTLIMB3_H,
    OVERCOAT_RIGHTLIMB3_I,
    OVERCOAT_RIGHTLIMB4_A,
    OVERCOAT_RIGHTLIMB4_B,
    OVERCOAT_RIGHTLIMB4_C,
    OVERCOAT_RIGHTLIMB4_D,
    OVERCOAT_RIGHTLIMB4_E,
    OVERCOAT_RIGHTLIMB4_F,
    OVERCOAT_RIGHTLIMB4_G,
    OVERCOAT_RIGHTLIMB4_H,
    OVERCOAT_RIGHTLIMB4_I,
    OVERCOAT_RIGHTLIMB5_A,
    OVERCOAT_RIGHTLIMB5_B,
    OVERCOAT_RIGHTLIMB5_C,
    OVERCOAT_RIGHTLIMB5_D,
    OVERCOAT_RIGHTLIMB5_E,
    OVERCOAT_RIGHTLIMB5_F,
    OVERCOAT_RIGHTLIMB5_G,
    OVERCOAT_RIGHTLIMB5_H,
    OVERCOAT_RIGHTLIMB5_I,
    OVERCOAT_RIGHTLIMB5_J,
    OVERCOAT_RIGHTLIMB5_K,
    OVERCOAT_RIGHTLIMB5_L,
    OVERCOAT_RIGHTLIMB6_A,
    OVERCOAT_RIGHTLIMB6_B,
    OVERCOAT_RIGHTLIMB6_C,
    OVERCOAT_RIGHTLIMB6_D,
    OVERCOAT_RIGHTLIMB6_E,
    OVERCOAT_RIGHTLIMB6_F,
    OVERCOAT_RIGHTLIMB6_G,
    OVERCOAT_RIGHTLIMB6_H,
    OVERCOAT_RIGHTLIMB6_I,
    OVERCOAT_RIGHTLIMB7_A,
    OVERCOAT_RIGHTLIMB7_B,
    OVERCOAT_RIGHTLIMB7_C,
    OVERCOAT_RIGHTLIMB7_D,
    OVERCOAT_RIGHTLIMB7_E,
    OVERCOAT_RIGHTLIMB7_F,
    OVERCOAT_RIGHTLIMB7_G,
    OVERCOAT_RIGHTLIMB7_H,
    OVERCOAT_RIGHTLIMB7_I,
    OVERCOAT_RIGHTLIMB8_A,
    OVERCOAT_RIGHTLIMB8_B,
    OVERCOAT_RIGHTLIMB8_C,
    OVERCOAT_RIGHTLIMB8_D,
    OVERCOAT_RIGHTLIMB8_E,
    OVERCOAT_RIGHTLIMB8_F,
    OVERCOAT_RIGHTLIMB8_G,
    OVERCOAT_RIGHTLIMB8_H,
    OVERCOAT_RIGHTLIMB8_I,
    OVERCOAT_RIGHTLIMB9_A,
    OVERCOAT_RIGHTLIMB9_B,
    OVERCOAT_RIGHTLIMB9_C,
    OVERCOAT_RIGHTLIMB9_D,
    OVERCOAT_RIGHTLIMB9_E,
    OVERCOAT_RIGHTLIMB9_F,
    OVERCOAT_RIGHTLIMB9_G,
    OVERCOAT_RIGHTLIMB9_H,
    OVERCOAT_RIGHTLIMB9_I,
} OvercoatLimbs;


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
                                    -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0, 
                                    0, 0);
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


    /********************************
     Set up vertical cloth chains
    ********************************/
	// Define each vertical cloth chain by first phys limb (pinned), chain length (number of phys
	// limbs in chain, from first pinned phys limb to last unpinned limb), and first phys bone
	// (from root limb to first pinned limb of that chain)
    static const OvercoatChainInit cloth_chains[OVERCOAT_CHAIN_COUNT] = {
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

	// Initialize every vertical cloth chain and its corresponding phys bones.
    for (int i = 0; i < OVERCOAT_CHAIN_COUNT; i++) {
        Overcoat_InitChain(this, player, gPhysLimbs, gPhysBones, cloth_chains[i].startLimb,
            cloth_chains[i].limbCount,  cloth_chains[i].startBone);
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
        this->hideInFirstPerson = 0;

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
        Vec3s rightlimb0_b_jointPos = { 0, 0, 224 };
        Vec3s rightlimb0_c_jointPos = { 0, 0, 224 };
        Vec3s rightlimb0_d_jointPos = { 0, 0, 224 };
        Vec3s rightlimb0_e_jointPos = { 0, 0, 224 };
        Vec3s rightlimb0_f_jointPos = { 0, 0, 224 };
        Vec3s rightlimb0_g_jointPos = { 0, 0, 224 };
        Vec3s rightlimb0_h_jointPos = { 0, 0, 224 };
        Vec3s rightlimb0_i_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_a_jointPos = { -168, -31, 335 };
        Vec3s rightlimb1_b_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_c_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_d_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_e_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_f_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_g_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_h_jointPos = { 0, 0, 224 };
        Vec3s rightlimb1_i_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_a_jointPos = { -225, -32, 240 };
        Vec3s rightlimb2_b_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_c_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_d_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_e_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_f_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_g_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_h_jointPos = { 0, 0, 224 };
        Vec3s rightlimb2_i_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_a_jointPos = { -282, -32, 145 };
        Vec3s rightlimb3_b_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_c_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_d_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_e_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_f_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_g_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_h_jointPos = { 0, 0, 224 };
        Vec3s rightlimb3_i_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_a_jointPos = { -338, -32, 51 };
        Vec3s rightlimb4_b_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_c_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_d_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_e_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_f_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_g_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_h_jointPos = { 0, 0, 224 };
        Vec3s rightlimb4_i_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_a_jointPos = { -395, -32, -44 };
        Vec3s rightlimb5_b_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_c_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_d_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_e_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_f_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_g_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_h_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_i_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_j_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_k_jointPos = { 0, 0, 224 };
        Vec3s rightlimb5_l_jointPos = { 0, 0, 188 };
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
        Overcoat_SetDefaultBodyPartsPos(this, player, overcoat_PhysLimbs, overcoat_PhysBones);
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

// Torso values for setting root for Overcoat's model and torso sphere collider
// World position of player's torso limb
Vec3f overcoat_torso_globalPos = { (f32)0, (f32)0, (f32)0 };    
// Rotation value for player's torso (with parent limbs' rotations included)     
Vec3s overcoat_torso_rotate = {0, 0, 0};

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

// Overcoat's current directional axes 
static Vec3f overcoat_forwardAxis = {0.f, 0.f, 0.f};
static Vec3f overcoat_leftAxis = {0.f, 0.f, 0.f};
static Vec3f overcoat_downAxis = {0.f, 0.f, 0.f};
static u8 overcoat_dirAxesReady = 0;

// Previous frame state values used for substep calculations
static u8 overcoat_stepHistoryReady = 0;
static PhysSphereCollider overcoat_prevStepColliders[OVERCOAT_SUBSTEP_COLLIDER_COUNT];
static Vec3f overcoat_previousSubstepForward;
static Vec3f overcoat_previousSubstepDown;
static f32 overcoat_previousSubstepScale;


/**
 * @brief Capture the directional axes of the torso and apply it to the overcoat
 * 
 * More specifically, this function extracts the directional axes of Jackie's torso and
 * assign them to global variables that carry the directional axes of the overcoat.
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

    // Mark flag variable that indicates that the directional axes for the overcoat has been
    // set up now.
    overcoat_dirAxesReady = 1;
}


/**
 * @brief Limit the given phys bone's upward direction (and inward direction if indicated)
 * 
 * This function limits the phys bone's upward direction, where the phys bone is indicated by the
 * function's parent phys limb and child phys limb. This function is used so that the overcoat's
 * phys bones do not exceed its horizontal limit to prevent unwanted flapping behavior of the
 * overcoat.
 * 
 * This function also limits the phys bone's inward direction when indicated via the argument
 * 'limitInward'. When this is '1', then the 'outwardDirVec' is used as reference to push the
 * phys bone that is going inward towards Jackie's legs back outward.
 * 
 * This function is a manual fix/adjustment on the overcoat's physics that sometimes rotate or
 * bend in extreme or unwanted directions.
 * 
 * @param parentPhysLimb    Parent phys limb of the target phys bone
 * @param childPhysLimb     Child phys limb of the target phys bone
 * @param outwardDirVec     Outward direction axis of the target phys bone
 * @param limitInward       Flag value indicating if the phys bone has to be pushed outward
 */
void Overcoat_LimitBoneDirection(PhysLimb* parentPhysLimb, PhysLimb* childPhysLimb, 
    Vec3f* outwardDirVec, u8 limitInward) {
    if (childPhysLimb->pinned) {
        return;
    }

    // Calculate the direction vector from parent phys limb to child phys limb
    Vec3f directionVec = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&childPhysLimb->curr_pos, &parentPhysLimb->curr_pos, &directionVec);

    // Check if the direction vector does not have a length of zero
    f32 originalLengthSq = Math3D_Vec3fMagnitudeSq(&directionVec);
    if (originalLengthSq < GREATER_THAN_ZERO2) {
        return;
    }

    // This vector will carry the new direction of the phys limb after the limits have been applied.
    Vec3f newDirVec = directionVec;
    u8 boneAdjusted = 0;        // Flag that indicates if the direction of the phys bone was changed

    /********************************
     Remove Inward Rotation of Phys Bone
    ********************************/
    // This part of the code only runs when indicated via boolean argument limitInward.
    // To limit the inward motions of the phys bone in respect to the overcoat's current position
    // and rotation, remove the inward rotation from the current direction of the phys bones.
    if (limitInward) {
        // Calculate just how much the direction of the phys bone is going outward via dot product
        // of the phys bone's current direction and the outward direction axis of this bone.
        f32 outwardAmount = CustomMath_Vec3f_Dot(&newDirVec, outwardDirVec);

        // If the outward amount of the direction is negative, that means it's going inward.
        // Remove the inward direction by subtracting the current direction of the phys bone by
        // its outward direction scaled by the outward amount (which is negative, indicating that
        // this is an inward direction).
        if (outwardAmount < 0.f) {
            Vec3f inwardComponent = { 0.f, 0.f, 0.f };
            Math_Vec3f_ScaleAndStore(outwardDirVec, outwardAmount, &inwardComponent);
            Math_Vec3f_Diff(&newDirVec, &inwardComponent, &newDirVec);
            boneAdjusted = 1;
        }
    }

    /********************************
     Remove Upward Rotation of Phys Bone
    ********************************/
    // This part of the code runs all the time.
    // To limit the upward motion of the phys bone in respect to the overcoat's current position
    // and rotation, remove the upward rotation from the current direction of the phys bones.

    // Calculate just how much the direction of the phys bone is going downward via dot product of the 
    // phys bone's current direction and the downward direction axis of this bone.
    f32 downAmount = CustomMath_Vec3f_Dot(&newDirVec, &overcoat_downAxis);

    // If the downward amount of the direction is negative, then that means it's going upward. Remove 
    // the upward direction by subtracting the current direction of the phys bone by its downward 
    // direction scaled by its downward amount (which is negative, indicating that this is an upward 
    // direction).
    if (downAmount < 0.f) {
        Vec3f upwardComponent = { 0.f, 0.f, 0.f };
        Math_Vec3f_ScaleAndStore(&overcoat_downAxis, downAmount, &upwardComponent);
        Math_Vec3f_Diff(&newDirVec, &upwardComponent, &newDirVec);
        boneAdjusted = 1;
    }

    // Leave the function if the phys bone direction was not adjusted.
    if (!boneAdjusted) {
        return;
    }

    // If the phys bone direction was adjusted, then verify if the direction of the phys bone is valid 
    // (as in, not the length of 0). 
    f32 allowedLengthSq = Math3D_Vec3fMagnitudeSq(&newDirVec);

    // If the recalculated phys bone direction has the length of zero, then set the phys bone's direction 
    // as completely downward based on its down direction axis.
    if (allowedLengthSq < GREATER_THAN_ZERO) {
        newDirVec = overcoat_downAxis;
        allowedLengthSq = Math3D_Vec3fMagnitudeSq(&newDirVec);
    }

    // Preserve the current segment length.
    Math_Vec3f_Scale(&newDirVec, sqrtf(originalLengthSq / allowedLengthSq));

    // Change the phys bone's child phys limb's position based on the new direction vector from the phys 
    // bone's parent phys limb.
    Math_Vec3f_Sum(&parentPhysLimb->curr_pos, &newDirVec, &childPhysLimb->curr_pos);
}


/**
 * @brief Apply bend limits to every vertical overcoat cloth chain of phys limbs.
 *
 * This function goes through each vertical cloth chain and applies Overcoat_LimitBoneDirection() to 
 * every bone segment in that chain.
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
void Overcoat_LimitUpperChains(PhysLimb* gPhysLimbs[], const int chainStarts[], int chainCount) {
    // If directional axes of the overcoat is not established in the world space yet, then do not run 
    // this function.
    if (!overcoat_dirAxesReady) {
        return;
    }

    // For each vertical chain of physlimbs, keep its phys bones from bending too far upward or inward, 
    // depending on where this chain is located in within the overcoat.
    for (int currChain = 0; currChain < chainCount; currChain++) {
        // By default, it is assumed in this for loop that the current chain of phys limbs is at the 
        // front.
        int startIdx = chainStarts[currChain];  // Index of first phys limb in current chain
        Vec3f outward = overcoat_forwardAxis;  // Set the outward direction of the chain as forward so 
                                                // that it doesn't bend forward too much and then start 
                                                // bending upward.

        // If the starting index of the chain of phys limbs is the first phys limb of the left most 
        // chain of phys limbs in the overcoat, then make the outward direction of this chain as left.
        if (startIdx == OVERCOAT_BODYPART_LEFTLIMB5_A) {
            outward = overcoat_leftAxis;
        }
        // If the starting index of the chain of phys limbs is the first phys limb of the right most 
        // chain of phys limbs in the overcoat, then make the outward direction of this chain as right.
        else if (startIdx == OVERCOAT_BODYPART_RIGHTLIMB5_A) {
            outward = overcoat_leftAxis;
            Math_Vec3f_Scale(&outward, -1.f);   // Right is basically the opposite of left.
        }
        // If the current chain of phys limbs is one of the back chains of phys limbs in the overcoat, 
        // then make the outward direction of this chain as back.
        // (Left6-9, Right9-6)
        else if (currChain >= 6 && currChain <= 14) {
            Math_Vec3f_Scale(&outward, -1.f);   // Back is basically the opposite of forward.
        }

        // Left/Right-most chains have A-L limbs, which is 11 phys bones, while the rest front and back 
        // chains have A-I limbs, which is 8 phys bones.
        int boneCount = 8;
        if (startIdx == OVERCOAT_BODYPART_LEFTLIMB5_A || 
            startIdx == OVERCOAT_BODYPART_RIGHTLIMB5_A) {
            boneCount = 11;
        }

        // Limit the current chain's direction so that it is not going upward or inward too much.
        // The directional axes of the overcoat are used to help with this operation.
        for (int currPhysLimb = 0; currPhysLimb < boneCount; currPhysLimb++) {
            // Limit the first 3 phys limbs' inward direction
            bool limitInward = currPhysLimb < OVERCOAT_LIMBS_WITH_INWARD_LIMIT;
            Overcoat_LimitBoneDirection(gPhysLimbs[startIdx + currPhysLimb], 
                gPhysLimbs[startIdx + currPhysLimb + 1], &outward, limitInward);
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
RECOMP_HOOK("Player_PostLimbDrawGameplay") void Overcoat_on_Player_PostLimbDrawGameplay(PlayState* play, 
    s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    /********************************
     Torso Limb Positions and Rotations
    ********************************/
    if (limbIndex == PLAYER_LIMB_TORSO) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's Torso limb
        MtxF* mtx = Matrix_GetCurrent();

        // Capture the forward, left, and down directions that is relative to the torso limb, which is
        // used by the overcoat's directional constraints.
        Overcoat_CaptureLimitAxes(mtx);

        // Get torso's world position directly from the current matrix
        overcoat_torso_globalPos.x = mtx->xw;
        overcoat_torso_globalPos.y = mtx->yw;
        overcoat_torso_globalPos.z = mtx->zw;

        // Get torso rotation values (used for rotating the overcoat actor properly)
        Matrix_MtxFToYXZRot(mtx, &overcoat_torso_rotate, 1);

        // Move the torso sphere collider forward
        Vec3f torsoColliderLocalOffset = { 0.f, 0.f, 0.f };     // No offset applied for now

        // Multiply the current matrix that has the torso position by the offset vector local to
        // torso to apply the center of the torso collider to the torso limb with offset applied.
        Matrix_MultVec3f(&torsoColliderLocalOffset, &overcoat_torsoCollider.center);

        // Pop matrix
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

        // Assign colliders' center positions
        // Keep note that because of the way the thigh limbs are rotated in idle pose, Y-axis indicates
        // front-back and Z-axis indicates left-right

        // Local offset for the back thigh collider (175 units back and 40 units left)
        Vec3f leftThighBackColliderLocalOffset = {0.f, 175.f, -40.f};
        // Apply local offset to current world position of left thigh and apply that position as the left 
        // thigh back collider.
        Matrix_MultVec3f(&leftThighBackColliderLocalOffset, &overcoat_leftThighBackCollider.center);

        // Local offset for the front thigh collider (100 units forward and 30 units left)
        Vec3f leftThighFrontColliderLocalOffset = {0.f, -100.f, -30.f};
        // Apply local offset to current world position of left thigh and apply that position as the left 
        // thigh front collider
        Matrix_MultVec3f(&leftThighFrontColliderLocalOffset, &overcoat_leftThighFrontCollider.center);

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

        // Assign colliders' center positions
        // Keep note that because of the way the thigh limbs are rotated in idle pose, Y-axis indicates
        // front-back and Z-axis indicates left-right

        // Local offset for the back thigh collider (175 units back and 40 units right)
        Vec3f rightThighBackColliderLocalOffset = {0.f, 175.f, 40.f};
        // Apply local offset to current world position of left thigh and apply that position as the left 
        // thigh back collider
        Matrix_MultVec3f(&rightThighBackColliderLocalOffset, &overcoat_rightThighBackCollider.center);

        // Local offset for the front thigh collider (100 units forward and 30 units right)
        Vec3f rightThighFrontColliderLocalOffset = {0.f, -100.f, 30.f};
        // Apply local offset to current world position of left thigh and apply that position as the left 
        // thigh front collider
        Matrix_MultVec3f(&rightThighFrontColliderLocalOffset, &overcoat_rightThighFrontCollider.center);

        // Pop matrix
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }

    /********************************
     Left Leg Limb Positions
    ********************************/
    if (limbIndex == PLAYER_LIMB_LEFT_SHIN) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's left leg/shin limb
        MtxF* mtx = Matrix_GetCurrent();

        // Assign colliders' center positions
        // Keep note that because of the way the leg/shin limbs are rotated in idle pose, Y-axis indicates
        // front-back and Z-axis indicates left-right

        // Local offset for the leg collider (70 units forward)
        Vec3f leftLegColliderLocalOffset = {0.f, -70.f, 0.f};
        // Apply local offset to current world position of left leg/shin and apply that position as the left 
        // leg collider
        Matrix_MultVec3f(&leftLegColliderLocalOffset, &overcoat_leftLegCollider.center);

        // Pop matrix
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }

    /********************************
     Righ Leg Limb Positions
    ********************************/
    if (limbIndex == PLAYER_LIMB_RIGHT_SHIN) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();

        // Get current matrix, which is currently positioned and rotated for Jackie's left leg/shin limb
        MtxF* mtx = Matrix_GetCurrent();

        // Assign colliders' center positions
        // Keep note that because of the way the leg/shin limbs are rotated in idle pose, Y-axis indicates
        // front-back and Z-axis indicates left-right

        // Local offset for the leg collider (70 units forward)
        Vec3f rightLegColliderLocalOffset = {0.f, -70.f, 0.f};
        // Apply local offset to current world position of right leg/shin and apply that position as the 
        // right leg collider
        Matrix_MultVec3f(&rightLegColliderLocalOffset, &overcoat_rightLegCollider.center);

        // Pop matrix
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
 * @param targetForward Current frame's target world-space forward direection of overcoat
 * @param targetDown    Current frame's target world-space down direction of overcoat.
 * @param t             Interpolation value between previous and current frame
 */
void Overcoat_SetSubstepDirAxes(Vec3f targetForward, Vec3f targetDown, f32 t) {
    // Interpolate overcoat's down direction from previous frame toward current frame's target down direction 
    // based on the current substep (as indicated by 't')
    Vec3f downDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&overcoat_previousSubstepDown, &targetDown, t, &downDirection);

    // Interpolate overcoat's forward direction from previous frame toward current frame's target forward 
    // direction based on the current substep
    Vec3f forwardDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&overcoat_previousSubstepForward, &targetForward, t, &forwardDirection);

    // Normalize the overcoat's interpolated down direction
    // If the down direction is too small to normalize, then use the final target down direction instead.
    if (!Overcoat_NormalizeSubstepDirectionVec(&downDirection)) {
        downDirection = targetDown;
    }

    // Using dot product, find projection to know how much of overcoat's forward direction points  in the 
    // same direction as overcoat's down direction.
    f32 projection = CustomMath_Vec3f_Dot(&forwardDirection, &downDirection);

    // Remove that part of the overcoat's forward direction so that forward direction becomes 
    // perpendicular to the overcoat's down direction
    forwardDirection.x -= projection * downDirection.x;
    forwardDirection.y -= projection * downDirection.y;
    forwardDirection.z -= projection * downDirection.z;

    // Normalize the overcoat's correct forward direction.
    // If the corrected forward direction is too small to normalize, then use the final target forward and 
    // down directions instead.
    if (!Overcoat_NormalizeSubstepDirectionVec(&forwardDirection)) {
        // Near-opposite poses have no unique linear interpolation.
        overcoat_forwardAxis = targetForward;
        overcoat_downAxis = targetDown;
    } 
    // Otherwise, store the interpolated and corrected substep directions of the overcoat
    else {
        overcoat_forwardAxis = forwardDirection;
        overcoat_downAxis = downDirection;
    }
    
    // Calculate the overcoat's left direction from the overcoat's forward and down directions.
    // This overcoat model's directional coordinate axes are now made: forward, down, and left
    Math3D_Vec3f_Cross(&overcoat_forwardAxis, &overcoat_downAxis, &overcoat_leftAxis);
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
 * @param torso_collider     Torso sphere collider
 * @param capsulePairs       Array of sphere collider pairs used as capsule colliders
 * @param capsuleColCount    Number of capsule colliders
 * @param contactColCenter   Output center position of the deepest collider contact.
 * @param contactColRadius   Output combined collider and target radius of the deepest contact.
 *
 * @return Deepest overlap depth found among all body colliders.
 */
f32 Overcoat_GroupContact(Vec3f targetPos, f32 pointRadius,  PhysSphereCollider* torso_collider, 
    PhysSphereCollider* capsulePairs[][2], int capsuleColCount, Vec3f* contactColCenter, 
    f32* contactColRadius) {
    
    // Set up variable indicating deepest depth value of targetPos in any collider in overcoat
    f32 deepest = 0.f;

	// Note: This is crudely coded because I did not handle list of colliders cleanly. The array
	//		 for colliders is only consisted of capsule colliders, but there is also the torso
	//		 collider, which is only a single sphere collider. Because of this, I started the index
	//		 at -1, where -1 indicates one special instance of a sphere collider and not a capsule
	//		 collider made up of two sphere colliders.
    for (int capsuleIdx = -1; capsuleIdx < capsuleColCount; capsuleIdx++) {
        PhysSphereCollider* capsuleColStart;
        PhysSphereCollider* capsuleColEnd;

        // Special case for when capsule collider index is at -1 (spherical torso collider)
        if (capsuleIdx < 0) {
            // Use the torso sphere for both ends so it can be handled like a capsule pair.
            capsuleColStart = torso_collider;
            capsuleColEnd = torso_collider;
        }
        // For rest of the normal capsule colliders in the capsulePairs array
        else {
            // Use the two sphere colliders that make up this capsule collider
            capsuleColStart = capsulePairs[capsuleIdx][0];
            capsuleColEnd = capsulePairs[capsuleIdx][1];
        }

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
 * @param torso_collider	Torso sphere collider
 * @param capsulePairs		Array of sphere collider pairs used as capsule colliders
 * @param capsuleColCount	Number of capsule colliders
 * 
 * @return true if target position is outside of all colliders, otherwise false
 */
bool Overcoat_GroupOutside(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* torso_collider, 
	PhysSphereCollider* capsulePairs[][2], int capsuleColCount) {
    Vec3f center = {0.f, 0.f, 0.f};		// Made only as function argument.
    f32 radius = 0.f;					// Made only as function argument.
	
	// Check if target position is inside colliders (torso and capsule) by calculating the
	// deepest depth the position is in any collider.
	f32 colliderDepth = Overcoat_GroupContact(targetPos, pointRadius, torso_collider, capsulePairs, 
		capsuleColCount, &center, &radius);
		
	// If target position is inside any collider, then return false
	bool posIsOutside = colliderDepth <= 0.f;
	
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
 * overcoat. 
 *
 * @param targetPhysLimbPos		Target phys limb's world position
 * @param physLimbDir			Direction along which a safe exit position for phys limb is searched
 * @param pointRadius			Collisionr radius of target phys limb
 * @param torso_collider		Torso sphere collider
 * @param capsulePairs			Array of pairs of sphere colliders used for capsule colliders
 * @param capsuleColCount		Count of capsule colliders 
 * @param padding				Small extra distance added so exit doesn't sit on collider boundary
 * @param exitPosition			Output position containing safe exit position for phys limb
 */
bool Overcoat_GroupRayExit(Vec3f targetPhysLimbPos, Vec3f physLimbDir, f32 pointRadius,
    PhysSphereCollider* torso_collider, PhysSphereCollider* capsulePairs[][2], int capsuleColCount,
    f32 padding, Vec3f* exitPosition) {
	/********************************
     Find a guaranteed outside distance
    ********************************/
    // Find a distance from phys limb, along the indicated search direction (physLimbDir), that is 
	// far enough that it should be past every collider
    f32 farDistance = 0.f;
	
	// Go through each collider (including both torso collider and other colliders) and see how far
	// the target phys limb should go to be outside all colliders (indicated by farDistance)
	// Note: This is crudely coded because I did not handle list of colliders cleanly. The array
	//		 for colliders is only consisted of capsule colliders, but there is also the torso
	//		 collider, which is only a single sphere collider. Because of this, I started the index
	//		 at -1, where -1 indicates one special instance of a sphere collider and not a capsule
	//		 collider made up of two sphere colliders.
    for (int capsuleIdx = -1; capsuleIdx < capsuleColCount; capsuleIdx++) {
        PhysSphereCollider* capsuleColStart;	// Capsule collider endpoint 1
		PhysSphereCollider* capsuleColEnd;		// Capsule collider endpoint 2
		
		// Special case for when capsule collider index is at -1 (spherical torso collider)
		if (capsuleIdx < 0) {
			// Use the torso sphere for both ends so it can be handled like a capsule pair.
			capsuleColStart = torso_collider;
			capsuleColEnd = torso_collider;
		}
		// For rest of the normal capsule colliders in the capsulePairs array
		else {
			// Use the two sphere colliders that make up this capsule collider
			capsuleColStart = capsulePairs[capsuleIdx][0];
			capsuleColEnd = capsulePairs[capsuleIdx][1];
		}
		
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
    if (!Overcoat_GroupOutside(*exitPosition, pointRadius, torso_collider, capsulePairs, 
		capsuleColCount)) { 
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
        bool testExitIsOutside = Overcoat_GroupOutside(testExitPos, pointRadius, torso_collider,
            capsulePairs, capsuleColCount);
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
 * @brief Push the given overcoat phys limb out of Jackie's colliders
 * 
 * This function checks whether a given phys limb is inside the torso collider or any of the capsule
 * colliders.
 *
 * If the phys limb is inside a colider, the function calculates an outward direction from the collider 
 * and tries to move the phys limbs in that direction out of the collider.
 * 
 * If the normal outward correction of the phys limb still places said phys limb inside another capsule 
 * collider, the function searches in both the outward and opposite directions for valid exit positions 
 * and uses the closest valid one.
 *
 * @param physLimb			Phys limb that is being checked and corrected
 * @param torso_collider	Torso sphere collider
 * @param collider_pairs	Array of pairs of sphere colliders used to form capsule colliders
 * @param capsuleColCount	Number of capsule collider pairs
 */
void Overcoat_SolveColliderGroup(PhysLimb* physLimb, PhysSphereCollider* torso_collider, 
	PhysSphereCollider* collider_pairs[][2], int capsuleColCount) {
	// Find which part of the collider where the given phys limb is currently inside
    Vec3f center = {0.f, 0.f, 0.f};
    f32 radius = 0.f;
	
	// If the phys limb is not inside any collider, no correction is needed
    if (Overcoat_GroupContact(physLimb->curr_pos, physLimb->collision_radius,
		torso_collider, collider_pairs, capsuleColCount, &center, &radius) <= 0.f) {
		return;
	}

	// Find direction pointing from the collider center toward the phys limb's current position
    Vec3f direction = {(physLimb->curr_pos.x - center.x), (physLimb->curr_pos.y - center.y), 
		(physLimb->curr_pos.z - center.z)};
		
	// Try to normalize direction from the collider's center to the phys limb's position. If the 
	// direction vector can't be normalized, that means the phys limb is positioned at a collider's 
	// center.
    if (!Overcoat_NormalizeSubstepDirectionVec(&direction)) {
        // Use phys limb's previous position instead to create another possible outward direction
        direction = (Vec3f){(physLimb->prev_pos.x - center.x), (physLimb->prev_pos.y - center.y), 
			(physLimb->prev_pos.z - center.z)};
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
	// Calculate a possible new position just outside the collider, which includes the padding gap 
	// so the phys limb does not sit directly on the boundary.
    Vec3f candidate = {(center.x + direction.x * (radius + padding)), 
		(center.y + direction.y * (radius + padding)), (center.z + direction.z * (radius + padding))};

    // Check whether phys limb would be outside all colliders if it were moved to this candidate
	// position. If the position is safe, then move the phys limb there and finish
    if (Overcoat_GroupOutside(candidate, physLimb->collision_radius, torso_collider, collider_pairs, 
		capsuleColCount)) {
        physLimb->curr_pos = candidate;
        return;
    }

	// Store possible safe exit positions in the normal outward direction (collider center to phys limb)
	// and in the opposite direction.
    Vec3f outwardExit = {0.f, 0.f, 0.f};	// exit position that goes from collider center to phys limb
    Vec3f oppositeExit = {0.f, 0.f, 0.f};	// opposite exit position that points from collider center
	
	// Create direction that points exactly opposite from the normal outward direction.
    Vec3f opposite = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&direction, -1.f, &opposite);
	
	// Search outward from phys limb for a position that is outside all colliders
	// outwardOK is a flag that indicates whether valid exit was found in normal direction.
	// outwardExit stores the position of that exit.
    bool outwardOK = Overcoat_GroupRayExit(physLimb->curr_pos, direction, physLimb->collision_radius, 
		torso_collider, collider_pairs, capsuleColCount, padding, &outwardExit);
	// Search in opposite direction for another valid exit that is outside all colliders
	// oppositeOK is a flag that indicates whether valid exit was found in opposite direction
	// oppositeExit stores the position of that exit.
    bool oppositeOK = Overcoat_GroupRayExit(physLimb->curr_pos, opposite, physLimb->collision_radius, 
		torso_collider, collider_pairs, capsuleColCount, padding, &oppositeExit);

    // If exit was found in normal outward direction, use it when either:
	// 1. no safe exit was found in the opposite direction OR
	// 2. outward exit is closer to the phys limb than the opposite exit
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
 * @param torso_collider	Torso sphere collider
 * @param collider_pairs	Array of pairs of colliders that are used for establishing capsule colliders
 * @param capsuleColCount	Number of capsule colliders
 */
void Overcoat_SolveMidpointGroup(PhysLimb* physLimb_a, PhysLimb* physLimb_b, 
	PhysSphereCollider* torso_collider, PhysSphereCollider* collider_pairs[][2], int capsuleColCount) {
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
    Overcoat_SolveColliderGroup(&midpoint, torso_collider, collider_pairs, capsuleColCount);
	// Calculate how far midpoint was moved by the collision correction.
    Vec3f correction = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&midpoint.curr_pos, &midpointBeforeCollision, &correction);

	// Apply the midpoint's collision correction to both neighboring phys limbs
	Math_Vec3f_Sum(&physLimb_a->curr_pos, &correction, &physLimb_a->curr_pos);
    Math_Vec3f_Sum(&physLimb_b->curr_pos, &correction, &physLimb_b->curr_pos);
}


/**
 * @brief Reset the overcoat's cloth shape after a physics reset
 * 
 * This function resets the unpinned phys limbs of each vertical overcoat chain after the physics state
 * of the phys limbs are reset and collapsed into an idle pose at the torso position.
 * 
 * It uses the following to reset each chain:
 * 
 * 1. The pinned phys limbs at the start of each chain
 * 
 * 2. Jackie's current down, forward, and left directional references
 * 
 * 3. Default lenngth of each phys limb offset
 * 
 * 4. Overcoat actor's current scale
 * 
 * Each overcoat chain is reset to be outward and downward from the pinned phys limbs. Front chains are
 * set to go forward, back chains are set to go backweard, and the 2 sidemost chains (left and right) are 
 * set to go left and right, respectively. The unpinned phys limbs are then placed one after another using 
 * their default bone lengths.
 * 
 * After resetting the cloth shape, all previous positions are matched to the reset current positions and 
 * all velocities are cleared so no movement from the reset state carries into the next physics update.
 * 
 * @param this          Overcoat actor
 * @param gPhysLimbs    Overcoat's phys limbs
 * @param chains        Array of starting phys limb index of each vertical chain of phys limbs
 * @param chainCount    Number of vertical cloth chains in overcoat
 */
void Overcoat_SetResetShape(Overcoat* this, PhysLimb* gPhysLimbs[], const int chains[], int chainCount) {
	/********************************
     Get current overcoat scale
    ********************************/
	// Use the overcoat's scale in the current frame
    // This is used to scale each phys limb's default joint pos length into phys bone length in world space
    f32 scale = fabsf(this->actor.scale.x);

	/********************************
     Rebuild each vertical cloth chain
    ********************************/
	// Go through every vertical cloth chain in the overcoat and reset its unpinned phys limbs starting from 
    // the pinned phys limb at the top of that chain
    for (int currChain = 0; currChain < chainCount; currChain++) {
        int startingLimbIdx = chains[currChain];
        int chainLimbCount = 9;
        Vec3f outwardDir = {0.f, 0.f, 0.f};

		/********************************
         Choose outward direction for current chain
        ********************************/
		// Determine which direction the current cloth chain should extend away from Jackie's body. The
		// direction depends on where the chain is located around the overcoat model.
		
		// Leftmost cloth chain
        if (startingLimbIdx == OVERCOAT_BODYPART_LEFTLIMB5_A) {
            outwardDir = overcoat_leftAxis;
            chainLimbCount = 12;
        }
		// Rightmost cloth chain
        else if (startingLimbIdx == OVERCOAT_BODYPART_RIGHTLIMB5_A) {
            Math_Vec3f_ScaleAndStore(&overcoat_leftAxis, -1.f, &outwardDir);
            chainLimbCount = 12;
        }
		// Front (left and right) cloth chains
        else if (currChain < 5 || currChain > 15) {
            outwardDir = overcoat_forwardAxis;
        }
		// Back (left and right) cloth chains
        else {
            Math_Vec3f_ScaleAndStore(&overcoat_forwardAxis, -1.f, &outwardDir);
        }

		/********************************
         Calculate reset direction
        ********************************/
        // Combine Jackie's downward direction with the current chain's outward direction. This makes the 
        // reset chain extend both downward and away from Jackie's body.
        Vec3f direction = {0.f, 0.f, 0.f};
        Math_Vec3f_Sum(&overcoat_downAxis, &outwardDir, &direction);

		// Find lengthy of the combined vector so it can be normalized
        f32 directionLength = sqrtf(Math3D_Vec3fMagnitudeSq(&direction));

		// Skip current chain if direction vector is zero (or very close to zero).
        if (directionLength < GREATER_THAN_ZERO) {
            continue;
        }
		
		// Normalize direction so it can only represent direction, not distance
		Math_Vec3f_Scale(&direction, 1.f / directionLength);

		/********************************
         Rebuild unpinned phys limb in current chain
        ********************************/
		// Starting from the chain's pinned limb, rebuild each unpinned phys limb after reset. Each child is 
        // placed one scaled bone length away from its parent along the calculated downward/outward reset 
        // direction.
        for (int currPhysLimb = 1; currPhysLimb < chainLimbCount; currPhysLimb++) {
			// Get parent phys limb and the child phys limb directly below it in current chain
            PhysLimb* parentPhysLimb = gPhysLimbs[startingLimbIdx + currPhysLimb - 1];
            PhysLimb* childPhysLimb = gPhysLimbs[startingLimbIdx + currPhysLimb];

			// Get child phys limb's default joint pos offset from parent phys limb
            Vec3f jointOffset = {childPhysLimb->default_jointPos.x, childPhysLimb->default_jointPos.y,
                childPhysLimb->default_jointPos.z};

            // Calculate the bone's world-space length.
            f32 boneLength = sqrtf(Math3D_Vec3fMagnitudeSq(&jointOffset)) * scale;

            // Move along the reset direction by one bone length.
            Vec3f boneOffset = {0.f, 0.f, 0.f};
            Math_Vec3f_ScaleAndStore(&direction, boneLength, &boneOffset);
            Math_Vec3f_Sum(&parentPhysLimb->curr_pos, &boneOffset, &childPhysLimb->curr_pos);

            // Save the rebuilt phys limb position as the corresponding overcoat bodyPartsPos
            this->bodyPartsPos[startingLimbIdx + currPhysLimb] = childPhysLimb->curr_pos;
        }
    }

    // Clear movement data for phys limbs without changing the pinned phys limb's positions.
    for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
        gPhysLimbs[i]->prev_pos = gPhysLimbs[i]->curr_pos;
        gPhysLimbs[i]->curr_vel = (Vec3f){0.f, 0.f, 0.f};
        gPhysLimbs[i]->prev_vel = (Vec3f){0.f, 0.f, 0.f};
    }
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
 * @param anchorStart           Previous frame's pinned phys limbs' starting position
 * @param chains                Array of starting phys limb indices of each vertical chains of overcoat
 * @param chainCount            Number of vertical cloth chains in the overcoat
 * @param sidewaysMaxDistance   Maximum allowed distance between matching phys limbs next to each other
 * @param capsulePairs          Pair of sphere colliders used as capsule colliders
 * @param capsuleCount          Number of pairs of sphere colliders contained in capsulePairs
 */
void Overcoat_SolvePhysAtSubsteps(Overcoat* this, Player* player, Vec3f apply_force, PhysLimb* gPhysLimbs[], 
	PhysBone* gPhysBones[], Vec3f anchorStart[], const int chains[], int chainCount, 
    f32 sidewaysMaxDistance[][9], PhysSphereCollider* capsulePairs[][2], int capsuleCount
) {
	/********************************
     Set up target positions of current frame
    ********************************/
    // Store target world position of each limb in current frame.
	// For pinned limbs, this becomes the target position they move toward across the substeps. This says
	// where each pinned limb should end up by the end of the frame.
    static Vec3f anchorTarget[OVERCOAT_BODYPART_MAX];
	
	// Store target length of each bone for current frame, which says how long each bone should be by the
	// end of frame
    static f32 targetBoneLengths[OVERCOAT_BONE_MAX];
	
	// Store pointers to all sphere colliders used by overcoat's limbs
	// Most of these sphere colliders are to be used for capsule colliders
    PhysSphereCollider* colliders[OVERCOAT_SUBSTEP_COLLIDER_COUNT] = {
        &overcoat_torsoCollider,
        &overcoat_leftThighBackCollider, &overcoat_rightThighBackCollider,
        &overcoat_leftThighFrontCollider, &overcoat_rightThighFrontCollider,
        &overcoat_leftLegCollider, &overcoat_rightLegCollider
    };
	
	// Store each collider's target position and radius for current frame
	// Like the limbs, colliders' positions will change every substep
	// Colliders' radii would change for capsule colliders with differently-sized sphere colliders
    PhysSphereCollider targetColliders[OVERCOAT_SUBSTEP_COLLIDER_COUNT];
	
	// Set up Jackie's forward, down, and left directions in world space.
	// The overcoat uses these directions to keep its connected cloth chains moving correctly
	// relative to Jackie's body.
	// This system is designed for overcoat specifically because it has chains of limbs that are horizontally 
	// connected to each other. So, this directional system is set up to help with both vertical AND 
	// horizontal constraints, whereas belt strap hook and ponytail only has vertical constraints
    Vec3f targetForward = overcoat_forwardAxis;
    Vec3f targetDown = overcoat_downAxis;
    Vec3f targetLeft = overcoat_leftAxis;
	
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
    Vec3f opposingVelocity = player->actor.velocity;
    opposingVelocity.y = 0.f;	// ignore vertical movement

    // Disable the opposing movement while riding Epona, climbing ladder/wall, or opening door.
    // Riding Epona: PLAYER_STATE1_800000
    // Climbing Wall: PLAYER_STATE1_200000
    // Opening Door: Player_Action_36
    // Note: Physics for riding Epona was disabled due to some unwanted physics behavior. This may be due to
    //       the codebase relying on player->actor.velocity for player's world velocity. This works for most
    //       cases EXCEPT when player is riding on Epona, where player->actor.velocity might be saving the
    //       player's last frame's world velocity. 
    if (player->stateFlags1 & (PLAYER_STATE1_800000 | PLAYER_STATE1_200000) ||
        player->actionFunc == Player_Action_36) {
        opposingVelocity = (Vec3f){0.f, 0.f, 0.f};
    }

	/********************************
     Save target colliders' state in current frame
    ********************************/
	// Save each collider's target position and radius for current frame.
	// During the substeps, the colliders will move from their position in the previous frame to their target
	// position in the current frame, substep by substep.
    for (int c = 0; c < OVERCOAT_SUBSTEP_COLLIDER_COUNT; c++) {
        targetColliders[c] = *colliders[c];
    }
	
	/********************************
     Save phys limbs' target positions in current frame
    ********************************/
	// Save each phys limb's target world positions at the current frame.
	// Note: For pinned phys limbs, this position was already set by Overcoat_UpdatePinnedLimb()
    for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
        anchorTarget[i] = gPhysLimbs[i]->curr_pos;
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
        for (int c = 0; c < OVERCOAT_SUBSTEP_COLLIDER_COUNT; c++) {
            overcoat_prevStepColliders[c] = targetColliders[c];
        }
		
		// Initialize starting positions of phys limbs
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
			// If the phys limb is pinned and it has no valid previous frame, start this phys limb at its
			// current frame's target position
            if (gPhysLimbs[i]->pinned) {
                anchorStart[i] = anchorTarget[i];
            }
			// Remove any old Verlet Integration movement from the phys limb by assigning current frame's 
			// target position as the phys previous position
            gPhysLimbs[i]->prev_pos = gPhysLimbs[i]->curr_pos;
        }
		
		// Use current frame's target directions (described in world space) and scale as previous frame's
		// values. This is only done when a valid previous frame does not exist, so the first substeps
		// do not interpolate from outdated directions or actor scale.
        overcoat_previousSubstepForward = targetForward;
        overcoat_previousSubstepDown = targetDown;
        overcoat_previousSubstepScale = targetScale;
    }

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
     Solve Verlet Integration Across Substeps (for 1 frame)
    ********************************/
	// Instead of solving the verlet integration of limbs from 1 previous frame to current frame, divides
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
         Interpolate actor scale
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
         Interpolate colliders
        ********************************/
		// Move each collider to a new position between the previous frame's position and the current
		// frame's target position based on what substep it is (t) between the two frames.
		// This is found via interpolation with the (t) value
        for (int c = 0; c < OVERCOAT_SUBSTEP_COLLIDER_COUNT; c++) {
			// Interpolate collider center position for the current substep
            CustomMath_Vec3f_Lerp(&overcoat_prevStepColliders[c].center, 
				&targetColliders[c].center, t, &colliders[c]->center);
			// Interpolate collider radius for the current substep
            colliders[c]->radius = overcoat_prevStepColliders[c].radius +
                (targetColliders[c].radius - overcoat_prevStepColliders[c].radius) * t;
        }
		
		/********************************
         Interpolate directional reference axes
        ********************************/
		// Set Jackie's forward and down directions (in world space) to the orientation they should have 
		// during the current substep. These directions are interpolated between the previous frame's
		// directional references and the current frame's target directional references of Jackie.
        if (overcoat_dirAxesReady) {
            Overcoat_SetSubstepDirAxes(targetForward, targetDown, t);
        }
		
		/********************************
         Update bone lengths based on current substep
        ********************************/
		// Set each bone's length to the properly scaled value for the current substep
        for (int j = 0; j < OVERCOAT_BONE_MAX; j++) {
            gPhysBones[j]->bone_length = targetBoneLengths[j] * scaleRatio;
        }
		
		/********************************
         Move pinned and unpinned phys limbs
        ********************************/
		// Update every phys limb of the overcoat for the current substep.
		// Pinned limbs are moved to their interpolated positions according to current substep.
		// Unpinned limbs are moved using Verlet Integration based on current substep's data.
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
            PhysLimb* currPhysLimb = gPhysLimbs[i];
			// For pinned phys limb...
            if (currPhysLimb->pinned) {
				// Save the pinned phys limb's position from the previous substep.
				if (currSubstep == 0) {
					// If this is a first substep, use the pinned phys limb's starting position
					// from the previous frame as its previous position
					currPhysLimb->prev_pos = anchorStart[i];
				}
				else {
					// For subsequent substeps, use the phys limb's position from the previous
					// substep as its previous position
					currPhysLimb->prev_pos = currPhysLimb->curr_pos;
				}
				// Move pinned phys limb to the interpolated position it should have during
				// current substep
                CustomMath_Vec3f_Lerp(&anchorStart[i], &anchorTarget[i], t, &currPhysLimb->curr_pos);
            } 
			// For unpinned limbs...
			else {
				// Move unpinned limbs using Verlet integration within this substep
                Verlet_LimbUpdatePosSubstep(currPhysLimb, &apply_force, &opposingVelocity, dt);
            }
        }
		
		/********************************
         Distribute constraint iterations across substeps
        ********************************/
		// Divide total number of constraint-solving iterations for one frame (e.g. collider collisions and 
        // bone length constraints) across all substeps. 
		// For instance, if COLLISION_FACTOR is 75, which means there is to be 75 constraint-solving
		// iterations, the 75 total iterations per frame is distibuted across every each substep 
		// (19 or 18 iterations per substep).
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
         Solve cloth constraints with calculated iterations for this substep
        ********************************/
        // Repeatedly correct overcoat's Phys limb positions for current substep. Each iteration fixes
		// bone lengths, limits sideays stretching between cloth chains horizontally, applies upper
		// overcoat movement restrictions, and pushes horizontal midpoint between cloth limb out of
		// colliders.
		
		// Repeat these corrections 'iterations' times for the current substep so the cloth can settle 
		// into a more stable position.
        for (int iteration = 0; iteration < iterations; iteration++) {
			// Correct every phys bone's length so that it matches the calculated respective target
			// lengths for this substep
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
			
			// Limit the direction each upper overcoat bone can bend relative to Jackie's body.
			// In other words, this applies artificial directional constraints to the bones to prevent
			// unwanted or unrealistic physics behavior. It is the most artificial element of this physics
			// project. Check function for more details.
            Overcoat_LimitUpperChains(gPhysLimbs, chains, chainCount);
			
			// Apply collision correction to all the unpinned phys limbs. If a phys limb position is 
			// calculated to be inside a collider in this substep, push it back out.
            for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
				// Skip pinned phys limbs
                if (gPhysLimbs[i]->pinned) {
                    continue;
                }
				
				// Take all the colliders (sphere and capsules) and correct the phys limb's position if needed
                Overcoat_SolveColliderGroup(gPhysLimbs[i], &overcoat_torsoCollider, capsulePairs, capsuleCount);
            }
			
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
                        &overcoat_torsoCollider, capsulePairs, capsuleCount);
                }
            }
        }

        // Final cleanup of every phys limb's position corrections for current substep
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
            PhysLimb* limb = gPhysLimbs[i];
			
			// If current phys limb is not pinned, then run a collision check one more time
            if (!limb->pinned) {
                Overcoat_SolveColliderGroup(limb, &overcoat_torsoCollider, capsulePairs, capsuleCount);
            }
			
			// Set curretn phys limb's current velocity to previous velocity
            limb->prev_vel = limb->curr_vel;
			// Calculate current velocity of phys limb based on change in position in this substep
            Math_Vec3f_Diff(&limb->curr_pos, &limb->prev_pos, &limb->curr_vel);
			// Divide the movement by substep's time interval to convert it into correct velocity.
            Math_Vec3f_Scale(&limb->curr_vel, (1.f / dt));	
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
        *colliders[c] = targetColliders[c];
        overcoat_prevStepColliders[c] = targetColliders[c];
    }

    // Restore all pinned phys limbs to their current frame's target world positions
    for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
        if (gPhysLimbs[i]->pinned) {
            gPhysLimbs[i]->curr_pos = anchorTarget[i];
        }
    }

    // Restore every bone to its exact target length for the current frame.
    for (int j = 0; j < OVERCOAT_BONE_MAX; j++) {
        gPhysBones[j]->bone_length = targetBoneLengths[j];
    }

    // Restore Jackie's directional references back to current frame's original directional references
    overcoat_forwardAxis = targetForward;
    overcoat_downAxis = targetDown;
    overcoat_leftAxis = targetLeft;

    // Save current frame's directional references and scale as previous frame's values for next frame
    overcoat_previousSubstepForward = targetForward;
    overcoat_previousSubstepDown = targetDown;
    overcoat_previousSubstepScale = targetScale;

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
 * @brief Update the positions of overcoat based on verlet integration
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
    // Save Jackie's previous position as overcoat's previous position
    Math_Vec3f_Copy(&this->actor.prevPos, &player->actor.prevPos);

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
        return;
    }

    // Save starting positions of the phys limbs from the previous frame before overwriting the pinned phys
    // limbs with the new current frame's target positions
    // This is necessary because the overcoat interpolates pinned phys limbs' positions across multiple 
    // substeps instead of moving them directly from one frame position to the next.
    static Vec3f substepAnchorStart[OVERCOAT_BODYPART_MAX];
    for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
        substepAnchorStart[i] = gPhysLimbs[i]->curr_pos;
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
    // Store the starting phys limb of each overcoat chain.
    // Each of these phys limb is pinned like the overcoat root limb and acts as the fixed point
    // for the rest of the unpinned phys limbs in their respective vertical cloth chain
    const int pinnedLimbs[] = {
		OVERCOAT_BODYPART_LEFTLIMB0_A,  // front left limb
		OVERCOAT_BODYPART_LEFTLIMB1_A,
		OVERCOAT_BODYPART_LEFTLIMB2_A,
		OVERCOAT_BODYPART_LEFTLIMB3_A,
		OVERCOAT_BODYPART_LEFTLIMB4_A,
		OVERCOAT_BODYPART_LEFTLIMB5_A,
		OVERCOAT_BODYPART_LEFTLIMB6_A,
		OVERCOAT_BODYPART_LEFTLIMB7_A,
		OVERCOAT_BODYPART_LEFTLIMB8_A,
		OVERCOAT_BODYPART_LEFTLIMB9_A,

		OVERCOAT_BODYPART_BACKLIMB_A,   // back limb

		OVERCOAT_BODYPART_RIGHTLIMB9_A, 
		OVERCOAT_BODYPART_RIGHTLIMB8_A,
		OVERCOAT_BODYPART_RIGHTLIMB7_A,
		OVERCOAT_BODYPART_RIGHTLIMB6_A,
		OVERCOAT_BODYPART_RIGHTLIMB5_A,
		OVERCOAT_BODYPART_RIGHTLIMB4_A,
		OVERCOAT_BODYPART_RIGHTLIMB3_A,
		OVERCOAT_BODYPART_RIGHTLIMB2_A,
		OVERCOAT_BODYPART_RIGHTLIMB1_A,
		OVERCOAT_BODYPART_RIGHTLIMB0_A  // front right limb
	};

    // Update every pinned phys limb so each cloth chain starts from its current
    // world position based on the overcoat root limb's position
	for (int i = 0; i < OVERCOAT_CHAIN_COUNT; i++) {
		Overcoat_UpdatePinnedLimb(this, player, gPhysLimbs, pinnedLimbs[i]);
	}

    /********************************
     Scale body colliders
    ********************************/
    // Match the size of the colliders with Jackie's current actor scale.
    // Note: 0.01f is Jackie's normal actor scale.
    f32 colliderScale = player->actor.scale.x / 0.01f;
    
    // Scale each collider's default radius based on Jackie's current actor scale.
    overcoat_leftThighBackCollider.radius = LEFTTHIGH_COLLIDER_DEFAULT_RADIUS * colliderScale;
    overcoat_rightThighBackCollider.radius = RIGHTTHIGH_COLLIDER_DEFAULT_RADIUS * colliderScale;
    overcoat_leftThighFrontCollider.radius = LEFTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS * colliderScale;
    overcoat_rightThighFrontCollider.radius = RIGHTTHIGH_FRONT_COLLIDER_DEFAULT_RADIUS * colliderScale;
    overcoat_leftLegCollider.radius = LEFTLEG_COLLIDER_DEFAULT_RADIUS * colliderScale;
    overcoat_rightLegCollider.radius = RIGHTLEG_COLLIDER_DEFAULT_RADIUS * colliderScale;
    overcoat_torsoCollider.radius = TORSO_COLLIDER_DEFAULT_RADIUS * colliderScale;

    /********************************
     Set up sideways cloth connections for horizontal distance limits
    ********************************/
    // Store the starting phys limb index of each vertical overcoat chain from front left to front right
    // Note that front left (LeftLimb0) and front right (RightLimb0) chains are not connected.
    // Next to one another in this array are treated as neighboring cloth chains and are connected 
    // horizontally by horizontal distance constraints.
    const int sidewaysChains[] = {
        OVERCOAT_BODYPART_LEFTLIMB0_A,
        OVERCOAT_BODYPART_LEFTLIMB1_A,
        OVERCOAT_BODYPART_LEFTLIMB2_A,
        OVERCOAT_BODYPART_LEFTLIMB3_A,
        OVERCOAT_BODYPART_LEFTLIMB4_A,
        OVERCOAT_BODYPART_LEFTLIMB5_A,
        OVERCOAT_BODYPART_LEFTLIMB6_A,
        OVERCOAT_BODYPART_LEFTLIMB7_A,
        OVERCOAT_BODYPART_LEFTLIMB8_A,
        OVERCOAT_BODYPART_LEFTLIMB9_A,
        OVERCOAT_BODYPART_BACKLIMB_A,
        OVERCOAT_BODYPART_RIGHTLIMB9_A,
        OVERCOAT_BODYPART_RIGHTLIMB8_A,
        OVERCOAT_BODYPART_RIGHTLIMB7_A,
        OVERCOAT_BODYPART_RIGHTLIMB6_A,
        OVERCOAT_BODYPART_RIGHTLIMB5_A,
        OVERCOAT_BODYPART_RIGHTLIMB4_A,
        OVERCOAT_BODYPART_RIGHTLIMB3_A,
        OVERCOAT_BODYPART_RIGHTLIMB2_A,
        OVERCOAT_BODYPART_RIGHTLIMB1_A,
        OVERCOAT_BODYPART_RIGHTLIMB0_A
    };

    // Calculate number of sideways cloth chains
    int sidewaysChainCount = sizeof(sidewaysChains) / sizeof(sidewaysChains[0]);    // Should be 21

    /********************************
     Set up capsule colliders with sphere collider pairs
    ********************************/
    // Two sphere colliders are paired together to create a capsule collider
    // Capsule colliders essentially cover the hips and legs to prevent the overcoat from clipping
    // through Jackie's body between individual sphere colliders.
    PhysSphereCollider* capsulePairs[][2] = {
        {&overcoat_leftThighBackCollider, &overcoat_rightThighBackCollider},
        {&overcoat_leftThighFrontCollider, &overcoat_rightThighFrontCollider},
        {&overcoat_leftThighFrontCollider, &overcoat_leftLegCollider},
        {&overcoat_leftThighBackCollider, &overcoat_leftLegCollider},
        {&overcoat_rightThighFrontCollider, &overcoat_rightLegCollider},
        {&overcoat_rightThighBackCollider, &overcoat_rightLegCollider},
        {&overcoat_rightThighFrontCollider, &overcoat_leftLegCollider},     // Criss cross from right front thigh to left leg/knee
        {&overcoat_leftThighFrontCollider, &overcoat_rightLegCollider},     // Criss cross from left front thigh to right leg/knee
    };

    // Calculate number of capsule colliders
    int capsuleCount = sizeof(capsulePairs) / sizeof(capsulePairs[0]);      // should be 8

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
     Rebuild overcoat after physics reset
    ********************************/
    // If there is no valid previous frame physics values, then reset the unpinned phys limbs into
    // valid pre-set cloth shape before starting the substep simulation
    if (!overcoat_stepHistoryReady) {
        // Wait until Jackie's torso direction references are set, since they are needed to
        // reset the overcoat shape
        if (!overcoat_dirAxesReady) {
            return;
        }

        // Reset the unpinned phys limbs from the pinned chain anchors so the overcoat starts from
        // valid pre-set cloth shape instead of remaining collapsed at torso
        Overcoat_SetResetShape(this, gPhysLimbs, sidewaysChains, sidewaysChainCount);

        // Use the reset phys limb positions as starting positions for the upcoming substep
        // physics operations
        for (int i = 0; i < OVERCOAT_BODYPART_MAX; i++) {
            substepAnchorStart[i] = gPhysLimbs[i]->curr_pos;
        }
    }

    /********************************
     Physics Operations with Substeps
    ********************************/
    // Run the physics operations (e.g. position changes, constraints, etc.) across multiple
    // substeps 
    Overcoat_SolvePhysAtSubsteps(
        this, player, apply_force, gPhysLimbs, gPhysBones, substepAnchorStart,
        sidewaysChains, sidewaysChainCount, sidewaysMaxDistance,
        capsulePairs, capsuleCount
    );
}


/**
 * @brief Rotate bone with pinned parent limb while using rotation stabilizer
 * 
 * Rotate actual overcoat bone in the direction of the matching phys bone. To do this, find a stable shortest 
 * rotation possible for the actual bone. This is done by using a quaternion, which helps with:
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
 * @param this          Ponytail actor
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
        Vec3s* jointRot = &this->skelAnime.jointTable[i + 1];   // Joint's rotation in ponytail skeleton
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
 * @brief Rotate overcoat' limbs (joints) based on phys bones' phys limbs' positions
 * 
 * This function takes the overcoat and rotates its limbs so that it matches with the overcoat's phys
 * bones. This is done by rotating each chain of limbs in the overcoat at a time using the 
 * Overcoat_RotateChain() function.
 * 
 * @param this          Ponytail actor
 * @param gPhysBones    Physics bones that have been impacted by verlet integration for this frame
 * @param player        Player actor
 */
void Overcoat_RotateJoints(Overcoat* this, PhysBone* gPhysBones[], Player* player) {
    Vec3f zero = {0.f, 0.f, 0.f};

    /********************************
     Define overcoat physics chains
    ********************************/
    // Establish chains of overcoat limbs. Each chain contains:
    // [0] = Root -> limb_A bone index for this chain
    // [1] = Number of bones in a chain of limbs
    s16 chainCount = 21;

    s16 chains[][2] = {
        {OVERCOAT_BONE_ROOT_BACKLIMB_A, 9},     // Backmost chain in overcoat
        {OVERCOAT_BONE_ROOT_LEFTLIMB0_A, 9},    // Front-left chain in overcoat
        {OVERCOAT_BONE_ROOT_LEFTLIMB1_A, 9},
        {OVERCOAT_BONE_ROOT_LEFTLIMB2_A, 9},
        {OVERCOAT_BONE_ROOT_LEFTLIMB3_A, 9},
        {OVERCOAT_BONE_ROOT_LEFTLIMB4_A, 9},
        {OVERCOAT_BONE_ROOT_LEFTLIMB5_A, 12},   // Leftmost chain in overcoat
        {OVERCOAT_BONE_ROOT_LEFTLIMB6_A, 9},
        {OVERCOAT_BONE_ROOT_LEFTLIMB7_A, 9},
        {OVERCOAT_BONE_ROOT_LEFTLIMB8_A, 9},
        {OVERCOAT_BONE_ROOT_LEFTLIMB9_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB0_A, 9},   // Front-right chain in overcoat
        {OVERCOAT_BONE_ROOT_RIGHTLIMB1_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB2_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB3_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB4_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB5_A, 12},  // Rightmost chain in overcoat
        {OVERCOAT_BONE_ROOT_RIGHTLIMB6_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB7_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB8_A, 9},
        {OVERCOAT_BONE_ROOT_RIGHTLIMB9_A, 9}
    };

    /********************************
     Set shared root orientation
    ********************************/
    // Set up matrix to face in the direction of overcoat
    Matrix_Push();
    // Make matrix face overcoat actor's rotation (which should also be player actor's rotation)
    // Y-axis primarily defines actor's normal facing/yaw direction, so rotate by Y-axis first
    Matrix_SetTranslateRotateYXZ(0.f, 0.f, 0.f, &this->actor.shape.rot);
    // Rotate matrix again, this time by the overcoat's root rotation (player's skeleton head limb)
    Matrix_TranslateRotateZYX(&zero, &this->skelAnime.jointTable[OVERCOAT_ROOT_ROT]);

    /********************************
     Rotate each overcoat chain
    ********************************/
    // Iterate through every chain in the overcoat to rotate its limbs one by one
    for (int i = 0; i < chainCount; i++) {
        Overcoat_RotateChain(this, gPhysBones, chains[i][0], chains[i][1]);
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

    // Bool that indicates if overcoat should be drawn or not in main draw function
    this->hideInFirstPerson = 0;

    // Get reset positions of overcoats' phys limbs when transforming from non-human to human
    if (player->transformation != PLAYER_FORM_HUMAN) {
        this->needsReset = 1;
    }

    // Explaining some states:
    // - PLAYER_STATE1_100000 indicates first person mode for bow/hookshot
    // - Camera->mode helps indicate first person mode for C-Up
    // - There is no player state for the transition into regular C-Up first person,
    //   so distance between camera and head is also used

    // Camera information
    Camera* camera = GET_ACTIVE_CAM(play);
    f32 cameraDist = Math_Vec3f_DistXYZ( &play->view.eye, &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);

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

    // Remove overcoat when player transforms from human to non-human
    if (player->transformation != PLAYER_FORM_HUMAN && gPlayerOvercoat != NULL &&
        player->actor.draw == NULL) {
        Actor_Kill(&this->actor);
        gPlayerOvercoat = NULL;
        return;
    }
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
        if (this->hideInFirstPerson) {
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

            // Calculate rotations based on overcoat limbs' calculated positions
            Overcoat_RotateJoints(this, overcoat_PhysBones, player);
        }

        // Draw overcoat
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