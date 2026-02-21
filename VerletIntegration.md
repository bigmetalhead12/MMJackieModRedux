# Introduction
This documentation serves as a sort of a condensed log on how I designed the ponytail with real-time physics. In here, I try to describe in detail on what I learned before jumping into this project to make this mod a reality, what approaches I took to complete this mod, and how I coded this physics system into this mod.

I also hope that other readers who are interested can learn from this document and make their own physics-based mods. Verlet integration is surprisingly easy to understand, and I have no doubt many will find this system useful for their own projects.

# Prerequisites
Before jumping into this project, I had to get used to three things:

* Verlet Integration
* Blender
* [ProxySaw's CustomAPI Library](https://thunderstore.io/c/zelda-64-recompiled/p/ProxyMM/CustomActor/)

## Verlet Integration
To implement real-time physics to Jackie's ponytail model, I used the Verlet Integration.

Verlet Integration is an effective way to handle real-time physics based on a target object's positions. More specifically, Verlet Integration simulates physics by using a target object's previous and current position to calculate the next position. This method is particularly useful because it is easy to understand and relatively simple to integrate.

A good explanation of this concept and a simple code demonstration are offered in the linked video. I strongly suggest watching this if you are interested in learning more about Verlet Integration.

[![Verlet Integration](https://img.youtube.com/vi/-GWTDhOQU6M/0.jpg)](https://www.youtube.com/watch?v=-GWTDhOQU6M)

The physics itself is easy to design. The challenge is implementing this system into a game that was not designed with Verlet Integration in mind. For this reason, I had to customize some parts of Verlet Integration to make this system properly work in MM Recomp.

## Blender
If you are used to creating custom player models for Majora's Mask, then there isn't too much else to know about Blender for this project.

## ProxySaw's CustomAPI Library
ProxySaw developed a really useful library that allows people to add their own actors into Majora's Mask Recomp. It is used as a foundation for this mod.

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

-add illustration here-

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

target_name = "gPonytailSkel"
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
(*gPonytailSkel* is the name of my ponytail model's skeleton)

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

-Diagram here

So, the parameters are as follows:
* Previous Position of the Target Object's limb
* Current Position of the Target Object's limb
* Force (accleration) being applied to Target Object's limb
* Change in time (*dt* which is just 1)
* Player's velocity
* Player's head limb's rotation

# Code
Before jumping into the code, it is important to understand three parts that are involved in this project: 
* How Majora's Mask manipulates in-game models 
* How to use ProxySaw's CustomActor Library
* How Verlet Integration can be implemented
