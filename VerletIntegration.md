# Verlet Integration
To implement real-time physics to Jackie's ponytail model, I used the Verlet Integration.

Verlet Integration is an effective way to handle real-time physics based on a target object's positions. More specifically, Verlet Integration simulates physics by using a target object's previous and current position to calculate the next position. This method is particularly useful because it is easy to understand and relatively simple to integrate.

A good explanation of this concept and a simple code demonstration are offered in the linked video. I strongly suggest watching this if you are interested in learning more about Verlet Integration.

[![Verlet Integration](https://img.youtube.com/vi/-GWTDhOQU6M/0.jpg)](https://www.youtube.com/watch?v=-GWTDhOQU6M)

The physics itself is easy to design. The challenge is implementing this system into a game that was not designed with Verlet Integration in mind. For this reason, I had to customize some parts of Verlet Integration to make this system properly work in MM Recomp.

## Designing Verlet Integration in Majora's Mask Recomp
### Parameters and Behaviors of Verlet Integration
If you learned about Verlet Integration, you would know that the four main parameters for Verlet Integration are:
* Previous Position of the Target Object
* Current Position of the Target Object
* Force (accleration) being applied to Target Object
* The difference in time between the previous frame and the current frame.

Regarding the target object, you would know how an object's points and bones are treated. The target object's points would be either pinned or unpinned, where pinned points stay in place on the canvas and unpinned points get affected by the force/acceleration (i.e. gravity). The target object's bones, which are two points grouped into one entity, are constrained to maintain its initial distance. This constraint is what forces bones to stay at a certain length while also adjusting its position to maintain this length at all times.

## Applying Verlet Integration to MM Recomp
Show Blender process, high level implementation of Verlet Integration on ponytail, and step-by-step explanation of relevant functions here
