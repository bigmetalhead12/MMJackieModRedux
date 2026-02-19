//-----------------------------------------------------------------------------
//
// $ Author: Bigmetalhead12 $
//
// 2026
//
// Jackie Redux Custom Text Mod (using LT_Schmiddy's EZ Text Replacer API)
// API Source: https://github.com/LT-Schmiddy/mm-ez-text-replacement-utils
//-----------------------------------------------------------------------------

// Header
#include "eztr_api.h"

// Guard
#include "overlays/actors/ovl_En_Stop_heishi/z_en_stop_heishi.h"

// Prof Shikashi
#include "overlays/actors/ovl_En_Bji_01/z_en_bji_01.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

// Disable guard blocking Jackie
RECOMP_HOOK("func_80AE7F34") void on_func_80AE7F34(EnStopheishi* this, PlayState* play) {
    if (recomp_get_config_u32("alternative_text")) {
        SET_WEEKEVENTREG(WEEKEVENTREG_12_20);   // Sets event to already having talked to guard as human
    }
}


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

EZTR_ON_INIT void replace_msgs() {
    if (recomp_get_config_u32("alternative_text")) {
        // Jim
        EZTR_Basic_ReplaceText(
            0x0719,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|02What do you want, weirdo?" EZTR_CC_NEWLINE "I'm busy practicing with my" EZTR_CC_NEWLINE "blowgun! Don't mess with me!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0753,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|03Are you the lady who just popped" EZTR_CC_NEWLINE "that up there?" EZTR_CC_NEWLINE "Whoa!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0754,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Hey, just for you, miss, I can make" EZTR_CC_NEWLINE "you a member of our gang, the" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_RED "Bombers" EZTR_CC_COLOR_DEFAULT "!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0758,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|00What's the deal, lady?" EZTR_CC_NEWLINE "You dress like you think you're all" EZTR_CC_NEWLINE "cool, but you're nothing special!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0710,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|03That was crazy! You're way too" EZTR_CC_NEWLINE "good at this for a grown up." EZTR_CC_NEWLINE "So what's your name, miss?" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "" EZTR_CC_DELAY "|00|0A" EZTR_CC_COLOR_RED "" EZTR_CC_NAME "" EZTR_CC_COLOR_DEFAULT ", huh?" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "All right! That Skull Kid broke our" EZTR_CC_NEWLINE "rules and he's been doing all" EZTR_CC_NEWLINE "kinds of bad things lately." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 "Starting today, he's out! I'm" EZTR_CC_NEWLINE "making " EZTR_CC_COLOR_RED "" EZTR_CC_NAME "" EZTR_CC_COLOR_DEFAULT " a " EZTR_CC_COLOR_RED "Bomber" EZTR_CC_COLOR_DEFAULT "! You" EZTR_CC_NEWLINE "guys don't mind, right?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Tingle
        EZTR_Basic_ReplaceText(
            0x1D00,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|79What's this?" EZTR_CC_NEWLINE "Strange clothes...White fairy..." EZTR_CC_NEWLINE "Ma'am, could you, by chance, be a" EZTR_CC_NEWLINE "forest fairy?" EZTR_CC_BOX_BREAK "Oh my!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x1D01,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|7AMy name is " EZTR_CC_COLOR_PINK "Tingle" EZTR_CC_COLOR_DEFAULT "!" EZTR_CC_NEWLINE "I think I am the same as you, ma'am." EZTR_CC_NEWLINE "A forest fairy!" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 "Alas, though I am already age 35," EZTR_CC_NEWLINE "no fairy has come to me yet..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "My father tells me to grow up" EZTR_CC_NEWLINE "and act my age, but why?" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "I tell you...Tingle is the very" EZTR_CC_NEWLINE "reincarnation of a fairy!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x1D0C,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|79Uh, ma'am?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x1D0D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|7AHello, Ms. Fairy!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x1D0A,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|7BMa'am, you don't have enough" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_PINK "Rupees" EZTR_CC_COLOR_DEFAULT "!" EZTR_CC_END "",
            NULL
        );

        // Guru-Guru
        EZTR_Basic_ReplaceText(
            0x292F,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|59Why could a...? Why could a...?" EZTR_CC_NEWLINE \
            "Why could a man join?!?" EZTR_CC_NEWLINE \
            "That's 'cause a man is an animal," EZTR_CC_NEWLINE \
            "too, my girl!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Lottery
        EZTR_Basic_ReplaceText(
            0x2B62,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Mmm, ma'am, if you don't have any" EZTR_CC_NEWLINE "" \
            EZTR_CC_COLOR_PINK "Rupees" EZTR_CC_COLOR_DEFAULT ", you can't buy your" EZTR_CC_NEWLINE \
            "dreams." EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x2B63,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Ma'am, we are now exchanging" EZTR_CC_NEWLINE \
            "winning tickets." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "If you'd like to buy a ticket, we'll" EZTR_CC_NEWLINE \
            "start selling them tomorrow" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_RED \
            "morning " EZTR_CC_COLOR_DEFAULT "at" EZTR_CC_COLOR_RED " 6 o'clock" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_END "",
            NULL
        );

        // Banker
        // First Time Meeting
        EZTR_Basic_ReplaceText(
            0x044C,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Hello there, miss!" EZTR_CC_NEWLINE \
            "Won't you deposit some " EZTR_CC_COLOR_PINK "Rupees" EZTR_CC_COLOR_DEFAULT "?" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Nowadays, even if people have" EZTR_CC_NEWLINE \
            "money, they don't deposit any." EZTR_CC_NEWLINE "" EZTR_CC_QUICKTEXT_ENABLE "Nothing!" EZTR_CC_QUICKTEXT_DISABLE "  " EZTR_CC_QUICKTEXT_ENABLE "Nothing!!!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 "So, for a limited time, I'll give you" EZTR_CC_NEWLINE "a special gift based on how much" EZTR_CC_NEWLINE "you deposit." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0461,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "So, lady, what's your name?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x045A,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "All right, miss, now I've got a" EZTR_CC_NEWLINE "total of " EZTR_CC_COLOR_PINK "" EZTR_CC_RUPEES_TOTAL "" EZTR_CC_COLOR_DEFAULT " from you!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0467,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "What's this? You need somethin'" EZTR_CC_NEWLINE "on a day like this?" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "You haven't evacuated yet? You're" EZTR_CC_NEWLINE "one brave lady!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        // Transaction
        EZTR_Basic_ReplaceText(
            0x046A,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Ah, yes..." EZTR_CC_COLOR_RED "" EZTR_CC_NAME "" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_NEWLINE "If I remember, you're the young" EZTR_CC_NEWLINE "lady who deposited " EZTR_CC_COLOR_PINK "" EZTR_CC_RUPEES_TOTAL "" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0458,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_QUICKTEXT_ENABLE "Hmm...You play mean jokes," EZTR_CC_NEWLINE "miss! You haven't even got a" EZTR_CC_NEWLINE "single " EZTR_CC_COLOR_PINK "Rupee" EZTR_CC_COLOR_DEFAULT "!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 "Come back and deposit some after" EZTR_CC_NEWLINE "you save up a bunch!" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0475,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "What's this?" EZTR_CC_NEWLINE "Look, miss, you can't hold" EZTR_CC_NEWLINE "this many rupees! You got that?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0459,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Heyyy! You don't have that much!" EZTR_CC_NEWLINE "You shouldn't lie to people!" EZTR_CC_NEWLINE "And you can take that to the" EZTR_CC_NEWLINE "bank, miss!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0455,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Whaaat?!?" EZTR_CC_NEWLINE "How rich are you?! Are you" EZTR_CC_NEWLINE "some noble lady or what?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x045F,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_QUICKTEXT_ENABLE "Excuuuse me! But I can't take" EZTR_CC_NEWLINE "anymore deposits!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Sheesh, miss! " EZTR_CC_NEWLINE "You're young, but you sure are" EZTR_CC_NEWLINE "serious about saving!" EZTR_CC_NEWLINE "I'm impressed!" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0478,
            EZTR_STANDARD_TEXT_BOX_II,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Look, miss, all the " EZTR_CC_COLOR_PINK "Rupees" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_DEFAULT "you deposited are gone, so you" EZTR_CC_NEWLINE "can't use that " EZTR_CC_COLOR_RED "stamp" EZTR_CC_COLOR_DEFAULT " anymore." EZTR_CC_END "",
            NULL
        );
        // Rewards
        EZTR_Basic_ReplaceText(
            0x045B,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_QUICKTEXT_ENABLE "What's this? You've already" EZTR_CC_NEWLINE \
            "saved up " EZTR_CC_COLOR_RED "200 Rupees" EZTR_CC_COLOR_DEFAULT "!?!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2\
            "Well, miss, here's your special" EZTR_CC_NEWLINE "gift. Take it!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x045D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_QUICKTEXT_ENABLE "What's this? You've already saved" EZTR_CC_NEWLINE "up " EZTR_CC_COLOR_RED "5000 Rupees" EZTR_CC_COLOR_DEFAULT "?!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Well, miss, I can't take any" EZTR_CC_NEWLINE "more deposits. Sorry, but this is" EZTR_CC_NEWLINE "all I can give you." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Trading Post Guy
        EZTR_Basic_ReplaceText(
            0x232E,
            EZTR_STANDARD_TEXT_BOX_I,
            48,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|52Oh! Who's the lucky man?" EZTR_CC_NEWLINE "Congratulations." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Trading Post Part Timer
        EZTR_Basic_ReplaceText(
            0x06DE,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Heyyy! I stole that Rupee from a" EZTR_CC_NEWLINE "crow! Don't go stealing it, dude!" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x06C8,
            EZTR_STANDARD_TEXT_BOX_I,
            48,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|6CMiss, I just work here part-time," EZTR_CC_NEWLINE "so I don't really know this stuff." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Honey and Darling
        EZTR_Basic_ReplaceText(
            0x2872,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|71Honey, she says she isn't playing..." EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x2878,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|2CBut if you fall from this platform," EZTR_CC_NEWLINE "you're out. Isn't she, Darling?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x287E,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|70Did you see that? Honey, she got" EZTR_CC_NEWLINE "a perfect score..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "We have to share our happiness" EZTR_CC_NEWLINE "with her, Honey." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x287F,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "That's annoying, so let's show off" EZTR_CC_NEWLINE "even more for her, Darling." EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x2880,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|2CH-H-Honey!!!" EZTR_CC_NEWLINE \
            "She...She got a perfect score three" EZTR_CC_NEWLINE \
            "days in a row." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "This is all we have left to give" EZTR_CC_NEWLINE "her, Honey." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Shooting Gallery
        EZTR_Basic_ReplaceText(
            0x03E8,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|4BIf you think you can do it, why" EZTR_CC_NEWLINE "don'tcha give it a try, missy?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x03FD,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|4BAll right, missy!" EZTR_CC_NEWLINE "The rules are simple..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Hit as many " EZTR_CC_COLOR_RED "red ones" EZTR_CC_COLOR_DEFAULT " as you can" EZTR_CC_NEWLINE "within the time limit." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "If you hit a " EZTR_CC_COLOR_BLUE "blue one" EZTR_CC_COLOR_DEFAULT ", you lose" EZTR_CC_NEWLINE "time, so watch out!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "You can use as many arrows as" EZTR_CC_NEWLINE "you'd like. Press " EZTR_CC_BTN_B " to shoot" EZTR_CC_NEWLINE "them." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Treasure Chest Lady
        EZTR_Basic_ReplaceText(
            0x076D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            0x0771,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|2ACome in! You look like a" EZTR_CC_NEWLINE "competitive one. How about it?" EZTR_CC_NEWLINE "Wanna play?" EZTR_CC_CONTINUE "" EZTR_CC_END "",
            NULL
        );

        // Talon
        EZTR_Basic_ReplaceText(
            0x2AF9,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "The bar opens at night, ma'am." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "And it's members-only. We refuse" EZTR_CC_NEWLINE \
            "people who don't have " EZTR_CC_COLOR_RED "proof of" EZTR_CC_NEWLINE "membership" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 "Well, now I'm getting things ready," EZTR_CC_NEWLINE "so as long as you don't get in the" EZTR_CC_NEWLINE "way of my cleaning, you can stay." EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x2B09,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Ma'am...It's the rules. Please wear" EZTR_CC_NEWLINE "your " EZTR_CC_COLOR_RED "Romani's Mask" EZTR_CC_COLOR_DEFAULT " inside the bar." EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x2B0C,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "You don't have enough " EZTR_CC_COLOR_PINK "Rupees" EZTR_CC_COLOR_DEFAULT ", ma'am." EZTR_CC_END "",
            NULL
        );

        // Mayor
        EZTR_Basic_ReplaceText(
            0x2AD0,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Thank you for allowing me to put" EZTR_CC_NEWLINE \
            "an end to all that pointless" EZTR_CC_NEWLINE \
            "bickering." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "You seem to understand just how" EZTR_CC_NEWLINE \
            "shameful this was. I appreciate it." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Here is a token of my gratitude." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Anju
        EZTR_Basic_ReplaceText(
            0x28AA,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Your name is " EZTR_CC_NAME ", is it?" EZTR_CC_NEWLINE "One moment, please." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        EZTR_Basic_ReplaceText(
            0x28AB,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "I have you down for an afternoon" EZTR_CC_NEWLINE "arrival." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 "Your room is our 'Knife Chamber'" EZTR_CC_NEWLINE "on the " EZTR_CC_COLOR_RED "second floor" EZTR_CC_COLOR_DEFAULT ". Here is your" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_RED "key" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Business Deku Scrub (Town)
        EZTR_Basic_ReplaceText(
            0x0614,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|3A|D1Excuse me!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "I keep hearing about a stone" EZTR_CC_NEWLINE \
            "called a " EZTR_CC_COLOR_RED "Moon's Tear" EZTR_CC_COLOR_DEFAULT ", and it's" EZTR_CC_NEWLINE \
            "supposed to be the shiniest stone" EZTR_CC_NEWLINE \
            "in the world!" EZTR_CC_BOX_BREAK "Do you have one?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0616,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|3A|D2If you have a " EZTR_CC_COLOR_RED "Moon's Tear" EZTR_CC_COLOR_DEFAULT "," EZTR_CC_NEWLINE "I must get it from you...And I'm" EZTR_CC_NEWLINE "not asking to get it for free!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0619,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|38|81You've really helped me out!" EZTR_CC_NEWLINE \
            "Now I have the perfect souvenir" EZTR_CC_NEWLINE \
            "for my wife!" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "She hasn't seen a jewel or" EZTR_CC_NEWLINE \
            "strange, shining stone like this in" EZTR_CC_NEWLINE \
            "quite a long time...Hee-hee!" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "The " EZTR_CC_COLOR_RED "Title Deed" EZTR_CC_COLOR_DEFAULT " for this spot will" EZTR_CC_NEWLINE \
            "be in big demand by Deku Scrubs" EZTR_CC_NEWLINE \
            "outside of town." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "You should be able to sell it to a" EZTR_CC_NEWLINE \
            "Deku Scrub for a good price!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Deku Princess
        EZTR_Basic_ReplaceText(
            0x0966,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|39|37W-Well, we haven't any time to" EZTR_CC_NEWLINE \
            "lose..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Quickly, Ms. " EZTR_CC_NAME ", could you" EZTR_CC_NEWLINE \
            "please find something to " EZTR_CC_COLOR_RED "carry" EZTR_CC_COLOR_DEFAULT " me" EZTR_CC_NEWLINE \
            "in so you can take me to the" EZTR_CC_NEWLINE "Deku Palace?" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0971,
            EZTR_STANDARD_TEXT_BOX_I,
            16,
            EZTR_ICON_DEKU_PRINCESS,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|37Ungh! Ms. " EZTR_CC_COLOR_RED "" EZTR_CC_NAME "" EZTR_CC_COLOR_DEFAULT "!" EZTR_CC_NEWLINE \
            "There's nuh-no time to lose!" EZTR_CC_NEWLINE \
            "Huh-Hurry! Take me to the palace!" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x0969,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_DEKU_PRINCESS,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|39|37Hwh-Hwhy are you hwaiting?" EZTR_CC_NEWLINE \
            "Ms. " EZTR_CC_COLOR_RED "" EZTR_CC_NAME "" EZTR_CC_COLOR_DEFAULT "!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Huh-Hurry! Ho-Hopen this " EZTR_CC_COLOR_RED "bottle" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_DEFAULT \
            "and release me right hwh-hwhere" EZTR_CC_NEWLINE \
            "my father is standing." EZTR_CC_FADE "|00|1E" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x096D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|39|8CWhy yes, thanks to" EZTR_CC_NEWLINE \
            "MS. " EZTR_CC_NAME " here." EZTR_CC_NEWLINE \
            "I am very, truly grateful." EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x096F,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|39|8CIt seems our butler has taken" EZTR_CC_NEWLINE \
            "somewhat of a liking to you," EZTR_CC_NEWLINE \
            "Ms. " EZTR_CC_NAME "." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "I believe you'll find him waiting in" EZTR_CC_NEWLINE \
            "the shrine that will be to your" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_RED "right" EZTR_CC_COLOR_DEFAULT " when you leave the palace." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        
        // Shiro
        EZTR_Basic_ReplaceText(
            0x1473,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "...Oh, wow." EZTR_CC_NEWLINE "...I actually caught your attention..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x1474,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "...You look startled. Are you" EZTR_CC_NEWLINE \
            "that surprised to see me...?" EZTR_CC_NEWLINE \
            "...Don't worry about it..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 
            "I've been here for many years," EZTR_CC_NEWLINE \
            "waving my arms around and asking" EZTR_CC_NEWLINE \
            "for help, but everyone ignores me" EZTR_CC_NEWLINE \
            "and passes me by." EZTR_CC_BOX_BREAK \
            "It's 'cause I'm about as impressive" EZTR_CC_NEWLINE \
            "as a stone, right?...I'm used to it," EZTR_CC_NEWLINE \
            "though." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x1472,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Huh? I don't stand out at all?" EZTR_CC_NEWLINE \
            "...Oh well." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "...Um. By the way, you can" EZTR_CC_NEWLINE \
            "stop giving me that weird look..." EZTR_CC_NEWLINE \
            "Thanks." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Captain Keeta
        EZTR_Basic_ReplaceText(
            0x153C,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_QUICKTEXT_ENABLE "Captain, ma'am!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "May I take leave, ma'am?" EZTR_CC_CONTINUE "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x153D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|3A|C5Yes, ma'am!" EZTR_CC_END "",
            NULL
        );

        // Gorman Brothers
        EZTR_Basic_ReplaceText(
            0x346F,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "This lady's funny!" EZTR_CC_NEWLINE "Why don't you give us " EZTR_CC_COLOR_PINK "10 Rupees" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_DEFAULT "for the chance to race against" EZTR_CC_NEWLINE "us?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3470,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "If you win, we'll give you somethin'" EZTR_CC_NEWLINE "nice, lady." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3483,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "If you win, we'll give you" EZTR_CC_NEWLINE "somethin' nice, lady." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x347E,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "We gotta give ya something, " EZTR_CC_NEWLINE "so you got yourself some milk," EZTR_CC_NEWLINE "lady!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Kafei
        EZTR_Basic_ReplaceText(
            0x2969,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Messy hair..." EZTR_CC_NEWLINE "" EZTR_CC_COLOR_RED "Body scars" EZTR_CC_COLOR_DEFAULT "..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Anju wrote about you in her" EZTR_CC_NEWLINE "letter." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "It seems you are looking for" EZTR_CC_NEWLINE "Kafei." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x00AB,
            EZTR_TRANSLUSCENT_BLUE_TEXT_BOX,
            1,
            EZTR_ICON_PENDANT_OF_MEMORIES,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_QUICKTEXT_ENABLE "You accepted the " EZTR_CC_COLOR_RED "Pendant of" EZTR_CC_NEWLINE \
            "Memories" EZTR_CC_COLOR_DEFAULT "!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Going against your mischievous" EZTR_CC_NEWLINE \
            "desires to mess with this child," EZTR_CC_NEWLINE \
            "you decide to just take the" EZTR_CC_NEWLINE \
            "pendant to " EZTR_CC_COLOR_RED "Anju" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_END "",

            NULL
        );
        EZTR_Basic_ReplaceText(
            0x145D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "I found him, miss..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Curiosity Shop Keeper
        EZTR_Basic_ReplaceText(
            0x29E0,
            EZTR_STANDARD_TEXT_BOX_I,
            48,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Huh?" EZTR_CC_NEWLINE \
            "You the messy-haired lady?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Ikana Canyon Wanderer
        EZTR_Basic_ReplaceText(
            0x14F1,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Even now, the spirits wander" EZTR_CC_NEWLINE \
            "in search of one who can save" EZTR_CC_NEWLINE \
            "them." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "It is no place even for one with" EZTR_CC_NEWLINE \
            "certain 'passions' such as you..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14F3,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Without that mask, you will" EZTR_CC_NEWLINE "fail to 'seek' them." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Until then, I will not let you pass" EZTR_CC_NEWLINE "by here." EZTR_CC_NEWLINE "Yee-hee-hee!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14ED,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Now it is the gathering place" EZTR_CC_NEWLINE \
            "for the spirits of those with" EZTR_CC_NEWLINE \
            "lingering regrets." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "Until now they wander, seeking" EZTR_CC_NEWLINE \
            "one who can save them." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "You won't find them with that" EZTR_CC_NEWLINE "" EZTR_CC_COLOR_RED "mask" EZTR_CC_COLOR_DEFAULT \
            "...Have you no other? It is" EZTR_CC_NEWLINE \
            "unfortunate, but I cannot let you" EZTR_CC_NEWLINE \
            "pass. Yee-hee-hee." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14E8,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Use it to ask them their regrets..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Perhaps you can make better use" EZTR_CC_NEWLINE \
            "of yourself with it instead of" EZTR_CC_NEWLINE \
            "satiating your " EZTR_CC_COLOR_RED "bloodlust" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_NEWLINE \
            "Yee-hee-hee!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14EA,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "With that, you may be able to call" EZTR_CC_NEWLINE \
            "out their spirits that even now" EZTR_CC_NEWLINE \
            "are still wandering." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "Perhaps you can make better use" EZTR_CC_NEWLINE \
            "of yourself with it instead of" EZTR_CC_NEWLINE \
            "satiating your " EZTR_CC_COLOR_RED "bloodlust" EZTR_CC_COLOR_DEFAULT "." EZTR_CC_NEWLINE \
            "Yee-hee-hee!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14D1,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "If you are seeking to satiate" EZTR_CC_NEWLINE \
            "your " EZTR_CC_COLOR_RED "bloodlust" EZTR_CC_COLOR_DEFAULT ", you may find" EZTR_CC_NEWLINE \
            "what you are seeking here..." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "from a group of spirits plagued by" EZTR_CC_NEWLINE \
            "lingering regrets." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14D2,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "If you have faith in your skills" EZTR_CC_NEWLINE \
            "and ferocity..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "then make use of them against" EZTR_CC_NEWLINE \
            "these spirits. " EZTR_CC_NEWLINE \
            "Yee-hee-hee..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14D6,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "We don't run charities here. Do not" EZTR_CC_NEWLINE \
            "expect us to fill your hedonistic" EZTR_CC_NEWLINE \
            "tastes for free. Yee-hee-hee!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14D8,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "You are as eager as you look." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Defeat the ladies within the time" EZTR_CC_NEWLINE \
            "limit and rest their souls in peace." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14D9,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "But if your life energy drops" EZTR_CC_NEWLINE \
            "below three hearts, I will stop" EZTR_CC_NEWLINE \
            "the match." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "Mercy is the least I can offer," EZTR_CC_NEWLINE \
            "even to you..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14DB,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Gripped by fear?" EZTR_CC_NEWLINE "Yee-hee-hee!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14D3,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "It is impossible in your current" EZTR_CC_NEWLINE \
            "state, but once you have eased" EZTR_CC_NEWLINE \
            "your weariness and have gained" EZTR_CC_NEWLINE \
            "faith in your skills..." EZTR_CC_BOX_BREAK \
            "You may try to defeat these" EZTR_CC_NEWLINE \
            "wandering spirits." EZTR_CC_NEWLINE \
            "Yee-hee-hee..." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14DC,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "It seems their souls have been" EZTR_CC_NEWLINE \
            "healed through their defeat." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14E1,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Whatever mystical powers you" EZTR_CC_NEWLINE \
            "possess will not help you..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "If it takes you that long, you" EZTR_CC_NEWLINE \
            "cannot defeat these foes..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14F5,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Yee-hee-hee." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "It seems, in your own twisted" EZTR_CC_NEWLINE \
            "way, you managed to send Ikana's" EZTR_CC_NEWLINE \
            "wandering spirits into peace..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14F6,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "But outside of Ikana..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN \
            "There are still swarms of" EZTR_CC_BOX_BREAK \
            "wandering spirits with lingering" EZTR_CC_NEWLINE \
            "regrets..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14F9,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Yee-hee-hee." EZTR_CC_NEWLINE \
            "You really are a ferocious woman," EZTR_CC_NEWLINE \
            "" EZTR_CC_NAME "." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x14FA,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "It seems your savagery has ended" EZTR_CC_NEWLINE \
            "the suffering of their souls..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Maybe I shall vanish soon myself." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        
        // Marine Research Professor
        EZTR_Basic_ReplaceText(
            0x0FA1,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|49Hmm? What is it, girl?" EZTR_CC_NEWLINE "Did you also come to watch the" EZTR_CC_NEWLINE "Zora eggs hatch?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Pamela's Father
        EZTR_Basic_ReplaceText(
            0x158F,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|63" EZTR_CC_QUICKTEXT_ENABLE "Oh!" EZTR_CC_NEWLINE \
            "Isn't this a fairy?" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Is this yours, " EZTR_CC_QUICKTEXT_ENABLE "young lady?" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x1594,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Say, " EZTR_CC_QUICKTEXT_ENABLE "young lady!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE \
            "Would you permit me to do" EZTR_CC_NEWLINE \
            "research on this fairy?" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x15A5,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|63" EZTR_CC_QUICKTEXT_ENABLE "Oh! That mask is certainly a" EZTR_CC_NEWLINE \
            "Gibdo!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "" EZTR_CC_QUICKTEXT_ENABLE \
            "Young lady!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE \
            "Where did you get that?" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x15A7,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_QUICKTEXT_ENABLE "Say, young lady!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE \
            "Would you permit me to do" EZTR_CC_NEWLINE \
            "research on that mask?" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );

        // Romani
        EZTR_Basic_ReplaceText(
            0x333C,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|2EHow about..." EZTR_CC_NEWLINE "" \
            EZTR_CC_COLOR_ORANGE "Mantis" EZTR_CC_COLOR_DEFAULT "?" EZTR_CC_NEWLINE \
            "That's the name Romani gives you." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "See, you're wearing green clothes," EZTR_CC_NEWLINE \
            "See, you look kinda ferocious," EZTR_CC_NEWLINE \
            "and you move around in a sneaky" EZTR_CC_NEWLINE \
            "way, so " EZTR_CC_COLOR_ORANGE "Mantis " EZTR_CC_COLOR_DEFAULT "it is!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3341,
            EZTR_STANDARD_TEXT_BOX_I,
            1,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Hey, Mantis, I'm recruiting" EZTR_CC_NEWLINE \
            "for an assistant right now!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "You're a grown-up, won't you try?" EZTR_CC_NEWLINE " " EZTR_CC_NEWLINE "" \
            EZTR_CC_COLOR_GREEN "" EZTR_CC_TWO_CHOICE "Yes" EZTR_CC_NEWLINE "No" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3346,
            EZTR_STANDARD_TEXT_BOX_II,
            1,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Will you help me, Mantis?" EZTR_CC_NEWLINE " " EZTR_CC_NEWLINE "" \
            EZTR_CC_COLOR_GREEN "" EZTR_CC_TWO_CHOICE "Yes" EZTR_CC_NEWLINE "No" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3343,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|69|2EGreat!" EZTR_CC_NEWLINE \
            "Now that's the spirit," EZTR_CC_NEWLINE \
            "Mantis!" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "OK then, I'm going straight into" EZTR_CC_NEWLINE \
            "my strategy!" EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3345,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Mantis!" EZTR_CC_NEWLINE \
            "Let's practice right away!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x332D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            0x332E,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "It's almost time..." EZTR_CC_NEWLINE \
            "You take care of things outside," EZTR_CC_NEWLINE \
            "Mantis!" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "If you have a map, watch your" EZTR_CC_NEWLINE \
            "" EZTR_CC_COLOR_RED "map " EZTR_CC_COLOR_DEFAULT "carefully!" \
            EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "OK? Whatever happens, do not let" EZTR_CC_NEWLINE\
            "them get into the barn!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "They'll come from " EZTR_CC_COLOR_RED "behind the barn" EZTR_CC_COLOR_DEFAULT ", " EZTR_CC_NEWLINE \
            "too, so watch out." EZTR_CC_CONTINUE "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3334,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "It's almost time for my sister to" EZTR_CC_NEWLINE \
            "get up, so I have to get back to" EZTR_CC_NEWLINE \
            "bed..." EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "See you later..." EZTR_CC_DELAY \
            "|00|0ABig hero." EZTR_CC_EVENT2 "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x3369,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Listen, Mantis!" EZTR_CC_NEWLINE \
            "Romani is looking after things!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Now that we chased them away," EZTR_CC_NEWLINE \
            "things are safe. My sister doesn't" EZTR_CC_NEWLINE \
            "need to know at all!" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 \
            "Hey, Mantis, can I ask you" EZTR_CC_NEWLINE \
            "to help next yea..." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x336A,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            ".....!" EZTR_CC_NEWLINE \
            "I know, Mantis!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "Why don't you just live at the" EZTR_CC_NEWLINE \
            "ranch? I'll lend you Romani's bed." EZTR_CC_EVENT "" EZTR_CC_END "",
            NULL
        );

        // Cremia
        EZTR_Basic_ReplaceText(
            0x33B9,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Girl, get your bow ready..." EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x33BB,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            0x33BC,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Are you ready, girl?" EZTR_CC_NEWLINE \
            "I'm going to try to get us through" EZTR_CC_NEWLINE \
            "here as fast as I can!" EZTR_CC_CONTINUE "" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x33D8,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "Thank you..." EZTR_CC_NEWLINE \
            "You were pretty cool..." EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 \
            "This isn't very big, but accept my" EZTR_CC_NEWLINE \
            "thanks!" EZTR_CC_END "",
            NULL
        );

        // Igos Du Ikana & Henchmen
        EZTR_Basic_ReplaceText(
            0x151C,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|3A|36You're blocking me!" EZTR_CC_NEWLINE \
            "Get out of the way!" EZTR_CC_NEWLINE \
            "I can still get her!" EZTR_CC_END "",
            NULL
        );
        EZTR_Basic_ReplaceText(
            0x151D,
            EZTR_STANDARD_TEXT_BOX_I,
            0,
            EZTR_ICON_NO_ICON,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            EZTR_NO_VALUE,
            true,
            "" EZTR_CC_SFX "|3A|39B-blocking you?!" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "" EZTR_CC_SFX \
            "|3A|39The reason she beat us is because" EZTR_CC_NEWLINE \
            "you were so feeble! Don't blame" EZTR_CC_NEWLINE \
            "this on me!" EZTR_CC_END "",
            NULL
        );
    }
}