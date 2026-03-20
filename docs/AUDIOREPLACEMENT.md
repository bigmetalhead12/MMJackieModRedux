# Audio Replacement
## Preface
I want to give big thanks to [Magemods](https://github.com/magemods) for his [MM Audio API](https://github.com/magemods/mm-audio-api). Audio replacement in Majora's Mask: Recomp has been an aspect of MM modding that was so difficult that many talented and skilled modders started and gave up on this altogether. Magemod's contribution with this API is arguably one of the most noteworthy milestone of the MM modding community, and modding MM's audio would not have been this easy (let alone possible) without it.

## Introduction
In this doc, I describe my process for changing Human Link's voice to Jackie's voice. There are actually two versions of the Audio API you can use:
*  Magemods' [Original MM Audio API](https://github.com/magemods/mm-audio-api)
*  [Ben](https://github.com/B3n-17)'s [Updated MM Audio API](https://thunderstore.io/c/zelda-64-recompiled/p/MAA/Audio_API/)

There is a reason two versions of the API exists. Although Magemods developed the original audio API, he never completed the API to his liking and released it on thunderstore before going absent. After his prolonged absence, Ben picked up where Magemods left off and continued the project.

In this project, I specifically used Magemod's original API. The pro of this approach is that my audio replacement mod **can work with either of the two APIs** loaded in the mod folder of MM Recomp (meaning, even just having Magemod's original Audio API files in MM:Recomp's mod folder would make this Jackie mod fully functional). However, the con of this approach is that this project does not fully utilize the new features Ben implemented in his version of the audio API. For this reason, my method, although fully functional with honestly no noticeable drawbacks, may be more inconvenient to implement than it should be.

# Prerequisites
Before modding the audio, you would need...
1. An audio editing tool (e.g. Audacity)
2. [ffmpeg](https://www.ffmpeg.org/)
3. Decompiled MM (to know what in-game audio clips to replace)

# Finding Audio Clips to Replace
When replacing audio clips in the game with your own, you need to know which audio clip in the game to replace. To do this, you need to know the audio clips' ID, which can be found in your decompiled MM. To find the audio clips' IDs, go to your decompiled MM directory and open `build/n64-us/assets/audio/soundfonts/Soundfont_0.c`. Afterward, search for `SF0_SFX_LIST[]` in the file. You should see something that looks like this:

```c
NO_REORDER SECTION_DATA ALIGNED(16) SoundEffect SF0_SFX_LIST[] = {
    { { &SF0_AdultLinkAttack1_HEADER, 0.6250000000000000000000f } },
    { { &SF0_AdultLinkAttack2_HEADER, 0.6250000000000000000000f } },
    { { &SF0_AdultLinkAttack3_HEADER, 0.6250000000000000000000f } },
    { { &SF0_AdultLinkAttack4_HEADER, 0.6250000000000000000000f } },
    // ...
```

This is the list of audio clips in the game. In here, `AdultLinkAttack1` has the ID of `0`, since it's the first in the list. `AdultLinkAttack2` has the ID of `1`, and so on.

The uncomfortable part of looking through this list, however, is that a lot of audio clips are labeled as `SAMPLE_X_YYY`, where `X` represents the samplebank number and YYY represents the sample number in the samplebank. This in itself does not explain what each audio clip actually is. For example, the 29th audio clip (or the audio clip of ID `28`) has `SAMPLE_0_193`, which means the sample is in SampleBank_0 and the audio clip's name is "Sample193". You can access this through the directory of the decompiled MM: `extracted\n64-us\assets\audio\samples\SampleBank_0`.

Playing the sound indicates that it is one of the audio clips of Human Link doing a normal attack or roll. Because I want to replace Link's attack audio clip with Jackie's attack audio clip, I would have to remember audio clip ID `28`.

# Creating Audio Files
This document assumes that you know how to use an audio tool of your choice to create audio clips.

First, you have to create a `.WAV` file of your audio clip. When doing this, be sure to have the channels be set to "Mono", the Sample Rate to "48kHz", and Encoding to "Signed 16-bit PCM".

Here is an example of my exporting settings for my first attack audio clip that I used to replace the audio clip ID `28`.

<img width="369" height="317" alt="image" src="https://github.com/user-attachments/assets/a7a99fd8-bc09-4553-81c7-acae5276948c" />

Once this is done, create `.RAW` files out of your `.WAV` files. When doing this, you have to use ffmpeg.

In your powershell/terminal, run the following command:

`ffmpeg -i attack1.wav -f s16be -acodec pcm_s16be -ac 1 -ar 32000 attack1.raw`

The `32000` indicates 32000 Hz sample rate of the audio file. Do this for every audio files.

In my project, I have done this and placed them into this mod's [`/src/jackieVoice`](https://github.com/bigmetalhead12/MMJackieModRedux/tree/main/src/jackieVoice) directory.

# Code
Before moving forward, be sure to set up your include properly. You can do this by copying [this directory from the API source code](https://github.com/magemods/mm-audio-api/tree/main/include/audio_api) and pasting them into your include, like how I did in my mode's [include directory](https://github.com/bigmetalhead12/MMJackieModRedux/tree/main/include/audio_api).

Afterward, put all of your `.RAW` audio clips into your `src` directory. Here is my [example](https://github.com/bigmetalhead12/MMJackieModRedux/tree/main/src/jackieVoice).

Once you complete this, you can move on to replacing the game's audio clips with your audio clips.

First, set up your includes properly like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_sound.c#L11-L14):

```c
#include "global.h"
#include "modding.h"
#include "recomputils.h"
#include "audio_api/all.h"
```

Afterward, `INCBIN` your audio clip into your code. Here is an example of my `attack1.raw` being `INCBIN`ed into my [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_sound.c#L19-L20):

```c
// Attack (regular attack & roll)
INCBIN(attack1, "src/jackieVoice/attack1.raw");
```

Once `INCBIN`ing the audio clip, use `magemods_audio_api` like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_sound.c#L110):

```c
RECOMP_CALLBACK("magemods_audio_api", AudioApi_Init) void my_mod_on_init_attack_test() {
    // ...
}
```

This function is where the audio replacement actually happens. You are to do 2 things in this function:

1. Set up an audio sample the game can recognize using the respective `.RAW` file
2. Replace the game's audio sample with your audio sample

## Setting up Audio Sample
When I set up my `attack1` audio sample, I did the [following](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_sound.c#L126-L139):

```c
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
```

Keep note that `attack1.raw` is the file name, and `_end` indicates the end of the file. For your code, be sure to just replace `attack1` with your `.RAW` file's name to set up your audio sample.

## Replacing Audio Sample
After setting up my `attack1` audio sample, I simply wrote the following [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_sound.c#L747):

```c
AudioApi_ReplaceSoundEffect(0x00, 28, &attack1_sfx);
```

In here, `28` refers to the audio ID `28`, which is the 29th audio from the list of audio clips earlier. `attack1_sfx` is the `SoundEffect` type object that was created out of `attack1` audio sample.

So essentially, whatever audio is in the `28`th slot is being replaced with `attack1_sfx`.

Repeat this for all of your audio clips inside this function to complete your voice modding.

## Note about Mask-Wearing Audio
The audio clips that play when Link wears a transformation mask is considered a type of drum sound effect, oddly enough. This requires a separate function like in this [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_sound.c#L835-L917).

Refer to this when messing with mask-wearing audio clips.
