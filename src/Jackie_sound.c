//-----------------------------------------------------------------------------
//
// $ Author: Bigmetalhead12 $
//
// 2026
//
// Jackie Custom Sound Mod (using Magemod's Audio API)
// API Source: https://github.com/magemods/mm-audio-api
//-----------------------------------------------------------------------------

#include "global.h"
#include "modding.h"
#include "recomputils.h"
#include "audio_api/all.h"

// Choked (Redead, Wallmaster, etc.)
INCBIN(choke, "src/jackieVoice/choke.raw");

// Attack (regular attack & roll)
INCBIN(attack1, "src/jackieVoice/attack1.raw");
INCBIN(attack2, "src/jackieVoice/attack2.raw");
INCBIN(attack3, "src/jackieVoice/attack3.raw");
INCBIN(attack4, "src/jackieVoice/attack4.raw");

// Strong Attack (jump attack & spin attack)
INCBIN(strongattack1, "src/jackieVoice/strongattack1.raw");
INCBIN(strongattack2, "src/jackieVoice/strongattack2.raw");

// Ledge Dangle
INCBIN(ledgedangle1, "src/jackieVoice/ledgedangle1.raw");
INCBIN(ledgedangle2, "src/jackieVoice/ledgedangle2.raw");

// Ledgeclimb
INCBIN(ledgeclimb1, "src/jackieVoice/ledgeclimb1.raw");
INCBIN(ledgeclimb2, "src/jackieVoice/ledgeclimb2.raw");

// Hurt
INCBIN(hurt1, "src/jackieVoice/hurt1.raw");
INCBIN(hurt2, "src/jackieVoice/hurt2.raw");
INCBIN(hurt3, "src/jackieVoice/hurt3.raw");

// Knocked Down 
INCBIN(knockeddown1, "src/jackieVoice/knockeddown1.raw");
INCBIN(knockeddown2, "src/jackieVoice/knockeddown2.raw");
INCBIN(knockeddown3, "src/jackieVoice/knockeddown3.raw");

// Fall
INCBIN(fall1, "src/jackieVoice/falling1.raw");
INCBIN(fall2, "src/jackieVoice/falling2.raw");

// Gasp
INCBIN(gasp1, "src/jackieVoice/gasp1.raw");
INCBIN(gasp2, "src/jackieVoice/gasp2.raw");

// Low health breathing
INCBIN(lowhealth1, "src/jackieVoice/lowhealth1.raw");
INCBIN(lowhealth2, "src/jackieVoice/lowhealth2.raw");

// Hurt Landing
INCBIN(hurtlanding, "src/jackieVoice/hurtlanding.raw");

// Resurface (resurface & finish drinking)
INCBIN(resurface, "src/jackieVoice/resurface.raw");

// Jump (jump & bottle)
INCBIN(jump1, "src/jackieVoice/jump1.raw");
INCBIN(jump2, "src/jackieVoice/jump2.raw");

// Charge Up (unused in game)
INCBIN(chargeup, "src/jackieVoice/chargeup.raw");

// Din's Fire
INCBIN(dinsfire, "src/jackieVoice/dinsfire.raw");

// Die
INCBIN(die1, "src/jackieVoice/die1.raw");
INCBIN(die2, "src/jackieVoice/die2.raw");
INCBIN(die3, "src/jackieVoice/die3.raw");

// Shivering
INCBIN(shivering, "src/jackieVoice/shivering.raw");

// Sneeze
INCBIN(sneeze, "src/jackieVoice/sneeze.raw");

// Moan from heat
INCBIN(moanfromheat, "src/jackieVoice/moanfromheat.raw");

// Wiping sweat
INCBIN(wipingsweat, "src/jackieVoice/wipingsweat.raw");

// Yawn
INCBIN(yawn1, "src/jackieVoice/yawn1.raw");
INCBIN(yawn2, "src/jackieVoice/yawn2.raw");
INCBIN(yawn3, "src/jackieVoice/yawn3.raw");

// Heavy Gasp
INCBIN(heavygasp, "src/jackieVoice/heavygasp.raw");

// Epona / Hookshot (epona & hookshot sound)
INCBIN(eponahookshot1, "src/jackieVoice/eponahookshot1.raw");
INCBIN(eponahookshot2, "src/jackieVoice/eponahookshot2.raw");


// Mask Transformation
INCBIN(wearmask1, "src/jackieVoice/gasp1.raw");
INCBIN(wearmask2, "src/jackieVoice/gasp2.raw");
INCBIN(wearmask3, "src/jackieVoice/falling1.raw");

RECOMP_CALLBACK("magemods_audio_api", AudioApi_Init) void my_mod_on_init_attack_test() {
    // Choked
    AdpcmLoop choke_sample_LOOP = {
        { 0, (choke_end - choke) / 2, 0, 0 }, {}
    };
    Sample choke_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        choke_end - choke,
        choke,
        &choke_sample_LOOP,
        NULL
    };
    SoundEffect choke_sfx = {
        { &choke_sample, 1.0f },
    };

    // Attack 1
    AdpcmLoop attack1_sample_LOOP = {
        { 0, (attack1_end - attack1) / 2, 0, 0 }, {}
    };
    Sample attack1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        attack1_end - attack1,
        attack1,
        &attack1_sample_LOOP,
        NULL
    };
    SoundEffect attack1_sfx = {
        { &attack1_sample, 1.0f },
    };

    // Attack 2
    AdpcmLoop attack2_sample_LOOP = {
        { 0, (attack2_end - attack2) / 2, 0, 0 }, {}
    };
    Sample attack2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        attack2_end - attack2,
        attack2,
        &attack2_sample_LOOP,
        NULL
    };
    SoundEffect attack2_sfx = {
        { &attack2_sample, 1.0f },
    };

    // Attack 3
    AdpcmLoop attack3_sample_LOOP = {
        { 0, (attack3_end - attack3) / 2, 0, 0 }, {}
    };
    Sample attack3_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        attack3_end - attack3,
        attack3,
        &attack3_sample_LOOP,
        NULL
    };
    SoundEffect attack3_sfx = {
        { &attack3_sample, 1.0f },
    };

    // Attack 4
    AdpcmLoop attack4_sample_LOOP = {
        { 0, (attack4_end - attack4) / 2, 0, 0 }, {}
    };
    Sample attack4_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        attack4_end - attack4,
        attack4,
        &attack4_sample_LOOP,
        NULL
    };
    SoundEffect attack4_sfx = {
        { &attack4_sample, 1.0f },
    };
    
    // Strong Attack 1
    AdpcmLoop strongattack1_sample_LOOP = {
        { 0, (strongattack1_end - strongattack1) / 2, 0, 0 }, {}
    };
    Sample strongattack1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        strongattack1_end - strongattack1,
        strongattack1,
        &strongattack1_sample_LOOP,
        NULL
    };
    SoundEffect strongattack1_sfx = {
        { &strongattack1_sample, 1.0f },
    };

    // Strong Attack 2
    AdpcmLoop strongattack2_sample_LOOP = {
        { 0, (strongattack2_end - strongattack2) / 2, 0, 0 }, {}
    };
    Sample strongattack2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        strongattack2_end - strongattack2,
        strongattack2,
        &strongattack2_sample_LOOP,
        NULL
    };
    SoundEffect strongattack2_sfx = {
        { &strongattack2_sample, 1.0f },
    };
    
    // Ledge Dangle 1
    AdpcmLoop ledgedangle1_sample_LOOP = {
        { 0, (ledgedangle1_end - ledgedangle1) / 2, 0, 0 }, {}
    };
    Sample ledgedangle1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        ledgedangle1_end - ledgedangle1,
        ledgedangle1,
        &ledgedangle1_sample_LOOP,
        NULL
    };
    SoundEffect ledgedangle1_sfx = {
        { &ledgedangle1_sample, 1.0f },
    };

    // Ledge Dangle 1
    AdpcmLoop ledgedangle2_sample_LOOP = {
        { 0, (ledgedangle2_end - ledgedangle2) / 2, 0, 0 }, {}
    };
    Sample ledgedangle2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        ledgedangle2_end - ledgedangle2,
        ledgedangle2,
        &ledgedangle2_sample_LOOP,
        NULL
    };
    SoundEffect ledgedangle2_sfx = {
        { &ledgedangle2_sample, 1.0f },
    };

    // Ledgeclimb 1
    AdpcmLoop ledgeclimb1_sample_LOOP = {
        { 0, (ledgeclimb1_end - ledgeclimb1) / 2, 0, 0 }, {}
    };
    Sample ledgeclimb1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        ledgeclimb1_end - ledgeclimb1,
        ledgeclimb1,
        &ledgeclimb1_sample_LOOP,
        NULL
    };
    SoundEffect ledgeclimb1_sfx = {
        { &ledgeclimb1_sample, 1.0f },
    };
    
    // Ledgeclimb 2
    AdpcmLoop ledgeclimb2_sample_LOOP = {
        { 0, (ledgeclimb2_end - ledgeclimb2) / 2, 0, 0 }, {}
    };
    Sample ledgeclimb2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        ledgeclimb2_end - ledgeclimb2,
        ledgeclimb2,
        &ledgeclimb2_sample_LOOP,
        NULL
    };
    SoundEffect ledgeclimb2_sfx = {
        { &ledgeclimb2_sample, 1.0f },
    };

    // Hurt 1
    AdpcmLoop hurt1_sample_LOOP = {
        { 0, (hurt1_end - hurt1) / 2, 0, 0 }, {}
    };
    Sample hurt1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        hurt1_end - hurt1,
        hurt1,
        &hurt1_sample_LOOP,
        NULL
    };
    SoundEffect hurt1_sfx = {
        { &hurt1_sample, 1.0f },
    };

    // Hurt 2
    AdpcmLoop hurt2_sample_LOOP = {
        { 0, (hurt2_end - hurt2) / 2, 0, 0 }, {}
    };
    Sample hurt2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        hurt2_end - hurt2,
        hurt2,
        &hurt2_sample_LOOP,
        NULL
    };
    SoundEffect hurt2_sfx = {
        { &hurt2_sample, 1.0f },
    };

    // Hurt 3
    AdpcmLoop hurt3_sample_LOOP = {
        { 0, (hurt3_end - hurt3) / 2, 0, 0 }, {}
    };
    Sample hurt3_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        hurt3_end - hurt3,
        hurt3,
        &hurt3_sample_LOOP,
        NULL
    };
    SoundEffect hurt3_sfx = {
        { &hurt2_sample, 1.0f },
    };

    // Knocked Down 1
    AdpcmLoop knockeddown1_sample_LOOP = {
        { 0, (knockeddown1_end - knockeddown1) / 2, 0, 0 }, {}
    };
    Sample knockeddown1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        knockeddown1_end - knockeddown1,
        knockeddown1,
        &knockeddown1_sample_LOOP,
        NULL
    };
    SoundEffect knockeddown1_sfx = {
        { &knockeddown1_sample, 1.0f },
    };

    // Knocked Down 2
    AdpcmLoop knockeddown2_sample_LOOP = {
        { 0, (knockeddown2_end - knockeddown2) / 2, 0, 0 }, {}
    };
    Sample knockeddown2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        knockeddown2_end - knockeddown2,
        knockeddown2,
        &knockeddown2_sample_LOOP,
        NULL
    };
    SoundEffect knockeddown2_sfx = {
        { &knockeddown2_sample, 1.0f },
    };

    // Knocked Down 3
    AdpcmLoop knockeddown3_sample_LOOP = {
        { 0, (knockeddown3_end - knockeddown3) / 2, 0, 0 }, {}
    };
    Sample knockeddown3_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        knockeddown3_end - knockeddown3,
        knockeddown3,
        &knockeddown3_sample_LOOP,
        NULL
    };
    SoundEffect knockeddown3_sfx = {
        { &knockeddown3_sample, 1.0f },
    };
    
    // Fall 1
    AdpcmLoop fall1_sample_LOOP = {
        { 0, (fall1_end - fall1) / 2, 0, 0 }, {}
    };
    Sample fall1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        fall1_end - fall1,
        fall1,
        &fall1_sample_LOOP,
        NULL
    };
    SoundEffect fall1_sfx = {
        { &fall1_sample, 1.0f },
    };

    // Fall 2
    AdpcmLoop fall2_sample_LOOP = {
        { 0, (fall2_end - fall2) / 2, 0, 0 }, {}
    };
    Sample fall2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        fall2_end - fall2,
        fall2,
        &fall2_sample_LOOP,
        NULL
    };
    SoundEffect fall2_sfx = {
        { &fall2_sample, 1.0f },
    };

    // Gasp 1
    AdpcmLoop gasp1_sample_LOOP = {
        { 0, (gasp1_end - gasp1) / 2, 0, 0 }, {}
    };
    Sample gasp1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        gasp1_end - gasp1,
        gasp1,
        &gasp1_sample_LOOP,
        NULL
    };
    SoundEffect gasp1_sfx = {
        { &gasp1_sample, 1.0f },
    };

    // Gasp 2
    AdpcmLoop gasp2_sample_LOOP = {
        { 0, (gasp2_end - gasp2) / 2, 0, 0 }, {}
    };
    Sample gasp2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        gasp2_end - gasp2,
        gasp2,
        &gasp2_sample_LOOP,
        NULL
    };
    SoundEffect gasp2_sfx = {
        { &gasp2_sample, 1.0f },
    };

    // Low Health 1
    AdpcmLoop lowhealth1_sample_LOOP = {
        { 0, (lowhealth1_end - lowhealth1) / 2, 0, 0 }, {}
    };
    Sample lowhealth1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        lowhealth1_end - lowhealth1,
        lowhealth1,
        &lowhealth1_sample_LOOP,
        NULL
    };
    SoundEffect lowhealth1_sfx = {
        { &lowhealth1_sample, 1.0f },
    };

    // Low Health 1
    AdpcmLoop lowhealth2_sample_LOOP = {
        { 0, (lowhealth2_end - lowhealth2) / 2, 0, 0 }, {}
    };
    Sample lowhealth2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        lowhealth2_end - lowhealth2,
        lowhealth2,
        &lowhealth2_sample_LOOP,
        NULL
    };
    SoundEffect lowhealth2_sfx = {
        { &lowhealth2_sample, 1.0f },
    };

    // Hurt Landing
    AdpcmLoop hurtlanding_sample_LOOP = {
        { 0, (hurtlanding_end - hurtlanding) / 2, 0, 0 }, {}
    };
    Sample hurtlanding_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        hurtlanding_end - hurtlanding,
        hurtlanding,
        &hurtlanding_sample_LOOP,
        NULL
    };
    SoundEffect hurtlanding_sfx = {
        { &hurtlanding_sample, 1.0f },
    };

    // Resurface
    AdpcmLoop resurface_sample_LOOP = {
        { 0, (resurface_end - resurface) / 2, 0, 0 }, {}
    };
    Sample resurface_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        resurface_end - resurface,
        resurface,
        &resurface_sample_LOOP,
        NULL
    };
    SoundEffect resurface_sfx = {
        { &resurface_sample, 1.0f },
    };

    // Jump/Bottle 1
    AdpcmLoop jump1_sample_LOOP = {
        { 0, (jump1_end - jump1) / 2, 0, 0 }, {}
    };
    Sample jump1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        jump1_end - jump1,
        jump1,
        &jump1_sample_LOOP,
        NULL
    };
    SoundEffect jump1_sfx = {
        { &jump1_sample, 1.0f },
    };

    // Jump/Bottle 2
    AdpcmLoop jump2_sample_LOOP = {
        { 0, (jump2_end - jump2) / 2, 0, 0 }, {}
    };
    Sample jump2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        jump2_end - jump2,
        jump2,
        &jump2_sample_LOOP,
        NULL
    };
    SoundEffect jump2_sfx = {
        { &jump2_sample, 1.0f },
    };

    // Charge Up
    AdpcmLoop chargeup_sample_LOOP = {
        { 0, (chargeup_end - chargeup) / 2, 0, 0 }, {}
    };
    Sample chargeup_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        chargeup_end - chargeup,
        chargeup,
        &chargeup_sample_LOOP,
        NULL
    };
    SoundEffect chargeup_sfx = {
        { &chargeup_sample, 1.0f },
    };

    // Din's Fire
    AdpcmLoop dinsfire_sample_LOOP = {
        { 0, (dinsfire_end - dinsfire) / 2, 0, 0 }, {}
    };
    Sample dinsfire_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        dinsfire_end - dinsfire,
        dinsfire,
        &dinsfire_sample_LOOP,
        NULL
    };
    SoundEffect dinsfire_sfx = {
        { &dinsfire_sample, 1.0f },
    };

    // Die 1
    AdpcmLoop die1_sample_LOOP = {
        { 0, (die1_end - die1) / 2, 0, 0 }, {}
    };
    Sample die1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        die1_end - die1,
        die1,
        &die1_sample_LOOP,
        NULL
    };
    SoundEffect die1_sfx = {
        { &die1_sample, 1.0f },
    };

    // Die 2
    AdpcmLoop die2_sample_LOOP = {
        { 0, (die2_end - die2) / 2, 0, 0 }, {}
    };
    Sample die2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        die2_end - die2,
        die2,
        &die2_sample_LOOP,
        NULL
    };
    SoundEffect die2_sfx = {
        { &die2_sample, 1.0f },
    };

    // Die 3
    AdpcmLoop die3_sample_LOOP = {
        { 0, (die3_end - die3) / 2, 0, 0 }, {}
    };
    Sample die3_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        die3_end - die3,
        die3,
        &die3_sample_LOOP,
        NULL
    };
    SoundEffect die3_sfx = {
        { &die3_sample, 1.0f },
    };

    // Shivering
    AdpcmLoop shivering_sample_LOOP = {
        { 0, (shivering_end - shivering) / 2, 0, 0 }, {}
    };
    Sample shivering_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        shivering_end - shivering,
        shivering,
        &shivering_sample_LOOP,
        NULL
    };
    SoundEffect shivering_sfx = {
        { &shivering_sample, 1.0f },
    };

    // Sneeze
    AdpcmLoop sneeze_sample_LOOP = {
        { 0, (sneeze_end - sneeze) / 2, 0, 0 }, {}
    };
    Sample sneeze_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        sneeze_end - sneeze,
        sneeze,
        &sneeze_sample_LOOP,
        NULL
    };
    SoundEffect sneeze_sfx = {
        { &sneeze_sample, 1.0f },
    };

    // Moan from heat
    AdpcmLoop moanfromheat_sample_LOOP = {
        { 0, (moanfromheat_end - moanfromheat) / 2, 0, 0 }, {}
    };
    Sample moanfromheat_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        moanfromheat_end - moanfromheat,
        moanfromheat,
        &moanfromheat_sample_LOOP,
        NULL
    };
    SoundEffect moanfromheat_sfx = {
        { &moanfromheat_sample, 1.0f },
    };

    // Wiping sweat
    AdpcmLoop wipingsweat_sample_LOOP = {
        { 0, (wipingsweat_end - wipingsweat) / 2, 0, 0 }, {}
    };
    Sample wipingsweat_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        wipingsweat_end - wipingsweat,
        wipingsweat,
        &wipingsweat_sample_LOOP,
        NULL
    };
    SoundEffect wipingsweat_sfx = {
        { &wipingsweat_sample, 1.0f },
    };

    // Yawn 1
    AdpcmLoop yawn1_sample_LOOP = {
        { 0, (yawn1_end - yawn1) / 2, 0, 0 }, {}
    };
    Sample yawn1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        yawn1_end - yawn1,
        yawn1,
        &yawn1_sample_LOOP,
        NULL
    };
    SoundEffect yawn1_sfx = {
        { &yawn1_sample, 1.0f },
    };

    // Yawn 2
    AdpcmLoop yawn2_sample_LOOP = {
        { 0, (yawn2_end - yawn2) / 2, 0, 0 }, {}
    };
    Sample yawn2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        yawn2_end - yawn2,
        yawn2,
        &yawn2_sample_LOOP,
        NULL
    };
    SoundEffect yawn2_sfx = {
        { &yawn2_sample, 1.0f },
    };

    // Yawn 3
    AdpcmLoop yawn3_sample_LOOP = {
        { 0, (yawn3_end - yawn3) / 2, 0, 0 }, {}
    };
    Sample yawn3_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        yawn3_end - yawn3,
        yawn3,
        &yawn3_sample_LOOP,
        NULL
    };
    SoundEffect yawn3_sfx = {
        { &yawn3_sample, 1.0f },
    };

    // Heavy gasp
    AdpcmLoop heavygasp_sample_LOOP = {
        { 0, (heavygasp_end - heavygasp) / 2, 0, 0 }, {}
    };
    Sample heavygasp_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        heavygasp_end - heavygasp,
        heavygasp,
        &heavygasp_sample_LOOP,
        NULL
    };
    SoundEffect heavygasp_sfx = {
        { &heavygasp_sample, 1.0f },
    };

    // Epona/Hookshot 1
    AdpcmLoop eponahookshot1_sample_LOOP = {
        { 0, (eponahookshot1_end - eponahookshot1) / 2, 0, 0 }, {}
    };
    Sample eponahookshot1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        eponahookshot1_end - eponahookshot1,
        eponahookshot1,
        &eponahookshot1_sample_LOOP,
        NULL
    };
    SoundEffect eponahookshot1_sfx = {
        { &eponahookshot1_sample, 1.0f },
    };

    // Epona/Hookshot 2
    AdpcmLoop eponahookshot2_sample_LOOP = {
        { 0, (eponahookshot2_end - eponahookshot2) / 2, 0, 0 }, {}
    };
    Sample eponahookshot2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        eponahookshot2_end - eponahookshot2,
        eponahookshot2,
        &eponahookshot2_sample_LOOP,
        NULL
    };
    SoundEffect eponahookshot2_sfx = {
        { &eponahookshot2_sample, 1.0f },
    };

    // Replace Link's voices with new SoundEffect objects, also changing the tuning for a few of them so
    // that they play at different pitches.
    // Choked
    AudioApi_ReplaceSoundEffect(0x00, 20, &choke_sfx);

    // Attack
    AudioApi_ReplaceSoundEffect(0x00, 28, &attack1_sfx);        // 001C
    AudioApi_ReplaceSoundEffect(0x00, 29, &attack2_sfx);        // 001D
    AudioApi_ReplaceSoundEffect(0x00, 30, &attack3_sfx);        // 001E
    AudioApi_ReplaceSoundEffect(0x00, 31, &attack4_sfx);        // 001F

    // Strong Attack
    AudioApi_ReplaceSoundEffect(0x00, 32, &strongattack1_sfx);  // 0020
    AudioApi_ReplaceSoundEffect(0x00, 33, &strongattack2_sfx);  // 0021

    // Ledge Dangle
    AudioApi_ReplaceSoundEffect(0x00, 34, &ledgedangle1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 50, &ledgedangle2_sfx);

    // Ledgeclimb
    AudioApi_ReplaceSoundEffect(0x00, 35, &ledgeclimb1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 36, &ledgeclimb2_sfx);

    // Hurt
    AudioApi_ReplaceSoundEffect(0x00, 37, &hurt1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 38, &hurt2_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 39, &hurt3_sfx);

    // Knocked Down
    AudioApi_ReplaceSoundEffect(0x00, 40, &knockeddown1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 41, &knockeddown2_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 42, &knockeddown3_sfx);

    // Fall
    AudioApi_ReplaceSoundEffect(0x00, 43, &fall1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 44, &fall2_sfx);

    // Gasp 
    AudioApi_ReplaceSoundEffect(0x00, 45, &gasp1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 46, &gasp2_sfx);

    // Low Health Breathing
    AudioApi_ReplaceSoundEffect(0x00, 47, &lowhealth1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 48, &lowhealth2_sfx);

    // Hurt Landing
    AudioApi_ReplaceSoundEffect(0x00, 49, &hurtlanding_sfx);

    // Resurface
    AudioApi_ReplaceSoundEffect(0x00, 52, &resurface_sfx);

    // Jump / Bottle
    AudioApi_ReplaceSoundEffect(0x00, 53, &jump1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 54, &jump2_sfx);

    // Charge Up
    AudioApi_ReplaceSoundEffect(0x00, 62, &chargeup_sfx);

    // Din's Fire
    AudioApi_ReplaceSoundEffect(0x00, 63, &dinsfire_sfx);

    // Die
    AudioApi_ReplaceSoundEffect(0x00, 64, &choke_sfx);      // Can change to die1_sfx
    AudioApi_ReplaceSoundEffect(0x00, 65, &die2_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 66, &die3_sfx);

    // Shivering
    AudioApi_ReplaceSoundEffect(0x00, 67, &shivering_sfx);

    // Sneeze
    AudioApi_ReplaceSoundEffect(0x00, 69, &sneeze_sfx);

    // Moan from heat
    AudioApi_ReplaceSoundEffect(0x00, 71, &moanfromheat_sfx);

    // Awakening Grunt
    AudioApi_ReplaceSoundEffect(0x00, 72, &yawn1_sfx);

    // Wiping sweat
    AudioApi_ReplaceSoundEffect(0x00, 73, &wipingsweat_sfx);

    // Yawn
    AudioApi_ReplaceSoundEffect(0x00, 74, &yawn1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 75, &yawn2_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 76, &yawn3_sfx);

    // Heavy gasp
    AudioApi_ReplaceSoundEffect(0x00, 137, &heavygasp_sfx);

    // Epona / Hookshot
    AudioApi_ReplaceSoundEffect(0x00, 138, &eponahookshot1_sfx);
    AudioApi_ReplaceSoundEffect(0x00, 139, &eponahookshot2_sfx);
}

RECOMP_CALLBACK("magemods_audio_api", AudioApi_Init) void my_mod_on_init_drum_test() {

    // Mask Gasp 1
    AdpcmLoop wearmask1_sample_LOOP = {
        { 0, (wearmask1_end - wearmask1) / 2, 0, 0 }, {}
    };
    Sample wearmask1_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        wearmask1_end - wearmask1,
        wearmask1,
        &wearmask1_sample_LOOP,
        NULL
    };
    EnvelopePoint wearmask1_env[] = {
        ENVELOPE_POINT(    2, 32700),
        ENVELOPE_POINT(  298, 32700),
        ENVELOPE_POINT(32700, 29430),
        ENVELOPE_HANG(),
    };
    Drum wearmask1 = {
        211, 64, false,
        { &wearmask1_sample, 1.0f },
        wearmask1_env,
    };

    // Mask Gasp 2
    AdpcmLoop wearmask2_sample_LOOP = {
        { 0, (wearmask2_end - wearmask2) / 2, 0, 0 }, {}
    };
    Sample wearmask2_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        wearmask2_end - wearmask2,
        wearmask2,
        &wearmask2_sample_LOOP,
        NULL
    };
    EnvelopePoint wearmask2_env[] = {
        ENVELOPE_POINT(    2, 32700),
        ENVELOPE_POINT(  298, 32700),
        ENVELOPE_POINT(32700, 29430),
        ENVELOPE_HANG(),
    };
    Drum wearmask2 = {
        211, 64, false,
        { &wearmask2_sample, 1.0f },
        wearmask2_env,
    };

    // Mask Gasp 3
    AdpcmLoop wearmask3_sample_LOOP = {
        { 0, (wearmask3_end - wearmask3) / 2, 0, 0 }, {}
    };
    Sample wearmask3_sample = {
        0, CODEC_S16, MEDIUM_CART, false, false,
        wearmask3_end - wearmask3,
        wearmask3,
        &wearmask3_sample_LOOP,
        NULL
    };
    EnvelopePoint wearmask3_env[] = {
        ENVELOPE_POINT(    2, 32700),
        ENVELOPE_POINT(  298, 32700),
        ENVELOPE_POINT(32700, 29430),
        ENVELOPE_HANG(),
    };
    Drum wearmask3 = {
        211, 64, false,
        { &wearmask3_sample, 1.0f },
        wearmask3_env,
    };

    // In soundfont 0x03, the timpani is drumId 33-63.
    // Replace each entry with tuning ranging from 0.45f - 2.0f
    wearmask1.tunedSample.tuning = 0.8f;
    AudioApi_ReplaceDrum(0x00, 4, &wearmask1);

    wearmask2.tunedSample.tuning = 0.8f;
    AudioApi_ReplaceDrum(0x00, 5, &wearmask2);

    wearmask3.tunedSample.tuning = 0.8f;
    AudioApi_ReplaceDrum(0x00, 6, &wearmask3);

}