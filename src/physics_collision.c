/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Physics collision system to be used alongside verlet physics designed for Majora's Mask recomp    

========================================================================
*/

// MACROS
#define GREATER_THAN_ZERO   0.0001f
#define GREATER_THAN_ZERO2  0.00000001f
#define GREATER_THAN_ZERO3  0.0001f

#include "physics_collision.h"
#include "customMath.h"

// remove later
#include "ultra64.h"
#include "global.h"

#include "recomputils.h"

/***********************************************************************

	Physics Collision System

***********************************************************************/

/**
 * @brief Push target phys limb out of a sphere collider in the direction of the sphere collider's center to phys
 *        limb's current position inside the collider.
 * 
 * This function finds the direction from the sphere collider's center to the target phys limb's position if said
 * phys limb is inside the collider. Afterward, the phys limb is pushed out in that direction so that it is on
 * the surface. 
 * 
 * @param targetPhysLimb    Phys limb that is to be pushed out of target collider
 * @param targetCollider    Collider that the target phys limb needs to be pushed out of
 */
void PhysCol_SolveCollision(PhysLimb* targetPhysLimb, PhysSphereCollider* targetCollider) {
    // If phys limb is pinned, then do not solve collision and leave function.
    if (targetPhysLimb->pinned) {
        return;
    }

    // Find direction to push target limb out of target sphere collider (sphere collider center to phys limb's 
    // current position)
    Vec3f directionVec = { (f32)0, (f32)0, (f32)0 };
    Math_Vec3f_Diff(&targetPhysLimb->curr_pos, &targetCollider->center, &directionVec);

    // Find minimum distance that should be between limb and collider to make them not collide
    f32 distSquared = Math3D_Vec3fMagnitudeSq(&directionVec);
    f32 minDist = targetPhysLimb->collision_radius + targetCollider->radius;
    f32 minDistSquared = minDist * minDist;

    // If limb and collider do not overlap, there is no collision happening. Leave function.
    if (distSquared >= minDistSquared || distSquared < GREATER_THAN_ZERO) {
        return;
    }

    // Take direction vector from center of collider to phys limb's position and then scale it down to minimum
    // distance.
    CustomMath_Vec3f_Normalize(&directionVec, &directionVec);
    Math_Vec3f_Scale(&directionVec, minDist);

    // Override current positions so that target phys limb and target collider don't collide
    Vec3f newPos = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(&targetCollider->center, &directionVec, &newPos);
    Math_Vec3f_Copy(&targetPhysLimb->curr_pos, &newPos);
}


/**
 * @brief Push target phys limb out of the capsule collider in the direction of the capsule's inside to the
 *        phys limb's current position inside the collider.
 * 
 * This function finds the direction from the capsule collider's center to the target phys limb's position if said
 * phys limb is inside the collider. Afterward, the phys limb is pushed out in that direction so that it is on
 * the surface. 
 * 
 * @param physLimb      Target phys limb to be pushed out of collider
 * @param sphereColA    First endpoint of capsule collider
 * @param sphereColB    Second endpoint of capsule collider 
 */
void PhysCol_SolveCapsuleFromSpheres(PhysLimb* physLimb, PhysSphereCollider* sphereColA, 
    PhysSphereCollider* sphereColB) {
    // If phys limb is pinned, then do not solve collision and leave function.
    if (physLimb->pinned) {
        return;
    }

    // Calculate line segment from one endpoint of capsule to another.
    Vec3f segment = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Diff(&sphereColB->center, &sphereColA->center, &segment);
    f32 segLengthSq = Math3D_Vec3fMagnitudeSq(&segment);

    // If both sphere centers are basically in identical positions, just solve this collision as phys limb's
    // collision with first sphere collider.
    if (segLengthSq < GREATER_THAN_ZERO) {
        PhysCol_SolveCollision(physLimb, sphereColA);
        return;
    }

    // Set up distance from first endpoint to the target phys limb.
    Vec3f sphereToLimb = { 0.0f, 0.0f, 0.0f };
    Math_Vec3f_Diff(&physLimb->curr_pos, &sphereColA->center, &sphereToLimb);

    // Find percentage along the segment where point in segment is closest to the phys limb.
    f32 t = CustomMath_Vec3f_Dot(&sphereToLimb, &segment) / segLengthSq;

    // Keep the closest point between sphere A and sphere B, but also keep within the range of the distance
    // between the two endpoints.
    if (t < 0.0f) {
        t = 0.0f;
    }
    else if (t > 1.0f) {
        t = 1.0f;
    }

    // Find the closest point on center segment using t.
    Vec3f scaledSegment = {0.f, 0.f, 0.f};
    Vec3f closestPos = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&segment, t, &scaledSegment);
    Math_Vec3f_Sum(&sphereColA->center, &scaledSegment, &closestPos);   // Store closest position in capsule

    // Interpolate radius between sphere A and sphere B (in case radius is different for both Sphere A and Sphere B)
    f32 capsuleRadius = sphereColA->radius + ((sphereColB->radius - sphereColA->radius) * t);

    // Treat this point along the capsule as a temporary sphere center
    PhysSphereCollider tempSphere = {
        .center = closestPos,
        .radius = capsuleRadius
    };

    // Reuse normal sphere collision solver at this point in the capsule.
    PhysCol_SolveCollision(physLimb, &tempSphere);
}


/**
 * @brief Update capsule collider's metrics from its endpoint spheres.
 *
 * Take an existing capsule collider and define its metrics with this function based on the current position 
 * and radius of each endpoint's sphere colliders.
 * 
 * Note: This should preferably be called after updating the endpoint sphere colliders' world position and 
 *       radius before running collision check every substep.
 * 
 * @param capsuleCollider   Target capsule collider to update
 * @param startEndpoint     Starting endpoint sphere collider of target capsule collider
 * @param endEndpoint       Ending endpoint sphere collider of target capsule collider
 */
void PhysCol_UpdateCapsule(PhysCapsuleCollider* capsuleCollider, PhysSphereCollider* startEndpoint, 
    PhysSphereCollider* endEndpoint) {
    // Copy endpoint positions and radii
    capsuleCollider->endpoints[0] = *startEndpoint;
    capsuleCollider->endpoints[1] = *endEndpoint;

    // Calculate the vector from the starting endpoint's center to the ending endpoint's center.
    Math_Vec3f_Diff(&endEndpoint->center, &startEndpoint->center, &capsuleCollider->axis);

    // Store how much the radius changes from starting endpoint to ending endpoint.
    capsuleCollider->radiusDifference = endEndpoint->radius - startEndpoint->radius;

    // Store the larger endpoint radius, which is to be used exit-search calculations during collision
    if (startEndpoint->radius > endEndpoint->radius) {
        capsuleCollider->maxRadius = startEndpoint->radius;
    }
    else {
        capsuleCollider->maxRadius = endEndpoint->radius;
    }
}


/**
 * @brief Find the closest point along a given capsule collider to a target world position
 * 
 * This function projects the target position onto the line segment between the two sphere colliders' centers 
 * and clamps the result so the closest point remains between the two sphere colliders.
 * 
 * The function also interpolates the capsule's radius at that closest point. This allows the capsule with 
 * differently sized endpoint sphere colliders to be handled properly.
 * 
 * @param targetPos         Target world position 
 * @param capsuleCollider   Target capsule collider where the closest point is to be located in
 * @param colRadius         Output radius of the sphere collider at the closest point in capsule
 * 
 * @return  World position of the closest point to targetPos along the capsule collider's center line
 */
Vec3f PhysCol_FindClosestPointOnCapsule(Vec3f targetPos, PhysCapsuleCollider* capsuleCollider, 
    f32* colRadius) {
    // Starting endpoint's center's world position
    Vec3f startCenterPos = {0.f, 0.f, 0.f};
    Math_Vec3f_Copy(&startCenterPos, &capsuleCollider->endpoints[0].center);

    // Get vector pointing from starting endpoint's center to ending endpoint's center in capsule.
    Vec3f capsuleAxis = {0.f, 0.f, 0.f};
    Math_Vec3f_Copy(&capsuleAxis, &capsuleCollider->axis);

    // Calculate the squared length of capsule's center line
    f32 lengthSq = Math3D_Vec3fMagnitudeSq(&capsuleAxis);

    // Set up variable (t) that indicates where the point closest to targetPos is between the two capsule 
    // endpoint sphere colliders.
    f32 t = 0.f;

    // Only project onto the capsule's center line if the two sphere centers are actually apart instead of 
    // just being a sphere collider.
    if (lengthSq >= GREATER_THAN_ZERO) {
        // Find the vector from the starting endpoint's center to the target position.
        Vec3f startToTarget = {0.f, 0.f, 0.f};
        Math_Vec3f_Diff(&targetPos, &startCenterPos, &startToTarget);

        // Project the target onto the capsule's center line.
        f32 projectedDot = CustomMath_Vec3f_Dot(&startToTarget, &capsuleAxis);

        t = projectedDot / lengthSq;

        // Keep the closest point between the endpoint centers.
        if (t < 0.f) {
            t = 0.f;
        }
        else if (t > 1.f) {
            t = 1.f;
        }
    }

    // Interpolate the capsule radius at the closest point.
    *colRadius = capsuleCollider->endpoints[0].radius + capsuleCollider->radiusDifference * t;

    // Move from the starting center along the axis by fraction t.
    Vec3f scaledCapsuleAxis = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&capsuleAxis, t, &scaledCapsuleAxis);      // The direction and distance
    Vec3f closestPoint = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(&startCenterPos, &scaledCapsuleAxis, &closestPoint);
    
    return closestPoint;
}


/**
 * @brief Calculate the deepest depth between a target position and any of the colliders.
 *
 * This function checks a target position against every capsule collider used by the overcoat.
 *
 * For each collider, the function finds a point along the collider that is closest to the target 
 * position and calculates the collider radius at that point. It then calculates how deep the target
 * position is inside the collider. This includes the target's own collision radius. 
 *
 * @param targetPos         Target world position being checked
 * @param pointRadius       Collision radius of target point/position
 * @param capsulesCols      Array of capsule colliders
 * @param capsuleColCount   Number of capsule colliders
 * @param contactColCenter  Output center position of the deepest collider contact.
 * @param contactColRadius  Output combined collider and target radius of the deepest contact.
 *
 * @return Deepest overlap depth found among all body colliders.
 */
f32 PhysCol_GroupContact(Vec3f targetPos, f32 pointRadius, PhysCapsuleCollider* capsulesCols, 
    int capsuleColCount, Vec3f* contactColCenter, f32* contactColRadius) {

    // Set up variable indicating deepest depth value of targetPos in any collider in overcoat
    f32 deepest = 0.f;

    // Note: This is crudely coded because I did not handle list of colliders cleanly. The array
	//		 for colliders is only consisted of capsule colliders.
    for (int capsuleIdx = 0; capsuleIdx < capsuleColCount; capsuleIdx++) {
        // Find what part of the current capsule collider is closest to target position
        // Set up variable for radius of sphere collider that would be located in closest point in 
        // capsule. This is important in case capsule has differently sized endpoint sphere colliders,
        // in which case the radius would be interpolated based on its position in capsule.
        f32 colRadius = 0.f;
        // Find center position of sphere collider at a point in capsule collider that would be located
        // closest to the target position.
        Vec3f colCenter = PhysCol_FindClosestPointOnCapsule(targetPos, &capsulesCols[capsuleIdx], 
            &colRadius);
        // Find the depth, or just how deep the target position is located inside current collider
        f32 currDepth = colRadius + pointRadius - Math_Vec3f_DistXYZ(&targetPos, &colCenter);
        // If the calculated depth is deeper than the currently recorded deepest depth so far, then
        // replace that variable's value with this newly calculated current depth. Also keep note of the
        // collider's center position and radius.
        if (currDepth > deepest) {
            deepest = currDepth;
            *contactColCenter = colCenter;
            *contactColRadius = colRadius + pointRadius;
        }
    }

    // Return the deepest depth recorded in any of collider.
    return deepest;
}


/**
 * @brief Check whether target position of phys limb is outside all of colliders
 *
 * The function determines that if a given given target position is inside even one of the colliders,
 * then it cannot be outside of the group of colliders.
 *
 * @param targetPos			Target world position of phys limb being checked
 * @param pointRadius		Collision radius of target phys limb
 * @param capsuleCols		Array of capsule colliders
 * @param capsuleColCount	Number of capsule colliders
 * 
 * @return true if target position is outside of all colliders, otherwise false
 */
u8 PhysCol_GroupOutside(Vec3f targetPos, f32 pointRadius, PhysCapsuleCollider* capsuleCols, 
    int capsuleColCount) {
    // Look through every capsule collider
    for (int capsuleIdx = 0; capsuleIdx < capsuleColCount; capsuleIdx++) {
        // For current capsule collider, find the sphere collider within this capsule that is closest
        // to target phys limb's position
        f32 colRadius = 0.f;
        Vec3f colCenter = PhysCol_FindClosestPointOnCapsule(targetPos, &capsuleCols[capsuleIdx],
            &colRadius);

        // Calculate distance between target world position and closest sphere collider in capsule
        Vec3f distToColliderCenter;
        Math_Vec3f_Diff(&targetPos, &colCenter, &distToColliderCenter);

        // Find if distance between target position and collider is less than collider radius itself.
        f32 distanceSq = Math3D_Vec3fMagnitudeSq(&distToColliderCenter);
        f32 combinedRadius = colRadius + pointRadius;  // Mind phys limb's own collision radius

        // If distance is less than collider radius (plus phys limb's own radius), then that means the
        // phys limb itself is within the collider.
        if (distanceSq < (combinedRadius * combinedRadius)) {
            // Target phys limb is NOT outside group of colliders. Return false.
            return false;
        }
    }

    // By this point, it means the target phys limb was not in any of the capsule colliders, meaning that
    // the phys limb was outside group of colliders. Return true.
    return true;
}


/**
 * @brief Find safe exit position for phys limb from given group of colliders.
 * 
 * Estimate a distance beyond every given capsule, then use binary search to refine the safe exit position 
 * out of group of colliders.
 *
 * @param targetPos             Target phys limb's world position
 * @param physLimbDir			Direction along which a safe exit position for phys limb is searched
 * @param pointRadius			Collision radius of target phys limb
 * @param capsuleCols			Array of capsule colliders
 * @param capsuleColCount		Count of capsule colliders 
 * @param padding				Small extra distance added so exit doesn't sit on collider boundary
 * @param exitSearchSteps       Number of binary-search iterations.
 * @param exitPosition			Output position containing safe exit position for phys limb
 */
u8 PhysCol_GroupRayExit(Vec3f targetPos, Vec3f physLimbDir, f32 pointRadius, PhysCapsuleCollider* capsuleCols,
    int capsuleColCount, f32 padding, int exitSearchSteps, Vec3f* exitPosition) {
    // Find a distance from phys limb, along the indicated search direction (physLimbDir), that is 
	// far enough that it should be past every collider
    f32 farDistance = 0.f;

    // Go through each collider in the array of capsule colliders and see how far the target phys limb 
    // should go to be outside all colliders (indicated by farDistance)
    for (int capsuleIdx = 0; capsuleIdx < capsuleColCount; capsuleIdx++) {
        // Get current capsule collider from given array of colliders.
        PhysCapsuleCollider* capsule = &capsuleCols[capsuleIdx];

        // Get the larger radius of the two endpoints of the capsule collider
        // Note: Also consider the collision radius around the target phys limb.
		f32 colRadius = capsule->maxRadius + pointRadius;

        // Check the two sphere collider endpoints of the current capsule collider.
		// The focus is going to be on the current capsule collider's sphere collider with the larger 
        // radius for finding a distance that is more certain to be big enough to push the phys limb out 
        // of the colliders.
        for (int capsuleEndIdx = 0; capsuleEndIdx < 2; capsuleEndIdx++) {
            // Get the current capsule's current endpoint sphere collider's center
            Vec3f currEndpointCenter = { 0.f, 0.f, 0.f };
            Math_Vec3f_Copy(&currEndpointCenter, &capsule->endpoints[capsuleEndIdx].center);

            // Project vector from the target phys limb's position to this current endpoint onto the
            // search direction.
            Vec3f targetToCurrEndpointCenter = {0.f, 0.f, 0.f};
            Math_Vec3f_Diff(&currEndpointCenter, &targetPos, &targetToCurrEndpointCenter);

            // Calculate a safe distance along the input direction (physLimbDir) that would move phys limb 
            // beyond capsule's sphere collider endpoint.
            f32 projectedDistance = CustomMath_Vec3f_Dot(&targetToCurrEndpointCenter, &physLimbDir);
            f32 currDistance = projectedDistance + colRadius;

            // If the calculated distance is greater than the current farthest distance from  collider 
            // boundary to phys limb, then keep track of that new farthest distance.
            if (currDistance > farDistance) {
				farDistance = currDistance;
			}
        }
    }

    // Ensure that the farthest point is not precisely on the collider boundary by adding padding
    farDistance += padding;
    f32 nearDistance = 0.f;	// Distance from the target PhysLimb's current position

    // Use the calculated farthest distance from any collider (farDistance) to find the point along the 
    // input phys limb direction where it would be outside all colliders. Update the exitPosition 
    // argument with this new position value
    Vec3f travelVec = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&physLimbDir, farDistance, &travelVec);
    Math_Vec3f_Sum(&targetPos, &travelVec, exitPosition);

    // If the newly calculated position is still inside any collider, return false and get out of function.
    if (!PhysCol_GroupOutside(*exitPosition, pointRadius, capsuleCols, capsuleColCount)) { 
		return false;
	}

    // Up to this point, a safe far distance was found that places the PhysLimb outside all colliders. 
	// However, this distance may be farther than necessary.
	//
	// Use binary search to find a closer safe exit position along input direction (physLimbDir).
	// nearDistance represents the inside side of the search, while farDistance represents  the known safe 
    // outside side. Each iteration cuts the gap between them in half.
	//
	// More iterations (exitSearchSteps) make exitPosition closer to the point where the PhysLimb first 
    // becomes outside all colliders along this direction.
    for (int iter = 0; iter < exitSearchSteps; iter++) {
		// Find midpoint between nearDistance and farDistance
        f32 midpointDist = (nearDistance + farDistance) * 0.5f;
		
		// Check where the phys limb would end up by moving it along the input direction to the midpoint 
        // distance
        Vec3f midpointTravelVec = {0.f, 0.f, 0.f};
        Math_Vec3f_ScaleAndStore(&physLimbDir, midpointDist, &midpointTravelVec);
        Vec3f testExitPos = {0.f, 0.f, 0.f};
        Math_Vec3f_Sum(&targetPos, &midpointTravelVec, &testExitPos);

		// If the test exit position at mid point is outside all colliders, then that is the new 
		// farthest distance between position outside the colliders and the closest collider's
		// boundary.
        u8 testExitIsOutside = PhysCol_GroupOutside(testExitPos, pointRadius, capsuleCols, 
            capsuleColCount);
        if (testExitIsOutside) {
            // Keep this closer safe position.
            farDistance = midpointDist;
            *exitPosition = testExitPos;
        } 
		// If the text exit position at mid point is inside a collider still, then it closest position inside 
        // the collider is assigned that midpoint distance
		else {
            nearDistance = midpointDist;
        }
		
		// Continue to run this to get closer to boundary of collider
    }

    return true;
}


/**
 * @brief Initialize settings for a table of set-up sphere colliders.
 *
 * Uses default settings where available. For APPEAR_WHEN entries without defaults, uses the first alternative 
 * containing settings.
 *
 * Colliders remain disabled until the behavior runs. Colliders' center world positions and newly scaled radii 
 * are updated separately.
 *
 * @param sphereColliderEntries Array of sphere collider entries
 * @param entryCount            Number of sphere collider entries
 */
void PhysCol_InitSphereColliderEntries(PhysSphereColliderEntry* sphereColliderEntries, int entryCount) {
    // Iterate through every sphere collider entry
    for (int entryIdx = 0; entryIdx < entryCount; entryIdx++) {
        // Point to current sphere collider entry and entry's sphere collider
        PhysSphereColliderEntry* currEntry = &sphereColliderEntries[entryIdx];
        PhysSphereCollider* currCollider = currEntry->collider;

        // Leave collision participation to the behavior evaluator.
        currCollider->enabled = 0;

        // Have initial settings for current sphere collider entry point to current entry's default settings.
        PhysSphereSettings* initialSettings = currEntry->defaultSettings;

        // Appear-only colliders may have no default settings.
        // Initialize their position and radius settings from the first valid alternative, even when they are not 
        // currently enabled.
        if (initialSettings == NULL && currEntry->behavior == PHYS_COLLIDER_APPEAR_WHEN &&
            currEntry->altSettingsAndConditions != NULL) {
            // Find first valid alternative setting
            for (int altSettingIdx = 0; altSettingIdx < currEntry->altSettingsAndConditionsCount; altSettingIdx++) {
                // Pointer to alternative setting in current sphere collider entry
                PhysSphereAltSettingsAndCondition* currAlt = &currEntry->altSettingsAndConditions[altSettingIdx];

                // First valid alternative setting
                if (currAlt->settings != NULL) {
                    initialSettings = currAlt->settings;
                    break;
                }
            }
        }

        // If initial setting is defined, then have collider apply those settings
        if (initialSettings != NULL) {
            currCollider->settings = *initialSettings;
        }
        else {
            // Invalid configuration: no usable initial settings.
            // Leave this collider disabled.
            currCollider->settings = (PhysSphereSettings){0};
        }
    }
}


/**
 * @brief Evaluate collider entries and apply their current settings and enabled states.
 *
 * Check alternative settings in array order and use the first match. Call after initialization and before updating 
 * the colliders' positions and radii in the 3d space.
 *
 * @param sphereColliderEntries Array of sphere collider entries.
 * @param entryCount            Number of entries.
 * @param player                Player actor
 */
void PhysCol_ApplySphereColliderEntries(PhysSphereColliderEntry* sphereColliderEntries, int entryCount, 
    Player* player) {
    // Iterate through every sphere collider entry
    for (int entryIdx = 0; entryIdx < entryCount; entryIdx++) {
        // Point to current sphere collider entry and entry's sphere collider
        PhysSphereColliderEntry* currEntry = &sphereColliderEntries[entryIdx];
        PhysSphereCollider* currCollider = currEntry->collider;

        // Have initial settings for current sphere collider entry point to current entry's default settings.
        PhysSphereSettings* selectedSettings = currEntry->defaultSettings;
        // Pointer to alternative setting and condition for current sphere collider entry
        PhysSphereAltSettingsAndCondition* matchingAlternative = NULL;

        // Constant colliders do not need condition checks.
        if (currEntry->behavior != PHYS_COLLIDER_CONSTANT && currEntry->altSettingsAndConditions != NULL) {
            // Find first alternative setting that returns true
            for (int altSettingIdx = 0; altSettingIdx < currEntry->altSettingsAndConditionsCount; altSettingIdx++) {
                PhysSphereAltSettingsAndCondition* currAlt = &currEntry->altSettingsAndConditions[altSettingIdx];
                PhysColliderCondition* condition = currAlt->condition;

                // If there is no condition or an existing condition has no actual check, then this is not a valid
                // alternative setting.
                if (condition == NULL || condition->customCheck == NULL) {
                    continue;
                }

                // First alternative setting that returns true found
                if (condition->customCheck(player)) {
                    matchingAlternative = currAlt;
                    break;
                }
            }
        }

        // Start disabled. Collider's selected behavior determines activation.
        currCollider->enabled = 0;

        // Activate based on collider's behavior, which is in collider entry
        switch (currEntry->behavior) {
            case PHYS_COLLIDER_CONSTANT:
                // Always use the default settings.
                currCollider->enabled = (selectedSettings != NULL);
                break;

            case PHYS_COLLIDER_APPEAR_WHEN:
                // In this case, default settings would be null, so selected settings would also be null.
                selectedSettings = NULL;

                // However, colliders that are set to appear for specific animation(s) or state(s) have at least one
                // alternative setting.
                // If the matching alternative setting's condition that returned true was found in the previous for 
                // loop, then this alternative setting's collider settings should be applied for this frame.
                if (matchingAlternative != NULL) {
                    // Apply alternative collider settings
                    selectedSettings = matchingAlternative->settings;
                    currCollider->enabled = (selectedSettings != NULL);
                }
                break;

            case PHYS_COLLIDER_DISAPPEAR_WHEN:
                // In this case, collider should disappear if specified animation/state is currently playing right
                // now.
                currCollider->enabled = (matchingAlternative == NULL && selectedSettings != NULL);
                break;

            case PHYS_COLLIDER_SWITCH_SETTINGS_WHEN:
                // In this case, selected settings is default settings.
                // But, if specified animation/state is currently playing right now, then alternative settings is
                // selected instead.
                if (matchingAlternative != NULL) {
                    selectedSettings = matchingAlternative->settings;
                }

                currCollider->enabled = (selectedSettings != NULL);
                break;

            default:
                // Unknown behavior: leave disabled and retain settings.
                selectedSettings = NULL;
                break;
        }

        // Copy the selected preset without modifying its definition.
        if (selectedSettings != NULL) {
            currCollider->settings = *selectedSettings;
        }
    }
}


/**
 * @brief Update the positions of sphere colliders attached to the current skeleton limb.
 *
 * The current matrix must belong to the indicated skeleton limb. Its position, rotation, and scale are applied 
 * to each collider's local offset.
 *
 * @param sphereColliderEntries Array of sphere collider entries
 * @param entryCount            Number of collider entries
 * @param limbIndex             Skeleton limb whose matrix is currently active
 */
void PhysCol_UpdateSphereCentersForLimb(PhysSphereColliderEntry* sphereColliderEntries, int entryCount, 
    int limbIndex) {
    // Iterate through every sphere collider entry
    for (int entryIdx = 0; entryIdx < entryCount; entryIdx++) {
        PhysSphereColliderEntry* currEntry = &sphereColliderEntries[entryIdx];

        // Only update colliders attached to this skeleton limb.
        if (currEntry->limbIndex != limbIndex) {
            continue;
        }

        PhysSphereCollider* currCollider = currEntry->collider;

        // Convert the selected offset into the collider's position in the game.
        Matrix_MultVec3f(&currCollider->settings.localOffset, &currCollider->center);
    }
}


/**
 * @brief Update sphere collider radii using their selected settings and a scale multiplier.
 *
 * @param sphereColliderEntries Array of collider entries
 * @param entryCount            Number of collider entries
 * @param colliderScale         Multiplier applied to each configured radius
 */
void PhysCol_UpdateSphereRadii(PhysSphereColliderEntry* sphereColliderEntries, int entryCount, f32 colliderScale) {
    // Iterate through every sphere collider entry
    for (int entryIdx = 0; entryIdx < entryCount; entryIdx++) {
        PhysSphereCollider* currCollider = sphereColliderEntries[entryIdx].collider;

        currCollider->radius = currCollider->settings.tempRadius * colliderScale;
    }
}


/**
 * @brief Build a list of capsule connections to use for the current frame.
 *
 * potentialPairs contains every possible connection between two sphere colliders, including connections involving 
 * colliders that only appear during certain animations.
 *
 * This function checks each connection. If both endpoint spheres are enabled, their pointers are added to 
 * activePairs. If either endpoint is disabled, the connection is skipped.
 *
 * Evaluate the sphere collider entries before calling this function so their enabled flags reflect the current 
 * conditions.
 *
 * This function only selects connections. It does not update collider positions, radii, or the contents of 
 * PhysCapsuleCollider objects.
 *
 * @param potentialPairs       Array of every possible capsule connection
 * @param potentialPairCount   Number of possible connections
 * @param activePairs          Output array receiving enabled connections
 *
 * @return Number of active capsule pairs written to the output array
 */
s16 PhysCol_BuildActiveCapsulePairs(PhysSphereCollider* potentialPairs[][2], int potentialPairCount,
    PhysSphereCollider* activePairs[][2]) {
    // Keep count of active capsule colliders in current frame
    s16 activePairCount = 0;

    // Iterate through every possible capsule colliders
    for (s16 currPairIdx = 0; currPairIdx < potentialPairCount; currPairIdx++) {
        // Look at current possible capsule collider
        PhysSphereCollider* startEndpoint = potentialPairs[currPairIdx][0];
        PhysSphereCollider* endEndpoint = potentialPairs[currPairIdx][1];

        // Both endpoints must be enabled for this connection to be used.
        if (!startEndpoint->enabled || !endEndpoint->enabled) {
            continue;
        }

        // By this point, add endpoint sphere colliders to activePairs array
        activePairs[activePairCount][0] = startEndpoint;
        activePairs[activePairCount][1] = endEndpoint;

        // Iterate count of active capsule collider(s)
        activePairCount++;
    }

    // Return count of active capsule collider(s)
    return activePairCount;
}


/**
 * @brief Normalize a direction vector if it is long enough.
 *
 * @return 1 if normalized, otherwise 0.
 */
u8 PhysCol_NormalizeSubstepDirectionVec(Vec3f* targetVec) {
    f32 lengthSq = Math3D_Vec3fMagnitudeSq(targetVec);

    if (lengthSq < GREATER_THAN_ZERO2) {
        return 0;
    }

    Math_Vec3f_Scale(targetVec, 1.f / sqrtf(lengthSq));
    return 1;
}


/**
 * @brief Push a phys limb out of a group of capsule colliders.
 *
 * Find the deepest contact and try moving the limb away from that contact's center.
 * If that position still overlaps another collider, search from the limb's original position in the outward and 
 * opposite directions. Choose the closer successful exit, preferring outward when their distances are equal.
 *
 * If no usable direction can be calculated from the limb's current or previous position, use fallbackDirection. 
 * If that direction is also unusable, use upward.
 *
 * Pinned limbs remain unchanged. Only curr_pos is updated; prev_pos remains unchanged. If neither exit search 
 * succeeds, the limb remains in its original position.
 *
 * @param physLimb          Phys limb being checked and corrected
 * @param capsuleCols       Array of capsule colliders updated for the current substep
 * @param capsuleColCount   Number of capsule colliders
 * @param fallbackDirection Backup push direction   (e.g. a physics object's forward axis)
 * @param exitSearchSteps   Number of binary-search iterations for each exit search
 */
void PhysCol_SolveColliderGroup(PhysLimb* physLimb, PhysCapsuleCollider* capsuleCols, int capsuleColCount,
    Vec3f fallbackDirection, int exitSearchSteps) {
    // Ignore if phys limb is pinned.
    if (physLimb->pinned) {
        return;
    }

    // Find the collider where the phys limb is deepest in within the group of colliders.
    // First find the closest point along this capsule collider's centerline to the phys limb.
    Vec3f center = {0.f, 0.f, 0.f};     // Center of sphere collider in capsule collider
    f32 radius = 0.f;                   // Radius of sphere collider in capsule collider

    // If the phys limb is not inside any collider, no correction is needed.
    if (PhysCol_GroupContact(physLimb->curr_pos, physLimb->collision_radius, capsuleCols, capsuleColCount,
        &center, &radius) <= 0.f) {
        return;
    }

    // Find direction pointing from the collider center toward the phys limb's current position
    Vec3f direction = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&physLimb->curr_pos, &center, &direction);

    // Try to normalize the direction from the target collider center toward the phys limb.
    // If it is too short to normalize, try the previous position of phys limb instead.
    if (!PhysCol_NormalizeSubstepDirectionVec(&direction)) {
        // Use phys limb's previous position instead to create another possible outward direction
        Math_Vec3f_Diff(&physLimb->prev_pos, &center, &direction);

        // Try to normalize direction from collider center to phys limb's previous position If  this fails, 
        // then even the previous position of the phys limb is too close to the collider's center.
        if (!PhysCol_NormalizeSubstepDirectionVec(&direction)) {
            // Use Jackie's forward direction as an alternative.
            direction = fallbackDirection;

            // Try to normalize Jackie's forward direction. If even this fails, then just use straight upward 
            // direction as final fallback direction.
            if (!PhysCol_NormalizeSubstepDirectionVec(&direction)) {
                direction = (Vec3f){0.f, 1.f, 0.f};
            }
        }
    }

    // Create very small padding gap between collider and phys limb so that phys limb won't end up exactly on the 
    // collider's boundary. This is because if the phys limb ends up exactly on the collider's boundary, then the 
    // phys limb would still be considered colliding against the collider.
    f32 padding = radius * GREATER_THAN_ZERO3;

    // Fail safe for when padding somehow ends up being 0 or less
    if (padding < GREATER_THAN_ZERO3) {
        padding = GREATER_THAN_ZERO3;
    }

    // Calculate the offset from the collider center to just outside its boundary.
    Vec3f outwardOffset = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&direction, radius + padding, &outwardOffset);

    // Calculate the candidate world position.
    Vec3f candidate = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(&center, &outwardOffset, &candidate);

    // Check whether phys limb would be outside all colliders if it were moved to this candidate position. 
    // If the position is safe, then move the phys limb there and finish.
    if (PhysCol_GroupOutside(candidate, physLimb->collision_radius, capsuleCols, capsuleColCount)) {
        physLimb->curr_pos = candidate;
        return;
    }

    // Store possible safe exit positions in the normal outward direction (collider center to phys limb) and in 
    // the opposite direction.
    Vec3f outwardExit = {0.f, 0.f, 0.f};    // exit position that goes from collider center to phys limb
    Vec3f oppositeExit = {0.f, 0.f, 0.f};   // opposite exit position that points to collider center and beyond to 
                                            // the other side of the sphere collider.
    
    // Create direction that points exactly opposite from the normal outward direction.
    Vec3f opposite = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&direction, -1.f, &opposite);

    // Search outward from phys limb for a position that is outside all colliders
	// outwardOK is a flag that indicates whether valid exit was found in normal direction.
	// outwardExit stores the position of that exit.
    u8 outwardOK = PhysCol_GroupRayExit(physLimb->curr_pos, direction, physLimb->collision_radius, capsuleCols,
        capsuleColCount, padding, exitSearchSteps, &outwardExit);
    u8 oppositeOK = PhysCol_GroupRayExit(physLimb->curr_pos, opposite, physLimb->collision_radius, capsuleCols,
        capsuleColCount, padding, exitSearchSteps, &oppositeExit);

    // Choose the closer successful exit, preferring outward on equal distances.
    u8 useOutwardExit = outwardOK;

    if (outwardOK && oppositeOK) {
        f32 outwardDistance = Math_Vec3f_DistXYZ(&physLimb->curr_pos, &outwardExit);
        f32 oppositeDistance = Math_Vec3f_DistXYZ(&physLimb->curr_pos, &oppositeExit);

        useOutwardExit = (outwardDistance <= oppositeDistance);
    }

    if (useOutwardExit) {
        physLimb->curr_pos = outwardExit;
    }
    else if (oppositeOK) {
        physLimb->curr_pos = oppositeExit;
    }

    // If neither search succeeded, leave the limb unchanged.
}


/**
 * @brief Push the midpoint between two phys limbs out of a group of capsule colliders.
 *
 * Create a temporary phys limb using the average of the two limbs' current positions, previous positions, and 
 * collision radii. Correct its position using PhysCol_SolveColliderGroup, then apply the resulting movement to 
 * the original limbs.
 *
 * When both limbs can move, each receives the full correction. When one is pinned, only the movable limb receives 
 * twice the correction, moving their midpoint by the required amount. If both are pinned, neither moves.
 *
 * Only current positions are updated. Previous positions remain unchanged. Checking the midpoint does not 
 * guarantee that the entire segment is outside the colliders.
 *
 * @param physLimb_a        First phys limb
 * @param physLimb_b        Second phys limb
 * @param capsuleCols       Array of capsule colliders updated for the current substep
 * @param capsuleColCount   Number of capsule colliders
 * @param fallbackDirection Backup push direction passed to PhysCol_SolveColliderGroup
 * @param exitSearchSteps   Number of binary-search iterations for each exit search
 */
void PhysCol_SolveMidpointGroup(PhysLimb* physLimb_a, PhysLimb* physLimb_b, PhysCapsuleCollider* capsuleCols,
    int capsuleColCount, Vec3f fallbackDirection, int exitSearchSteps) {
    // Neither endpoint can move because both are pinned.
    if (physLimb_a->pinned && physLimb_b->pinned) {
        return;
    }

    // Create temporary phys limb that represents midpoint between two neighboring phys limbs.
    PhysLimb midpoint = {0};

    // Calculate midpoint's current position by averaging out two phys limb's positions.
    // Average current position
    Math_Vec3f_Sum(&physLimb_a->curr_pos, &physLimb_b->curr_pos, &midpoint.curr_pos);
    Math_Vec3f_Scale(&midpoint.curr_pos, 0.5f);

    // Average previous position
    Math_Vec3f_Sum(&physLimb_a->prev_pos, &physLimb_b->prev_pos, &midpoint.prev_pos);
    Math_Vec3f_Scale(&midpoint.prev_pos, 0.5f);

    // Calculate midpoint phys limb's averaged collision radius
    midpoint.collision_radius = (physLimb_a->collision_radius + physLimb_b->collision_radius) * 0.5f;

    // Save midpoint's position before collision correction constraint
    Vec3f midpointBeforeCollision = midpoint.curr_pos;

    // Apply collider correction on midpoint temp phys limb
	// This is basically the same way phys limbs get their collider correction applied
    PhysCol_SolveColliderGroup(&midpoint, capsuleCols, capsuleColCount, fallbackDirection, exitSearchSteps);

    // Calculate how far midpoint was moved by the collision correction.
    Vec3f correction = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(&midpoint.curr_pos, &midpointBeforeCollision, &correction);

    // One movable endpoint must move twice as far to produce the same midpoint movement.
    if (physLimb_a->pinned || physLimb_b->pinned) {
        Math_Vec3f_Scale(&correction, 2.f);
    }

    // Apply the correction only to movable limbs.
    if (!physLimb_a->pinned) {
        Math_Vec3f_Sum(&physLimb_a->curr_pos, &correction, &physLimb_a->curr_pos);
    }

    if (!physLimb_b->pinned) {
        Math_Vec3f_Sum(&physLimb_b->curr_pos, &correction, &physLimb_b->curr_pos);
    }
}