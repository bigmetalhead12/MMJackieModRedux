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