/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

#ifndef CUSTOMMATH_H
#define CUSTOMMATH_H

#include "ultra64.h"
#include "global.h"
#include "math.h"

/***********************************************************************

	Custom Math Functions

***********************************************************************/

// Pitch, Yaw, and Roll
// Designed to calculate bone x-axis rotations beyond -90/+90 degrees from origin
s16 CustomMath_Vec3f_Pitch(Vec3f* b, Vec3f* a);

// Designed to calculate bone y-axis rotations beyond -90/+90 degrees from origin
s16 CustomMath_Vec3f_Yaw(Vec3f* a, Vec3f* b);

// Designed to calculate bone z-axis rotations 
s16 CustomMath_Vec3f_Roll(Vec3f* a, Vec3f* b);


// Vector s16 operations
void CustomMath_Vec3s_Sum(Vec3s* l, Vec3s* r, Vec3s* dest);
void CustomMath_Vec3s_Diff(Vec3s* l, Vec3s* r, Vec3s* dest);
void CustomMath_Vec3s_Scale_ToVec3f(Vec3s* target, f32 scale, Vec3f* dest);

// Rotate Vec3s
void CustomMath_Vec3s_RotateByX(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_RotateByY(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_RotateByZ(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_Rotate(Vec3s* src, Vec3s* rotAngle, Vec3s* dest);
void CustomMath_Vec3s_RotateInverse(Vec3s* src, Vec3s* rotAngle, Vec3s* dest);

// Rotate Vec3f
void CustomMath_Vec3f_RotateByX(Vec3f* src, s16 rotAngle, Vec3f* dest);
void CustomMath_Vec3f_RotateByY(Vec3f* src, s16 rotAngle, Vec3f* dest);
void CustomMath_Vec3f_RotateByZ(Vec3f* src, s16 rotAngle, Vec3f* dest);
void CustomMath_Vec3f_Rotate(Vec3f* src, Vec3s* rotAngle, Vec3f* dest);
void CustomMath_Vec3f_InverseRotate(Vec3f* src, Vec3s* rotAngle, Vec3f* dest);

#endif