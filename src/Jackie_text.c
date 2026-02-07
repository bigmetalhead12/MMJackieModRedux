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

EZTR_ON_INIT void replace_msgs() {
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
        "Hello there, miss!" EZTR_CC_NEWLINE "Won't you deposit some " EZTR_CC_COLOR_PINK "Rupees" EZTR_CC_COLOR_DEFAULT "?" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Nowadays, even if people have" EZTR_CC_NEWLINE "money, they don't deposit any." EZTR_CC_NEWLINE "" EZTR_CC_QUICKTEXT_ENABLE "Nothing!" EZTR_CC_QUICKTEXT_DISABLE "  " EZTR_CC_QUICKTEXT_ENABLE "Nothing!!!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_BOX_BREAK2 "So, for a limited time, I'll give you" EZTR_CC_NEWLINE "a special gift based on how much" EZTR_CC_NEWLINE "you deposit." EZTR_CC_EVENT "" EZTR_CC_END "",
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
        0x045B,
        EZTR_STANDARD_TEXT_BOX_I,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "" EZTR_CC_QUICKTEXT_ENABLE "What's this? You've already" EZTR_CC_NEWLINE "saved up " EZTR_CC_COLOR_RED "200 Rupees" EZTR_CC_COLOR_DEFAULT "!?!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Well, little guy, here's your special" EZTR_CC_NEWLINE "gift. Take it!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
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
        "" EZTR_CC_QUICKTEXT_ENABLE "What's this? You've already" EZTR_CC_NEWLINE "saved up " EZTR_CC_COLOR_RED "200 Rupees" EZTR_CC_COLOR_DEFAULT "!?!" EZTR_CC_QUICKTEXT_DISABLE "" EZTR_CC_NEWLINE "" EZTR_CC_CARRIAGE_RETURN "" EZTR_CC_BOX_BREAK2 "Well, miss, here's your special" EZTR_CC_NEWLINE "gift. Take it!" EZTR_CC_EVENT2 "" EZTR_CC_END "",
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
}