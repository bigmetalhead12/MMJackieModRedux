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
Pitch, Yaw, and Roll
=================
*/

// Designed to calculate bone x-axis rotations beyond -90/+90 degrees from origin
s16 CustomMath_Vec3f_Pitch(Vec3f* b, Vec3f* a) {
    return Math_Atan2S_XY( b->z - a->z, b->y - a->y);
}

// UNUSED
s16 CustomMath_Vec3f_Yaw(Vec3f* a, Vec3f* b, Player* player) {
    // If vector b's z position is >= vector a's z position, proceed as is
    if (b->x >= a->x) {
        if (b->y >= a->y) {
            return Math_Vec3f_Yaw(a, b);
        }
        return -Math_Vec3f_Yaw(a, b);
    }
    // Else, if b's z position is > a's z position, then apply offset
    else {
        if (b->y >= a->y) {
            return Math_Vec3f_Yaw(a, b);
        }
        return -Math_Vec3f_Yaw(a, b);
    }
        
}

// Designed to calculate bone z-axis rotations 
s16 CustomMath_Vec3f_Roll(Vec3f* a, Vec3f* b) {
    f32 x_dist = b->x - a->x;
    f32 y_dist = b->y - a->y;
    f32 z_dist = b->z - a->z;
    u16 low_offset = 32768;    // -180 degrees
    if (z_dist <= 0) {
        if (b->y > 0) {
            return (s16)(low_offset - (u16)(Math_Atan2S_XY(y_dist, x_dist)));
        }
        return (s16)(low_offset+Math_Atan2S_XY(y_dist, x_dist));
    }
    else {
        if (b->y > 0) {
            return (s16)(low_offset - (u16)(Math_Atan2S_XY(y_dist, x_dist)));
        }
        return (s16)(low_offset+Math_Atan2S_XY(y_dist, x_dist));
    }
}

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
Finding Closest Point in Line Segment
=================
*/

f32 CustomMath_Clamp(f32 val, f32 min, f32 max) {
    if (val < min) {
        return min;
    }
    else if (val > max) {
        return max;
    }
    else {
        return val;
    }
}

// Find point from line segment closest to target point
void CustomMath_Vec3f_ClosestPoint(Vec3f* src, Vec3f* pointA, Vec3f* pointB, Vec3f* dest) {
    // Line segment between point A and point B
    Vec3f segment = { (pointB->x - pointA->x), (pointB->y - pointA->y), (pointB->z - pointA->z) };

    // Source vec3f point's offset from point A
    Vec3f center_offset_from_a = { (src->x - pointA->x), (src->y - pointA->y), (src->z - pointA->z) };

    // Length of segment squared
    f32 segment_length_squared = (segment.x * segment.x) + (segment.y * segment.y) + (segment.z * segment.z);

    // Scalar value
    f32 t = ((center_offset_from_a.x * segment.x) + (center_offset_from_a.y * segment.y) + (center_offset_from_a.z * segment.z))/segment_length_squared;

    // Clamp
    t = CustomMath_Clamp(t, 0.0f, 1.0f);

    // Calculate closest point with clamp
    Vec3f closest_point = { (pointA->x + (segment.x * t)), (pointA->y + (segment.y * t)), (pointA->z + (segment.z * t)) };

    Math_Vec3f_Copy(dest, &closest_point);
}


/*
=================
Vec3s (s16) math operations 
=================
*/

// Vec3s version for Vector Sum
void CustomMath_Vec3s_Sum(Vec3s* l, Vec3s* r, Vec3s* dest) {
    dest->x = l->x + r->x;
    dest->y = l->y + r->y;
    dest->z = l->z + r->z;
}

// Vec3s version for Vector Difference
void CustomMath_Vec3s_Diff(Vec3s* l, Vec3s* r, Vec3s* dest) {
    dest->x = l->x - r->x;
    dest->y = l->y - r->y;
    dest->z = l->z - r->z;
}

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
    CustomMath_Vec3s_RotateByX(src, rotAngle->x, dest);
    CustomMath_Vec3s_RotateByY(dest, rotAngle->y, dest);
    CustomMath_Vec3s_RotateByZ(dest, rotAngle->z, dest);
}

void CustomMath_Vec3s_RotateInverse(Vec3s* src, Vec3s* rotAngle, Vec3s* dest) {
    CustomMath_Vec3s_RotateByZ(src, -(rotAngle->z), dest);
    CustomMath_Vec3s_RotateByY(dest, -(rotAngle->y), dest);
    CustomMath_Vec3s_RotateByX(dest, -(rotAngle->x), dest);
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

void CustomMath_Vec3f_InverseRotate(Vec3f* src, Vec3s* rotAngle, Vec3f* dest) {
    Vec3f tempZ = { 0.0f, 0.0f, 0.0f };
    Vec3f tempY = { 0.0f, 0.0f, 0.0f };

    // Reverse the forward X -> Y -> Z operation.
    CustomMath_Vec3f_RotateByZ(src, -rotAngle->z, &tempZ);
    CustomMath_Vec3f_RotateByY(&tempZ,-rotAngle->y,&tempY);
    CustomMath_Vec3f_RotateByX(&tempY, -rotAngle->x, dest);
}