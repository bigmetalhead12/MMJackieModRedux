# Audio Replacement
## Preface
I want to give big thanks to [Magemods](https://github.com/magemods) for his [MM Audio API](https://github.com/magemods/mm-audio-api). Audio replacement in Majora's Mask: Recomp has been an aspect of MM modding that was so difficult that many talented and skilled modders started and gave up on this altogether. Magemod's contribution with this API is arguably one of the most noteworthy milestone of the MM modding community, and modding MM's audio would not have been this easy (let alone possible) without it.

## Introduction
In this doc, I describe my process for changing Human Link's voice to Jackie's voice. There are actually two versions of the Audio API you can use:
a.  Magemods' [Original MM Audio API](https://github.com/magemods/mm-audio-api)
b.  [Ben](https://github.com/B3n-17)'s [Updated MM Audio API](https://thunderstore.io/c/zelda-64-recompiled/p/MAA/Audio_API/)

There is a reason two versions of the API exists. Although Magemods developed the original audio API, he never completed the API to his liking and released it on thunderstore before going absent. After his prolonged absence, Ben picked up where Magemods left off and continued the project.

In this project, I specifically used Magemod's original API. The pro of this approach is that my audio replacement mod **can work with either of the two APIs** loaded in the mod folder of MM Recomp (meaning, even just having Magemod's original Audio API files in MM:Recomp's mod folder would make this Jackie mod fully functional). However, the con of this approach is that this project does not fully utilize the new features Ben implemented in his version of the audio API. For this reason, my method, although fully functional with honestly no noticeable drawbacks, may be more inconvenient than it should be.

# Prerequisites
Before modding the audio, you would need...
1. An audio editing tool (e.g. Audacity)
2. [ffmpeg](https://www.ffmpeg.org/)
3. Decompiled MM (to know what in-game audio clips to replace)

# Finding Audio Clips to Replace
When replacing audio clips in the game with your own, you need to know which audio clip in the game to replace. To do this, you need to know the audio clips' ID, which can be found in your decompiled MM. To find the audio clips' IDs, go to your decompiled MM directory and open `build/n64-us/assets/audio/soundfonts/Soundfont_0.c`.

# Creating Audio Files
This document assumes that you know how to use an audio tool of your choice to create audio clips
