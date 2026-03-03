# Model Modding
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
* Left hand holding the bottle (bottle should not be included in this model)
* First person view arms and hands holding bow
* First person view arm and hand holding hookshot

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
#include "gJackieSkel.h"
```

## Link's Properties
If your model is to be the equivalent of Young Link, then Link's properties most likely do not need to be changed. If your model is to be the equivalent of Adult Link, however, then Link's properties will have to be changed to match your model's behavior.

To do this, `PlayerAgeProperties` need to be adjusted to match Adult Link's properties. In my [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c#L316-L376), this is how I set up Jackie's properties

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
```

I used the properties of both Fierce Deity Link and Zora Link to match Jackie's properties, which are intended to match Adult Link's properties.


## Setting Up Display Lists (DLs)
DLs make up different parts of Link's model. For example, Link's hand carrying a Kokiri sword is a separate DL from Link's hand carrying a Razor Sword, or a Hero's Shield, and so on. Setting up DLs is tedious but easy as long as you made the model parts for them.



