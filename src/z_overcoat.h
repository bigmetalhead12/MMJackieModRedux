/*
========================================================================

Jackie Playermodel Redux

by BigMetalHead12

2026

========================================================================
*/

#ifndef Z_OVERCOAT_H
#define Z_OVERCOAT_H

#include "ultra64.h"
#include "global.h"
#include "gOvercoatSkel.h"
#include "verlet_physics.h"
#include "physics_collision.h"


/***********************************************************************

	Overcoat Physics

***********************************************************************/
// Set up physics object that carries velocity and rotation of overcoat actor
PhysPlayer gOvercoatPhysPlayer;

// StandardLimb of overcoat's model generated through Fast 64 (from gOvercoatSkel)
extern StandardLimb gOvercoatSkelLimb_000;      // Root
extern StandardLimb gOvercoatSkelLimb_001;
extern StandardLimb gOvercoatSkelLimb_002;
extern StandardLimb gOvercoatSkelLimb_003;
extern StandardLimb gOvercoatSkelLimb_004;
extern StandardLimb gOvercoatSkelLimb_005;
extern StandardLimb gOvercoatSkelLimb_006;
extern StandardLimb gOvercoatSkelLimb_007;
extern StandardLimb gOvercoatSkelLimb_008;
extern StandardLimb gOvercoatSkelLimb_009;
extern StandardLimb gOvercoatSkelLimb_010;
extern StandardLimb gOvercoatSkelLimb_011;
extern StandardLimb gOvercoatSkelLimb_012;
extern StandardLimb gOvercoatSkelLimb_013;
extern StandardLimb gOvercoatSkelLimb_014;
extern StandardLimb gOvercoatSkelLimb_015;
extern StandardLimb gOvercoatSkelLimb_016;
extern StandardLimb gOvercoatSkelLimb_017;
extern StandardLimb gOvercoatSkelLimb_018;
extern StandardLimb gOvercoatSkelLimb_019;
extern StandardLimb gOvercoatSkelLimb_020;
extern StandardLimb gOvercoatSkelLimb_021;
extern StandardLimb gOvercoatSkelLimb_022;
extern StandardLimb gOvercoatSkelLimb_023;
extern StandardLimb gOvercoatSkelLimb_024;
extern StandardLimb gOvercoatSkelLimb_025;
extern StandardLimb gOvercoatSkelLimb_026;
extern StandardLimb gOvercoatSkelLimb_027;
extern StandardLimb gOvercoatSkelLimb_028;
extern StandardLimb gOvercoatSkelLimb_029;
extern StandardLimb gOvercoatSkelLimb_030;
extern StandardLimb gOvercoatSkelLimb_031;
extern StandardLimb gOvercoatSkelLimb_032;
extern StandardLimb gOvercoatSkelLimb_033;
extern StandardLimb gOvercoatSkelLimb_034;
extern StandardLimb gOvercoatSkelLimb_035;
extern StandardLimb gOvercoatSkelLimb_036;
extern StandardLimb gOvercoatSkelLimb_037;
extern StandardLimb gOvercoatSkelLimb_038;
extern StandardLimb gOvercoatSkelLimb_039;
extern StandardLimb gOvercoatSkelLimb_040;
extern StandardLimb gOvercoatSkelLimb_041;
extern StandardLimb gOvercoatSkelLimb_042;
extern StandardLimb gOvercoatSkelLimb_043;
extern StandardLimb gOvercoatSkelLimb_044;
extern StandardLimb gOvercoatSkelLimb_045;
extern StandardLimb gOvercoatSkelLimb_046;
extern StandardLimb gOvercoatSkelLimb_047;
extern StandardLimb gOvercoatSkelLimb_048;
extern StandardLimb gOvercoatSkelLimb_049;
extern StandardLimb gOvercoatSkelLimb_050;
extern StandardLimb gOvercoatSkelLimb_051;
extern StandardLimb gOvercoatSkelLimb_052;
extern StandardLimb gOvercoatSkelLimb_053;
extern StandardLimb gOvercoatSkelLimb_054;
extern StandardLimb gOvercoatSkelLimb_055;
extern StandardLimb gOvercoatSkelLimb_056;
extern StandardLimb gOvercoatSkelLimb_057;
extern StandardLimb gOvercoatSkelLimb_058;
extern StandardLimb gOvercoatSkelLimb_059;
extern StandardLimb gOvercoatSkelLimb_060;
extern StandardLimb gOvercoatSkelLimb_061;
extern StandardLimb gOvercoatSkelLimb_062;
extern StandardLimb gOvercoatSkelLimb_063;
extern StandardLimb gOvercoatSkelLimb_064;
extern StandardLimb gOvercoatSkelLimb_065;
extern StandardLimb gOvercoatSkelLimb_066;
extern StandardLimb gOvercoatSkelLimb_067;
extern StandardLimb gOvercoatSkelLimb_068;
extern StandardLimb gOvercoatSkelLimb_069;
extern StandardLimb gOvercoatSkelLimb_070;
extern StandardLimb gOvercoatSkelLimb_071;
extern StandardLimb gOvercoatSkelLimb_072;
extern StandardLimb gOvercoatSkelLimb_073;
extern StandardLimb gOvercoatSkelLimb_074;
extern StandardLimb gOvercoatSkelLimb_075;
extern StandardLimb gOvercoatSkelLimb_076;
extern StandardLimb gOvercoatSkelLimb_077;
extern StandardLimb gOvercoatSkelLimb_078;
extern StandardLimb gOvercoatSkelLimb_079;
extern StandardLimb gOvercoatSkelLimb_080;
extern StandardLimb gOvercoatSkelLimb_081;
extern StandardLimb gOvercoatSkelLimb_082;
extern StandardLimb gOvercoatSkelLimb_083;
extern StandardLimb gOvercoatSkelLimb_084;
extern StandardLimb gOvercoatSkelLimb_085;
extern StandardLimb gOvercoatSkelLimb_086;
extern StandardLimb gOvercoatSkelLimb_087;
extern StandardLimb gOvercoatSkelLimb_088;
extern StandardLimb gOvercoatSkelLimb_089;
extern StandardLimb gOvercoatSkelLimb_090;
extern StandardLimb gOvercoatSkelLimb_091;
extern StandardLimb gOvercoatSkelLimb_092;
extern StandardLimb gOvercoatSkelLimb_093;
extern StandardLimb gOvercoatSkelLimb_094;
extern StandardLimb gOvercoatSkelLimb_095;
extern StandardLimb gOvercoatSkelLimb_096;
extern StandardLimb gOvercoatSkelLimb_097;
extern StandardLimb gOvercoatSkelLimb_098;
extern StandardLimb gOvercoatSkelLimb_099;
extern StandardLimb gOvercoatSkelLimb_100;
extern StandardLimb gOvercoatSkelLimb_101;
extern StandardLimb gOvercoatSkelLimb_102;
extern StandardLimb gOvercoatSkelLimb_103;
extern StandardLimb gOvercoatSkelLimb_104;
extern StandardLimb gOvercoatSkelLimb_105;
extern StandardLimb gOvercoatSkelLimb_106;
extern StandardLimb gOvercoatSkelLimb_107;
extern StandardLimb gOvercoatSkelLimb_108;
extern StandardLimb gOvercoatSkelLimb_109;
extern StandardLimb gOvercoatSkelLimb_110;
extern StandardLimb gOvercoatSkelLimb_111;
extern StandardLimb gOvercoatSkelLimb_112;
extern StandardLimb gOvercoatSkelLimb_113;
extern StandardLimb gOvercoatSkelLimb_114;
extern StandardLimb gOvercoatSkelLimb_115;
extern StandardLimb gOvercoatSkelLimb_116;
extern StandardLimb gOvercoatSkelLimb_117;
extern StandardLimb gOvercoatSkelLimb_118;
extern StandardLimb gOvercoatSkelLimb_119;
extern StandardLimb gOvercoatSkelLimb_120;
extern StandardLimb gOvercoatSkelLimb_121;
extern StandardLimb gOvercoatSkelLimb_122;
extern StandardLimb gOvercoatSkelLimb_123;
extern StandardLimb gOvercoatSkelLimb_124;
extern StandardLimb gOvercoatSkelLimb_125;
extern StandardLimb gOvercoatSkelLimb_126;
extern StandardLimb gOvercoatSkelLimb_127;
extern StandardLimb gOvercoatSkelLimb_128;
extern StandardLimb gOvercoatSkelLimb_129;
extern StandardLimb gOvercoatSkelLimb_130;
extern StandardLimb gOvercoatSkelLimb_131;
extern StandardLimb gOvercoatSkelLimb_132;
extern StandardLimb gOvercoatSkelLimb_133;
extern StandardLimb gOvercoatSkelLimb_134;
extern StandardLimb gOvercoatSkelLimb_135;
extern StandardLimb gOvercoatSkelLimb_136;
extern StandardLimb gOvercoatSkelLimb_137;
extern StandardLimb gOvercoatSkelLimb_138;
extern StandardLimb gOvercoatSkelLimb_139;
extern StandardLimb gOvercoatSkelLimb_140;
extern StandardLimb gOvercoatSkelLimb_141;
extern StandardLimb gOvercoatSkelLimb_142;
extern StandardLimb gOvercoatSkelLimb_143;
extern StandardLimb gOvercoatSkelLimb_144;
extern StandardLimb gOvercoatSkelLimb_145;
extern StandardLimb gOvercoatSkelLimb_146;
extern StandardLimb gOvercoatSkelLimb_147;
extern StandardLimb gOvercoatSkelLimb_148;
extern StandardLimb gOvercoatSkelLimb_149;
extern StandardLimb gOvercoatSkelLimb_150;
extern StandardLimb gOvercoatSkelLimb_151;
extern StandardLimb gOvercoatSkelLimb_152;
extern StandardLimb gOvercoatSkelLimb_153;
extern StandardLimb gOvercoatSkelLimb_154;
extern StandardLimb gOvercoatSkelLimb_155;
extern StandardLimb gOvercoatSkelLimb_156;
extern StandardLimb gOvercoatSkelLimb_157;
extern StandardLimb gOvercoatSkelLimb_158;
extern StandardLimb gOvercoatSkelLimb_159;
extern StandardLimb gOvercoatSkelLimb_160;
extern StandardLimb gOvercoatSkelLimb_161;
extern StandardLimb gOvercoatSkelLimb_162;
extern StandardLimb gOvercoatSkelLimb_163;
extern StandardLimb gOvercoatSkelLimb_164;
extern StandardLimb gOvercoatSkelLimb_165;
extern StandardLimb gOvercoatSkelLimb_166;
extern StandardLimb gOvercoatSkelLimb_167;
extern StandardLimb gOvercoatSkelLimb_168;
extern StandardLimb gOvercoatSkelLimb_169;
extern StandardLimb gOvercoatSkelLimb_170;
extern StandardLimb gOvercoatSkelLimb_171;
extern StandardLimb gOvercoatSkelLimb_172;
extern StandardLimb gOvercoatSkelLimb_173;
extern StandardLimb gOvercoatSkelLimb_174;
extern StandardLimb gOvercoatSkelLimb_175;
extern StandardLimb gOvercoatSkelLimb_176;
extern StandardLimb gOvercoatSkelLimb_177;
extern StandardLimb gOvercoatSkelLimb_178;
extern StandardLimb gOvercoatSkelLimb_179;
extern StandardLimb gOvercoatSkelLimb_180;
extern StandardLimb gOvercoatSkelLimb_181;
extern StandardLimb gOvercoatSkelLimb_182;
extern StandardLimb gOvercoatSkelLimb_183;
extern StandardLimb gOvercoatSkelLimb_184;
extern StandardLimb gOvercoatSkelLimb_185;
extern StandardLimb gOvercoatSkelLimb_186;
extern StandardLimb gOvercoatSkelLimb_187;
extern StandardLimb gOvercoatSkelLimb_188;
extern StandardLimb gOvercoatSkelLimb_189;
extern StandardLimb gOvercoatSkelLimb_190;
extern StandardLimb gOvercoatSkelLimb_191;
extern StandardLimb gOvercoatSkelLimb_192;
extern StandardLimb gOvercoatSkelLimb_193;
extern StandardLimb gOvercoatSkelLimb_194;
extern StandardLimb gOvercoatSkelLimb_195;

// Enum for indicating overcoat's skeleton limbs
typedef enum OvercoatBodyPart {
    OVERCOAT_BODYPART_ROOT,     // Root
    OVERCOAT_BODYPART_BACKLIMB_A,        // Back limb A
    OVERCOAT_BODYPART_BACKLIMB_B,
    OVERCOAT_BODYPART_BACKLIMB_C,
    OVERCOAT_BODYPART_BACKLIMB_D,
    OVERCOAT_BODYPART_BACKLIMB_E,
    OVERCOAT_BODYPART_BACKLIMB_F,
    OVERCOAT_BODYPART_BACKLIMB_G,
    OVERCOAT_BODYPART_BACKLIMB_H,
    OVERCOAT_BODYPART_BACKLIMB_I,
    OVERCOAT_BODYPART_LEFTLIMB0_A,      // Left Limb 0 A
    OVERCOAT_BODYPART_LEFTLIMB0_B,
    OVERCOAT_BODYPART_LEFTLIMB0_C,
    OVERCOAT_BODYPART_LEFTLIMB0_D,
    OVERCOAT_BODYPART_LEFTLIMB0_E,
    OVERCOAT_BODYPART_LEFTLIMB0_F,
    OVERCOAT_BODYPART_LEFTLIMB0_G,
    OVERCOAT_BODYPART_LEFTLIMB0_H,
    OVERCOAT_BODYPART_LEFTLIMB0_I,
    OVERCOAT_BODYPART_LEFTLIMB1_A,      // Left Limb 1 A
    OVERCOAT_BODYPART_LEFTLIMB1_B,
    OVERCOAT_BODYPART_LEFTLIMB1_C,
    OVERCOAT_BODYPART_LEFTLIMB1_D,
    OVERCOAT_BODYPART_LEFTLIMB1_E,
    OVERCOAT_BODYPART_LEFTLIMB1_F,
    OVERCOAT_BODYPART_LEFTLIMB1_G,
    OVERCOAT_BODYPART_LEFTLIMB1_H,
    OVERCOAT_BODYPART_LEFTLIMB1_I,
    OVERCOAT_BODYPART_LEFTLIMB2_A,      // Left Limb 2 A
    OVERCOAT_BODYPART_LEFTLIMB2_B,
    OVERCOAT_BODYPART_LEFTLIMB2_C,
    OVERCOAT_BODYPART_LEFTLIMB2_D,
    OVERCOAT_BODYPART_LEFTLIMB2_E,
    OVERCOAT_BODYPART_LEFTLIMB2_F,
    OVERCOAT_BODYPART_LEFTLIMB2_G,
    OVERCOAT_BODYPART_LEFTLIMB2_H,
    OVERCOAT_BODYPART_LEFTLIMB2_I,
    OVERCOAT_BODYPART_LEFTLIMB3_A,      // Left Limb 3 A
    OVERCOAT_BODYPART_LEFTLIMB3_B,
    OVERCOAT_BODYPART_LEFTLIMB3_C,
    OVERCOAT_BODYPART_LEFTLIMB3_D,
    OVERCOAT_BODYPART_LEFTLIMB3_E,
    OVERCOAT_BODYPART_LEFTLIMB3_F,
    OVERCOAT_BODYPART_LEFTLIMB3_G,
    OVERCOAT_BODYPART_LEFTLIMB3_H,
    OVERCOAT_BODYPART_LEFTLIMB3_I,
    OVERCOAT_BODYPART_LEFTLIMB4_A,      // Left Limb 4 A
    OVERCOAT_BODYPART_LEFTLIMB4_B,
    OVERCOAT_BODYPART_LEFTLIMB4_C,
    OVERCOAT_BODYPART_LEFTLIMB4_D,
    OVERCOAT_BODYPART_LEFTLIMB4_E,
    OVERCOAT_BODYPART_LEFTLIMB4_F,
    OVERCOAT_BODYPART_LEFTLIMB4_G,
    OVERCOAT_BODYPART_LEFTLIMB4_H,
    OVERCOAT_BODYPART_LEFTLIMB4_I,
    OVERCOAT_BODYPART_LEFTLIMB5_A,      // Left Limb 5 A
    OVERCOAT_BODYPART_LEFTLIMB5_B,
    OVERCOAT_BODYPART_LEFTLIMB5_C,
    OVERCOAT_BODYPART_LEFTLIMB5_D,
    OVERCOAT_BODYPART_LEFTLIMB5_E,
    OVERCOAT_BODYPART_LEFTLIMB5_F,
    OVERCOAT_BODYPART_LEFTLIMB5_G,
    OVERCOAT_BODYPART_LEFTLIMB5_H,
    OVERCOAT_BODYPART_LEFTLIMB5_I,
    OVERCOAT_BODYPART_LEFTLIMB5_J,
    OVERCOAT_BODYPART_LEFTLIMB5_K,
    OVERCOAT_BODYPART_LEFTLIMB5_L,
    OVERCOAT_BODYPART_LEFTLIMB6_A,      // Left Limb 6 A
    OVERCOAT_BODYPART_LEFTLIMB6_B,
    OVERCOAT_BODYPART_LEFTLIMB6_C,
    OVERCOAT_BODYPART_LEFTLIMB6_D,
    OVERCOAT_BODYPART_LEFTLIMB6_E,
    OVERCOAT_BODYPART_LEFTLIMB6_F,
    OVERCOAT_BODYPART_LEFTLIMB6_G,
    OVERCOAT_BODYPART_LEFTLIMB6_H,
    OVERCOAT_BODYPART_LEFTLIMB6_I,
    OVERCOAT_BODYPART_LEFTLIMB7_A,      // Left Limb 7 A
    OVERCOAT_BODYPART_LEFTLIMB7_B,
    OVERCOAT_BODYPART_LEFTLIMB7_C,
    OVERCOAT_BODYPART_LEFTLIMB7_D,
    OVERCOAT_BODYPART_LEFTLIMB7_E,
    OVERCOAT_BODYPART_LEFTLIMB7_F,
    OVERCOAT_BODYPART_LEFTLIMB7_G,
    OVERCOAT_BODYPART_LEFTLIMB7_H,
    OVERCOAT_BODYPART_LEFTLIMB7_I,
    OVERCOAT_BODYPART_LEFTLIMB8_A,      // Left Limb 8 A
    OVERCOAT_BODYPART_LEFTLIMB8_B,
    OVERCOAT_BODYPART_LEFTLIMB8_C,
    OVERCOAT_BODYPART_LEFTLIMB8_D,
    OVERCOAT_BODYPART_LEFTLIMB8_E,
    OVERCOAT_BODYPART_LEFTLIMB8_F,
    OVERCOAT_BODYPART_LEFTLIMB8_G,
    OVERCOAT_BODYPART_LEFTLIMB8_H,
    OVERCOAT_BODYPART_LEFTLIMB8_I,
    OVERCOAT_BODYPART_LEFTLIMB9_A,      // Left Limb 9 A
    OVERCOAT_BODYPART_LEFTLIMB9_B,
    OVERCOAT_BODYPART_LEFTLIMB9_C,
    OVERCOAT_BODYPART_LEFTLIMB9_D,
    OVERCOAT_BODYPART_LEFTLIMB9_E,
    OVERCOAT_BODYPART_LEFTLIMB9_F,
    OVERCOAT_BODYPART_LEFTLIMB9_G,
    OVERCOAT_BODYPART_LEFTLIMB9_H,
    OVERCOAT_BODYPART_LEFTLIMB9_I,
    OVERCOAT_BODYPART_RIGHTLIMB0_A,      // Right Limb 0 A
    OVERCOAT_BODYPART_RIGHTLIMB0_B,
    OVERCOAT_BODYPART_RIGHTLIMB0_C,
    OVERCOAT_BODYPART_RIGHTLIMB0_D,
    OVERCOAT_BODYPART_RIGHTLIMB0_E,
    OVERCOAT_BODYPART_RIGHTLIMB0_F,
    OVERCOAT_BODYPART_RIGHTLIMB0_G,
    OVERCOAT_BODYPART_RIGHTLIMB0_H,
    OVERCOAT_BODYPART_RIGHTLIMB0_I,
    OVERCOAT_BODYPART_RIGHTLIMB1_A,      // Right Limb 1 A
    OVERCOAT_BODYPART_RIGHTLIMB1_B,
    OVERCOAT_BODYPART_RIGHTLIMB1_C,
    OVERCOAT_BODYPART_RIGHTLIMB1_D,
    OVERCOAT_BODYPART_RIGHTLIMB1_E,
    OVERCOAT_BODYPART_RIGHTLIMB1_F,
    OVERCOAT_BODYPART_RIGHTLIMB1_G,
    OVERCOAT_BODYPART_RIGHTLIMB1_H,
    OVERCOAT_BODYPART_RIGHTLIMB1_I,
    OVERCOAT_BODYPART_RIGHTLIMB2_A,      // Right Limb 2 A
    OVERCOAT_BODYPART_RIGHTLIMB2_B,
    OVERCOAT_BODYPART_RIGHTLIMB2_C,
    OVERCOAT_BODYPART_RIGHTLIMB2_D,
    OVERCOAT_BODYPART_RIGHTLIMB2_E,
    OVERCOAT_BODYPART_RIGHTLIMB2_F,
    OVERCOAT_BODYPART_RIGHTLIMB2_G,
    OVERCOAT_BODYPART_RIGHTLIMB2_H,
    OVERCOAT_BODYPART_RIGHTLIMB2_I,
    OVERCOAT_BODYPART_RIGHTLIMB3_A,      // Right Limb 3 A
    OVERCOAT_BODYPART_RIGHTLIMB3_B,
    OVERCOAT_BODYPART_RIGHTLIMB3_C,
    OVERCOAT_BODYPART_RIGHTLIMB3_D,
    OVERCOAT_BODYPART_RIGHTLIMB3_E,
    OVERCOAT_BODYPART_RIGHTLIMB3_F,
    OVERCOAT_BODYPART_RIGHTLIMB3_G,
    OVERCOAT_BODYPART_RIGHTLIMB3_H,
    OVERCOAT_BODYPART_RIGHTLIMB3_I,
    OVERCOAT_BODYPART_RIGHTLIMB4_A,      // Right Limb 4 A
    OVERCOAT_BODYPART_RIGHTLIMB4_B,
    OVERCOAT_BODYPART_RIGHTLIMB4_C,
    OVERCOAT_BODYPART_RIGHTLIMB4_D,
    OVERCOAT_BODYPART_RIGHTLIMB4_E,
    OVERCOAT_BODYPART_RIGHTLIMB4_F,
    OVERCOAT_BODYPART_RIGHTLIMB4_G,
    OVERCOAT_BODYPART_RIGHTLIMB4_H,
    OVERCOAT_BODYPART_RIGHTLIMB4_I,
    OVERCOAT_BODYPART_RIGHTLIMB5_A,      // Right Limb 5 A
    OVERCOAT_BODYPART_RIGHTLIMB5_B,
    OVERCOAT_BODYPART_RIGHTLIMB5_C,
    OVERCOAT_BODYPART_RIGHTLIMB5_D,
    OVERCOAT_BODYPART_RIGHTLIMB5_E,
    OVERCOAT_BODYPART_RIGHTLIMB5_F,
    OVERCOAT_BODYPART_RIGHTLIMB5_G,
    OVERCOAT_BODYPART_RIGHTLIMB5_H,
    OVERCOAT_BODYPART_RIGHTLIMB5_I,
    OVERCOAT_BODYPART_RIGHTLIMB5_J,
    OVERCOAT_BODYPART_RIGHTLIMB5_K,
    OVERCOAT_BODYPART_RIGHTLIMB5_L,
    OVERCOAT_BODYPART_RIGHTLIMB6_A,      // Right Limb 6 A
    OVERCOAT_BODYPART_RIGHTLIMB6_B,
    OVERCOAT_BODYPART_RIGHTLIMB6_C,
    OVERCOAT_BODYPART_RIGHTLIMB6_D,
    OVERCOAT_BODYPART_RIGHTLIMB6_E,
    OVERCOAT_BODYPART_RIGHTLIMB6_F,
    OVERCOAT_BODYPART_RIGHTLIMB6_G,
    OVERCOAT_BODYPART_RIGHTLIMB6_H,
    OVERCOAT_BODYPART_RIGHTLIMB6_I,
    OVERCOAT_BODYPART_RIGHTLIMB7_A,      // Right Limb 7 A
    OVERCOAT_BODYPART_RIGHTLIMB7_B,
    OVERCOAT_BODYPART_RIGHTLIMB7_C,
    OVERCOAT_BODYPART_RIGHTLIMB7_D,
    OVERCOAT_BODYPART_RIGHTLIMB7_E,
    OVERCOAT_BODYPART_RIGHTLIMB7_F,
    OVERCOAT_BODYPART_RIGHTLIMB7_G,
    OVERCOAT_BODYPART_RIGHTLIMB7_H,
    OVERCOAT_BODYPART_RIGHTLIMB7_I,
    OVERCOAT_BODYPART_RIGHTLIMB8_A,      // Right Limb 8 A
    OVERCOAT_BODYPART_RIGHTLIMB8_B,
    OVERCOAT_BODYPART_RIGHTLIMB8_C,
    OVERCOAT_BODYPART_RIGHTLIMB8_D,
    OVERCOAT_BODYPART_RIGHTLIMB8_E,
    OVERCOAT_BODYPART_RIGHTLIMB8_F,
    OVERCOAT_BODYPART_RIGHTLIMB8_G,
    OVERCOAT_BODYPART_RIGHTLIMB8_H,
    OVERCOAT_BODYPART_RIGHTLIMB8_I,
    OVERCOAT_BODYPART_RIGHTLIMB9_A,      // Right Limb 9 A
    OVERCOAT_BODYPART_RIGHTLIMB9_B,
    OVERCOAT_BODYPART_RIGHTLIMB9_C,
    OVERCOAT_BODYPART_RIGHTLIMB9_D,
    OVERCOAT_BODYPART_RIGHTLIMB9_E,
    OVERCOAT_BODYPART_RIGHTLIMB9_F,
    OVERCOAT_BODYPART_RIGHTLIMB9_G,
    OVERCOAT_BODYPART_RIGHTLIMB9_H,
    OVERCOAT_BODYPART_RIGHTLIMB9_I,
    OVERCOAT_BODYPART_MAX
} OvercoatBodyPart;

// Phys limb for each overcoat's skeleton limb
PhysLimb overcoat_RootLimb;
PhysLimb overcoat_BackLimb_A;
PhysLimb overcoat_BackLimb_B;
PhysLimb overcoat_BackLimb_C;
PhysLimb overcoat_BackLimb_D;
PhysLimb overcoat_BackLimb_E;
PhysLimb overcoat_BackLimb_F;
PhysLimb overcoat_BackLimb_G;
PhysLimb overcoat_BackLimb_H;
PhysLimb overcoat_BackLimb_I;
PhysLimb overcoat_LeftLimb0_A;
PhysLimb overcoat_LeftLimb0_B;
PhysLimb overcoat_LeftLimb0_C;
PhysLimb overcoat_LeftLimb0_D;
PhysLimb overcoat_LeftLimb0_E;
PhysLimb overcoat_LeftLimb0_F;
PhysLimb overcoat_LeftLimb0_G;
PhysLimb overcoat_LeftLimb0_H;
PhysLimb overcoat_LeftLimb0_I;
PhysLimb overcoat_LeftLimb1_A;
PhysLimb overcoat_LeftLimb1_B;
PhysLimb overcoat_LeftLimb1_C;
PhysLimb overcoat_LeftLimb1_D;
PhysLimb overcoat_LeftLimb1_E;
PhysLimb overcoat_LeftLimb1_F;
PhysLimb overcoat_LeftLimb1_G;
PhysLimb overcoat_LeftLimb1_H;
PhysLimb overcoat_LeftLimb1_I;
PhysLimb overcoat_LeftLimb2_A;
PhysLimb overcoat_LeftLimb2_B;
PhysLimb overcoat_LeftLimb2_C;
PhysLimb overcoat_LeftLimb2_D;
PhysLimb overcoat_LeftLimb2_E;
PhysLimb overcoat_LeftLimb2_F;
PhysLimb overcoat_LeftLimb2_G;
PhysLimb overcoat_LeftLimb2_H;
PhysLimb overcoat_LeftLimb2_I;
PhysLimb overcoat_LeftLimb3_A;
PhysLimb overcoat_LeftLimb3_B;
PhysLimb overcoat_LeftLimb3_C;
PhysLimb overcoat_LeftLimb3_D;
PhysLimb overcoat_LeftLimb3_E;
PhysLimb overcoat_LeftLimb3_F;
PhysLimb overcoat_LeftLimb3_G;
PhysLimb overcoat_LeftLimb3_H;
PhysLimb overcoat_LeftLimb3_I;
PhysLimb overcoat_LeftLimb4_A;
PhysLimb overcoat_LeftLimb4_B;
PhysLimb overcoat_LeftLimb4_C;
PhysLimb overcoat_LeftLimb4_D;
PhysLimb overcoat_LeftLimb4_E;
PhysLimb overcoat_LeftLimb4_F;
PhysLimb overcoat_LeftLimb4_G;
PhysLimb overcoat_LeftLimb4_H;
PhysLimb overcoat_LeftLimb4_I;
PhysLimb overcoat_LeftLimb5_A;
PhysLimb overcoat_LeftLimb5_B;
PhysLimb overcoat_LeftLimb5_C;
PhysLimb overcoat_LeftLimb5_D;
PhysLimb overcoat_LeftLimb5_E;
PhysLimb overcoat_LeftLimb5_F;
PhysLimb overcoat_LeftLimb5_G;
PhysLimb overcoat_LeftLimb5_H;
PhysLimb overcoat_LeftLimb5_I;
PhysLimb overcoat_LeftLimb5_J;
PhysLimb overcoat_LeftLimb5_K;
PhysLimb overcoat_LeftLimb5_L;
PhysLimb overcoat_LeftLimb6_A;
PhysLimb overcoat_LeftLimb6_B;
PhysLimb overcoat_LeftLimb6_C;
PhysLimb overcoat_LeftLimb6_D;
PhysLimb overcoat_LeftLimb6_E;
PhysLimb overcoat_LeftLimb6_F;
PhysLimb overcoat_LeftLimb6_G;
PhysLimb overcoat_LeftLimb6_H;
PhysLimb overcoat_LeftLimb6_I;
PhysLimb overcoat_LeftLimb7_A;
PhysLimb overcoat_LeftLimb7_B;
PhysLimb overcoat_LeftLimb7_C;
PhysLimb overcoat_LeftLimb7_D;
PhysLimb overcoat_LeftLimb7_E;
PhysLimb overcoat_LeftLimb7_F;
PhysLimb overcoat_LeftLimb7_G;
PhysLimb overcoat_LeftLimb7_H;
PhysLimb overcoat_LeftLimb7_I;
PhysLimb overcoat_LeftLimb8_A;
PhysLimb overcoat_LeftLimb8_B;
PhysLimb overcoat_LeftLimb8_C;
PhysLimb overcoat_LeftLimb8_D;
PhysLimb overcoat_LeftLimb8_E;
PhysLimb overcoat_LeftLimb8_F;
PhysLimb overcoat_LeftLimb8_G;
PhysLimb overcoat_LeftLimb8_H;
PhysLimb overcoat_LeftLimb8_I;
PhysLimb overcoat_LeftLimb9_A;
PhysLimb overcoat_LeftLimb9_B;
PhysLimb overcoat_LeftLimb9_C;
PhysLimb overcoat_LeftLimb9_D;
PhysLimb overcoat_LeftLimb9_E;
PhysLimb overcoat_LeftLimb9_F;
PhysLimb overcoat_LeftLimb9_G;
PhysLimb overcoat_LeftLimb9_H;
PhysLimb overcoat_LeftLimb9_I;
PhysLimb overcoat_RightLimb0_A;
PhysLimb overcoat_RightLimb0_B;
PhysLimb overcoat_RightLimb0_C;
PhysLimb overcoat_RightLimb0_D;
PhysLimb overcoat_RightLimb0_E;
PhysLimb overcoat_RightLimb0_F;
PhysLimb overcoat_RightLimb0_G;
PhysLimb overcoat_RightLimb0_H;
PhysLimb overcoat_RightLimb0_I;
PhysLimb overcoat_RightLimb1_A;
PhysLimb overcoat_RightLimb1_B;
PhysLimb overcoat_RightLimb1_C;
PhysLimb overcoat_RightLimb1_D;
PhysLimb overcoat_RightLimb1_E;
PhysLimb overcoat_RightLimb1_F;
PhysLimb overcoat_RightLimb1_G;
PhysLimb overcoat_RightLimb1_H;
PhysLimb overcoat_RightLimb1_I;
PhysLimb overcoat_RightLimb2_A;
PhysLimb overcoat_RightLimb2_B;
PhysLimb overcoat_RightLimb2_C;
PhysLimb overcoat_RightLimb2_D;
PhysLimb overcoat_RightLimb2_E;
PhysLimb overcoat_RightLimb2_F;
PhysLimb overcoat_RightLimb2_G;
PhysLimb overcoat_RightLimb2_H;
PhysLimb overcoat_RightLimb2_I;
PhysLimb overcoat_RightLimb3_A;
PhysLimb overcoat_RightLimb3_B;
PhysLimb overcoat_RightLimb3_C;
PhysLimb overcoat_RightLimb3_D;
PhysLimb overcoat_RightLimb3_E;
PhysLimb overcoat_RightLimb3_F;
PhysLimb overcoat_RightLimb3_G;
PhysLimb overcoat_RightLimb3_H;
PhysLimb overcoat_RightLimb3_I;
PhysLimb overcoat_RightLimb4_A;
PhysLimb overcoat_RightLimb4_B;
PhysLimb overcoat_RightLimb4_C;
PhysLimb overcoat_RightLimb4_D;
PhysLimb overcoat_RightLimb4_E;
PhysLimb overcoat_RightLimb4_F;
PhysLimb overcoat_RightLimb4_G;
PhysLimb overcoat_RightLimb4_H;
PhysLimb overcoat_RightLimb4_I;
PhysLimb overcoat_RightLimb5_A;
PhysLimb overcoat_RightLimb5_B;
PhysLimb overcoat_RightLimb5_C;
PhysLimb overcoat_RightLimb5_D;
PhysLimb overcoat_RightLimb5_E;
PhysLimb overcoat_RightLimb5_F;
PhysLimb overcoat_RightLimb5_G;
PhysLimb overcoat_RightLimb5_H;
PhysLimb overcoat_RightLimb5_I;
PhysLimb overcoat_RightLimb5_J;
PhysLimb overcoat_RightLimb5_K;
PhysLimb overcoat_RightLimb5_L;
PhysLimb overcoat_RightLimb6_A;
PhysLimb overcoat_RightLimb6_B;
PhysLimb overcoat_RightLimb6_C;
PhysLimb overcoat_RightLimb6_D;
PhysLimb overcoat_RightLimb6_E;
PhysLimb overcoat_RightLimb6_F;
PhysLimb overcoat_RightLimb6_G;
PhysLimb overcoat_RightLimb6_H;
PhysLimb overcoat_RightLimb6_I;
PhysLimb overcoat_RightLimb7_A;
PhysLimb overcoat_RightLimb7_B;
PhysLimb overcoat_RightLimb7_C;
PhysLimb overcoat_RightLimb7_D;
PhysLimb overcoat_RightLimb7_E;
PhysLimb overcoat_RightLimb7_F;
PhysLimb overcoat_RightLimb7_G;
PhysLimb overcoat_RightLimb7_H;
PhysLimb overcoat_RightLimb7_I;
PhysLimb overcoat_RightLimb8_A;
PhysLimb overcoat_RightLimb8_B;
PhysLimb overcoat_RightLimb8_C;
PhysLimb overcoat_RightLimb8_D;
PhysLimb overcoat_RightLimb8_E;
PhysLimb overcoat_RightLimb8_F;
PhysLimb overcoat_RightLimb8_G;
PhysLimb overcoat_RightLimb8_H;
PhysLimb overcoat_RightLimb8_I;
PhysLimb overcoat_RightLimb9_A;
PhysLimb overcoat_RightLimb9_B;
PhysLimb overcoat_RightLimb9_C;
PhysLimb overcoat_RightLimb9_D;
PhysLimb overcoat_RightLimb9_E;
PhysLimb overcoat_RightLimb9_F;
PhysLimb overcoat_RightLimb9_G;
PhysLimb overcoat_RightLimb9_H;
PhysLimb overcoat_RightLimb9_I;

// Struct containing phys limbs of overcoat
PhysLimb* overcoat_PhysLimbs[196] = {
    &overcoat_RootLimb,
    &overcoat_BackLimb_A,
    &overcoat_BackLimb_B,
    &overcoat_BackLimb_C,
    &overcoat_BackLimb_D,
    &overcoat_BackLimb_E,
    &overcoat_BackLimb_F,
    &overcoat_BackLimb_G,
    &overcoat_BackLimb_H,
    &overcoat_BackLimb_I,
    &overcoat_LeftLimb0_A,
    &overcoat_LeftLimb0_B,
    &overcoat_LeftLimb0_C,
    &overcoat_LeftLimb0_D,
    &overcoat_LeftLimb0_E,
    &overcoat_LeftLimb0_F,
    &overcoat_LeftLimb0_G,
    &overcoat_LeftLimb0_H,
    &overcoat_LeftLimb0_I,
    &overcoat_LeftLimb1_A,
    &overcoat_LeftLimb1_B,
    &overcoat_LeftLimb1_C,
    &overcoat_LeftLimb1_D,
    &overcoat_LeftLimb1_E,
    &overcoat_LeftLimb1_F,
    &overcoat_LeftLimb1_G,
    &overcoat_LeftLimb1_H,
    &overcoat_LeftLimb1_I,
    &overcoat_LeftLimb2_A,
    &overcoat_LeftLimb2_B,
    &overcoat_LeftLimb2_C,
    &overcoat_LeftLimb2_D,
    &overcoat_LeftLimb2_E,
    &overcoat_LeftLimb2_F,
    &overcoat_LeftLimb2_G,
    &overcoat_LeftLimb2_H,
    &overcoat_LeftLimb2_I,
    &overcoat_LeftLimb3_A,
    &overcoat_LeftLimb3_B,
    &overcoat_LeftLimb3_C,
    &overcoat_LeftLimb3_D,
    &overcoat_LeftLimb3_E,
    &overcoat_LeftLimb3_F,
    &overcoat_LeftLimb3_G,
    &overcoat_LeftLimb3_H,
    &overcoat_LeftLimb3_I,
    &overcoat_LeftLimb4_A,
    &overcoat_LeftLimb4_B,
    &overcoat_LeftLimb4_C,
    &overcoat_LeftLimb4_D,
    &overcoat_LeftLimb4_E,
    &overcoat_LeftLimb4_F,
    &overcoat_LeftLimb4_G,
    &overcoat_LeftLimb4_H,
    &overcoat_LeftLimb4_I,
    &overcoat_LeftLimb5_A,
    &overcoat_LeftLimb5_B,
    &overcoat_LeftLimb5_C,
    &overcoat_LeftLimb5_D,
    &overcoat_LeftLimb5_E,
    &overcoat_LeftLimb5_F,
    &overcoat_LeftLimb5_G,
    &overcoat_LeftLimb5_H,
    &overcoat_LeftLimb5_I,
    &overcoat_LeftLimb5_J,
    &overcoat_LeftLimb5_K,
    &overcoat_LeftLimb5_L,
    &overcoat_LeftLimb6_A,
    &overcoat_LeftLimb6_B,
    &overcoat_LeftLimb6_C,
    &overcoat_LeftLimb6_D,
    &overcoat_LeftLimb6_E,
    &overcoat_LeftLimb6_F,
    &overcoat_LeftLimb6_G,
    &overcoat_LeftLimb6_H,
    &overcoat_LeftLimb6_I,
    &overcoat_LeftLimb7_A,
    &overcoat_LeftLimb7_B,
    &overcoat_LeftLimb7_C,
    &overcoat_LeftLimb7_D,
    &overcoat_LeftLimb7_E,
    &overcoat_LeftLimb7_F,
    &overcoat_LeftLimb7_G,
    &overcoat_LeftLimb7_H,
    &overcoat_LeftLimb7_I,
    &overcoat_LeftLimb8_A,
    &overcoat_LeftLimb8_B,
    &overcoat_LeftLimb8_C,
    &overcoat_LeftLimb8_D,
    &overcoat_LeftLimb8_E,
    &overcoat_LeftLimb8_F,
    &overcoat_LeftLimb8_G,
    &overcoat_LeftLimb8_H,
    &overcoat_LeftLimb8_I,
    &overcoat_LeftLimb9_A,
    &overcoat_LeftLimb9_B,
    &overcoat_LeftLimb9_C,
    &overcoat_LeftLimb9_D,
    &overcoat_LeftLimb9_E,
    &overcoat_LeftLimb9_F,
    &overcoat_LeftLimb9_G,
    &overcoat_LeftLimb9_H,
    &overcoat_LeftLimb9_I,
    &overcoat_RightLimb0_A,
    &overcoat_RightLimb0_B,
    &overcoat_RightLimb0_C,
    &overcoat_RightLimb0_D,
    &overcoat_RightLimb0_E,
    &overcoat_RightLimb0_F,
    &overcoat_RightLimb0_G,
    &overcoat_RightLimb0_H,
    &overcoat_RightLimb0_I,
    &overcoat_RightLimb1_A,
    &overcoat_RightLimb1_B,
    &overcoat_RightLimb1_C,
    &overcoat_RightLimb1_D,
    &overcoat_RightLimb1_E,
    &overcoat_RightLimb1_F,
    &overcoat_RightLimb1_G,
    &overcoat_RightLimb1_H,
    &overcoat_RightLimb1_I,
    &overcoat_RightLimb2_A,
    &overcoat_RightLimb2_B,
    &overcoat_RightLimb2_C,
    &overcoat_RightLimb2_D,
    &overcoat_RightLimb2_E,
    &overcoat_RightLimb2_F,
    &overcoat_RightLimb2_G,
    &overcoat_RightLimb2_H,
    &overcoat_RightLimb2_I,
    &overcoat_RightLimb3_A,
    &overcoat_RightLimb3_B,
    &overcoat_RightLimb3_C,
    &overcoat_RightLimb3_D,
    &overcoat_RightLimb3_E,
    &overcoat_RightLimb3_F,
    &overcoat_RightLimb3_G,
    &overcoat_RightLimb3_H,
    &overcoat_RightLimb3_I,
    &overcoat_RightLimb4_A,
    &overcoat_RightLimb4_B,
    &overcoat_RightLimb4_C,
    &overcoat_RightLimb4_D,
    &overcoat_RightLimb4_E,
    &overcoat_RightLimb4_F,
    &overcoat_RightLimb4_G,
    &overcoat_RightLimb4_H,
    &overcoat_RightLimb4_I,
    &overcoat_RightLimb5_A,
    &overcoat_RightLimb5_B,
    &overcoat_RightLimb5_C,
    &overcoat_RightLimb5_D,
    &overcoat_RightLimb5_E,
    &overcoat_RightLimb5_F,
    &overcoat_RightLimb5_G,
    &overcoat_RightLimb5_H,
    &overcoat_RightLimb5_I,
    &overcoat_RightLimb5_J,
    &overcoat_RightLimb5_K,
    &overcoat_RightLimb5_L,
    &overcoat_RightLimb6_A,
    &overcoat_RightLimb6_B,
    &overcoat_RightLimb6_C,
    &overcoat_RightLimb6_D,
    &overcoat_RightLimb6_E,
    &overcoat_RightLimb6_F,
    &overcoat_RightLimb6_G,
    &overcoat_RightLimb6_H,
    &overcoat_RightLimb6_I,
    &overcoat_RightLimb7_A,
    &overcoat_RightLimb7_B,
    &overcoat_RightLimb7_C,
    &overcoat_RightLimb7_D,
    &overcoat_RightLimb7_E,
    &overcoat_RightLimb7_F,
    &overcoat_RightLimb7_G,
    &overcoat_RightLimb7_H,
    &overcoat_RightLimb7_I,
    &overcoat_RightLimb8_A,
    &overcoat_RightLimb8_B,
    &overcoat_RightLimb8_C,
    &overcoat_RightLimb8_D,
    &overcoat_RightLimb8_E,
    &overcoat_RightLimb8_F,
    &overcoat_RightLimb8_G,
    &overcoat_RightLimb8_H,
    &overcoat_RightLimb8_I,
    &overcoat_RightLimb9_A,
    &overcoat_RightLimb9_B,
    &overcoat_RightLimb9_C,
    &overcoat_RightLimb9_D,
    &overcoat_RightLimb9_E,
    &overcoat_RightLimb9_F,
    &overcoat_RightLimb9_G,
    &overcoat_RightLimb9_H,
    &overcoat_RightLimb9_I
};

// Phys bone for overcoat's skeleton limbs, where phys bone contains 2 phys limbs
PhysBone overcoat_RootLimbBackLimb_A;
PhysBone overcoat_BackLimb_A_BackLimb_B;
PhysBone overcoat_BackLimb_B_BackLimb_C;
PhysBone overcoat_BackLimb_C_BackLimb_D;
PhysBone overcoat_BackLimb_D_BackLimb_E;
PhysBone overcoat_BackLimb_E_BackLimb_F;
PhysBone overcoat_BackLimb_F_BackLimb_G;
PhysBone overcoat_BackLimb_G_BackLimb_H;
PhysBone overcoat_BackLimb_H_BackLimb_I;
PhysBone overcoat_RootLimbLeftLimb0_A;
PhysBone overcoat_LeftLimb0_A_LeftLimb0_B;
PhysBone overcoat_LeftLimb0_B_LeftLimb0_C;
PhysBone overcoat_LeftLimb0_C_LeftLimb0_D;
PhysBone overcoat_LeftLimb0_D_LeftLimb0_E;
PhysBone overcoat_LeftLimb0_E_LeftLimb0_F;
PhysBone overcoat_LeftLimb0_F_LeftLimb0_G;
PhysBone overcoat_LeftLimb0_G_LeftLimb0_H;
PhysBone overcoat_LeftLimb0_H_LeftLimb0_I;
PhysBone overcoat_RootLimbLeftLimb1_A;
PhysBone overcoat_LeftLimb1_A_LeftLimb1_B;
PhysBone overcoat_LeftLimb1_B_LeftLimb1_C;
PhysBone overcoat_LeftLimb1_C_LeftLimb1_D;
PhysBone overcoat_LeftLimb1_D_LeftLimb1_E;
PhysBone overcoat_LeftLimb1_E_LeftLimb1_F;
PhysBone overcoat_LeftLimb1_F_LeftLimb1_G;
PhysBone overcoat_LeftLimb1_G_LeftLimb1_H;
PhysBone overcoat_LeftLimb1_H_LeftLimb1_I;
PhysBone overcoat_RootLimbLeftLimb2_A;
PhysBone overcoat_LeftLimb2_A_LeftLimb2_B;
PhysBone overcoat_LeftLimb2_B_LeftLimb2_C;
PhysBone overcoat_LeftLimb2_C_LeftLimb2_D;
PhysBone overcoat_LeftLimb2_D_LeftLimb2_E;
PhysBone overcoat_LeftLimb2_E_LeftLimb2_F;
PhysBone overcoat_LeftLimb2_F_LeftLimb2_G;
PhysBone overcoat_LeftLimb2_G_LeftLimb2_H;
PhysBone overcoat_LeftLimb2_H_LeftLimb2_I;
PhysBone overcoat_RootLimbLeftLimb3_A;
PhysBone overcoat_LeftLimb3_A_LeftLimb3_B;
PhysBone overcoat_LeftLimb3_B_LeftLimb3_C;
PhysBone overcoat_LeftLimb3_C_LeftLimb3_D;
PhysBone overcoat_LeftLimb3_D_LeftLimb3_E;
PhysBone overcoat_LeftLimb3_E_LeftLimb3_F;
PhysBone overcoat_LeftLimb3_F_LeftLimb3_G;
PhysBone overcoat_LeftLimb3_G_LeftLimb3_H;
PhysBone overcoat_LeftLimb3_H_LeftLimb3_I;
PhysBone overcoat_RootLimbLeftLimb4_A;
PhysBone overcoat_LeftLimb4_A_LeftLimb4_B;
PhysBone overcoat_LeftLimb4_B_LeftLimb4_C;
PhysBone overcoat_LeftLimb4_C_LeftLimb4_D;
PhysBone overcoat_LeftLimb4_D_LeftLimb4_E;
PhysBone overcoat_LeftLimb4_E_LeftLimb4_F;
PhysBone overcoat_LeftLimb4_F_LeftLimb4_G;
PhysBone overcoat_LeftLimb4_G_LeftLimb4_H;
PhysBone overcoat_LeftLimb4_H_LeftLimb4_I;
PhysBone overcoat_RootLimbLeftLimb5_A;
PhysBone overcoat_LeftLimb5_A_LeftLimb5_B;
PhysBone overcoat_LeftLimb5_B_LeftLimb5_C;
PhysBone overcoat_LeftLimb5_C_LeftLimb5_D;
PhysBone overcoat_LeftLimb5_D_LeftLimb5_E;
PhysBone overcoat_LeftLimb5_E_LeftLimb5_F;
PhysBone overcoat_LeftLimb5_F_LeftLimb5_G;
PhysBone overcoat_LeftLimb5_G_LeftLimb5_H;
PhysBone overcoat_LeftLimb5_H_LeftLimb5_I;
PhysBone overcoat_LeftLimb5_I_LeftLimb5_J;
PhysBone overcoat_LeftLimb5_J_LeftLimb5_K;
PhysBone overcoat_LeftLimb5_K_LeftLimb5_L;
PhysBone overcoat_RootLimbLeftLimb6_A;
PhysBone overcoat_LeftLimb6_A_LeftLimb6_B;
PhysBone overcoat_LeftLimb6_B_LeftLimb6_C;
PhysBone overcoat_LeftLimb6_C_LeftLimb6_D;
PhysBone overcoat_LeftLimb6_D_LeftLimb6_E;
PhysBone overcoat_LeftLimb6_E_LeftLimb6_F;
PhysBone overcoat_LeftLimb6_F_LeftLimb6_G;
PhysBone overcoat_LeftLimb6_G_LeftLimb6_H;
PhysBone overcoat_LeftLimb6_H_LeftLimb6_I;
PhysBone overcoat_RootLimbLeftLimb7_A;
PhysBone overcoat_LeftLimb7_A_LeftLimb7_B;
PhysBone overcoat_LeftLimb7_B_LeftLimb7_C;
PhysBone overcoat_LeftLimb7_C_LeftLimb7_D;
PhysBone overcoat_LeftLimb7_D_LeftLimb7_E;
PhysBone overcoat_LeftLimb7_E_LeftLimb7_F;
PhysBone overcoat_LeftLimb7_F_LeftLimb7_G;
PhysBone overcoat_LeftLimb7_G_LeftLimb7_H;
PhysBone overcoat_LeftLimb7_H_LeftLimb7_I;
PhysBone overcoat_RootLimbLeftLimb8_A;
PhysBone overcoat_LeftLimb8_A_LeftLimb8_B;
PhysBone overcoat_LeftLimb8_B_LeftLimb8_C;
PhysBone overcoat_LeftLimb8_C_LeftLimb8_D;
PhysBone overcoat_LeftLimb8_D_LeftLimb8_E;
PhysBone overcoat_LeftLimb8_E_LeftLimb8_F;
PhysBone overcoat_LeftLimb8_F_LeftLimb8_G;
PhysBone overcoat_LeftLimb8_G_LeftLimb8_H;
PhysBone overcoat_LeftLimb8_H_LeftLimb8_I;
PhysBone overcoat_RootLimbLeftLimb9_A;
PhysBone overcoat_LeftLimb9_A_LeftLimb9_B;
PhysBone overcoat_LeftLimb9_B_LeftLimb9_C;
PhysBone overcoat_LeftLimb9_C_LeftLimb9_D;
PhysBone overcoat_LeftLimb9_D_LeftLimb9_E;
PhysBone overcoat_LeftLimb9_E_LeftLimb9_F;
PhysBone overcoat_LeftLimb9_F_LeftLimb9_G;
PhysBone overcoat_LeftLimb9_G_LeftLimb9_H;
PhysBone overcoat_LeftLimb9_H_LeftLimb9_I;
PhysBone overcoat_RootLimbRightLimb0_A;
PhysBone overcoat_RightLimb0_A_RightLimb0_B;
PhysBone overcoat_RightLimb0_B_RightLimb0_C;
PhysBone overcoat_RightLimb0_C_RightLimb0_D;
PhysBone overcoat_RightLimb0_D_RightLimb0_E;
PhysBone overcoat_RightLimb0_E_RightLimb0_F;
PhysBone overcoat_RightLimb0_F_RightLimb0_G;
PhysBone overcoat_RightLimb0_G_RightLimb0_H;
PhysBone overcoat_RightLimb0_H_RightLimb0_I;
PhysBone overcoat_RootLimbRightLimb1_A;
PhysBone overcoat_RightLimb1_A_RightLimb1_B;
PhysBone overcoat_RightLimb1_B_RightLimb1_C;
PhysBone overcoat_RightLimb1_C_RightLimb1_D;
PhysBone overcoat_RightLimb1_D_RightLimb1_E;
PhysBone overcoat_RightLimb1_E_RightLimb1_F;
PhysBone overcoat_RightLimb1_F_RightLimb1_G;
PhysBone overcoat_RightLimb1_G_RightLimb1_H;
PhysBone overcoat_RightLimb1_H_RightLimb1_I;
PhysBone overcoat_RootLimbRightLimb2_A;
PhysBone overcoat_RightLimb2_A_RightLimb2_B;
PhysBone overcoat_RightLimb2_B_RightLimb2_C;
PhysBone overcoat_RightLimb2_C_RightLimb2_D;
PhysBone overcoat_RightLimb2_D_RightLimb2_E;
PhysBone overcoat_RightLimb2_E_RightLimb2_F;
PhysBone overcoat_RightLimb2_F_RightLimb2_G;
PhysBone overcoat_RightLimb2_G_RightLimb2_H;
PhysBone overcoat_RightLimb2_H_RightLimb2_I;
PhysBone overcoat_RootLimbRightLimb3_A;
PhysBone overcoat_RightLimb3_A_RightLimb3_B;
PhysBone overcoat_RightLimb3_B_RightLimb3_C;
PhysBone overcoat_RightLimb3_C_RightLimb3_D;
PhysBone overcoat_RightLimb3_D_RightLimb3_E;
PhysBone overcoat_RightLimb3_E_RightLimb3_F;
PhysBone overcoat_RightLimb3_F_RightLimb3_G;
PhysBone overcoat_RightLimb3_G_RightLimb3_H;
PhysBone overcoat_RightLimb3_H_RightLimb3_I;
PhysBone overcoat_RootLimbRightLimb4_A;
PhysBone overcoat_RightLimb4_A_RightLimb4_B;
PhysBone overcoat_RightLimb4_B_RightLimb4_C;
PhysBone overcoat_RightLimb4_C_RightLimb4_D;
PhysBone overcoat_RightLimb4_D_RightLimb4_E;
PhysBone overcoat_RightLimb4_E_RightLimb4_F;
PhysBone overcoat_RightLimb4_F_RightLimb4_G;
PhysBone overcoat_RightLimb4_G_RightLimb4_H;
PhysBone overcoat_RightLimb4_H_RightLimb4_I;
PhysBone overcoat_RootLimbRightLimb5_A;
PhysBone overcoat_RightLimb5_A_RightLimb5_B;
PhysBone overcoat_RightLimb5_B_RightLimb5_C;
PhysBone overcoat_RightLimb5_C_RightLimb5_D;
PhysBone overcoat_RightLimb5_D_RightLimb5_E;
PhysBone overcoat_RightLimb5_E_RightLimb5_F;
PhysBone overcoat_RightLimb5_F_RightLimb5_G;
PhysBone overcoat_RightLimb5_G_RightLimb5_H;
PhysBone overcoat_RightLimb5_H_RightLimb5_I;
PhysBone overcoat_RightLimb5_I_RightLimb5_J;
PhysBone overcoat_RightLimb5_J_RightLimb5_K;
PhysBone overcoat_RightLimb5_K_RightLimb5_L;
PhysBone overcoat_RootLimbRightLimb6_A;
PhysBone overcoat_RightLimb6_A_RightLimb6_B;
PhysBone overcoat_RightLimb6_B_RightLimb6_C;
PhysBone overcoat_RightLimb6_C_RightLimb6_D;
PhysBone overcoat_RightLimb6_D_RightLimb6_E;
PhysBone overcoat_RightLimb6_E_RightLimb6_F;
PhysBone overcoat_RightLimb6_F_RightLimb6_G;
PhysBone overcoat_RightLimb6_G_RightLimb6_H;
PhysBone overcoat_RightLimb6_H_RightLimb6_I;
PhysBone overcoat_RootLimbRightLimb7_A;
PhysBone overcoat_RightLimb7_A_RightLimb7_B;
PhysBone overcoat_RightLimb7_B_RightLimb7_C;
PhysBone overcoat_RightLimb7_C_RightLimb7_D;
PhysBone overcoat_RightLimb7_D_RightLimb7_E;
PhysBone overcoat_RightLimb7_E_RightLimb7_F;
PhysBone overcoat_RightLimb7_F_RightLimb7_G;
PhysBone overcoat_RightLimb7_G_RightLimb7_H;
PhysBone overcoat_RightLimb7_H_RightLimb7_I;
PhysBone overcoat_RootLimbRightLimb8_A;
PhysBone overcoat_RightLimb8_A_RightLimb8_B;
PhysBone overcoat_RightLimb8_B_RightLimb8_C;
PhysBone overcoat_RightLimb8_C_RightLimb8_D;
PhysBone overcoat_RightLimb8_D_RightLimb8_E;
PhysBone overcoat_RightLimb8_E_RightLimb8_F;
PhysBone overcoat_RightLimb8_F_RightLimb8_G;
PhysBone overcoat_RightLimb8_G_RightLimb8_H;
PhysBone overcoat_RightLimb8_H_RightLimb8_I;
PhysBone overcoat_RootLimbRightLimb9_A;
PhysBone overcoat_RightLimb9_A_RightLimb9_B;
PhysBone overcoat_RightLimb9_B_RightLimb9_C;
PhysBone overcoat_RightLimb9_C_RightLimb9_D;
PhysBone overcoat_RightLimb9_D_RightLimb9_E;
PhysBone overcoat_RightLimb9_E_RightLimb9_F;
PhysBone overcoat_RightLimb9_F_RightLimb9_G;
PhysBone overcoat_RightLimb9_G_RightLimb9_H;
PhysBone overcoat_RightLimb9_H_RightLimb9_I;

// Enum for indicating overcoat's bones (where each bone contains 2 limbs)
typedef enum Overcoat_BoneIndex {
    OVERCOAT_BONE_ROOT_BACKLIMB_A,              // Root & Back Limb A
    OVERCOAT_BONE_BACKLIMB_A_BACKLIMB_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_BACKLIMB_B_BACKLIMB_C,
    OVERCOAT_BONE_BACKLIMB_C_BACKLIMB_D,
    OVERCOAT_BONE_BACKLIMB_D_BACKLIMB_E,
    OVERCOAT_BONE_BACKLIMB_E_BACKLIMB_F,
    OVERCOAT_BONE_BACKLIMB_F_BACKLIMB_G,
    OVERCOAT_BONE_BACKLIMB_G_BACKLIMB_H,
    OVERCOAT_BONE_BACKLIMB_H_BACKLIMB_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB0_A,             // Root & Left Limb 0 A
    OVERCOAT_BONE_LEFTLIMB0_A_LEFTLIMB0_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB0_B_LEFTLIMB0_C,
    OVERCOAT_BONE_LEFTLIMB0_C_LEFTLIMB0_D,
    OVERCOAT_BONE_LEFTLIMB0_D_LEFTLIMB0_E,
    OVERCOAT_BONE_LEFTLIMB0_E_LEFTLIMB0_F,
    OVERCOAT_BONE_LEFTLIMB0_F_LEFTLIMB0_G,
    OVERCOAT_BONE_LEFTLIMB0_G_LEFTLIMB0_H,
    OVERCOAT_BONE_LEFTLIMB0_H_LEFTLIMB0_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB1_A,             // Root & Left Limb 1 A
    OVERCOAT_BONE_LEFTLIMB1_A_LEFTLIMB1_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB1_B_LEFTLIMB1_C,
    OVERCOAT_BONE_LEFTLIMB1_C_LEFTLIMB1_D,
    OVERCOAT_BONE_LEFTLIMB1_D_LEFTLIMB1_E,
    OVERCOAT_BONE_LEFTLIMB1_E_LEFTLIMB1_F,
    OVERCOAT_BONE_LEFTLIMB1_F_LEFTLIMB1_G,
    OVERCOAT_BONE_LEFTLIMB1_G_LEFTLIMB1_H,
    OVERCOAT_BONE_LEFTLIMB1_H_LEFTLIMB1_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB2_A,             // Root & Left Limb 2 A
    OVERCOAT_BONE_LEFTLIMB2_A_LEFTLIMB2_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB2_B_LEFTLIMB2_C,
    OVERCOAT_BONE_LEFTLIMB2_C_LEFTLIMB2_D,
    OVERCOAT_BONE_LEFTLIMB2_D_LEFTLIMB2_E,
    OVERCOAT_BONE_LEFTLIMB2_E_LEFTLIMB2_F,
    OVERCOAT_BONE_LEFTLIMB2_F_LEFTLIMB2_G,
    OVERCOAT_BONE_LEFTLIMB2_G_LEFTLIMB2_H,
    OVERCOAT_BONE_LEFTLIMB2_H_LEFTLIMB2_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB3_A,             // Root & Left Limb 3 A
    OVERCOAT_BONE_LEFTLIMB3_A_LEFTLIMB3_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB3_B_LEFTLIMB3_C,
    OVERCOAT_BONE_LEFTLIMB3_C_LEFTLIMB3_D,
    OVERCOAT_BONE_LEFTLIMB3_D_LEFTLIMB3_E,
    OVERCOAT_BONE_LEFTLIMB3_E_LEFTLIMB3_F,
    OVERCOAT_BONE_LEFTLIMB3_F_LEFTLIMB3_G,
    OVERCOAT_BONE_LEFTLIMB3_G_LEFTLIMB3_H,
    OVERCOAT_BONE_LEFTLIMB3_H_LEFTLIMB3_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB4_A,             // Root & Left Limb 4 A
    OVERCOAT_BONE_LEFTLIMB4_A_LEFTLIMB4_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB4_B_LEFTLIMB4_C,
    OVERCOAT_BONE_LEFTLIMB4_C_LEFTLIMB4_D,
    OVERCOAT_BONE_LEFTLIMB4_D_LEFTLIMB4_E,
    OVERCOAT_BONE_LEFTLIMB4_E_LEFTLIMB4_F,
    OVERCOAT_BONE_LEFTLIMB4_F_LEFTLIMB4_G,
    OVERCOAT_BONE_LEFTLIMB4_G_LEFTLIMB4_H,
    OVERCOAT_BONE_LEFTLIMB4_H_LEFTLIMB4_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB5_A,             // Root & Left Limb 5 A
    OVERCOAT_BONE_LEFTLIMB5_A_LEFTLIMB5_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB5_B_LEFTLIMB5_C,
    OVERCOAT_BONE_LEFTLIMB5_C_LEFTLIMB5_D,
    OVERCOAT_BONE_LEFTLIMB5_D_LEFTLIMB5_E,
    OVERCOAT_BONE_LEFTLIMB5_E_LEFTLIMB5_F,
    OVERCOAT_BONE_LEFTLIMB5_F_LEFTLIMB5_G,
    OVERCOAT_BONE_LEFTLIMB5_G_LEFTLIMB5_H,
    OVERCOAT_BONE_LEFTLIMB5_H_LEFTLIMB5_I,
    OVERCOAT_BONE_LEFTLIMB5_I_LEFTLIMB5_J,
    OVERCOAT_BONE_LEFTLIMB5_J_LEFTLIMB5_K,
    OVERCOAT_BONE_LEFTLIMB5_K_LEFTLIMB5_L,
    OVERCOAT_BONE_ROOT_LEFTLIMB6_A,             // Root & Left Limb 6 A
    OVERCOAT_BONE_LEFTLIMB6_A_LEFTLIMB6_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB6_B_LEFTLIMB6_C,
    OVERCOAT_BONE_LEFTLIMB6_C_LEFTLIMB6_D,
    OVERCOAT_BONE_LEFTLIMB6_D_LEFTLIMB6_E,
    OVERCOAT_BONE_LEFTLIMB6_E_LEFTLIMB6_F,
    OVERCOAT_BONE_LEFTLIMB6_F_LEFTLIMB6_G,
    OVERCOAT_BONE_LEFTLIMB6_G_LEFTLIMB6_H,
    OVERCOAT_BONE_LEFTLIMB6_H_LEFTLIMB6_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB7_A,             // Root & Left Limb 7 A
    OVERCOAT_BONE_LEFTLIMB7_A_LEFTLIMB7_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB7_B_LEFTLIMB7_C,
    OVERCOAT_BONE_LEFTLIMB7_C_LEFTLIMB7_D,
    OVERCOAT_BONE_LEFTLIMB7_D_LEFTLIMB7_E,
    OVERCOAT_BONE_LEFTLIMB7_E_LEFTLIMB7_F,
    OVERCOAT_BONE_LEFTLIMB7_F_LEFTLIMB7_G,
    OVERCOAT_BONE_LEFTLIMB7_G_LEFTLIMB7_H,
    OVERCOAT_BONE_LEFTLIMB7_H_LEFTLIMB7_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB8_A,             // Root & Left Limb 8 A
    OVERCOAT_BONE_LEFTLIMB8_A_LEFTLIMB8_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB8_B_LEFTLIMB8_C,
    OVERCOAT_BONE_LEFTLIMB8_C_LEFTLIMB8_D,
    OVERCOAT_BONE_LEFTLIMB8_D_LEFTLIMB8_E,
    OVERCOAT_BONE_LEFTLIMB8_E_LEFTLIMB8_F,
    OVERCOAT_BONE_LEFTLIMB8_F_LEFTLIMB8_G,
    OVERCOAT_BONE_LEFTLIMB8_G_LEFTLIMB8_H,
    OVERCOAT_BONE_LEFTLIMB8_H_LEFTLIMB8_I,
    OVERCOAT_BONE_ROOT_LEFTLIMB9_A,             // Root & Left Limb 9 A
    OVERCOAT_BONE_LEFTLIMB9_A_LEFTLIMB9_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_LEFTLIMB9_B_LEFTLIMB9_C,
    OVERCOAT_BONE_LEFTLIMB9_C_LEFTLIMB9_D,
    OVERCOAT_BONE_LEFTLIMB9_D_LEFTLIMB9_E,
    OVERCOAT_BONE_LEFTLIMB9_E_LEFTLIMB9_F,
    OVERCOAT_BONE_LEFTLIMB9_F_LEFTLIMB9_G,
    OVERCOAT_BONE_LEFTLIMB9_G_LEFTLIMB9_H,
    OVERCOAT_BONE_LEFTLIMB9_H_LEFTLIMB9_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB0_A,             // Root & Right Limb 0 A
    OVERCOAT_BONE_RIGHTLIMB0_A_RIGHTLIMB0_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB0_B_RIGHTLIMB0_C,
    OVERCOAT_BONE_RIGHTLIMB0_C_RIGHTLIMB0_D,
    OVERCOAT_BONE_RIGHTLIMB0_D_RIGHTLIMB0_E,
    OVERCOAT_BONE_RIGHTLIMB0_E_RIGHTLIMB0_F,
    OVERCOAT_BONE_RIGHTLIMB0_F_RIGHTLIMB0_G,
    OVERCOAT_BONE_RIGHTLIMB0_G_RIGHTLIMB0_H,
    OVERCOAT_BONE_RIGHTLIMB0_H_RIGHTLIMB0_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB1_A,             // Root & Left Limb 1 A
    OVERCOAT_BONE_RIGHTLIMB1_A_RIGHTLIMB1_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB1_B_RIGHTLIMB1_C,
    OVERCOAT_BONE_RIGHTLIMB1_C_RIGHTLIMB1_D,
    OVERCOAT_BONE_RIGHTLIMB1_D_RIGHTLIMB1_E,
    OVERCOAT_BONE_RIGHTLIMB1_E_RIGHTLIMB1_F,
    OVERCOAT_BONE_RIGHTLIMB1_F_RIGHTLIMB1_G,
    OVERCOAT_BONE_RIGHTLIMB1_G_RIGHTLIMB1_H,
    OVERCOAT_BONE_RIGHTLIMB1_H_RIGHTLIMB1_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB2_A,             // Root & Left Limb 2 A
    OVERCOAT_BONE_RIGHTLIMB2_A_RIGHTLIMB2_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB2_B_RIGHTLIMB2_C,
    OVERCOAT_BONE_RIGHTLIMB2_C_RIGHTLIMB2_D,
    OVERCOAT_BONE_RIGHTLIMB2_D_RIGHTLIMB2_E,
    OVERCOAT_BONE_RIGHTLIMB2_E_RIGHTLIMB2_F,
    OVERCOAT_BONE_RIGHTLIMB2_F_RIGHTLIMB2_G,
    OVERCOAT_BONE_RIGHTLIMB2_G_RIGHTLIMB2_H,
    OVERCOAT_BONE_RIGHTLIMB2_H_RIGHTLIMB2_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB3_A,             // Root & Left Limb 3 A
    OVERCOAT_BONE_RIGHTLIMB3_A_RIGHTLIMB3_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB3_B_RIGHTLIMB3_C,
    OVERCOAT_BONE_RIGHTLIMB3_C_RIGHTLIMB3_D,
    OVERCOAT_BONE_RIGHTLIMB3_D_RIGHTLIMB3_E,
    OVERCOAT_BONE_RIGHTLIMB3_E_RIGHTLIMB3_F,
    OVERCOAT_BONE_RIGHTLIMB3_F_RIGHTLIMB3_G,
    OVERCOAT_BONE_RIGHTLIMB3_G_RIGHTLIMB3_H,
    OVERCOAT_BONE_RIGHTLIMB3_H_RIGHTLIMB3_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB4_A,             // Root & Left Limb 4 A
    OVERCOAT_BONE_RIGHTLIMB4_A_RIGHTLIMB4_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB4_B_RIGHTLIMB4_C,
    OVERCOAT_BONE_RIGHTLIMB4_C_RIGHTLIMB4_D,
    OVERCOAT_BONE_RIGHTLIMB4_D_RIGHTLIMB4_E,
    OVERCOAT_BONE_RIGHTLIMB4_E_RIGHTLIMB4_F,
    OVERCOAT_BONE_RIGHTLIMB4_F_RIGHTLIMB4_G,
    OVERCOAT_BONE_RIGHTLIMB4_G_RIGHTLIMB4_H,
    OVERCOAT_BONE_RIGHTLIMB4_H_RIGHTLIMB4_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB5_A,             // Root & Left Limb 5 A
    OVERCOAT_BONE_RIGHTLIMB5_A_RIGHTLIMB5_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB5_B_RIGHTLIMB5_C,
    OVERCOAT_BONE_RIGHTLIMB5_C_RIGHTLIMB5_D,
    OVERCOAT_BONE_RIGHTLIMB5_D_RIGHTLIMB5_E,
    OVERCOAT_BONE_RIGHTLIMB5_E_RIGHTLIMB5_F,
    OVERCOAT_BONE_RIGHTLIMB5_F_RIGHTLIMB5_G,
    OVERCOAT_BONE_RIGHTLIMB5_G_RIGHTLIMB5_H,
    OVERCOAT_BONE_RIGHTLIMB5_H_RIGHTLIMB5_I,
    OVERCOAT_BONE_RIGHTLIMB5_I_RIGHTLIMB5_J,
    OVERCOAT_BONE_RIGHTLIMB5_J_RIGHTLIMB5_K,
    OVERCOAT_BONE_RIGHTLIMB5_K_RIGHTLIMB5_L,
    OVERCOAT_BONE_ROOT_RIGHTLIMB6_A,             // Root & Left Limb 6 A
    OVERCOAT_BONE_RIGHTLIMB6_A_RIGHTLIMB6_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB6_B_RIGHTLIMB6_C,
    OVERCOAT_BONE_RIGHTLIMB6_C_RIGHTLIMB6_D,
    OVERCOAT_BONE_RIGHTLIMB6_D_RIGHTLIMB6_E,
    OVERCOAT_BONE_RIGHTLIMB6_E_RIGHTLIMB6_F,
    OVERCOAT_BONE_RIGHTLIMB6_F_RIGHTLIMB6_G,
    OVERCOAT_BONE_RIGHTLIMB6_G_RIGHTLIMB6_H,
    OVERCOAT_BONE_RIGHTLIMB6_H_RIGHTLIMB6_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB7_A,             // Root & Left Limb 7 A
    OVERCOAT_BONE_RIGHTLIMB7_A_RIGHTLIMB7_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB7_B_RIGHTLIMB7_C,
    OVERCOAT_BONE_RIGHTLIMB7_C_RIGHTLIMB7_D,
    OVERCOAT_BONE_RIGHTLIMB7_D_RIGHTLIMB7_E,
    OVERCOAT_BONE_RIGHTLIMB7_E_RIGHTLIMB7_F,
    OVERCOAT_BONE_RIGHTLIMB7_F_RIGHTLIMB7_G,
    OVERCOAT_BONE_RIGHTLIMB7_G_RIGHTLIMB7_H,
    OVERCOAT_BONE_RIGHTLIMB7_H_RIGHTLIMB7_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB8_A,             // Root & Left Limb 8 A
    OVERCOAT_BONE_RIGHTLIMB8_A_RIGHTLIMB8_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB8_B_RIGHTLIMB8_C,
    OVERCOAT_BONE_RIGHTLIMB8_C_RIGHTLIMB8_D,
    OVERCOAT_BONE_RIGHTLIMB8_D_RIGHTLIMB8_E,
    OVERCOAT_BONE_RIGHTLIMB8_E_RIGHTLIMB8_F,
    OVERCOAT_BONE_RIGHTLIMB8_F_RIGHTLIMB8_G,
    OVERCOAT_BONE_RIGHTLIMB8_G_RIGHTLIMB8_H,
    OVERCOAT_BONE_RIGHTLIMB8_H_RIGHTLIMB8_I,
    OVERCOAT_BONE_ROOT_RIGHTLIMB9_A,             // Root & Left Limb 9 A
    OVERCOAT_BONE_RIGHTLIMB9_A_RIGHTLIMB9_B,        // Back Limb A to Back Limb B
    OVERCOAT_BONE_RIGHTLIMB9_B_RIGHTLIMB9_C,
    OVERCOAT_BONE_RIGHTLIMB9_C_RIGHTLIMB9_D,
    OVERCOAT_BONE_RIGHTLIMB9_D_RIGHTLIMB9_E,
    OVERCOAT_BONE_RIGHTLIMB9_E_RIGHTLIMB9_F,
    OVERCOAT_BONE_RIGHTLIMB9_F_RIGHTLIMB9_G,
    OVERCOAT_BONE_RIGHTLIMB9_G_RIGHTLIMB9_H,
    OVERCOAT_BONE_RIGHTLIMB9_H_RIGHTLIMB9_I,
    OVERCOAT_BONE_MAX  
} Overcoat_BoneIndex;

// Struct containing phys bones of overcoat
PhysBone* overcoat_PhysBones[OVERCOAT_BONE_MAX] = {
    &overcoat_RootLimbBackLimb_A,
    &overcoat_BackLimb_A_BackLimb_B,
    &overcoat_BackLimb_B_BackLimb_C,
    &overcoat_BackLimb_C_BackLimb_D,
    &overcoat_BackLimb_D_BackLimb_E,
    &overcoat_BackLimb_E_BackLimb_F,
    &overcoat_BackLimb_F_BackLimb_G,
    &overcoat_BackLimb_G_BackLimb_H,
    &overcoat_BackLimb_H_BackLimb_I,
    &overcoat_RootLimbLeftLimb0_A,
    &overcoat_LeftLimb0_A_LeftLimb0_B,
    &overcoat_LeftLimb0_B_LeftLimb0_C,
    &overcoat_LeftLimb0_C_LeftLimb0_D,
    &overcoat_LeftLimb0_D_LeftLimb0_E,
    &overcoat_LeftLimb0_E_LeftLimb0_F,
    &overcoat_LeftLimb0_F_LeftLimb0_G,
    &overcoat_LeftLimb0_G_LeftLimb0_H,
    &overcoat_LeftLimb0_H_LeftLimb0_I,
    &overcoat_RootLimbLeftLimb1_A,
    &overcoat_LeftLimb1_A_LeftLimb1_B,
    &overcoat_LeftLimb1_B_LeftLimb1_C,
    &overcoat_LeftLimb1_C_LeftLimb1_D,
    &overcoat_LeftLimb1_D_LeftLimb1_E,
    &overcoat_LeftLimb1_E_LeftLimb1_F,
    &overcoat_LeftLimb1_F_LeftLimb1_G,
    &overcoat_LeftLimb1_G_LeftLimb1_H,
    &overcoat_LeftLimb1_H_LeftLimb1_I,
    &overcoat_RootLimbLeftLimb2_A,
    &overcoat_LeftLimb2_A_LeftLimb2_B,
    &overcoat_LeftLimb2_B_LeftLimb2_C,
    &overcoat_LeftLimb2_C_LeftLimb2_D,
    &overcoat_LeftLimb2_D_LeftLimb2_E,
    &overcoat_LeftLimb2_E_LeftLimb2_F,
    &overcoat_LeftLimb2_F_LeftLimb2_G,
    &overcoat_LeftLimb2_G_LeftLimb2_H,
    &overcoat_LeftLimb2_H_LeftLimb2_I,
    &overcoat_RootLimbLeftLimb3_A,
    &overcoat_LeftLimb3_A_LeftLimb3_B,
    &overcoat_LeftLimb3_B_LeftLimb3_C,
    &overcoat_LeftLimb3_C_LeftLimb3_D,
    &overcoat_LeftLimb3_D_LeftLimb3_E,
    &overcoat_LeftLimb3_E_LeftLimb3_F,
    &overcoat_LeftLimb3_F_LeftLimb3_G,
    &overcoat_LeftLimb3_G_LeftLimb3_H,
    &overcoat_LeftLimb3_H_LeftLimb3_I,
    &overcoat_RootLimbLeftLimb4_A,
    &overcoat_LeftLimb4_A_LeftLimb4_B,
    &overcoat_LeftLimb4_B_LeftLimb4_C,
    &overcoat_LeftLimb4_C_LeftLimb4_D,
    &overcoat_LeftLimb4_D_LeftLimb4_E,
    &overcoat_LeftLimb4_E_LeftLimb4_F,
    &overcoat_LeftLimb4_F_LeftLimb4_G,
    &overcoat_LeftLimb4_G_LeftLimb4_H,
    &overcoat_LeftLimb4_H_LeftLimb4_I,
    &overcoat_RootLimbLeftLimb5_A,
    &overcoat_LeftLimb5_A_LeftLimb5_B,
    &overcoat_LeftLimb5_B_LeftLimb5_C,
    &overcoat_LeftLimb5_C_LeftLimb5_D,
    &overcoat_LeftLimb5_D_LeftLimb5_E,
    &overcoat_LeftLimb5_E_LeftLimb5_F,
    &overcoat_LeftLimb5_F_LeftLimb5_G,
    &overcoat_LeftLimb5_G_LeftLimb5_H,
    &overcoat_LeftLimb5_H_LeftLimb5_I,
    &overcoat_LeftLimb5_I_LeftLimb5_J,
    &overcoat_LeftLimb5_J_LeftLimb5_K,
    &overcoat_LeftLimb5_K_LeftLimb5_L,
    &overcoat_RootLimbLeftLimb6_A,
    &overcoat_LeftLimb6_A_LeftLimb6_B,
    &overcoat_LeftLimb6_B_LeftLimb6_C,
    &overcoat_LeftLimb6_C_LeftLimb6_D,
    &overcoat_LeftLimb6_D_LeftLimb6_E,
    &overcoat_LeftLimb6_E_LeftLimb6_F,
    &overcoat_LeftLimb6_F_LeftLimb6_G,
    &overcoat_LeftLimb6_G_LeftLimb6_H,
    &overcoat_LeftLimb6_H_LeftLimb6_I,
    &overcoat_RootLimbLeftLimb7_A,
    &overcoat_LeftLimb7_A_LeftLimb7_B,
    &overcoat_LeftLimb7_B_LeftLimb7_C,
    &overcoat_LeftLimb7_C_LeftLimb7_D,
    &overcoat_LeftLimb7_D_LeftLimb7_E,
    &overcoat_LeftLimb7_E_LeftLimb7_F,
    &overcoat_LeftLimb7_F_LeftLimb7_G,
    &overcoat_LeftLimb7_G_LeftLimb7_H,
    &overcoat_LeftLimb7_H_LeftLimb7_I,
    &overcoat_RootLimbLeftLimb8_A,
    &overcoat_LeftLimb8_A_LeftLimb8_B,
    &overcoat_LeftLimb8_B_LeftLimb8_C,
    &overcoat_LeftLimb8_C_LeftLimb8_D,
    &overcoat_LeftLimb8_D_LeftLimb8_E,
    &overcoat_LeftLimb8_E_LeftLimb8_F,
    &overcoat_LeftLimb8_F_LeftLimb8_G,
    &overcoat_LeftLimb8_G_LeftLimb8_H,
    &overcoat_LeftLimb8_H_LeftLimb8_I,
    &overcoat_RootLimbLeftLimb9_A,
    &overcoat_LeftLimb9_A_LeftLimb9_B,
    &overcoat_LeftLimb9_B_LeftLimb9_C,
    &overcoat_LeftLimb9_C_LeftLimb9_D,
    &overcoat_LeftLimb9_D_LeftLimb9_E,
    &overcoat_LeftLimb9_E_LeftLimb9_F,
    &overcoat_LeftLimb9_F_LeftLimb9_G,
    &overcoat_LeftLimb9_G_LeftLimb9_H,
    &overcoat_LeftLimb9_H_LeftLimb9_I,
    &overcoat_RootLimbRightLimb0_A,
    &overcoat_RightLimb0_A_RightLimb0_B,
    &overcoat_RightLimb0_B_RightLimb0_C,
    &overcoat_RightLimb0_C_RightLimb0_D,
    &overcoat_RightLimb0_D_RightLimb0_E,
    &overcoat_RightLimb0_E_RightLimb0_F,
    &overcoat_RightLimb0_F_RightLimb0_G,
    &overcoat_RightLimb0_G_RightLimb0_H,
    &overcoat_RightLimb0_H_RightLimb0_I,
    &overcoat_RootLimbRightLimb1_A,
    &overcoat_RightLimb1_A_RightLimb1_B,
    &overcoat_RightLimb1_B_RightLimb1_C,
    &overcoat_RightLimb1_C_RightLimb1_D,
    &overcoat_RightLimb1_D_RightLimb1_E,
    &overcoat_RightLimb1_E_RightLimb1_F,
    &overcoat_RightLimb1_F_RightLimb1_G,
    &overcoat_RightLimb1_G_RightLimb1_H,
    &overcoat_RightLimb1_H_RightLimb1_I,
    &overcoat_RootLimbRightLimb2_A,
    &overcoat_RightLimb2_A_RightLimb2_B,
    &overcoat_RightLimb2_B_RightLimb2_C,
    &overcoat_RightLimb2_C_RightLimb2_D,
    &overcoat_RightLimb2_D_RightLimb2_E,
    &overcoat_RightLimb2_E_RightLimb2_F,
    &overcoat_RightLimb2_F_RightLimb2_G,
    &overcoat_RightLimb2_G_RightLimb2_H,
    &overcoat_RightLimb2_H_RightLimb2_I,
    &overcoat_RootLimbRightLimb3_A,
    &overcoat_RightLimb3_A_RightLimb3_B,
    &overcoat_RightLimb3_B_RightLimb3_C,
    &overcoat_RightLimb3_C_RightLimb3_D,
    &overcoat_RightLimb3_D_RightLimb3_E,
    &overcoat_RightLimb3_E_RightLimb3_F,
    &overcoat_RightLimb3_F_RightLimb3_G,
    &overcoat_RightLimb3_G_RightLimb3_H,
    &overcoat_RightLimb3_H_RightLimb3_I,
    &overcoat_RootLimbRightLimb4_A,
    &overcoat_RightLimb4_A_RightLimb4_B,
    &overcoat_RightLimb4_B_RightLimb4_C,
    &overcoat_RightLimb4_C_RightLimb4_D,
    &overcoat_RightLimb4_D_RightLimb4_E,
    &overcoat_RightLimb4_E_RightLimb4_F,
    &overcoat_RightLimb4_F_RightLimb4_G,
    &overcoat_RightLimb4_G_RightLimb4_H,
    &overcoat_RightLimb4_H_RightLimb4_I,
    &overcoat_RootLimbRightLimb5_A,
    &overcoat_RightLimb5_A_RightLimb5_B,
    &overcoat_RightLimb5_B_RightLimb5_C,
    &overcoat_RightLimb5_C_RightLimb5_D,
    &overcoat_RightLimb5_D_RightLimb5_E,
    &overcoat_RightLimb5_E_RightLimb5_F,
    &overcoat_RightLimb5_F_RightLimb5_G,
    &overcoat_RightLimb5_G_RightLimb5_H,
    &overcoat_RightLimb5_H_RightLimb5_I,
    &overcoat_RightLimb5_I_RightLimb5_J,
    &overcoat_RightLimb5_J_RightLimb5_K,
    &overcoat_RightLimb5_K_RightLimb5_L,
    &overcoat_RootLimbRightLimb6_A,
    &overcoat_RightLimb6_A_RightLimb6_B,
    &overcoat_RightLimb6_B_RightLimb6_C,
    &overcoat_RightLimb6_C_RightLimb6_D,
    &overcoat_RightLimb6_D_RightLimb6_E,
    &overcoat_RightLimb6_E_RightLimb6_F,
    &overcoat_RightLimb6_F_RightLimb6_G,
    &overcoat_RightLimb6_G_RightLimb6_H,
    &overcoat_RightLimb6_H_RightLimb6_I,
    &overcoat_RootLimbRightLimb7_A,
    &overcoat_RightLimb7_A_RightLimb7_B,
    &overcoat_RightLimb7_B_RightLimb7_C,
    &overcoat_RightLimb7_C_RightLimb7_D,
    &overcoat_RightLimb7_D_RightLimb7_E,
    &overcoat_RightLimb7_E_RightLimb7_F,
    &overcoat_RightLimb7_F_RightLimb7_G,
    &overcoat_RightLimb7_G_RightLimb7_H,
    &overcoat_RightLimb7_H_RightLimb7_I,
    &overcoat_RootLimbRightLimb8_A,
    &overcoat_RightLimb8_A_RightLimb8_B,
    &overcoat_RightLimb8_B_RightLimb8_C,
    &overcoat_RightLimb8_C_RightLimb8_D,
    &overcoat_RightLimb8_D_RightLimb8_E,
    &overcoat_RightLimb8_E_RightLimb8_F,
    &overcoat_RightLimb8_F_RightLimb8_G,
    &overcoat_RightLimb8_G_RightLimb8_H,
    &overcoat_RightLimb8_H_RightLimb8_I,
    &overcoat_RootLimbRightLimb9_A,
    &overcoat_RightLimb9_A_RightLimb9_B,
    &overcoat_RightLimb9_B_RightLimb9_C,
    &overcoat_RightLimb9_C_RightLimb9_D,
    &overcoat_RightLimb9_D_RightLimb9_E,
    &overcoat_RightLimb9_E_RightLimb9_F,
    &overcoat_RightLimb9_F_RightLimb9_G,
    &overcoat_RightLimb9_G_RightLimb9_H,
    &overcoat_RightLimb9_H_RightLimb9_I
};

// Overcoat struct 
struct Overcoat;

typedef void (*OvercoatActionFunc)(struct Overcoat*, struct PlayState*);

typedef struct Overcoat {
    Actor actor;
    SkelAnime skelAnime;
    Vec3s jointTable[GOVERCOATSKEL_NUM_LIMBS];
    Vec3s morphTable[GOVERCOATSKEL_NUM_LIMBS];
    Vec3f bodyPartsPos[OVERCOAT_BODYPART_MAX];
    ColliderJntSph collider;
    u8 needsReset;
    u8 hideInFirstPerson;
    u8 wasGiant;
} Overcoat;


typedef struct {
    s16 startLimb;   // First PhysLimb in the chain, which is A
    s16 limbCount;   // Number of PhysLimbs in the chain
    s16 startBone;   // First PhysBone used by the chain
} OvercoatChainInit;


void Overcoat_Init(Actor* thisx, PlayState* play);
void Overcoat_Destroy(Actor* thisx, PlayState* play);
void Overcoat_Update(Actor* thisx, PlayState* play);
void Overcoat_Draw(Actor* thisx, PlayState* play); 

void Overcoat_RotateJoints(Overcoat* this, PhysBone* gPhysBones[], Player* player);
void Overcoat_InitChain(Overcoat* this, Player* player, PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[],
    int startLimb, int limbCount, int startBone);
void Overcoat_SetDefaultBodyPartsPos(Overcoat* this, Player* player, PhysLimb* gPhysLimbs[], 
    PhysBone* gPhysBones[]);
void Overcoat_CaptureLimitAxes(MtxF* mtx);
void Overcoat_LimitBoneDirection(PhysLimb* parentPhysLimb, PhysLimb* childPhysLimb, 
    Vec3f* outwardDirVec, u8 limitInward);
void Overcoat_LimitUpperChains(PhysLimb* gPhysLimbs[], const int chainStarts[], int chainCount);
void Overcoat_LimitSidewaysDistance(PhysLimb* physLimbA, PhysLimb* physLimbB, f32 maxDistance);
bool Overcoat_NormalizeSubstepDirectionVec(Vec3f* targetVec);
void Overcoat_SetSubstepDirAxes(Vec3f targetForward, Vec3f targetDown, f32 t);
Vec3f Overcoat_FindClosestPointOnCapsule(Vec3f targetPos, PhysSphereCollider* capsuleColStart, 
    PhysSphereCollider* capsuleColEnd, f32* colRadius);
f32 Overcoat_GroupContact(Vec3f targetPos, f32 pointRadius,  PhysSphereCollider* torso_collider, 
    PhysSphereCollider* capsulePairs[][2], int capsuleColCount, Vec3f* contactColCenter, 
    f32* contactColRadius);
bool Overcoat_GroupOutside(Vec3f targetPos, f32 pointRadius, PhysSphereCollider* torso_collider, 
	PhysSphereCollider* capsulePairs[][2], int capsuleColCount);
bool Overcoat_GroupRayExit(Vec3f targetPhysLimbPos, Vec3f physLimbDir, f32 pointRadius,
    PhysSphereCollider* torso_collider, PhysSphereCollider* capsulePairs[][2], int capsuleColCount,
    f32 padding, Vec3f* exitPosition);
void Overcoat_SolveColliderGroup(PhysLimb* physLimb, PhysSphereCollider* torso_collider, 
	PhysSphereCollider* collider_pairs[][2], int capsuleColCount);
void Overcoat_SolveMidpointGroup(PhysLimb* physLimb_a, PhysLimb* physLimb_b, 
	PhysSphereCollider* torso_collider, PhysSphereCollider* collider_pairs[][2], int capsuleColCount);
void Overcoat_SetResetShape(Overcoat* this, PhysLimb* gPhysLimbs[], const int chains[], int chainCount);
void Overcoat_SolvePhysAtSubSteps(Overcoat* this, Player* player, Vec3f apply_force, PhysLimb* gPhysLimbs[], 
	PhysBone* gPhysBones[], Vec3f anchorStart[], const int chains[], int chainCount, 
    f32 sidewaysMaxDistance[][9], PhysSphereCollider* capsulePairs[][2], int capsuleCount);
void Overcoat_UpdatePinnedLimb(Overcoat* this, Player* player, PhysLimb* gPhysLimbs[], int pinnedLimbIdx);
void Overcoat_UpdateBodyPartsPos(Overcoat* this, Player* player, Vec3f apply_force, 
    PhysLimb* gPhysLimbs[], PhysBone* gPhysBones[]);
void Overcoat_RotateFromPinned(Vec3f* direction, Vec3s* rotation);
void Overcoat_RotateChain(Overcoat* this, PhysBone* gPhysBones[], int rootBoneIdx, 
    int chainLength);

#endif