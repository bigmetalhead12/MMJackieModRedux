/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

#include "customMath.h"

#include "recomputils.h"

/***********************************************************************

	Custom Math Functions

***********************************************************************/

/*
=================
Vec3f Misc Functions
=================
*/

// Dot product between vector a and vector b
f32 CustomMath_Vec3f_Dot(Vec3f* a, Vec3f* b) {
    return (a->x * b->x) +
           (a->y * b->y) +
           (a->z * b->z);
}

// Normalize given vector
void CustomMath_Vec3f_Normalize(Vec3f* src, Vec3f* dest) {
    f32 length = sqrtf(Math3D_Vec3fMagnitudeSq(src));

    if (length == 0.0f) {
        Vec3f zero_vec = { (f32)0, (f32)0, (f32)0 };
        Math_Vec3f_Copy(dest, &zero_vec);
        return;
    }

    dest->x = src->x / length;
    dest->y = src->y / length;
    dest->z = src->z / length;
}


/*
=================
Line Segment Functions
=================
*/

// Clamp a value between the given minimum and maximum.
f32 CustomMath_Clamp(f32 value, f32 min, f32 max) {
    if (value < min) {
        return min;
    }

    if (value > max) {
        return max;
    }

    return value;
}

// Linearly interpolate between two Vec3f values
void CustomMath_Vec3f_Lerp(Vec3f* pointA, Vec3f* pointB, f32 t, Vec3f* dest) {
    dest->x = pointA->x + (pointB->x - pointA->x) * t;
    dest->y = pointA->y + (pointB->y - pointA->y) * t;
    dest->z = pointA->z + (pointB->z - pointA->z) * t;
}


/*
=================
Vec3s (s16) math operations 
=================
*/

// Scale target Vec3s by input f32 value and put it into Vec3f dest
void CustomMath_Vec3s_Scale_ToVec3f(Vec3s* target, f32 scale, Vec3f* dest) {
    Vec3s transformVec3s = {0,0,0};
    Vec3f transformVec3f = {0.f, 0.f, 0.f};

    Math_Vec3s_Copy(&transformVec3s, target);
    Math_Vec3s_ToVec3f(&transformVec3f, &transformVec3s);
    Math_Vec3f_Scale(&transformVec3f, scale);

    Math_Vec3f_Copy(dest, &transformVec3f);
}


/*
=================
Vec3s (s16) rotation operations 
=================
*/

void CustomMath_Vec3s_RotateByX(Vec3s* src, s16 rotAngle, Vec3s* dest) {
    f32 sin = Math_SinS(rotAngle);
    f32 cos = Math_CosS(rotAngle);

    s16 x = src->x;
    s16 y = src->y;
    s16 z = src->z;

    dest->x = (s16)(x + 0.f + 0.f);
    dest->y = (s16)(0.f + (y * cos) - (z * sin));
    dest->z = (s16)(0.f + (y * sin) + (z * cos));
}

void CustomMath_Vec3s_RotateByY(Vec3s* src, s16 rotAngle, Vec3s* dest) {
    f32 sin = Math_SinS(rotAngle);
    f32 cos = Math_CosS(rotAngle);
    s16 x = src->x;
    s16 y = src->y;
    s16 z = src->z;

    dest->x = (s16)((x * cos) + 0.f + (z * sin));
    dest->y = (s16)(0.f + y + 0.f);
    dest->z = (s16)((-x * sin) + 0.f + (z * cos));
}

void CustomMath_Vec3s_RotateByZ(Vec3s* src, s16 rotAngle, Vec3s* dest) {
    f32 sin = Math_SinS(rotAngle);
    f32 cos = Math_CosS(rotAngle);

    s16 x = src->x;
    s16 y = src->y;
    s16 z = src->z;

    dest->x = (s16)((x * cos) - (y * sin) + 0.f);
    dest->y = (s16)((x * sin) + (y * cos) + 0.f);
    dest->z = (s16)(0.f + 0.f + z);
}

void CustomMath_Vec3s_Rotate(Vec3s* src, Vec3s* rotAngle, Vec3s* dest) {
    CustomMath_Vec3s_RotateByY(src, rotAngle->y, dest);
    CustomMath_Vec3s_RotateByX(dest, rotAngle->x, dest);
    CustomMath_Vec3s_RotateByZ(dest, rotAngle->z, dest);
}

/*
=================
Vec3f (f32) rotation operations 
=================
*/

void CustomMath_Vec3f_RotateByX(Vec3f* src, s16 rotAngle, Vec3f* dest) {
    f32 sin = Math_SinS(rotAngle);
    f32 cos = Math_CosS(rotAngle);

    f32 x = src->x;
    f32 y = src->y;
    f32 z = src->z;

    dest->x = (f32)(x + 0.f + 0.f);
    dest->y = (f32)(0.f + (y * cos) - (z * sin));
    dest->z = (f32)(0.f + (y * sin) + (z * cos));
}

void CustomMath_Vec3f_RotateByY(Vec3f* src, s16 rotAngle, Vec3f* dest) {
    f32 sin = Math_SinS(rotAngle);
    f32 cos = Math_CosS(rotAngle);

    f32 x = src->x;
    f32 y = src->y;
    f32 z = src->z;

    dest->x = (f32)((x * cos) + 0.f + (z * sin));
    dest->y = (f32)(0.f + y + 0.f);
    dest->z = (f32)((-x * sin) + 0.f + (z * cos));
}

void CustomMath_Vec3f_RotateByZ(Vec3f* src, s16 rotAngle, Vec3f* dest) {
    f32 sin = Math_SinS(rotAngle);
    f32 cos = Math_CosS(rotAngle);

    f32 x = src->x;
    f32 y = src->y;
    f32 z = src->z;

    dest->x = (f32)((x * cos) - (y * sin) + 0.f);
    dest->y = (f32)((x * sin) + (y * cos) + 0.f);
    dest->z = (f32)(0.f + 0.f + z);
}

void CustomMath_Vec3f_Rotate(Vec3f* src, Vec3s* rotAngle, Vec3f* dest) {
    CustomMath_Vec3f_RotateByX(src, rotAngle->x, dest);
    CustomMath_Vec3f_RotateByY(dest, rotAngle->y, dest);
    CustomMath_Vec3f_RotateByZ(dest, rotAngle->z, dest);
}

void CustomMath_Vec3f_InverseTransformDirection(MtxF* matrix, Vec3f* worldDir, Vec3f* localDir) {
    localDir->x = (matrix->xx * worldDir->x) + (matrix->yx * worldDir->y) + (matrix->zx * worldDir->z);
    localDir->y = (matrix->xy * worldDir->x) + (matrix->yy * worldDir->y) + (matrix->zy * worldDir->z);
    localDir->z = (matrix->xz * worldDir->x) + (matrix->yz * worldDir->y) + (matrix->zz * worldDir->z);
}


/*
=================
Degree and Binary Angle Conversion
=================
*/

s16 CustomMath_DegreeToBinaryAngle(f32 degrees) {
    s16 binaryAngle = (s16)(degrees * (32768.f / 180.f));
    return binaryAngle;
}


/*
=================
Direction Manipulation in 3D World Space
=================
*/

/**
 * @brief Normalize a direction vector if it is long enough.
 *
 * Note: Leave the vector unchanged if its squared length is below 0.00000001f.
 *
 * @param targetVec Direction vector to normalize
 *
 * @return 1 if normalized, otherwise 0
 */
u8 CustomMath_NormalizeSubstepDirectionVec(Vec3f* targetVec) {
    // Find length of target direction vector with dot product of itself (basically squaring the length of the 
    // vector).
    f32 lengthSq = Math3D_Vec3fMagnitudeSq(targetVec);

    // Treat extremely small values that's very close to 0 as just 0.
    // Return "false" if the length of the target vector is basically 0.
    if (lengthSq < 0.00000001f) {
        return 0;
    }

    // Otherwise, scale the vector by 1/length of vector so that its final length becomes 1.
    Math_Vec3f_Scale(targetVec, 1.f / sqrtf(lengthSq));
    return 1;
}


/**
 * @brief Rotate the target point about the pivot point by a given angle along the y-axis.
 * 
 * The 'targetPoint' param will get overwritten with a new, rotated position of old 'targetPoint'
 *
 * @param targetPoint   World position to rotate and update in place
 * @param pivotPoint    World position around which the point rotates
 * @param rotAngle      Rotate targetPoint around pivotPoint by this value
 */
void CustomMath_RotatePointAroundY(Vec3f* targetPoint, Vec3f* pivotPoint, s16 rotAngle) {
    // Find the line between targetPoint and pivotPoint
    Vec3f offset = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(targetPoint, pivotPoint, &offset);
	
	// Rotate that line by the input angle
    CustomMath_Vec3f_RotateByY(&offset, rotAngle, &offset);
	
	// Add that rotated line on to pivotPoint to get new targetPoint (saved over old targetPoint)
    Math_Vec3f_Sum(pivotPoint, &offset, targetPoint);
}


/**
 * @brief Interpolate a position while accounting for pivot movement and yaw.
 * 
 * This function uses the previous frame and the current frame to calculate the new target position in the 
 * current substep (indicated by t) without applying verlet integration. 
 * This function was designed to consider yaw rotation AND other animation-based change in position when 
 * calculating the colliders' positions in the current substep.
 * 
 * Note that this function is specifically for rotating colliders and phys limbs that are to skip verlet 
 * integration (i.e. pinned phys limbs). This function is not meant to be used for unpinned phys limbs.
 *
 * @param prevFramePos          Position in the previous frame
 * @param currFrameTargetPos    Target position in the current frame
 * @param prevFramePivotPoint   Pivot position in the previous frame
 * @param currFramePivotPoint   Pivot position in the current frame
 * @param yawRotation           Total yaw change expressed as a binary angle
 * @param t                     Interpolation fraction (current substep / # of substeps)
 * @param currSubstepNewPos     Output interpolated position
 */
void CustomMath_InterpolatePosWithRot(Vec3f* prevFramePos, Vec3f* currFrameTargetPos, 
    Vec3f* prevFramePivotPoint, Vec3f* currFramePivotPoint, s16 yawRotation, f32 t, Vec3f* currSubstepNewPos) {
    /********************************
     Calculate Target Phys Object's Position at Current Substep Due to Yaw
    ********************************/
	// Find the current substep's pivot point.
    Vec3f substepPivotPoint = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(prevFramePivotPoint, currFramePivotPoint, t, &substepPivotPoint);

    // In previous frame, find the line segment between target phys object's world position and the
	// pivot point.
    Vec3f prevFrameOffset = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(prevFramePos, prevFramePivotPoint, &prevFrameOffset);

    // Rotate that line segment by the given yaw rotation scaled down to current substep (t)
    Vec3f prevFrameOffsetRotated = prevFrameOffset;
    CustomMath_Vec3f_RotateByY(&prevFrameOffset, (s16)(yawRotation * t), &prevFrameOffsetRotated);

    /********************************
     Calculate Target Phys Object's Position at Current Substep Due to Other Movements
    ********************************/
	// Movements outside of yaw like crouching may impact position of target phys object at substep.
	
	// Calculate the full target rotation at the end of substeps between prev and current frame (t = 1)
    Vec3f currFrameTargetOffsetRotated = prevFrameOffset;
    CustomMath_Vec3f_RotateByY(&prevFrameOffset, yawRotation * 1, &currFrameTargetOffsetRotated);

    // Using the target rotation by end of substeps, calculate where the target phys object is supposed 
	// to end up at current frame (t = 1).
    Vec3f predictedEndPosFromYaw = {0.f, 0.f, 0.f};
    Math_Vec3f_Sum(currFramePivotPoint, &currFrameTargetOffsetRotated, &predictedEndPosFromYaw);

    // Take into account of other animation (bending, crouching, etc.) in current frame.
	// Calculate the difference from current frame's target position impacted by current animation
	// and where the current frame's target position would have been just from Jackie's yaw rotation.
    Vec3f residual = {0.f, 0.f, 0.f};
    Math_Vec3f_Diff(currFrameTargetPos, &predictedEndPosFromYaw, &residual);

    // Scale that difference by the current substep's value.
    Math_Vec3f_Scale(&residual, t);

    /********************************
     Combine the Change in Position in Current Substep based on the two
    ********************************/
	// From the current substep's pivot point world position, find the point where the phys object is
	// currently ending up in based on the yaw alone.
    Math_Vec3f_Sum(&substepPivotPoint, &prevFrameOffsetRotated, currSubstepNewPos);
	
	// From that position resulted just from the yaw, find the world position where the phys object is
	// in the current substep that is caused by the other animations.
    Math_Vec3f_Sum(currSubstepNewPos, &residual, currSubstepNewPos);
}


/**
 * @brief Extract and normalize directional axes using the given matrix mapped to a limb.
 *
 * Matrix local +Y becomes forward, +Z becomes left, and -X becomes down.
 * Output vectors must refer to separate Vec3f objects.
 *
 * If an axis is too short, return 0.
 *
 * @param mtx         Matrix containing the target limb's world orientation
 * @param forwardAxis Output forward direction
 * @param leftAxis    Output left direction
 * @param downAxis    Output down direction
 *
 * @return 1 if all three axes were normalized, otherwise 0
 */
u8 CustomMath_CaptureLimitAxes(MtxF* mtx, Vec3f* forwardAxis, Vec3f* leftAxis, Vec3f* downAxis) {
    // Convert the matrix values into Vec3f directional axes relative to target limb matrix is oriented to
    *forwardAxis = (Vec3f){mtx->xy, mtx->yy, mtx->zy};
    *leftAxis = (Vec3f){mtx->xz, mtx->yz, mtx->zz};
    *downAxis = (Vec3f){-mtx->xx, -mtx->yx, -mtx->zx};

    // Calculate the lengths of the direcitonal axes vectors
    f32 forwardLength = sqrtf(Math3D_Vec3fMagnitudeSq(forwardAxis));
    f32 leftLength = sqrtf(Math3D_Vec3fMagnitudeSq(leftAxis));
    f32 downLength = sqrtf(Math3D_Vec3fMagnitudeSq(downAxis));

    // Flag variable that indicates that the directional axes for target physics object
    if (forwardLength < 0.000001f ||
        leftLength < 0.000001f ||
        downLength < 0.000001f) {
        return 0;
    }

    // Normalize the directional axes of the overcoat.
    Math_Vec3f_Scale(forwardAxis, 1.f / forwardLength);
    Math_Vec3f_Scale(leftAxis, 1.f / leftLength);
    Math_Vec3f_Scale(downAxis, 1.f / downLength);

    // Return flag variable that indicates that the directional axes for target physics object has been set
    // up now.
    return 1;
}


/**
 * @brief Calculate directional axes for the current physics substep.
 * 
 * This function interpolates the previous frame's forward and down directions towards the current frame's 
 * target directions based on the current substep interpolation value.
 * 
 * After interpolation, the down direction is normalized. After that, the forward direction is corrected 
 * so that it is perpendicular to the down direction. Then, the forward direction is normalized also.
 * 
 * If either interpolated direction is unable to be normalized, then the function falls back to the current 
 * frame's target directions.
 *
 * The left direction is calculated using the cross product of the forward and down directions. From this, we 
 * get the forward, down, and left directions in world space for this current substep.
 *
 * Output vectors must refer to separate Vec3f objects.
 *
 * @param previousFrameForward  Previous frame's forward direction
 * @param previousFrameDown     Previous frame's down direction
 * @param targetForward         Current frame's normalized target forward direction
 * @param targetDown            Current frame's normalized target down direction, perpendicular to targetForward
 * @param t                     Interpolation fraction, normally between 0 and 1
 * @param forwardAxis           Output substep forward direction
 * @param leftAxis              Output substep left direction
 * @param downAxis              Output substep down direction
 */
void CustomMath_SetSubstepDirAxes(Vec3f previousFrameForward, Vec3f previousFrameDown, Vec3f targetForward, 
    Vec3f targetDown, f32 t, Vec3f* forwardAxis, Vec3f* leftAxis, Vec3f* downAxis) {
    // Interpolate overcoat's down direction from previous frame toward current frame's target down
	// direction based on current substep.
    Vec3f downDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&previousFrameDown, &targetDown, t, &downDirection);

    // Interpolate overcoat's forward direction from previous frame toward current frame's target forward 
    // direction based on the current substep.
    Vec3f forwardDirection = {0.f, 0.f, 0.f};
    CustomMath_Vec3f_Lerp(&previousFrameForward, &targetForward, t, &forwardDirection);

    // Normalize the interpolated down direction
    // If the down direction is too small to normalize, then use the final target down direction instead.
    if (!CustomMath_NormalizeSubstepDirectionVec(&downDirection)) {
        Math_Vec3f_Copy(&downDirection, &targetDown);
    }

    // Remove the part of forward that points along down.
    f32 projection = CustomMath_Vec3f_Dot(&forwardDirection, &downDirection);

    // Calculate the part of the forward direction that lies along the down direction.
    Vec3f forwardDownComponent = {0.f, 0.f, 0.f};
    Math_Vec3f_ScaleAndStore(&downDirection, projection, &forwardDownComponent);
    
    // Remove that component so forward becomes perpendicular to down.
    Math_Vec3f_Diff(&forwardDirection, &forwardDownComponent, &forwardDirection);

    // Near-opposite directions may produce an unusable interpolated forward.
    if (!CustomMath_NormalizeSubstepDirectionVec(&forwardDirection)) {
        Math_Vec3f_Copy(forwardAxis, &targetForward);
        Math_Vec3f_Copy(downAxis, &targetDown);
    }
    // Otherwise, store the interpolated and corrected substep directions 
    else {
        Math_Vec3f_Copy(forwardAxis, &forwardDirection);
        Math_Vec3f_Copy(downAxis, &downDirection);
    }

    // Calculate the left direction from the forward and down directions.
    // The directional coordinate axes are now made: forward, down, and left
    Math3D_Vec3f_Cross(forwardAxis, downAxis, leftAxis);
}