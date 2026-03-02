# Model Modding
This doc covers two parts of model modding Majora's Mask Recomp. This mod specifically replaced Link's human form model with my custom model. Notably, my custom model used OoT Adult Link's skeleton, so there were additional steps I had to take for this mod.

The two parts of model modding are:
1. Exporting your model
2. Implementing the model into the game through code

# Prerequisite
Before modding MM models, you only need:
1. [Custom MM-branch Fast64](https://github.com/Yanis002/fast64/tree/mm_dev)
2. Blender 3.2+

# Exporting Model
There is a highly informative video that explains step-by-step on how to take a model and export it in a format that Zelda64 likes. Although this video was made for Ship of Harkinian, the steps for setting up your model and exporting it should be *almost* exactly the same.

The only difference is that you will have to use the [Custom MM-branch Fast 64](https://github.com/Yanis002/fast64/tree/mm_dev) instead of the one used in the video.

[![Modeling](https://img.youtube.com/vi/6Ji_12w5B1M/0.jpg)](https://www.youtube.com/watch?v=6Ji_12w5B1M)

For each skeleton you export, you should get a .C file and a .H file. 

For example, I exported Jackie's model + skeleton with the custom Fast64 and got 
* [gJackieSkel.c](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.c)
* [gJackieSkel.h](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gJackieSkel.h)

Both files need to be put into the mod template's "src" directory.

# Implementing the Model
To replace a target model (human Link in this mod's case) with the exported model, a separate .C file needs to be set up. In this mod's case, (jackie_code.c)[https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/Jackie_code.c] was set up.

To properly implement the exported custom model, this file needs to do the following:
1. Setting Link's properties to match your model's intended property
2. Setting up Display Lists
3. Adjusting miscellaneous in-game characteristics

## Link's Properties
If your model is to be the equivalent of Young Link, then Link's properties most likely do not need to be changed. If your model is to be the equivalent of Adult Link, however, then Link's properties will have to be changed to match your model's behavior.

To do this, `PlayerAgeProperties` need to be adjusted to match Adult Link's properties.

(link to code block here)
