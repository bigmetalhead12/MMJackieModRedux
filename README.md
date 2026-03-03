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

# Mod Template
This mod was created using [Wiseguy's MMRecompModTemplate](https://github.com/Zelda64Recomp/MMRecompModTemplate).

If you are new to MM Recomp Modding, it is crucial you understand how to work with this template before doing anything else. If you are able to build an NRM from this template on your machine, you are ready to mod the game.

# Further Reading
This section includes the explanation of how this mod was constructed. I divided up the info to explain each key component of this mod separately. If you are new to MM modding and want to use this mod as learning material to make your own mod, or if you just want to know how this mod was made, feel free to read through this section.

This mod contains the following components: 
* [Model Modding](docs/MODEL.md)
* Audio (voice) replacement
* Text replacement
* [Real-time physics (for ponytail)](docs/VERLETINTEGRATION.md)

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
