/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

// MACROS
#define CHAR_EPONA_HEIGHT_MODIFIER 1100.f
#define CHAR_BREMEN_HEIGHT_MODIFIER 1250.0f

#define OSN_MODIFIER 0.f    // Happy Mask Salesman Height modifier
#define ITEM_HEIGHT_MODIFIER 2.5f

#define MOVEARROWX -0x0070
#define MOVEARROWY 0x0120
#define MOVEARROWZ 0x0000

#define CASTLE_RD_GRABSTART_POS_Y 30.f
#define CASTLE_RD_GRABATTACK_POS_Y 0.f


// Default Header Files
#include "modding.h"
#include "global.h"
#include "ultra64.h"

// Model Header Files
#include "gJackieSkel.h"
#include "gJackieSkel_hands_closed.h"
#include "gJackieSkel_hands_bottle.h"
#include "gJackieSkel_hands_ocarina.h"
#include "gJackieSkel_fps_bow.h"
#include "gJackieSkel_fps_hookshot.h"
#include "gJackieSkel_fps_leftarmhand.h"

#include "gJackiePonytailSkel.h"

// Redead/Gibdo Files for adjusting their grab height on Jackie
#include "overlays/actors/ovl_En_Rd/z_en_rd.h"
#include "overlays/actors/ovl_En_Railgibud/z_en_railgibud.h"
#include "overlays/actors/ovl_En_Talk_Gibud/z_en_talk_gibud.h"

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));


/***********************************************************************

	Jackie Model

***********************************************************************/

/*
=================
Eyes and Mouth Flipbooks
=================
*/

extern TexturePtr sPlayerMouthTextures[PLAYER_MOUTH_MAX];   // Mouth
extern TexturePtr sPlayerEyesTextures[PLAYER_EYES_MAX];     // Eyes

// Eyes Texture Variables
// Default Head 
extern u64 gJackieEyesOpenTex[];
extern u64 gJackieEyesHalfTex[];
extern u64 gJackieEyesClosedTex[];
extern u64 gJackieEyesRollLeftTex[];
extern u64 gJackieEyesRollRightTex[];
extern u64 gJackieEyesShockTex[];
extern u64 gJackieEyesRollDownTex[];    // gLinkAdultEyesUnk1Tex 
extern u64 gJackieEyesShutTex[];        // gLinkAdultEyesUnk2Tex

// Ponytail Head
extern u64 gJackiePonytailSkel_eyes_open_LOWRES_rgba16[];
extern u64 gJackiePonytailSkel_eyes_half_LOWRES_rgba16[];
extern u64 gJackiePonytailSkel_eyes_closed_LOWRES_rgba16[];
extern u64 gJackiePonytailSkel_eyes_roll_left_LOWRES_rgba16[];
extern u64 gJackiePonytailSkel_eyes_roll_right_LOWRES_rgba16[];
extern u64 gJackiePonytailSkel_eyes_shocked_LOWRES_rgba16[];
extern u64 gJackiePonytailSkel_eyes_roll_down_LOWRES_rgba16[];    // gLinkAdultEyesUnk1Tex 
extern u64 gJackiePonytailSkel_eyes_shut_LOWRES_rgba16[];        // gLinkAdultEyesUnk2Tex

// Eyes Flipbook for Jackie
// Default Head
void* sEyeTextures[PLAYER_EYES_MAX] = {
    gJackieEyesOpenTex,
    gJackieEyesHalfTex,
    gJackieEyesClosedTex,
    gJackieEyesRollLeftTex,
    gJackieEyesRollRightTex,
    gJackieEyesShockTex,
    gJackieEyesRollDownTex,
    gJackieEyesShutTex,
};

// Ponytail Head
void* sPTEyeTextures[PLAYER_EYES_MAX] = {
    gJackiePonytailSkel_eyes_open_LOWRES_rgba16,
    gJackiePonytailSkel_eyes_half_LOWRES_rgba16,
    gJackiePonytailSkel_eyes_closed_LOWRES_rgba16,
    gJackiePonytailSkel_eyes_roll_left_LOWRES_rgba16,
    gJackiePonytailSkel_eyes_roll_right_LOWRES_rgba16,
    gJackiePonytailSkel_eyes_shocked_LOWRES_rgba16,
    gJackiePonytailSkel_eyes_roll_down_LOWRES_rgba16,
    gJackiePonytailSkel_eyes_shut_LOWRES_rgba16,
};

// Mouth Texture Variables
// Default
extern u64 gJackieMouthClosed[];    // gLinkAdultMouth1Tex
extern u64 gJackieMouthHalf[];      // gLinkAdultMouth2Tex
extern u64 gJackieMouthOpen[];      // gLinkAdultMouth3Tex
extern u64 gJackieMouthSmile[];     // gLinkAdultMouth4Tex

// Ponytail
extern u64 gJackiePonytailSkel_mouth_default_LOWRES_rgba16[];    // gLinkAdultMouth1Tex
extern u64 gJackiePonytailSkel_mouth_half_LOWRES_rgba16[];      // gLinkAdultMouth2Tex
extern u64 gJackiePonytailSkel_mouth_open_LOWRES_rgba16[];      // gLinkAdultMouth3Tex
extern u64 gJackiePonytailSkel_mouth_smile_LOWRES_rgba16[];     // gLinkAdultMouth4Tex

// Mouth Flipbook for Jackie
// Default
void* sMouthTextures[PLAYER_MOUTH_MAX] = {
    gJackieMouthClosed,
    gJackieMouthHalf,
    gJackieMouthOpen,
    gJackieMouthSmile,
};

// Ponytail
void* sPTMouthTextures[PLAYER_MOUTH_MAX] = {
    gJackiePonytailSkel_mouth_default_LOWRES_rgba16,
    gJackiePonytailSkel_mouth_half_LOWRES_rgba16,
    gJackiePonytailSkel_mouth_open_LOWRES_rgba16,
    gJackiePonytailSkel_mouth_smile_LOWRES_rgba16,
};

// Rough fix for some issue caused by replacing these for child/adult (from OoT Link mod)
// Not including this will cause eyes and mouth textures for Goron Link and Zora Link to not work
// Goron eyes texture variables
extern u64 gLinkGoronEyesOpenTex[];
extern u64 gLinkGoronEyesHalfTex[];
extern u64 gLinkGoronEyesClosedTex[];
extern u64 gLinkGoronEyesSurprisedTex[];

// Eyes Flipbook for Goron Link
TexturePtr sGoronEyesTextures[PLAYER_EYES_MAX] = {
    gLinkGoronEyesOpenTex,      // PLAYER_EYES_OPEN
    gLinkGoronEyesHalfTex,      // PLAYER_EYES_HALF
    gLinkGoronEyesClosedTex,    // PLAYER_EYES_CLOSED
    gLinkGoronEyesSurprisedTex, // PLAYER_EYES_ROLL_RIGHT
    gLinkGoronEyesSurprisedTex, // PLAYER_EYES_ROLL_LEFT
    gLinkGoronEyesSurprisedTex, // PLAYER_EYES_ROLL_UP
    gLinkGoronEyesSurprisedTex, // PLAYER_EYES_ROLL_DOWN
    gLinkGoronEyesSurprisedTex, // PLAYER_EYES_7
};

// Zora eyes texture variables
extern u64 gLinkZoraEyesOpenTex[];
extern u64 gLinkZoraEyesHalfTex[];
extern u64 gLinkZoraEyesClosedTex[];
extern u64 gLinkZoraEyesRollRightTex[];
extern u64 gLinkZoraEyesRollLeftTex[];
extern u64 gLinkZoraEyesRollUpTex[];
extern u64 gLinkZoraEyesRollDownTex[];
extern u64 object_link_zora_Tex_003800[];

// Eyes Flipbook for Zora Link
TexturePtr sZoraEyesTextures[PLAYER_EYES_MAX] = {
    gLinkZoraEyesOpenTex,        // PLAYER_EYES_OPEN
    gLinkZoraEyesHalfTex,        // PLAYER_EYES_HALF
    gLinkZoraEyesClosedTex,      // PLAYER_EYES_CLOSED
    gLinkZoraEyesRollRightTex,   // PLAYER_EYES_ROLL_RIGHT
    gLinkZoraEyesRollLeftTex,    // PLAYER_EYES_ROLL_LEFT
    gLinkZoraEyesRollUpTex,      // PLAYER_EYES_ROLL_UP
    gLinkZoraEyesRollDownTex,    // PLAYER_EYES_ROLL_DOWN
    object_link_zora_Tex_003800, // PLAYER_EYES_7
};

// Zora Link mouth texture variables
extern u64 gLinkZoraMouthClosedTex[];
extern u64 object_link_zora_Tex_004400[];
extern u64 gLinkZoraMouthAngryTex[];
extern u64 gLinkZoraMouthHappyTex[];

// Mouth Flipbook for Zora Link
TexturePtr sZoraMouthTextures[PLAYER_MOUTH_MAX] = {
    gLinkZoraMouthClosedTex,        // PLAYER_MOUTH_CLOSED
    object_link_zora_Tex_004400,    // PLAYER_MOUTH_TEETH
    gLinkZoraMouthAngryTex,         // PLAYER_MOUTH_ANGRY
    gLinkZoraMouthHappyTex,         // PLAYER_MOUTH_HAPPY
};


/*
=================
Replace Link's DLs with Jackie's DLs
=================
*/

// Right Hand
extern Gfx* gPlayerRightHandOpenDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerRightHandClosedDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerRightHandBowDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerRightHandInstrumentDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerRightHandHookshotDLs[2 * PLAYER_FORM_MAX];

// Left Hand
extern Gfx* gPlayerLeftHandOpenDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerLeftHandClosedDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerLeftHandTwoHandSwordDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerLeftHandOneHandSwordDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerLeftHandBottleDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerWaistDLs[2 * PLAYER_FORM_MAX];

// First Person Left Hand
extern Gfx* sPlayerFirstPersonLeftHandDLs[PLAYER_FORM_MAX];
extern Gfx* sPlayerFirstPersonLeftForearmDLs[PLAYER_FORM_MAX];

// First Person Right Hand
extern Gfx* sPlayerFirstPersonRightShoulderDLs[PLAYER_FORM_MAX];
extern Gfx* sPlayerFirstPersonRightHandDLs[PLAYER_FORM_MAX];
extern Gfx* sPlayerFirstPersonRightHandHookshotDLs[PLAYER_FORM_MAX];

// Mirror Shield
extern Gfx* gLinkHumanMirrorShieldDL[];
extern Gfx* gLinkHumanHerosShieldDL[];

// Kokiri Sword
extern Gfx* gKokiriSwordDL[];

// Razor Sword
extern Gfx* gRazorSwordDL[];

// Gilded Sword
extern Gfx* gLinkHumanGildedSwordHandleDL[];
extern Gfx* gLinkHumanGildedSwordBladeDL[];

// Great Fairy's Sword
extern Gfx* gLinkHumanGreatFairysSwordDL[];

// Bow
extern Gfx* gLinkHumanBowDL[];

// Hookshot
extern Gfx* gLinkHumanHookshotDL[];

// Array of DLs for different kinds of sword in hand
extern Gfx* D_801C018C[];

// Array of DLs for different kinds of shield in hand
extern Gfx* gPlayerHandHoldingShields[];

// Used to replace animation to adult link animation
extern PlayerAnimationHeader* D_8085BE84[PLAYER_ANIMGROUP_MAX][PLAYER_ANIMTYPE_MAX];

// Adult Link door animation
extern LinkAnimationHeader gPlayerAnim_clink_demo_doorA_link;
extern LinkAnimationHeader gPlayerAnim_clink_demo_doorB_link;

// DLs

Gfx gJackieRightHandHoldingMirrorShieldDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque),
    gsSPBranchList(gLinkHumanMirrorShieldDL),
};

Gfx gJackieRightHandHoldingHeroShieldDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque),
    gsSPBranchList(gLinkHumanHerosShieldDL),
};

Gfx gJackieLeftHandHoldingGildedSwordDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque),
    gsSPDisplayList(gLinkHumanGildedSwordHandleDL),
    gsSPBranchList(gLinkHumanGildedSwordBladeDL),
};

Gfx gJackieLeftHandHoldingKokiriSwordDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque),
    gsSPBranchList(gKokiriSwordDL),
};

Gfx gJackieLeftHandHoldingRazorSwordDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque),
    gsSPBranchList(gRazorSwordDL),
};

Gfx gJackieLeftHandHoldingGreatFairysSwordDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque),
    gsSPBranchList(gLinkHumanGreatFairysSwordDL),
};

Gfx gJackieRightHandShoulderAndForearm[] = {
    gsSPDisplayList(gJackieSkel_fps_bow_bone017_gLinkAdultRightArmLimb_mesh_layer_Opaque),
    gsSPBranchList(gJackieSkel_fps_bow_bone016_gLinkAdultRightShoulderLimb_mesh_layer_Opaque),
};

Gfx gJackieRightHandHoldingBowDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque),
    gsSPBranchList(gLinkHumanBowDL),
};

Gfx gJackieRightHandHoldingHookshotDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque),
    gsSPBranchList(gLinkHumanHookshotDL),
};


/*
=================
Jackie Properties
=================
*/
// Player Age properties
extern PlayerAgeProperties sPlayerAgeProperties[PLAYER_FORM_MAX];

extern Input* sPlayerControlInput;

// Function to update original Link with new properties (age, voice, and animation)
PlayerAgeProperties JackieProperties;
void updateLink(PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (JackieProperties.ceilingCheckHeight == 0) {
        JackieProperties = sPlayerAgeProperties[PLAYER_FORM_ZORA];
        JackieProperties.unk_28 = 44.8f;
        JackieProperties.unk_3C = 15.0f;
        JackieProperties.unk_44 = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_44;
        JackieProperties.unk_4A->x = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_4A->x;
        JackieProperties.unk_4A->y = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_4A->y;
        JackieProperties.unk_4A->z = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_4A->z;
        JackieProperties.unk_62->x = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_62->x;
        JackieProperties.unk_62->y = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_62->y;
        JackieProperties.unk_62->z = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_62->z;
        JackieProperties.unk_7A->x = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_7A->x;
        JackieProperties.unk_7A->y = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_7A->y;
        JackieProperties.unk_7A->z = sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY].unk_7A->z;
        JackieProperties.voiceSfxIdOffset = SFX_VOICE_BANK_SIZE * 1;
        JackieProperties.surfaceSfxIdOffset = 0x80;
        
    }
    if (player->transformation == PLAYER_FORM_HUMAN) {
        sPlayerAgeProperties[PLAYER_FORM_HUMAN] = JackieProperties;
        player->ageProperties = &JackieProperties;
    }
}

// Enable Jackie's Strength to be equal to Zora Link's strength for pushable blocks
extern u8 sPlayerStrengths[];
RECOMP_PATCH u8 Player_GetStrength(void) {
    if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
        return sPlayerStrengths[PLAYER_FORM_ZORA];
    }
    else {
        return sPlayerStrengths[GET_PLAYER_FORM];
    }
}

// Function to update Jackie's height with Zora Link's height
Player *gOriginalPlayer;
u8 gOriginalPlayerTransformation;
RECOMP_HOOK ("Player_GetHeight") void on_Player_GetHeight(Player* player) {
    gOriginalPlayer = player;
    gOriginalPlayerTransformation = player->transformation;
    player->transformation = PLAYER_FORM_ZORA;
}

RECOMP_HOOK_RETURN ("Player_GetHeight") void return_Player_GetHeight(void) {
    gOriginalPlayer->transformation = gOriginalPlayerTransformation;
}

extern FlexSkeletonHeader gJackiePTSkel;


// Function to replace Link's model with Jackie's model
RECOMP_HOOK("Player_Init") void on_Player_Init(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);

    gPlayerSkeletons[PLAYER_FORM_HUMAN] = &gJackieSkel;

    // Change body based on hairstyle toggle
    if (recomp_get_config_u32("change_hairstyle")) {
        gPlayerSkeletons[PLAYER_FORM_HUMAN] = &gJackiePTSkel;
    }
    
    // Right Hand DLs
    gPlayerRightHandOpenDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    gPlayerRightHandOpenDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    
    gPlayerRightHandClosedDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    gPlayerRightHandClosedDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    gPlayerRightHandInstrumentDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_hands_ocarina_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    gPlayerRightHandInstrumentDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_hands_ocarina_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    
    gPlayerRightHandBowDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieRightHandHoldingBowDL;
    gPlayerRightHandBowDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieRightHandHoldingBowDL;

    gPlayerRightHandHookshotDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieRightHandHoldingHookshotDL;
    gPlayerRightHandHookshotDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieRightHandHoldingHookshotDL;

    // Left Hand DLs
    gPlayerLeftHandOpenDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    gPlayerLeftHandOpenDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    
    gPlayerLeftHandClosedDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_hands_closed_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    gPlayerLeftHandClosedDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_hands_closed_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    gPlayerLeftHandOneHandSwordDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    gPlayerLeftHandOneHandSwordDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    gPlayerLeftHandTwoHandSwordDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieLeftHandHoldingGreatFairysSwordDL;
    gPlayerLeftHandTwoHandSwordDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieLeftHandHoldingGreatFairysSwordDL;
    gPlayerLeftHandBottleDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_hands_bottle_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    gPlayerLeftHandBottleDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_hands_bottle_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;

    // First Person Left Limb DLs
    sPlayerFirstPersonLeftForearmDLs[PLAYER_FORM_HUMAN] = gJackieSkel_fps_leftarmhand_bone014_gLinkAdultLeftArmLimb_mesh_layer_Opaque;
    sPlayerFirstPersonLeftHandDLs[PLAYER_FORM_HUMAN] = gJackieSkel_fps_leftarmhand_bone015_gLinkAdultLeftHandLimb_mesh_layer_Opaque;
    
    // First Person Right Limb DLs
    sPlayerFirstPersonRightShoulderDLs[PLAYER_FORM_HUMAN] = gJackieRightHandShoulderAndForearm;
    sPlayerFirstPersonRightHandDLs[PLAYER_FORM_HUMAN] = gJackieSkel_fps_bow_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    sPlayerFirstPersonRightHandHookshotDLs[PLAYER_FORM_HUMAN] = gJackieSkel_fps_hookshot_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    
    // Waist DLs
    gPlayerWaistDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_bone001_gLinkAdultWaistLimb_mesh_layer_Opaque;
    gPlayerWaistDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_bone001_gLinkAdultWaistLimb_mesh_layer_Opaque;

    // Shield DL
    gPlayerHandHoldingShields[0] = gJackieRightHandHoldingHeroShieldDL;
    gPlayerHandHoldingShields[1] = gJackieRightHandHoldingHeroShieldDL;
    gPlayerHandHoldingShields[2] = gJackieRightHandHoldingMirrorShieldDL;
    gPlayerHandHoldingShields[3] = gJackieRightHandHoldingMirrorShieldDL;

    // Sword DL
    D_801C018C[0] = gJackieLeftHandHoldingKokiriSwordDL;
    D_801C018C[1] = gJackieLeftHandHoldingKokiriSwordDL;
    D_801C018C[2] = gJackieLeftHandHoldingRazorSwordDL;
    D_801C018C[3] = gJackieLeftHandHoldingRazorSwordDL;
    D_801C018C[4] = gJackieLeftHandHoldingGildedSwordDL;
    D_801C018C[5] = gJackieLeftHandHoldingGildedSwordDL;
    
}

// Replace Door Animation
RECOMP_HOOK("Player_Door_Knob") void replaceDoorAnim_on_Player_Door_Knob(PlayState *play, Player *this, Actor *door) {
    if (this->actor.category == ACTORCAT_PLAYER && this->transformation == PLAYER_FORM_HUMAN) {
        for (int i = 0; i < PLAYER_ANIMTYPE_MAX; i++) {
            D_8085BE84[PLAYER_ANIMGROUP_doorA][i] = D_8085BE84[PLAYER_ANIMGROUP_doorA_free][i];
            D_8085BE84[PLAYER_ANIMGROUP_doorB][i] = D_8085BE84[PLAYER_ANIMGROUP_doorB_free][i];
        }
    }
}

RECOMP_HOOK_RETURN("Player_Door_Knob") void replaceDoorAnim_on_return_Player_Door_Knob() {
    for (int i = 0; i < PLAYER_ANIMTYPE_MAX; i++) {
        D_8085BE84[PLAYER_ANIMGROUP_doorA][i] = &gPlayerAnim_clink_demo_doorA_link;
        D_8085BE84[PLAYER_ANIMGROUP_doorB][i] = &gPlayerAnim_clink_demo_doorB_link;
    }
}


// Remove Child Link's chest-opening voice
// Credit: Neirn

typedef struct AnimSfxEntry {
    /* 0x0 */ u16 sfxId;
    /* 0x2 */ s16 flags; // negative marks the end
} AnimSfxEntry;          // size = 0x4

void dontPlayChildSfx_on_Player_Action_65(Player *this) {
    extern AnimSfxEntry D_8085D73C[];
    D_8085D73C[0].flags = -1;
    D_8085D73C[1].sfxId = NA_SE_NONE;
    D_8085D73C[2].sfxId = NA_SE_NONE;
    D_8085D73C[3].flags = -1;
}

RECOMP_HOOK ("Player_PlayAnimSfx") void on_Player_Action_65(Player* this, PlayState* play) {
    dontPlayChildSfx_on_Player_Action_65(this);
}

// Hook Player_Update: Apply new eyes and mouth flipbooks to Jackie
// Credit: MelonSpeedruns
RECOMP_HOOK("Player_DrawImpl") void on_Player_DrawImpl(PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, s32 lod,
                     PlayerTransformation playerForm, s32 boots, s32 face, OverrideLimbDrawFlex overrideLimbDraw,
                     PostLimbDrawFlex postLimbDraw, Actor* actor) {
    Player* player = GET_PLAYER(play);

    switch (playerForm) {
        case PLAYER_FORM_HUMAN:
            for (int i = 0; i < PLAYER_MOUTH_MAX; i++) {
                sPlayerMouthTextures[i] = sMouthTextures[i];
            }
            for (int i = 0; i < PLAYER_EYES_MAX; i++) {
                sPlayerEyesTextures[i] = sEyeTextures[i];
            }
            break;
        case PLAYER_FORM_ZORA:
            for (int i = 0; i < PLAYER_MOUTH_MAX; i++) {
                sPlayerMouthTextures[i] = sZoraMouthTextures[i];
            }
            for (int i = 0; i < PLAYER_EYES_MAX; i++) {
                sPlayerEyesTextures[i] = sZoraEyesTextures[i];
            }
        case PLAYER_FORM_GORON:
            for (int i = 0; i < PLAYER_EYES_MAX; i++) {
                sPlayerEyesTextures[i] = sGoronEyesTextures[i];
            }
        default:
            break;
    }
}

// Maintain Jackie's properties as Adult Link instead of reverting back to Young Link
RECOMP_CALLBACK("*", recomp_on_play_main)
void mainUpdate(PlayState* play) {
    updateLink(play);
}


/*
=================
Jackie's Height on Epona Fix
=================
*/
PlayState* gPlayState;
RECOMP_HOOK("Player_UpdateCommon") void Player_UpdateCommon(Player* this, PlayState* play, Input* input) {
    gPlayState = play;
}

RECOMP_HOOK_RETURN("Player_UpdateCommon") void Player_UpdateCommonReturn(void) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->stateFlags1 & PLAYER_STATE1_800000) {
        player->actor.shape.yOffset -= CHAR_EPONA_HEIGHT_MODIFIER;
    }
}


/*
=================
EnOsn_Draw fix
=================
*/
RECOMP_HOOK ("EnOsn_Draw") void on_EnOsn_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    Matrix_Push();
    Matrix_Translate(0.f, OSN_MODIFIER, 0.f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    CLOSE_DISPS(play->state.gfxCtx);
}

RECOMP_HOOK_RETURN ("EnOsn_Draw") void return_EnOsn_Draw(Actor* thisx, PlayState* play) {
    Matrix_Pop();
}


/*
=================
Get Item Screen Position Fix
=================
*/
RECOMP_HOOK ("Player_DrawGetItemImpl") void on_Player_DrawGetItemImpl(PlayState* play, Player* player, Vec3f* refPos, s32 drawIdPlusOne) {
    refPos->y += ITEM_HEIGHT_MODIFIER;
    refPos->z += 0.f;
}


/*
=================
Bremen March Fix
=================
*/
u8 gPushedMatrixBremen = 0;
RECOMP_HOOK("Player_Draw") void on_Player_Draw(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;

    if (this-> stateFlags3 & PLAYER_STATE3_20000000) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        gPushedMatrixBremen = 1;
        Matrix_Translate(0.f, CHAR_BREMEN_HEIGHT_MODIFIER, 0.f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

RECOMP_HOOK_RETURN ("Player_Draw") void return_Player_Draw(Actor* thisx, PlayState* play) {
    if (gPushedMatrixBremen == 1) {
        Matrix_Pop();
    }
    gPushedMatrixBremen = 0;
}

/*
=================
Drawn Arrow Position Fix
=================
*/
RECOMP_HOOK ("Player_PostLimbDrawGameplay") void on_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    Player* player = (Player*) actor;
    if (limbIndex == PLAYER_LIMB_LEFT_HAND && player->actor.scale.y >= 0.0f) {
        Actor* heldActor;
        MtxF sp230;
        if (!Player_IsHoldingHookshot(player) && ((heldActor = player->heldActor) != NULL)) {
            if ((player->stateFlags3 & PLAYER_STATE3_40) && (player->transformation != PLAYER_FORM_DEKU)) {
                if (player->transformation == PLAYER_FORM_HUMAN) {
                    Vec3s* temp_s1;
                    Matrix_Translate(MOVEARROWX, MOVEARROWY, MOVEARROWZ, MTXMODE_APPLY);    // Move arrow's drawn position
    
                    Matrix_Get(&sp230);
                    temp_s1 = &heldActor->world.rot;
                    Matrix_MtxFToYXZRot(&sp230, temp_s1, false);
                    heldActor->shape.rot = *temp_s1;
                }
            }
        }
    }
}

/*
=================
Redead/Gibdo Grab Position Fix
=================
*/
// There are three different actor types for Gibdos/Redeads:
// 1) Ikana Castle (redeads only)
// 2) Music Box
// 3) Well

// Ikana Castle Redeads grab fix
typedef enum {
    /* 0 */ EN_RD_GRAB_START,
    /* 1 */ EN_RD_GRAB_INITIAL_DAMAGE,
    /* 2 */ EN_RD_GRAB_ATTACK,
    /* 3 */ EN_RD_GRAB_RELEASE,
    /* 4 */ EN_RD_GRAB_END
} EnRdGrabState;

RECOMP_HOOK ("EnRd_Grab") void on_EnRd_Grab(EnRd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if ((this->grabState == EN_RD_GRAB_START) && (player->transformation == PLAYER_FORM_HUMAN)) {
        this->actor.shape.yOffset = CASTLE_RD_GRABSTART_POS_Y;
    }

    if ((this->grabState == EN_RD_GRAB_ATTACK) && (player->transformation == PLAYER_FORM_HUMAN)) {
        this->actor.shape.yOffset = CASTLE_RD_GRABATTACK_POS_Y;
    }
}

// Music Box Gibdos/Redeads grab fix
// Only difference is that the first if statement block was commented out
RECOMP_PATCH s32 EnRailgibud_MoveToIdealGrabPositionAndRotation(EnRailgibud* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f targetPos;
    f32 distanceFromTargetPos;
    f32 distanceFromTargetYOffset = 0.0f;
    s16 distanceFromTargetAngle;

    targetPos = player->actor.world.pos;
    targetPos.x -= 25.0f * Math_SinS(player->actor.shape.rot.y);
    targetPos.z -= 25.0f * Math_CosS(player->actor.shape.rot.y);
    distanceFromTargetPos = Math_Vec3f_StepTo(&this->actor.world.pos, &targetPos, 10.0f);
    distanceFromTargetAngle = Math_SmoothStepToS(&this->actor.shape.rot.y, player->actor.shape.rot.y, 1, 0x1770, 0x64);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    
    if ((distanceFromTargetPos == 0.0f) && (ABS_ALT(distanceFromTargetAngle) < 100) &&
        (distanceFromTargetYOffset == 0.0f)) {
        return true;
    }

    return false;
}

// Well Gibdos/Redeads grab fix
// Only difference is that the first if statement block was commented out
RECOMP_PATCH s32 EnTalkGibud_MoveToIdealGrabPositionAndRotation(EnTalkGibud* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f targetPos;
    f32 distanceFromTargetPos;
    f32 distanceFromTargetYOffset = 0.0f;
    s16 distanceFromTargetAngle;

    targetPos = player->actor.world.pos;
    targetPos.x -= 25.0f * Math_SinS(player->actor.shape.rot.y);
    targetPos.z -= 25.0f * Math_CosS(player->actor.shape.rot.y);
    distanceFromTargetPos = Math_Vec3f_StepTo(&this->actor.world.pos, &targetPos, 10.0f);
    distanceFromTargetAngle = Math_SmoothStepToS(&this->actor.shape.rot.y, player->actor.shape.rot.y, 1, 0x1770, 0x64);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if ((distanceFromTargetPos == 0.0f) && (ABS_ALT(distanceFromTargetAngle) < 100) &&
        (distanceFromTargetYOffset == 0.0f)) {
        return true;
    }

    return false;
}


