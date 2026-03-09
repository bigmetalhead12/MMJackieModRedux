# Text Replacement
## Preface
This mod was possible thanks to [LT_Schmiddy](https://github.com/LT-Schmiddy)'s [EZ Text Replacer mod](https://thunderstore.io/c/zelda-64-recompiled/p/LT_Schmiddy/EZ_Text_Replacer_API/). The tool not only has many functionalities but is also very easy to use (as its name implies). 

If you are interested in using this tool for your mod, you can click the link above to go to the download page. Be sure to also check out the [official documentation](https://lt-schmiddy.github.io/docs/EZTR_for_Zelda64Recomp/index.html) of the tool.

## Introduction
This doc explains how text was modded in for Jackie. 

There are two types of text replacements in this mod. First, this mod changed the behaviors of a few NPCs that treated Human Link like a child. Second, this mod also used EZTR to modify some NPCs' texts.

# Prerequisite
If EZTR is to be used, two files are needed:
1. The NRM file from the [official download](https://thunderstore.io/c/zelda-64-recompiled/p/LT_Schmiddy/EZ_Text_Replacer_API/)
- Install the NRM file by starting your Majora's Mask Recomp, opening the Mod directory, and clicking-and-dragging the NRM file into the game screen
2. A copy of [eztr_api.h](https://github.com/LT-Schmiddy/mm-ez-text-replacement-utils/blob/main/include_in_dependents/eztr_api.h)
- Download the file from this page, and then place it into your mod template's "include' directory [like this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/include/eztr_api.h)

In my `mod.toml`, I also had to add EZTR to my list of dependencies. This feature allows this mod to actually use EZTR that is placed in Majora's Mask Recomp to replace texts. However, it also makes installing NRM a requirement for anyone who wants to use this Jackie mod.

I added EZTR to my list of dependencies like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/mod.toml#L37) in my code.

```
dependencies = [
	"MM_EZ_Text_Replacer_API"
]
```

# Optional Flag: Making the Text Replacement Toggleable
I set up this mod so that text replacement can be toggleable when configuring this Jackie mod's options. 

If you want to do this, you can add this [type of code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/mod.toml#L189-L197) in your `mod.toml`:
```
[[manifest.config_options]]
id = "alternative_text"
name = "Change Text"
description = """
Toggle to change the text (Default or Alternative)
"""
type = "Enum"
options = [ "Default", "Alternative" ]
default = "Default"
```

Keep note of what values were used for `id` and `options`. For `options`, I set it up so that `Default` is the first option in  `options`, which means it is `0`. Meanwhile, `Alternative` is the second option, which means it is `1`.

# Changing NPC Behavior
For note, I am modding Human Link, who is a child, with Jackie, who has the properties of Adult Link. This would be a problem because some NPCs would be treating Jackie like a child.

I found 2 types of NPCs to have this trait: The townguards and Professor Shikashi (observatory man).

## Townguards
For the townguards, I wanted them to automatically allow Jackie through the gates like they would for Zora and Goron Links. So, I implemented the following [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_text.c#L26-L31)

```c
// Disable guard blocking Jackie
RECOMP_HOOK("func_80AE7F34") void on_func_80AE7F34(EnStopheishi* this, PlayState* play) {
    if (recomp_get_config_u32("alternative_text")) {
        SET_WEEKEVENTREG(WEEKEVENTREG_12_20);   // Sets event to already having talked to guard as human
    }
}
```

`WEEKEVENTREG_12_20` is an event that indicates that the townguard already spoke to Human Link and won't be guarding the gate (unless Deku Link is out). `func_80AE7F34` is a function in the townguard file (`z_en_stop_heishi.c`) that handles if the townguard should be stopping Link or not. So, by hooking onto this function and always setting the `WEEKEVENTREG_12_20` event, the townguard won't be blocking Human Link (or Jackie in this case).

`if (recomp_get_config_u32("alternative_text"))` means "if `alternative_text` is not 0...". More specifically, this is saying that if `alternative_text` in `mod.toml` is set to any option but `Default` (which is `0`), then execute the following line(s) of code. This is used throughout every function where this text change is happening so that the toggle actually enables or disables all text changes based on what the player selected.

## Prof. Shikashi (Astral Observatory Man)
Prof. Shikashi has a different dialogue for Zora and Goron Links, and I want him to speak to Jackie as he would with those two. So, I implemented this folowing [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_text.c#L33-L76)

```c
// Prof. Shikashi
// Change text dialogue to target Zora Link
extern SaveContext gSaveContext;
u8 in_human_form = 0;

RECOMP_HOOK("func_809CCEE8") void on_func_809CCEE8(EnBji01* this, PlayState* play){
    u8 playerform = gSaveContext.save.playerForm;
    if (recomp_get_config_u32("alternative_text")) {
        if (playerform == PLAYER_FORM_HUMAN) {
            gSaveContext.save.playerForm = PLAYER_FORM_ZORA;
            in_human_form = 1;
        }
    }
}

RECOMP_HOOK_RETURN("func_809CCEE8") void return_func_809CCEE8(EnBji01* this, PlayState* play){
    u8 playerform = gSaveContext.save.playerForm;
    if (recomp_get_config_u32("alternative_text")) {
        if (in_human_form == 1) {
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
            in_human_form = 0;
        }
    }
}

RECOMP_HOOK("EnBji01_DialogueHandler") void on_EnBji01_DialogueHandler(EnBji01* this, PlayState* play){
    u8 playerform = gSaveContext.save.playerForm;
    if (recomp_get_config_u32("alternative_text")) {
        if (playerform == PLAYER_FORM_HUMAN) {
            gSaveContext.save.playerForm = PLAYER_FORM_ZORA;
            in_human_form = 1;
        }
    }
}

RECOMP_HOOK_RETURN("EnBji01_DialogueHandler") void return_EnBji01_DialogueHandler(EnBji01* this, PlayState* play){
    u8 playerform = gSaveContext.save.playerForm;
    if (recomp_get_config_u32("alternative_text")) {
        if (in_human_form == 1) {
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
            in_human_form = 0;
        }
    }
}

```

