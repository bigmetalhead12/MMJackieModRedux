# Majora's Mask Recomp: Jackie Quilt Mod Redux

This mod replaces Link with my custom model of Jackie Quilt, a character from Eternal Return.

Made with the MM build of the Fast64 Blender plugin, this Jackie model replaces only the player model of Link's human form. The model was designed to be (somewhat) accurate to the OoT/MM model aesthetic. The model also uses the Adult Link skeleton, with none of the animations altered.

This mod features real-time physics in the alternative, selectable ponytail hairstyle. The ponytail moves purely based on Jackie's movement; no animation is involved in its behavior. The ponytail was designed with Verlet Integration.

# Current Features
* Complete model replacement
* Alternative ponytail hairstyle with real-time physics
* Complete voice replacement
* Alternative texts written with EZTR

# Installation
To install...
1. Download the NRM file
2. Open MM Recomp and select the Mods page
3. Click and drag the downloaded NRM file into the game.

# Additional Information
## Modeling
Creating a model for MM Recomp should be a similar process to creating a model for Ship of Harkinian. As such, this video should provide the important steps for making your own model:

[![Modeling](https://img.youtube.com/vi/6Ji_12w5B1M/0.jpg)](https://www.youtube.com/watch?v=6Ji_12w5B1M)

For the Fast64 plugin, be sure to get the MM Branch version of Fast64 from [here](https://github.com/Yanis002/fast64/tree/mm_dev).

## Verlet Integration
The implementation of verlet integration is arguably this mod's key feature. I explain the process of how I designed this sytem in this link:
[Verlet Integration — How the Ponytail Physics Work](VerletIntegration.md)

# Planned Features
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
