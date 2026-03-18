# Introduction
This documentation serves as a sort of a condensed log on how I designed the ponytail with real-time physics. In here, I try to describe in detail on what I learned before jumping into this project to make this mod a reality, what approaches I took to complete this mod, and how I coded this physics system into this mod.

I also hope that other readers who are interested can learn from this document and make their own physics-based mods. Verlet integration is surprisingly easy to understand, and I have no doubt many will find this system useful for their own projects.

# Prerequisites
Before jumping into this project, I had to get used to three things:

* Verlet Integration
* Blender
* [ProxySaw's CustomActor API Library](https://thunderstore.io/c/zelda-64-recompiled/p/ProxyMM/CustomActor/)

## Verlet Integration
To implement real-time physics to Jackie's ponytail model, I used the Verlet Integration.

Verlet Integration is an effective way to handle real-time physics based on a target object's positions. More specifically, Verlet Integration simulates physics by using a target object's previous and current position to calculate the next position. This method is particularly useful because it is easy to understand and relatively simple to integrate.

A good explanation of this concept and a simple code demonstration are offered in the linked video. I strongly suggest watching this if you are interested in learning more about Verlet Integration.

[![Verlet Integration](https://img.youtube.com/vi/-GWTDhOQU6M/0.jpg)](https://www.youtube.com/watch?v=-GWTDhOQU6M)

The physics itself is easy to design. The challenge is implementing this system into a game that was not designed with Verlet Integration in mind. For this reason, I had to customize some parts of Verlet Integration to make this system properly work in MM Recomp.

## Blender
If you are used to creating custom player models for Majora's Mask, then there isn't too much else to know about Blender for this project.

## ProxySaw's CustomActor API Library
ProxySaw developed a really useful library that allows people to add their own actors into Majora's Mask Recomp. It is used as a foundation for this mod.

You can download the library [here](https://thunderstore.io/c/zelda-64-recompiled/p/ProxyMM/CustomActor/). If you're curious, you can view the source code [here](https://github.com/garrettjoecox/ProxyMM_RecompMods/tree/main/packages/CustomActor)

# Designing Verlet Integration in Majora's Mask Recomp
## Parameters and Behaviors of Verlet Integration
Verlet integration is all about positions. This real-time physics system is essentially an equation that figures out a target object's next position based on the following:
* Previous Position of the Target Object
* Current Position of the Target Object
* Force (accleration) being applied to Target Object
* The difference in time between the previous frame and the current frame.

$$
x_{newPos} = x_{currPos} + (x_{currPos} - x_{prevPos}) + \vec{a} \Delta t^2
$$

To review Verlet Integration, there are two essential components that need to be used: **points** and **bones**. 

A target object has at least one **point**. A point needs to be either *pinned* or *unpinned*. A *pinned* point means that the point will stay in spot on the canvas it is being drawn on. An *unpinned* point means that the point will not stay in spot and will be affected by a force (like gravity).

A target object's **bone** refers to a set of two points. Bones are designed to always maintain the initially-set distance between its two assigned points (basically the length of the bone). Essentially, the bone constrains two points to maintain their distance from each other. This constraint is what causes bones of an object to move bsaed on real-time physics, especially when one of its point is pinned. For example, if one point in a given bone is pinned while the other is not, and the force of gravity is being applied to the bone, the bone will be hanging from its pinned point while the other, unpinned point will be hanging below the pinned point.

## Making the Ponytail Model (3D Modeling)
So, in my case, I want to create a ponytail that has real-time physics. Using Blender, I created the model (separate from the main player model) along with a unique skeleton for it. When doing this, I made sure to meet the following requirements for my model:
* The root limb for the ponytail's skeleton must be located at the same position as the limb I want to attach the ponytail to (in my case, Adult Link's head limb position).
* All the limbs are properly formatted.
* Weights are all properly applied to their respective vertex group.

When positioning the root limb of the ponytail, I did the following:
1. Set the origin point of the ponytail model to be the same as Link's skeleton
2. Copy and paste the XYZ position values of Link's skeleton's head limb to the ponytail's root limb

All limbs need to be formatted properly. To do this, I used this following script on the ponytail's skeleton.

```python
import bpy
import mathutils

target_name = "gPonytailSkel"    # Replace with name of your model's skeleton
1119
obj = bpy.data.objects.get(target_name)
bpy.context.view_layer.objects.active = obj
obj.select_set(True)
bpy.ops.object.mode_set(mode='EDIT')

for bone in obj.data.edit_bones:
    head = bone.head.copy()
    bone.tail = head + mathutils.Vector((0, 0, 0.3))
    bone.roll = 0.0
```
(Credit to kentonm for writing this script)

I ensured that all the limbs' weights were properly done. 

When finished with the model, be sure to export it with the proper settings on the MM Build of Fast64.

# Designing Verlet Integration for MM Recomp
## Initial Concept
Conceptualizing how Verlet Integration would work on the ponytail in this game was easy.

First, considering that the ponytail model has 5 limbs (where each limb would be considered a *point* by Verlet Integration):
* Root
* Limb1
* Limb2
* Limb3
* Limb4
 
the ponytail would have 4 bones:

* RootLimb1 (bone with root limb and limb1)
* Limb1Limb2
* Limb2Limb3
* Limb3Limb4
  
The ponytail's root limb and first hanging limb (limb1) would be pinned to the model. The two would always move consistently relative to Jackie's head limb's movement. They would not be affected by Verlet Integration. Consequently RootLimb1 bone is always going to remain intact relative to Jackie's head limb's position and rotation.

Second, the ponytail's subsequent limbs (limbs2-4) would be unpinned. That means any bone involving any of these limbs would be affected by Verlet Integration. RootLimb1's limbs are both pinned, so this bone will not move with real-time physics. However, Limb1Limb2 has one unpinned limb: limb2. So, Limb1Limb2 is going to behave in a way where the bone is hanging by limb1, with limb2 dangling below. Subsequent bones (like Limb2Limb3) is going to flail based on the player's movement and the parent bone's rotation (i.e. Limb2Limb3 is going to move and rotate based on the position and rotation of Limb1Limb2).

Remember, Verlet Integration is set up to use a target object's previous position, current position, Force/acceleration, and change of time to calculate the object's real-time physics. MM Recomp is actually able to allow us to use these values directly. That means implementing Verlet Integration should be good as is, right?

Wrong.

## Issue with the Initial Concept
The issue lies in how pinned points are originally implemented. Pinned points are meant to be pinned to a specific location in a given world. But, my pointail's pinned points are meant to be pinned to *Jackie's head limb*, not the *world*. This specific nature of Verlet Integration's implementation would conflict with how the ponytail needs to be added into the game. If we look back at the equation for Verlet Integratoin:

$$
x_{newPos} = x_{currPos} + (x_{currPos} - x_{prevPos}) + \vec{a} \Delta t^2
$$

the change of position and acceleration over a change of time are what calculates the real-time physics. However, when the points are pinned to Jackie's head limb when she's moving forward, and her velocity is not changing (no forward acceleration), then that means *there is no change of position and the acceleration is 0*. This means the ponytail, when Jackie is running in any direction with at a constant velocity, will actually *set down due to gravity, the only acceleration being applied to the ponytail*.

This means the Verlet Integration equation needs to be adjusted to consider Jackie's change of position also. This means that, ironically, *Jackie's global velocity* will have to be factored into this equation. Specifically, the opposite velocity of Jackie should be factored into the Verlet Integration.

So, the equation will look something like this:

$$
x_{newPos} = x_{currPos} + ((x_{currPos} - x_{prevPos}) - \vec{v}_{playerVelocity}) + \vec{a} \Delta t^2
$$

Additionally, Jackie's head is obviously not going to just be facing in one direction in game. This means that the ponytail needs to be flowing in real-time based on the rotation of Jackie's head limb also.

## Adjusted Concept
The behavior of the ponytail is the same: the two limbs (root and limb1) should be pinned while the rest of the limbs need to be unpinned. The bones are to set to always maintain the distance between their respective limbs.

The change of time is just worth a frame between two frames. Basically, *dt* is 1. It should have no special impact to Verlet Integration in this game.

The ponytail itself should always have the force applied to its bones. This force includes gravity and any other acceleration caused by Jackie's movement. For instance, when Jackie starts running forward, this will have a forward acceleration, which will eventually go back to 0 once Jackie's velocity becomes constant.

When Jackie moves, however, her velocity needs to be applied directly to the Verlet Integration calculation. This way, the limbs would know that even when there is no acceleration based on movement, the player is still moving and the ponytail limbs should not set down.

And after these calculations are done, the bone's length will ensure that the points in their respective bones are moved to respect the original set length of each bone.

So, the parameters are as follows:
* Previous Position of the Target Object's limb
* Current Position of the Target Object's limb
* Force (accleration) being applied to Target Object's limb
* Change in time (*dt* which is just 1)
* Player's velocity
* Player's head limb's rotation

# Code
For the actual implementation of Verlet Integration on the ponytail, I have done the following
* Setting up the Fast64-exported Ponytail in code
* Using the Custom Actor for the Ponytail
* Applying Verlet Integration on the Ponytail

## Setting up the Ponytail
Here is how I set up the ponytail in Blender before exporting through Fast64.

<img width="564" height="440" alt="image" src="https://github.com/user-attachments/assets/2f1b4ffa-9128-4d72-b669-fb1759bdeff8" />

In this model, the ponytail's skeleton in Blender is called `gPonytailSkel`. In this skeleton, the limb starts from `gPonytailRootLimb`, goes to `gPonytailLimb1`, and then ends at gPonytailLimb5`.

In this mod, I made matching verlet integration **points** based on the ponytail skeleton (which is called `PhysLimb` in the [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/verlet_physics.h#L34-L44)):

With PhysLimb, I set up the following PhysLimbs in the [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.h#L55-L69):

```
gPonytailRootLimb => PhysLimb ponytailRootLimb     (pinned)
gPonytailLimb1    => PhysLimb ponytailLimb1        (pinned)
gPonytailLimb2    => PhysLimb ponytailLimb2        (unpinned)
gPonytailLimb3    => PhysLimb ponytailLimb3        (unpinned)
gPonytailLimb4    => PhysLimb ponytailLimb4        (unpinned)
gPonytailLimb5    => PhysLimb ponytailLimb5        (unpinned)
```

I set up verlet integration **bones** to be between **points** (which is called `PhysBone` in the [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/verlet_physics.h#L46-L50)).

I set up the PhysBones like this in the [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.h#L71-L92):

```
PhysBone ponytailRootLimbLimb1     (Between RootLimb and Limb1)
PhysBone ponytailLimb1Limb2        (Between Limb1 and Limb2)
PhysBone ponytailLimb2Limb3        (Between Limb2 and Limb3)
PhysBone ponytailLimb3Limb4        (Between Limb3 and Limb4)
PhysBone ponytailLimb4Limb5        (Between Limb4 and Limb5)
```

`ponytailRootLimb` and `ponytailLimb1` are pinned, so those points and `ponytailRootLimbLimb1` won't be affected by physics and will be following Jackie's head movement.

Meanwhile, the rest of the points and the rest of the bones in the ponytail would be affected by motion such as gravity and players' velocity.

## Using the Custom Actor
Using Proxy's Custom Actor API, I set up the Ponytail actor that would fit with Majora's Mask's actor system.

### Registering the Ponytail as an Actor
There are four lifecycle functions to keep track of when using this API's custom actor system, which has been implemented in [`z_ponytail.c`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L49-L56).

| Function | When it Runs | Purpose |
| :--- | :--- | :--- |
| `Ponytail_Init` | Once when the ponytail model spawns | Set up ponytail's skeleton and initialize phys limbs and bones (position and rotation) |
| `Ponytail_Update` | Every in-game frame | Remove ponytail model if player transforms from human to any other form |
| `Ponytail_Draw` | Every in-game frame | Render the ponytail model's mesh |
| `Ponytail_Destroy` | When ponytail actor is removed | Get rid of ponytail |

To properly register the ponytail as an actor, a profile was set for the ponytail in [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L79-L90):

```c
// Sets profile for ponytail before registering it as actor
ActorProfile Ponytail_Profile = {
    ACTOR_ID_MAX,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(Ponytail),
    Ponytail_Init,
    Ponytail_Destroy,
    Ponytail_Update,
    Ponytail_Draw,
};
```

Afterward, the profile is registered as an actor through this [code](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L92-L97):

```c
s16 CUSTOM_ACTOR_PONYTAIL = ACTOR_ID_MAX;

// Register ponytail as custom actor
RECOMP_CALLBACK("*", recomp_on_init) void Ponytail_OnRecompInit() {
    CUSTOM_ACTOR_PONYTAIL = CustomActor_Register(&Ponytail_Profile);
}
```

### Initializing the Ponytail
With the ponytail actor set, it's drawn into the game by `RECOMP_HOOK`ing to `Player_init` like [this](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L104-L113):

```c
Ponytail* gPlayerPonytail = NULL;
PlayState* gPlayStatePonytail = NULL;

RECOMP_HOOK("Player_Init") void on_player_init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("change_hairstyle") && gPlayerPonytail == NULL) {
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, CUSTOM_ACTOR_PONYTAIL, 
                                    -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0, 
                                    0, 0);
    }
}
```

The ponytail is properly initialized through the [`Ponytail_Init`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L167-L210) function.

Several tasks are done in this function. First, the ponytail's limbs' position values are directly copied from [`gPonytailSkel.c`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/gPonytailSkel.c#L252-L257). These position values are then copied into the `PhysLimbs`. Second, the ponytail's `skelAnime` is set up, which would have its own `jointTable`. The `jointTable` has the rotation values of every limb in the skeleton; meaning, the ponytail's `jointTable` is to be manipulated to handle the ponytail points' real-time rotation every frame. Third, [`Verlet_InitPhysPlayer`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/verlet_physics.c#L30-L34) is run to take note of Jackie's current rotation and velocity in the map. Finally, [`Ponytail_SetDefaultBodyPartsPos`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L115-L165) is run to initialize the ponytail's verlet integration.

### Destroying the Ponytail
The ponytail is destroyed using the following implementation of [`Ponytail_destroy`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L218-L220).

### Updating the Ponytail
Every frame, the ponytail's limbs' rotations and positions are updated based on Jackie and verlet intergration.

When updating, it is important to know the location and rotation value of Jackie's head limb. This information is found by `RECOMP_HOOK`ing `Player_PostLimbDrawGameplay`, which was done through [this method](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L233-L252). In this method, I pointed to `PLAYER_LIMB_HEAD` when that limb is being drawn in `Player_PostLimbDrawGameplay` and grab its rotation and position values.

At this part of the code, I also set up [`Ponytail_UpdateBodyPartsPos`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L322-L383), which calculates the position values of every limb in the ponytail skeleton in the frame

With these position values in mind, [`Ponytail_RotateJoints`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L322C6-L383) rotates each limb in the ponytail skeleton to match the calculated position values of every limb. 

Finally, I defined [`Ponytail_update`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L386-L396) so that it removes the ponytail's mesh whenever the player transforms from human to non-human form.

### Drawing the Ponytail
The draw functions run these verlet integration-related functions to draw the ponytail properly. This involves calculating the net force applied to the ponytail based on Jackie's current position and movement and using them to calculate the rotation values for every limb.

This is done through `RECOMP_HOOK`ing `Player_Draw` with the following [implementation](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L404-L424):

Finally, [`Ponytail_Draw`](https://github.com/bigmetalhead12/MMJackieModRedux/blob/main/src/z_ponytail.c#L427-L452) is implemented. It is also set up so that the ponytail blinks the same way as Jackie's model does when she gets damaged or jinxed. When using the function (`func_80122868`) that allows this, however, other random actors in the map also start blinking the same way as Jackie does. To prevent this behavior, the following code snippet is applied:

```c
// Reset fog to scene's default values after drawing so that other actors don't get affected
if (player->invincibilityTimer > 0 || gSaveContext.jinxTimer != 0) {
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
}
```
