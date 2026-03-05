/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

// Default Header Files
#include "modding.h"
#include "global.h"
#include "ultra64.h"

// Model Header Files
#include "gJackieElegy.h"

// Elegy header file
#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"

// Elegy Statue forms
#define HUMAN_FORM  0
#define GORON_FORM  1
#define ZORA_FORM   2
#define DEKU_FORM   3
#define FD_FORM     4


/***********************************************************************

	Jackie Elegy Statue

***********************************************************************/

static EnTorch2* gEnTorch2 = NULL;
static PlayState* gEnTorch2Play = NULL;
static u8 actualAlpha = 0;

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

// EnTorch2_Draw is run on the default human/FD statue with alpha set to 0 at this point

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