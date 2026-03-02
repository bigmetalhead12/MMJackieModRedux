# Majora's Mask Recomp: Jackie Quilt Mod Redux

This mod replaces Link (human form) with my custom model of Jackie Quilt, a character from Eternal Return.

This Jackie model replaces only the player model of Link's human form. The model was designed to be (somewhat) accurate to the OoT/MM model aesthetic. The model also uses the Adult Link skeleton, with none of the animations altered.

The arguably most notable feature of this mod is the selectable, alternative hairstyle: a ponytail hairstyle with **real-time physics**. Along with this feature, the mod also includes the HD texture option, full voice replacement, and minor text replacements.

# Installation
To install this mod, follow the instructions:
1. Download the NRM file
2. Open MM Recomp and select the Mods page
3. Click and drag the downloaded NRM file into the game.

# Toggleable Features
This mod gives you control over which features to activate or deactivate. After installing the NRM file into your Majora's Mask Recomp, you can toggle on/off the following features before starting the game:
*  **HD Textures** - This toggle lets you select between the *default low-res* textures or *HD textures*.
*  **Alternative Hairstyle** - This toggle lets you select between the *default hairstyle* or the *ponytail hairstyle*. Keep note that the ponytail hairstyle has real-time physics while the default hairstyle does not.
*  **Alternative Text** - This toggle lets you select between the *game's default texts* or the *slightly modified texts*. The slightly modified texts only changed the pronouns, titles, and few lines of text to match Jackie's character.

# Mod Construction
This section includes the explanation of how this mod was constructed. I divided up the info to explain each key component of this mod separately. If you are new to MM modding and want to use this mod as learning material to make your own mod, or if you just want to know how this mod was made, feel free to read through this section.

This mod contains the following components: 
* Model
* Audio (voice) replacement
* Text replacement
* Real-time physics (for ponytail).

## Mod Template
This mod was created using [Wiseguy's MMRecompModTemplate](https://github.com/Zelda64Recomp/MMRecompModTemplate).

If you are new to MM Recomp Modding, it is crucial you understand how to work with this template before doing anything else. If you are able to build an NRM from this template on your machine, you are ready to mod the game.

## Model (under construction)
Model modding consists of two parts: making a useable model and replacing the model in the game.

(possible link to separate MD here)

Creating a model for MM Recomp should be a similar process to creating a model for Ship of Harkinian. As such, this video should provide the important steps for making your own model:

[![Modeling](https://img.youtube.com/vi/6Ji_12w5B1M/0.jpg)](https://www.youtube.com/watch?v=6Ji_12w5B1M)

For the Fast64 plugin, be sure to get the MM Branch version of Fast64 from [here](https://github.com/Yanis002/fast64/tree/mm_dev).

## Audio Replacement (under construction)
Link's voice was replaced entirely with [Magemods' MM Audio API](https://github.com/magemods/mm-audio-api).

Audio replacement consists of two parts: making audio files and replacing the audio in the game using the API.

(possible link to separate MD here)

## Text Replacement (under construction)
The in-game text was slightly modified using [LT_Schmiddy's MM EZ Text Replacement](https://github.com/LT-Schmiddy/mm-ez-text-replacement-utils).

(possible link to separate MD here)

## Real-Time Physics for the Ponytail
The ponytail's real-time physics was designed with **verlet integration**. Verlet integration is a computationally cheap physics method that is easy to implement to any game engine. The ponytail serves as a demonstration of this real-time physics in action.

I explain the process of how I coded verlet integration into Majora's Mask Recomp in more detail in this link:
[Verlet Integration — How the Ponytail Physics Work](VerletIntegration.md)

# What's Missing
* Elegy of Emptiness Statue
* Collision and offset fix for ponytail rotations

# Acknowledgements and Thanks
This project could not have been possible without the help of the MM Recomp and Zelda Decompilation communities. I would also like to specifically thank the following people:
- To **LT_Schmiddy**, for creating the EZTR API, providing coding help, and the continued support throughout the development of this project.
- To **Lime**, for suggesting Verlet Integration for this project, providing coding help, and the words of encouragement.
- To **ProxySaw**, for creating the CustomActor library and providing coding help.
- To **Magemods**, for creating the Audio API and making custom voice clips possible.
- To **kentonm**, for suggesting Verlet Integration, providing coding help, and helping me understand the inner mechanisms of Majora's Mask.
- To **Reonu** and **Bird of Light**, for providing modeling help.
- To **Neirn**, **danielryb**, **Fig**, **Dragorn421**, and **MelonSpeedruns** for providing various coding help.
- To **PxExYxTxOxN**, for the [guide on creating player models](https://www.youtube.com/watch?v=6Ji_12w5B1M)
- To **grubbz** and **El Regolante**, for sharing my work in their Discord community's Monthly Art Gallery
- To **Pikuma**, for the [guide on Verlet Integration](https://www.youtube.com/watch?v=-GWTDhOQU6M)

# Feedback
If you notice any mistakes or omissions in the documentation, please leave a comment to report the issues. Thanks! 👍
