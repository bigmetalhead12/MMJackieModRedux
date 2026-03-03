# Model Modding
## Preface
This mod would not have been possible without [Reonu](https://github.com/Reonu)'s [CJ player model mod](https://thunderstore.io/c/zelda-64-recompiled/p/Reonu/Play_as_CJ/). His work served as a strong starting point for my Jackie mod code. While many parts of model modding code have been improved along the way, this modding category would not have started off so quickly without Reonu's early contribution to the MM Recomp community.

As a sidenote, if you wish to take a much more streamlined and simplified method of replacing Link's model with the same quality, I strongly recommend using [Neirn](https://github.com/YAZ64MT)'s [Player Model Manager](https://thunderstore.io/c/zelda-64-recompiled/p/YAZMT/Player_Model_Manager/) (PMM).

My mod takes a more manual approach to replacing Link's model with my model, similar to the approach Reonu used to replacing Link's model. While PMM makes modding Link's model easier than ever, this method is still useful as a bit of gentle introduction to modding in Majora's Mask, as it gives you a view into directly modifying Majora's Mask's functions.

## Introduction
This doc covers two parts of model modding Majora's Mask Recomp. This mod specifically replaced Link's human form model with my custom model. Notably, my custom model used OoT Adult Link's skeleton, so there were additional steps I had to take for this mod.

The two parts of model modding are:
1. Exporting your model
2. Implementing the model into the game through code

# Prerequisite
Before modding MM models, you only need:
1. [Custom MM-branch Fast64](https://github.com/Yanis002/fast64/tree/mm_dev)
2. Blender 3.2+

# Exporting Model
## Basics
There is a highly informative video that explains step-by-step on how to take a model and export it in a format that Zelda64 likes. Although this video was made for Ship of Harkinian, the steps for setting up your model and exporting it should be *almost* exactly the same.

The only difference is that you will have to use the [Custom MM-branch Fast 64](https://github.com/Yanis002/fast64/tree/mm_dev) instead of the one used in the video.

[![Modeling](https://img.youtube.com/vi/6Ji_12w5B1M/0.jpg)](https://www.youtube.com/watch?v=6Ji_12w5B1M)

For each skeleton you export, you should get a .C file and a .H file. 

For example, I exported Jackie's model + skeleton with the custom Fast64 and got 
* [gJackieSkel.c](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.c)
* [gJackieSkel.h](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.h)

Both files need to be put into the mod template's "src" directory.

## Hands
As shown in the video, hands are handled significantly different from other parts of the body. You will have to have separate hand models for the following:
* Default open hands (should be part of your base model)
* Closed hands (fists)
* Left hand holding the bottle (bottle should NOT be included in this model)
* Right hand holding the ocarina of time (ocarina SHOULD be included in this model)
* First person view of right arm holding bow (bow included)
* First person view of left arm pulling bowstring (bow not included)
* First person view arm and hand holding hookshot (hookshot included)

I had each mesh have its own skeleton, but it may be fine to just have these meshes be under a single skeleton. I am unsure about this, however, so if you want to be on the safe side and don't want to experiment, just have each mesh have its own skeleton.

Export each skeleton as you did for the base model using Fast64. Take the resulting sets of `.c` and `.h` files and save them into your mod template directory's `src` directory.

## Eyes and Mouth
Eyes and mouth are handled differently through a feature called "flipbooks". Flipbooks are, as the name implies, a book of textures for a part of the mesh where textures are meant to change in-game based on its events. For example, for the eyes flipbook, the default open eye expression would be set. When the character looks to the right, however, the eye flipbook would "flip" the texture to the eyes texture that is looking to the right.

Both flipbooks are set in the same way, but with different values for specific attributes.

### Eyes Flipbook
(here)

### Mouth Flipbook
(here)

# Implementing the Model
To replace a target model (human Link in this mod's case) with the exported model, a separate .C file needs to be set up. In this mod's case, [jackie_code.c](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c) was set up.

To properly implement the exported custom model, this file needs to do the following:
1. Setting Link's properties to match your model's intended property
2. Setting up Display Lists
3. Adjusting miscellaneous in-game characteristics

Before proceeding with this, it is important to include the right header files, like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L28-L34). By default, `modding.h`, `global.h`, and `ultra64.h` should be set. Afterward, the `.h` file of the exported model files should also be included. In my case, this was `gJackieSkel.h`.


```c
// Default Header Files
#include "modding.h"
#include "global.h"
#include "ultra64.h"

// Model Header Files
#include "gJackieSkel.h"                     // Header file for the base model
#include "gJackieSkel_hands_closed.h"        // Header file for model of closed hands
#include "gJackieSkel_hands_bottle.h"        // Header file for model of hand holding bottle (bottle model not included)
#include "gJackieSkel_hands_ocarina.h"       // Header file for model of hand holding ocarina of time (ocarina included)
#include "gJackieSkel_fps_bow.h"             // First person view of right arm holding bow (bow included)
#include "gJackieSkel_fps_hookshot.h"        // First person view of right arm holding hookshot (hookshot included)
#include "gJackieSkel_fps_leftarmhand.h"     // First person view of left arm (bow not included)
```

## Link's Properties
If your model is to be the equivalent of Young Link, then Link's properties most likely do not need to be changed. If your model is to be the equivalent of Adult Link, however, then Link's properties will have to be changed to match your model's behavior.

To do this, `PlayerAgeProperties` need to be adjusted to match Adult Link's properties. In my [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L316-L376), this is how I set up Jackie's properties.

```c
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
// The RECOMP_HOOK ("Player_GetHeight") function means that this function will run BEFORE Player_GetHeight() runs in the game
Player *gOriginalPlayer;
u8 gOriginalPlayerTransformation;
RECOMP_HOOK ("Player_GetHeight") void on_Player_GetHeight(Player* player) {
    gOriginalPlayer = player;                                    // Get player struct object that will be in Player_GetHeight()
    gOriginalPlayerTransformation = player->transformation;      // Get the player's transformation status
    player->transformation = PLAYER_FORM_ZORA;                   // Set player's transformation state to Zora
}

// The RECOMP_HOOK_RETURN( "Player_GetHeight") function means that this function will run AFTER Player_GetHeight() runs in th egame
RECOMP_HOOK_RETURN ("Player_GetHeight") void return_Player_GetHeight(void) {
    gOriginalPlayer->transformation = gOriginalPlayerTransformation;    // Re-apply the original transformation before Player_GetHeight() was run to the player struct object
}
```

The properties that are set up are then actually patched in [this code block](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L519-L523).

```c
// Maintain Jackie's properties as Adult Link instead of reverting back to Young Link
// This is where Jackie's properties that were set in void updateLink() are actually applied
RECOMP_CALLBACK("*", recomp_on_play_main)
void mainUpdate(PlayState* play) {
    updateLink(play);
}
```

I used the properties of both Fierce Deity Link and Zora Link to match Jackie's properties, which are intended to match Adult Link's properties.

## Setting Up Display Lists (DLs)
DLs make up different parts of Link's model. For example, Link's hand carrying a Kokiri sword is a separate DL from Link's hand carrying a Razor Sword, or a Hero's Shield, and so on. Setting up DLs is tedious but easy as long as you made the model parts for them.

To replace all of Link's DLs with my model parts, you need to...
1. Take out the original DLs from the main source code that you want to replace
2. use the model parts from your generated files to create new DLs
3. replace the original DLs with the new DLs

### Original DLs
To set up the original DLs you want to replace, you first need to redeclare the DLs using `extern`. My code completed this task like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L201-L258).

```c
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
```

These are all the DLs that are to be used for modding Link's model.  With the DLs set up, you can move on to creating new DLs to replace the original DLs by using your own model parts.

### New DLs
In this part, you should create new DLs to replace some of the original DLs. This part is where the extra model parts (i.e. closed hands, first person limbs) become useful.

When proceeding, you need to be careful with understanding how specific body parts in the model files are named. But essentially, you will have to dive into your `.h` files and use their objects to create your DLs.

For example, I need to create a DL of Jackie's right hand holding a Mirror Shield. To do this, I would need two parts:
* Mirror Shield 
* Jackie's right closed hand

For Mirror Shield, I can simply use the Mirror Shield DL, which is `gLinkHumanMirrorShieldDL`.

For Jackie's right closed hand, I would have to look into the header file that would contain this. For me, the `gJackieSkel_hands_closed.h` would contain the model of Jackie's right closed hand. When looking into the header file, I would have to look for a `Gfx` object of Jackie's right hand. The name format generally follows: `skeleton name` + `vertex group name` + `mesh_layer_Opaque[]`. In my case, [this line of code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel_hands_closed.h#L26) indicates Jackie's right closed hand: `gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque[]`.

So, using the Mirror Shield DL and the `Gfx` object of Jackie's right closed hand, I would make my DL of Jackie's right hand holding a Mirror Shield like this:

```c
Gfx gJackieRightHandHoldingMirrorShieldDL[] = {
    gsSPDisplayList(gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque),         // Right hand
    gsSPBranchList(gLinkHumanMirrorShieldDL),                                                            // Mirror Shield
};
```

This step would have to be repeated for every DL that is to be used for replacing Link's original DLs. In my `Jackie_code.c`, [this code block](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L267-L313) shows all the DLs I made:

```c
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
```

### Replacing Original DLs with New DLs
Once these DLs are set up, all of Link's original DLs can now be replaced. Some of Link's DLs (like closed hands DLs) would be replaced with the `Gfx` objects from the model `.h` files directly while other DLs (like hand carrying Mirror Shield) would be replaced with the newly created DLs.

The replacements can be done by hooking into main code's function called `Player_Init`, which initializes the player (Link) in the game. To hook into the function, `RECOMP_HOOK` needs to be used. `RECOMP_HOOK` essentially means that the subsequent function block will be exeucted before the target function is to be run in the game. For example, `RECOMP_HOOK("Player_Init") void on_Player_Init(Actor* thisx, PlayState* play)` means that before `Player_Init()` is run in the main game, the function block of `void on_Player_Init` will run first.

Using the mechanism, you can replace all of Link's DLs with your own DLs. This is demonstrated in [my code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L381-L447):

```c
// Function to replace Link's model with Jackie's model
RECOMP_HOOK("Player_Init") void on_Player_Init(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);

    gPlayerSkeletons[PLAYER_FORM_HUMAN] = &gJackieSkel;

    // ...
    
    // Right Hand DLs
    gPlayerRightHandOpenDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;    // Use Gfx object of right hand from base model
    gPlayerRightHandOpenDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    
    gPlayerRightHandClosedDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;    // Use Gfx object of right closed hand from "gJackieSkel_hands_closed.h"
    gPlayerRightHandClosedDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_hands_closed_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    gPlayerRightHandInstrumentDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieSkel_hands_ocarina_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    gPlayerRightHandInstrumentDLs[PLAYER_FORM_HUMAN * 2 + 1] = gJackieSkel_hands_ocarina_bone018_gLinkAdultRightHandLimb_mesh_layer_Opaque;
    
    gPlayerRightHandBowDLs[PLAYER_FORM_HUMAN * 2 + 0] = gJackieRightHandHoldingBowDL;        // Use new DL created earlier to replace an original DL
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
```

This function replaces all of Link's DLs. 

### Flipbooks for Eyes and Mouth
(explain flipbooks here)

## Misc. In-Game Characteristics
Although all the model parts are replaced by this point, there are still other quirks that need to be addressed for a more complete experience.

