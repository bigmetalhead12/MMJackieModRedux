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

When exporting this for Majora's Mask, be sure to first select the skeleton (not mesh) in your viewer. Afterward, enable "Replace Vanilla Skeletons on Export" and "Use Custom Path". Once these are selected, set your path to where you want to save your exported model-related files. Finally, press the "Export Skeleton". If there are issues with your model, error messages will show up, which you will have to address. If there are no issues, the model will be exported with no issue. For each skeleton you export, you should get a .C file and a .H file.

<img width="751" height="591" alt="image" src="https://github.com/user-attachments/assets/3cc93f55-d5ee-4061-8ab0-b13954b4612f" />

For example, I exported Jackie's model + skeleton with the custom Fast64 and got 
* [gJackieSkel.c](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.c)
* [gJackieSkel.h](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.h)

Both files need to be put into the mod template's "src" directory.

A common error you can make that would prevent you from exporting your model is invalid texture file. Make sure they are in the right size format and also in the F3D format.

Another common error you can make (which Fast64 will NOT catch) is ensuring that all default vertex groups of have something mapped to them. If one or more of your vertex groups do not have anything mapped to it, you can make a plane with an invisible F3D material and map that to a vertex group.

Speaking of vertex groups, make sure that no vertex point in your model is mapped to two or more vertex groups.

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

If you intend to add these flipbooks, be sure to first set your Fast64 Global Setting's game to "OoT", as the flipbook option will not show up in "MM".

<img width="251" height="152" alt="image" src="https://github.com/user-attachments/assets/bffa171c-d69a-4482-86ff-9f728967a00c" />

### Eyes Flipbook
When setting the eyes flipbook, be sure to have the following eye textures with the same image sizes:
* Open (default)
* Half-open
* Closed
* Looking left
* Looking right
* Shocked/Surprised
* Looking down
* Squeezed shut

On your mesh, select the faces of which you want to apply your default eye texture to and then assign it to a material. Afterward, apply the default open eyes image texture to the material.

<img width="581" height="309" alt="image" src="https://github.com/user-attachments/assets/409320fb-4c31-41ff-92ec-83b453f60d0f" />

Once the texture is set, be sure to enable "Use Texture Reference" and fill in `0x08000000`.

<img width="216" height="385" alt="image" src="https://github.com/user-attachments/assets/8920428a-9c49-4b10-8549-816b330d8533" />

This opens up the Flipbook Properties for the eye texture, which you can reach by scrolling down in the same tab. The flipbook property has two modes: "Array" and "Individual". This is where you have to upload the various eye textures and name them. For both modes, I did the following:

<img width="514" height="503" alt="image" src="https://github.com/user-attachments/assets/34f2b516-3212-417a-b4fc-75523e3db125" />

Keep note of the Texture Names for "Individual".

### Mouth Flipbook
When setting the mouth flipbook, be sure to have the following mouth textures with the same image sizes:
* Closed (Default)
* Half
* Open/Surprised
* Laugh/Smile

On your mesh, select the faces of which you want to apply your default closed mouth texture to and then assign it to a material. Afterward, apply the default closed mouth image texture to the material.

<img width="563" height="256" alt="image" src="https://github.com/user-attachments/assets/b0f8028e-cbbd-48ff-9e89-a40bda31792b" />

With the texture set, enable "Use Texture Reference" and fill in `0x09000000`.

<img width="263" height="401" alt="image" src="https://github.com/user-attachments/assets/01e462a0-f628-4d6c-8d82-05035caa3346" />

The mouth's Flipbook Properties should now be enabled, which you can reach by scrolling down the same Material tab. Just like for the eyes flipbook, upload your mouth texture files and assign values for both "Array" and "Individual":

<img width="521" height="287" alt="image" src="https://github.com/user-attachments/assets/2f442d63-c096-4c48-bb4c-547a47dc1a4c" />

Once this is finished, go back to Fast64 Global Settings and change the game from "OoT" back to "MM".

# Implementing the Model
To replace a target model (human Link in this mod's case) with the exported model, a separate .C file needs to be set up. In this mod's case, [jackie_code.c](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c) was set up.

To properly implement the exported custom model, this file needs to do the following:
1. Setting Link's properties to match your model's intended property
2. Setting up Display Lists
3. Adjusting miscellaneous in-game characteristics (For Adult Link Model Modding)

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

// ...

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
With the eyes and mouth flipbooks set up, you can use them in your model in the game.

The names of the usable flipbook textures should appear based on how you named each uploaded eye and mouth texture in the flipbook. For instance, Jackie's eyes and mouth textures are named like this in [`gJackieSkel.h`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.h#L37-L48).

```c
extern u64 gJackieMouthClosed[];
extern u64 gJackieMouthHalf[];
extern u64 gJackieMouthOpen[];
extern u64 gJackieMouthSmile[];
extern u64 gJackieEyesOpenTex[];
extern u64 gJackieEyesHalfTex[];
extern u64 gJackieEyesClosedTex[];
extern u64 gJackieEyesRollLeftTex[];
extern u64 gJackieEyesRollRightTex[];
extern u64 gJackieEyesShockTex[];
extern u64 gJackieEyesRollDownTex[];
extern u64 gJackieEyesShutTex[];
```

These textures are to be used in the main code to set up the eyes and mouth flipbooks.

In your main code, you should set up your eyes flipbook like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L65-L100) (ignore the ponytail part).

```c
extern TexturePtr sPlayerEyesTextures[PLAYER_EYES_MAX];     // Eyes

// ...

// Eyes Texture Variables
// Default Head
// Note the same names from gJackieSkel.h
extern u64 gJackieEyesOpenTex[];
extern u64 gJackieEyesHalfTex[];
extern u64 gJackieEyesClosedTex[];
extern u64 gJackieEyesRollLeftTex[];
extern u64 gJackieEyesRollRightTex[];
extern u64 gJackieEyesShockTex[];
extern u64 gJackieEyesRollDownTex[];    // gLinkAdultEyesUnk1Tex 
extern u64 gJackieEyesShutTex[];        // gLinkAdultEyesUnk2Tex

// ...

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
```

`sPlayerEyesTextures` is Link's eyes flipbook, which will be replaced with `sEyeTextures`, which is currently Jackie's eye flipbook.

The same thing must be done for the mouth flipbook like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L114-L134).

```c
extern TexturePtr sPlayerMouthTextures[PLAYER_MOUTH_MAX];   // Mouth

// ...

// Mouth Texture Variables
// Default
extern u64 gJackieMouthClosed[];    // gLinkAdultMouth1Tex
extern u64 gJackieMouthHalf[];      // gLinkAdultMouth2Tex
extern u64 gJackieMouthOpen[];      // gLinkAdultMouth3Tex
extern u64 gJackieMouthSmile[];     // gLinkAdultMouth4Tex

// ...

// Mouth Flipbook for Jackie
// Default
void* sMouthTextures[PLAYER_MOUTH_MAX] = {
    gJackieMouthClosed,
    gJackieMouthHalf,
    gJackieMouthOpen,
    gJackieMouthSmile,
};
```

`sPlayerMouthTextures` is Link's mouth flipbook, which will be replaced with `sMouthTextures`, which is currently Jackie's mouth flipbook.

To avoid an error, the eyes and mouth flipbooks need to be reapplied in this code like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L144-L198).

After this is complete, you can fully replace `sPlayerEyesTextures` and `sPlayerMouthTextures` like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L487-L517).

```c
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
```

With this set up, the eyes and mouth textures should now be fully replaced.

## Misc. In-Game Characteristics (Adult Link Model Modding)
Note: This section is primarily concerned with modding human Link with a model meant to replace Adult Link from OoT (i.e. my Jackie model). If your mod is clearly for Young Link, then this part can be disregarded.

Although all the model parts are replaced by this point, there are still other quirks that need to be addressed for a more complete experience. 

### Strength
If you're replacing Link with a model that's meant to be for Adult Link, then chances are your character should be as strong as Zora Link, at least. This strength attribute can be modified like I did in [this block of code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L354-L363):

```c
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
```

### Door-Opening Animation
If you don't touch Link's door animation code, then your character will always open a door like Young Link, which is probably what you're not going for if you have a model meant for Adult Link. In this case, you need to follow this series of steps:

1. Redeclare the animation array for door opening animations like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L260-L265).

```c
// Used to replace animation to adult link animation
extern PlayerAnimationHeader* D_8085BE84[PLAYER_ANIMGROUP_MAX][PLAYER_ANIMTYPE_MAX];

// Adult Link door animation
extern LinkAnimationHeader gPlayerAnim_clink_demo_doorA_link;
extern LinkAnimationHeader gPlayerAnim_clink_demo_doorB_link;
```

2. Replace the door animations like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L449-L464).

```c
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
```

### Treasure Chest Voice
When opening the treasure chest, even if you set the properties to not be like Child Link earlier, the chest-opening animation will still play Child Link's voices. Although strange considering that the Adult Link treasure chest animation plays, the voices will have to be manually disabled. Disabling the voices can be done like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L467-L481):

```c
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
```
Credit to Neirn for finding how to disable the voice audio in this animation

### Height while riding Epona
Unfortunately, there is no mod that replaces Epona with a full-grown Epona model. Until then, we are stuck with young Epona.

The issue with young Epona with an Adult Link-sized model is that your model will be floating above Epona. To fix this height issue, the following [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L526-L541) can be run:

```c
// At the beginning of the file
#define CHAR_EPONA_HEIGHT_MODIFIER 1100.f

// ...


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
```

### Getting Item Position Fix
When getting an item for the first time or from a chest, Link sometimes holds it above his head. The position of the floating item becomes misaligned when an Adult Link-sized model performs this animation. To adjust the item's position, the following [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L562-L570) can be used:

```c
// At the beginning of the file
#define ITEM_HEIGHT_MODIFIER 2.5f


// ...


/*
=================
Get Item Screen Position Fix
=================
*/
RECOMP_HOOK ("Player_DrawGetItemImpl") void on_Player_DrawGetItemImpl(PlayState* play, Player* player, Vec3f* refPos, s32 drawIdPlusOne) {
    refPos->y += ITEM_HEIGHT_MODIFIER;
    refPos->z += 0.f;
}
```

### Bremen March Animation Fix
When Adult Link performs a Bremen March, he sinks into the ground to match Young Link's position on the ground. To prevent this sinking, the following [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L573-L590) can be used:

```c
// At the beginning of the file
#define CHAR_BREMEN_HEIGHT_MODIFIER 1250.0f


// ...


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
```

### Redead/Gibdo Grab Position Fix
When Adult Link gets grabbed by a Redead or Gibdo, the body position does not get adjusted. To fix the grab position, the following [code[](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L625-L701) can be used

```c
//At the beginning of the file
#define CASTLE_RD_GRABSTART_POS_Y 30.f        // Position values written in float
#define CASTLE_RD_GRABATTACK_POS_Y 0.f


// ...


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
```

If you notice, there are three types of Redeads and Gibdos, which are all separately changed. This is because these three types are all written separately in the main source code.

*NOTE*: An issue I would like to see fixed with this set of functions is that they use `RECOMP_PATCH`. If you are unaware, `RECOMP_PATCH` directly replaces the target function with the following function block, which can be dangerous when applying other mods that may rely on the target functions. It is ideal if this set of functions are replaced with code written with `RECOMP_HOOK` or `RECOMP_HOOK_RETURN`.

### Drawn Arrow Position Fix
The messiest and most tedious part of adjust miscellaneous characteristics to match Adult Link's properties is fixing the position of the arrow's position in the player's first person view. When aiming with the bow in first person while using Adult Link properties, the position of the bow and arrow becomes very misaligned. This is an issue because the first person view becomes ugly to look at, and it also makes the arrows very difficult to aim with.

This part requires you to adjust the position of the arm models inside Blender to make them look right in the game. Additionally, the following [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L599-L623) manually fixes the issue:

```c
// At the beginning of the file
#define MOVEARROWX -0x0070        // Position values written in Hex
#define MOVEARROWY 0x0120
#define MOVEARROWZ 0x0000


// ...


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
```

Please keep in mind, however, that you may need to adjust your macro values `MOVEARROWX`, `MOVEARROWY`, and `MOVEARROWZ` based on how your first person arms and bow look in the game while you are aiming with the bow in first person. There is a ton of trial and error involved in this step, so be warned.

If there is any other cleaner method of fixing the position of the drawn arrow and arms in first person, let me know.

## Bonus: Elegy Statue
If you wish to replace the elegy statue with your own, you will need to have the following tools:

* Blender 4.0+ (3.X or lower will most likely not work)
* Fast64 MM Branch (same as above)
* Decompiled Majora's Mask

This document will assume you have immediate access to all three.

Open up Blender and have Fast 64 MM enabled. With the Fast64 Global Setting's game set to "MM", set the decomp path to your directory containing a decompiled Majora's Mask.

<img width="204" height="239" alt="image" src="https://github.com/user-attachments/assets/473dceb8-eece-4d90-aa2e-ef11d74acafa" />

Afterward, you're gonna have to import the Elegy statue DL from your decompiled Majora's Mask. Based on the elegy statue you want to change, you will have to select your DL from this list of DLs:
* gElegyShellHumanDL (for both Human and Fierce Deity Links)
* gElegyShellGoronDL
* gElegyShellZoraDL
* gElegyShellDekuDL

Scroll down in your OoT/MM tab menu down to where it says "Import DL". For DL, fill it in with your DL of choice. For Object, fill in "gameplay_keep".

<img width="259" height="167" alt="image" src="https://github.com/user-attachments/assets/35f93415-81fa-4081-9af6-bae224c88787" />

Press "Import DL" and you should get your elegy statue of choice. After this, edit the statue however you want. Be sure to convert the textures to F3D format. Use the imported statue's textures' settings as reference if you need to.

Once your statue is prepared, fill in the "Filename" with how you want to name your statue and then export it. This should give you the `.h` and `.c` files. In my case, I named it `gJackieElegy` and got `gJackieElegy.h` and `gJackieElegy.c`. Drag the files into your mod directory's `src` directory.
 
<img width="613" height="524" alt="image" src="https://github.com/user-attachments/assets/4689bc53-2706-4581-ab22-f4d51b953a65" />

Once this is complete, create a new `.c` file where you will be replacing your targeted elegy statue with your own. In my case, I set up [`Jackie_elegy.c`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_elegy.c). Be sure to include the right header files also.

```c
// Default Header Files
#include "modding.h"
#include "global.h"
#include "ultra64.h"

// Model Header Files
#include "gJackieElegy.h"    // My model's generated .h file from Fast64

// Elegy header file
#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"    // Include this header file. It's the struct for the elegy statue

// Elegy Statue forms (added for clarity's sake)
#define HUMAN_FORM  0
#define GORON_FORM  1
#define ZORA_FORM   2
#define DEKU_FORM   3
#define FD_FORM     4        // Same as human form
```

The function that actually spawns the elegy statue is `EnTorch2_Draw()`. I didn't want to `RECOMP_PATCH` the function, however. So, I took an alternative approach with `RECOMP_HOOK` and `RECOMP_HOOK_RETURN`.

I `RECOMP_HOOK`ed `EnTorch2_Draw()` to take the current frame's alpha value of the statue (since the statue spawns by slowly becoming solid). While doing this, I also apply a `0` alpha value to the original elegy statue. This part of the [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_elegy.c#L38-L56) is where this is executed:

```c
static EnTorch2* gEnTorch2 = NULL;         // To store current elegy statue
static PlayState* gEnTorch2Play = NULL;    // To store current playstate
static u8 actualAlpha = 0;                 // To store actual alpha value of statue in current frame

// Make the original Human Elegy Statue invisible
// Note: human statue is used in-game for human link and FD link
RECOMP_HOOK("EnTorch2_Draw") void before_EnTorch2_Draw(Actor* thisx, PlayState* play) {
    // Save current statue actor and playstate
    gEnTorch2 = (EnTorch2*)thisx;
    gEnTorch2Play = play;

    EnTorch2* this = (EnTorch2*)thisx;

    // Check if statue is from human/Fierce Deity
    if (this->actor.params == HUMAN_FORM || this->actor.params == FD_FORM) {
        actualAlpha = this->alpha;      // Save the current actual alpha value for Human Elegy Statue
        this->alpha = 0;                // Make Human Elegy Statue completely invisible
    }
}
```

The new alpha value of `0` is then used to draw the original elegy statue (if the player is currently human or FD Link).

Afterward, I `RECOMP_HOOK_RETURN`ed `EnTorch2_Draw()` to point to the new statue and draw it in with `Gfx_Draw...` functions while applying the current frame's actual alpha value. This part of the [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_elegy.c#L60-L95) is where this is done.

```c
// Apply the current actual alpha value of Human Elegy Statue to Jackie's Elegy Statue
RECOMP_HOOK_RETURN("EnTorch2_Draw") void after_EnTorch2_Draw(void) {
    // Check if all pointers are not NULL
    if (gEnTorch2 == NULL || gEnTorch2Play == NULL) {
        return;
    }

    // Point to current statue and playstate
    EnTorch2* this = gEnTorch2;         // statue
    PlayState* play = gEnTorch2Play;    // playstate

    // Check if statue is from human/Fierce Deity
    if (this->actor.params == HUMAN_FORM || this->actor.params == FD_FORM) {
        // Re-apply current actual alpha value to current statue
        this->alpha = actualAlpha;      

        // Point to Jackie statue
        Gfx* gfx = gElegyShellHumanDL_mesh;

        // Draw statue
        OPEN_DISPS(play->state.gfxCtx);     // Open current playstate's graphics context

        if (this->alpha == 255) {           // When statue is fully visible (alpha is max value 255)...
            Scene_SetRenderModeXlu(play, 0, 0x01);
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
            Gfx_DrawDListOpa(play, gfx);    // Draw statue
        } 
        else {                              // When statue is still fading in (alpha is rising to 255)...
            Scene_SetRenderModeXlu(play, 1, 0x02);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->alpha);
            Gfx_DrawDListXlu(play, gfx);    // Draw statue
        }

        CLOSE_DISPS(play->state.gfxCtx);    // Close game context
    }
}
```

So technically, the original statue is actually in the game. However, it is invisible while the new statue also gets draw at that position.
