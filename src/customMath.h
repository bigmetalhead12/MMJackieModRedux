/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

DESC:
    Some vec3s/vec3f functions I made for cases where MM's math library
    didn't provide them

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

// Dot product
f32 CustomMath_Vec3f_Dot(Vec3f* a, Vec3f* b);

// Normalize
void CustomMath_Vec3f_Normalize(Vec3f* src, Vec3f* dest);

// Linearly interpolate between two Vec3f values
void CustomMath_Vec3f_Lerp(Vec3f* pointA, Vec3f* pointB, f32 t, Vec3f* dest);

// Vector s16 operations
void CustomMath_Vec3s_Scale_ToVec3f(Vec3s* target, f32 scale, Vec3f* dest);

// Rotate Vec3s
void CustomMath_Vec3s_RotateByX(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_RotateByY(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_RotateByZ(Vec3s* src, s16 rotAngle, Vec3s* dest);
void CustomMath_Vec3s_Rotate(Vec3s* src, Vec3s* rotAngle, Vec3s* dest);

// Rotate Vec3f
void CustomMath_Vec3f_RotateByX(Vec3f* src, s16 rotAngle, Vec3f* dest);
void CustomMath_Vec3f_RotateByY(Vec3f* src, s16 rotAngle, Vec3f* dest);
void CustomMath_Vec3f_RotateByZ(Vec3f* src, s16 rotAngle, Vec3f* dest);
void CustomMath_Vec3f_Rotate(Vec3f* src, Vec3s* rotAngle, Vec3f* dest);

// Transform Direction
void CustomMath_Vec3f_InverseTransformDirection(MtxF* matrix, Vec3f* worldDir, Vec3f* localDir);

#endif