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
There is a highly informative video that explains step-by-step on how to take a model and export it in a format that Zelda64 likes. Although this video was made for Ship of Harkinian, the steps for setting up your model and exporting it should be *almost* exactly the same.

The only difference is that you will have to use the [Custom MM-branch Fast 64](https://github.com/Yanis002/fast64/tree/mm_dev) instead of the one used in the video.

[![Modeling](https://img.youtube.com/vi/6Ji_12w5B1M/0.jpg)](https://www.youtube.com/watch?v=6Ji_12w5B1M)

For each skeleton you export, you should get a .C file and a .H file. 

For example, I exported Jackie's model + skeleton with the custom Fast64 and got 
* [gJackieSkel.c](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.c)
* [gJackieSkel.h](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.h)

Both files need to be put into the mod template's "src" directory.

# Implementing the Model
To replace a target model (human Link in this mod's case) with the exported model, a separate .C file needs to be set up. In this mod's case, [jackie_code.c](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c) was set up.

To properly implement the exported custom model, this file needs to do the following:
1. Setting Link's properties to match your model's intended property
2. Setting up Display Lists
3. Adjusting miscellaneous in-game characteristics

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

I used the properties of both Fierce Deity Link and Zora Link to match Jackie's properties, which is intended to match Adult Link's properties.





