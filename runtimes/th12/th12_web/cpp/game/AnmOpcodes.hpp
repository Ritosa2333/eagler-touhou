#pragma once
#include "PortTitle.hpp"
#include "Types.hpp"
#include "Th16AnmDispatch.hpp"

// TH13 uses different instruction numbers and sometimes different behavior.
// "native" rows are checked against its own executable. The remaining historical
// vote/shape/family rows are hypotheses, not proof of semantic equivalence.
// tools/th-anm-opcode-resolve.py preserves native overrides when regenerating.
namespace th11 {
// `witnesses` is the number of measured votes whose arguments matched exactly (a family row names instead how
// many measured rows fix its displacement), and `evidence` says which of the three arguments carries the row:
// "measured" (votes), "family" (the two versions' tables agree, signature for signature, across a stretch of
// consecutive opcodes, with every measured row inside it agreeing on the displacement) or "shape" (the version's
// argument signature from `reference/thtk/thanm/thanm.c` admits only one candidate).
struct AnmOpcodeRow { u16 title_opcode, canonical_opcode, witnesses; const char* evidence; };
// Internal commands for behavior absent from the older canonical interpreter.
constexpr u16 anm_th13_layer = 0x7000;
constexpr u16 anm_th13_layer_flag = 0x7001;
constexpr u16 anm_th13_random_polar = 0x7002;
constexpr u16 anm_th13_flag15 = 0x7003;
constexpr u16 anm_th13_texture_address = 0x7004;
constexpr u16 anm_th13_texture_scale = 0x7005;
constexpr u16 anm_th13_texture_scale_interpolate = 0x7006;
constexpr u16 anm_th13_extra_scale = 0x7007;
constexpr u16 anm_th13_extra_scale_interpolate = 0x7008;
constexpr u16 anm_th13_pivot = 0x7009;
constexpr u16 anm_th13_rotation_order = 0x700a;
constexpr u16 anm_th13_polar_interpolate = 0x700b;
constexpr u16 anm_th13_vertex_color_mode = 0x700c;
constexpr u16 anm_th13_flag6 = 0x700d;
constexpr u16 anm_th13_flag7 = 0x700e;
constexpr u16 anm_th13_flag8 = 0x700f;
constexpr u16 anm_th13_parent_independent = 0x7010;
constexpr u16 anm_th13_rectangle = 0x7011;
constexpr u16 anm_th13_gradient_rectangle = 0x7012;
constexpr u16 anm_th13_soft_rectangle = 0x7013;
constexpr u16 anm_th13_soft_gradient_rectangle = 0x7014;
constexpr u16 anm_th13_circle = 0x7015;
constexpr u16 anm_th13_circle_outline = 0x7016;
constexpr u16 anm_th13_arc = 0x7017;
constexpr u16 anm_th13_callback_effect = 0x7018;
constexpr u16 anm_th14_layer = 0x7100;
constexpr u16 anm_th14_layer_flag = 0x7101;
constexpr u16 anm_th14_parent_independent = 0x7102;
constexpr u16 anm_th14_blend = 0x7103;
constexpr u16 anm_th14_visual_rng = 0x7104;
constexpr u16 anm_th14_rgb = 0x7105;
constexpr u16 anm_th14_resolution = 0x7106;
constexpr u16 anm_th14_flag22 = 0x7107;
constexpr u16 anm_th14_flag24 = 0x7108;
constexpr u16 anm_th14_polar = 0x7109;
constexpr u16 anm_th14_sprite = 0x710a;
constexpr u16 anm_th14_copy_variables = 0x710b;
constexpr u16 anm_th14_random_sprite = 0x710c;
constexpr u16 anm_th14_cylinder = 0x710d;
constexpr u16 anm_th14_world_arc = 0x710e;
constexpr u16 anm_th16_flag12 = 0x710f;  // the numbered flag the port was missing: slot 0x534 bit
                                          // 13 is flag 12 by the measured relation bit = flag + 1
constexpr u16 anm_th16_flag13 = 0x7110;  // slot 0x530 bit 13; the flags half is case 74 exactly,
                                          // and this home adds the numbered copy in flags2 bit 13
constexpr u16 anm_th16_blend = 0x7111;   // slot 0x530 bits 5..8: the measured field of op 303,
                                          // which no body in the vocabulary reproduced (mask 0x1e0)
constexpr u16 anm_th16_field25 = 0x7112;  // slot 0x530 bits 25..29: op 302 writes argument&31 there,
                                          // five bits, where the older body wrote only two at 22,23
constexpr u16 anm_th16_visual_pair = 0x7113; // slot 0x530 bits 30..31: op 312 writes argument2&3,
                                          // the rng and rgb selectors together, mirrored to flags2 9,10
constexpr u16 anm_th16_layer = 0x7114;    // slot 0x534 bit 18: op 304 writes one constant bit there
                                          // when the argument low byte is at least two and not 0xff

// TH16-only corrections from the complete retail handlers, not value matching.
constexpr u16 anm_th16_native_layer = 0x7115;
constexpr u16 anm_th16_native_mode = 0x7116;
constexpr u16 anm_th16_native_resolution = 0x7117;
constexpr u16 anm_th16_native_anchor = 0x7118;
constexpr u16 anm_th16_native_origin = 0x7119;
constexpr u16 anm_th16_scale_interpolate = 0x711a;
constexpr u16 anm_th16_secondary_rgb_interpolate = 0x711b;
constexpr u16 anm_th16_secondary_alpha_interpolate = 0x711c;
constexpr u16 anm_th16_angular_velocity = 0x711d;
constexpr u16 anm_th16_scale_velocity = 0x711e;
constexpr u16 anm_th16_scroll_x_velocity = 0x711f;
constexpr u16 anm_th16_scroll_y_velocity = 0x7120;
constexpr u16 anm_th16_rectangle = 0x7121;
constexpr u16 anm_th16_gradient_rectangle = 0x7122;
constexpr u16 anm_th16_soft_rectangle = 0x7123;
constexpr u16 anm_th16_soft_gradient_rectangle = 0x7124;
constexpr u16 anm_th16_circle = 0x7125;
constexpr u16 anm_th16_circle_outline = 0x7126;
constexpr u16 anm_th16_copy_variables = 0x7127;
constexpr u16 anm_th16_rectangle_mode26 = 0x7128;
constexpr u16 anm_th16_rectangle_mode27 = 0x7129;
constexpr u16 anm_th16_ring = 0x712a;
constexpr u16 anm_th16_centered_arc = 0x712b;
constexpr u16 anm_th16_arc = 0x712c;
constexpr u16 anm_th16_ellipse = 0x712d;
constexpr u16 anm_th16_native_flag13 = 0x712e;
constexpr u16 anm_th16_field2_bit10 = 0x712f;
constexpr u16 anm_th16_field2_bit11 = 0x7130;
constexpr u16 anm_th16_field2_bit23 = 0x7131;
constexpr u16 anm_th16_field2_bit25 = 0x7132;
constexpr u16 anm_th16_field2_bit13 = 0x7133;
constexpr u16 anm_th16_field2_bit7 = 0x7134;
constexpr u16 anm_th16_field2_bit8 = 0x7135;
constexpr u16 anm_th16_field2_bit9 = 0x7136;
constexpr u16 anm_th16_visible = 0x7137;
constexpr AnmOpcodeRow th13_anm_opcodes[] = {

// >>> th13 rows
    { 0, 0, 53, "measured" },  // 
    { 1, 1, 994, "measured" },  // 
    { 2, 2, 0, "native" },  // stop without hiding or retiring the VM
    { 3, 63, 1388, "measured" },  // 
    { 4, 69, 48, "measured" },  // 
    { 5, 64, 2306, "measured" },  // S
    { 6, 75, 0, "native" },  // delay the script timer
    { 7, 81, 541, "measured" },  // 
    { 100, 6, 138, "measured" },  // SS
    { 101, 7, 0, "family" },  // ff
    { 102, 8, 0, "family" },  // SS
    { 103, 9, 44, "measured" },  // ff
    { 104, 10, 0, "family" },  // SS
    { 105, 11, 18, "measured" },  // ff
    { 106, 12, 0, "family" },  // SS
    { 107, 13, 0, "family" },  // ff
    { 108, 14, 0, "family" },  // SS
    { 109, 15, 0, "family" },  // ff
    { 110, 16, 0, "family" },  // SS
    { 111, 17, 0, "family" },  // ff
    { 112, 18, 1, "family" },  // SSS
    { 113, 19, 0, "family" },  // fff
    { 114, 20, 0, "family" },  // SSS
    { 115, 21, 0, "family" },  // fff
    { 116, 22, 0, "family" },  // SSS
    { 117, 23, 42, "family" },  // fff
    { 118, 24, 0, "family" },  // SSS
    { 119, 25, 0, "family" },  // fff
    { 120, 26, 0, "family" },  // SSS
    { 121, 27, 0, "family" },  // fff
    { 122, 40, 0, "context" },  // SS
    { 123, 41, 0, "context" },  // ff
    { 124, 42, 0, "context" },  // ff
    { 125, 43, 0, "context" },  // ff
    { 126, 44, 0, "context" },  // ff
    { 127, 45, 0, "context" },  // ff
    { 128, 46, 0, "context" },  // ff
    { 129, 47, 0, "context" },  // f
    { 131, anm_th13_random_polar, 0, "native" },  // two float destinations, radial limits
    { 200, 4, 131, "measured" },  // ot
    { 201, 5, 0, "shape" },  // Sot
    { 202, 28, 0, "context" },  // SSot
    { 203, 29, 0, "context" },  // ffot
    { 204, 30, 0, "context" },  // SSot
    { 205, 31, 0, "context" },  // ffot
    { 206, 32, 0, "context" },  // SSot
    { 207, 33, 0, "context" },  // ffot
    { 208, 34, 0, "context" },  // SSot
    { 209, 35, 0, "context" },  // ffot
    { 210, 36, 0, "context" },  // SSot
    { 211, 37, 0, "context" },  // ffot
    { 212, 38, 0, "context" },  // SSot
    { 213, 39, 0, "context" },  // ffot
    { 300, 3, 691, "measured" },  // n
    { 301, 102, 0, "shape" },  // nS
    { 302, 67, 0, "native" },  // five-bit draw mode
    { 303, 66, 96, "measured" },  // S
    { 304, anm_th13_layer, 0, "native" },  // layer and flags2 bit 18
    { 305, 73, 4, "measured" },  // S
    { 306, anm_th13_flag15, 0, "native" },  // raw flags bit 15; never delays the timer
    { 307, 87, 368, "measured" },  // S
    { 308, 61, 18, "measured" },  // 
    { 309, 62, 0, "native" },  // vertical flip
    { 310, 72, 0, "native" },  // visibility
    { 311, 89, 187, "measured" },  // S
    { 312, anm_th13_texture_address, 0, "native" },  // U/V addressing, not alpha interpolation
    { 313, 0, 0, "native-default" },
    { 314, 0, 0, "native-default" },
    { 315, 0, 0, "native-default" },
    { 316, 0, 0, "native-default" },
    { 317, 0, 0, "native-default" },
    { 318, 0, 0, "native-default" },
    { 400, 48, 282, "family" },  // fff
    { 401, 49, 45, "measured" },  // fff
    { 402, 50, 280, "measured" },  // ff
    { 403, 51, 769, "measured" },  // S
    { 404, 52, 1942, "measured" },  // SSS
    { 405, 77, 0, "native" },  // secondary alpha, not primary alpha
    { 406, 76, 0, "shape" },  // SSS
    { 407, 56, 263, "measured" },  // SSfff
    { 408, 57, 742, "measured" },  // SSSSS
    { 409, 58, 1394, "measured" },  // SSS
    { 410, 59, 4, "family" },  // SSfff
    { 411, 93, 0, "shape" },  // SSf
    { 412, 60, 325, "measured" },  // SSff
    { 413, 78, 18, "measured" },  // SSSSS
    { 414, 79, 5, "measured" },  // SSS
    { 415, 53, 26, "measured" },  // fff
    { 416, 54, 0, "shape" },  // ff
    { 417, 55, 0, "native" },  // raw alpha, duration
    { 418, 98, 0, "context" },  // 
    { 419, 99, 0, "context" },  // S
    { 420, 100, 0, "context" },  // Sfffffffff
    { 421, 65, 400, "measured" },  // ss
    { 422, 83, 0, "native" },  // move physical position into script position
    { 423, anm_th13_vertex_color_mode, 0, "native" },
    { 424, anm_th13_flag6, 0, "native" },
    { 425, 70, 0, "native" },  // horizontal texture scroll velocity
    { 426, 71, 0, "native" },  // vertical texture scroll velocity
    { 427, 93, 0, "native" },  // horizontal scroll velocity interpolation
    { 428, 94, 0, "native" },  // vertical scroll velocity interpolation
    { 429, anm_th13_texture_scale, 0, "native" },
    { 430, anm_th13_texture_scale_interpolate, 0, "native" },
    { 431, anm_th13_flag7, 0, "native" },
    { 432, anm_th13_flag8, 0, "native" },
    { 433, anm_th13_polar_interpolate, 0, "native" },  // position target from angle and radius
    { 434, anm_th13_extra_scale, 0, "native" },
    { 435, anm_th13_extra_scale_interpolate, 0, "native" },
    { 436, anm_th13_pivot, 0, "native" },
    { 437, anm_th13_rotation_order, 0, "native" },
    { 438, anm_th13_layer_flag, 0, "native" },  // flags2 bit 18
    { 439, 0, 0, "native-default" },
    { 440, 0, 0, "native-default" },
    { 441, 0, 0, "native-default" },
    { 500, 88, 0, "native" },  // child, normal registry (46f990 flags 0)
    { 501, 90, 0, "native" },  // child, overlay registry (46f990 flags 4)
    { 502, 91, 0, "shape" },  // N
    { 503, 92, 0, "measured" },  // N
    { 504, 95, 0, "measured" },  // N
    { 505, 96, 0, "context" },  // Nff
    { 506, 97, 0, "context" },  // Nff
    { 507, anm_th13_parent_independent, 0, "native" },
    { 508, anm_th13_callback_effect, 0, "native" },  // install effect callbacks
    { 509, 0, 0, "native-default" },
    { 600, 84, 0, "native" },  // full ring allocation
    { 601, 101, 0, "native" },  // centered arc allocation
    { 602, anm_th13_arc, 0, "native" },  // arc from initial angle
    { 603, anm_th13_rectangle, 0, "native" },
    { 604, anm_th13_circle, 0, "native" },
    { 605, anm_th13_circle_outline, 0, "native" },
    { 606, anm_th13_gradient_rectangle, 0, "native" },
    { 607, anm_th13_soft_rectangle, 0, "native" },
    { 608, anm_th13_soft_gradient_rectangle, 0, "native" },
    { 609, 0, 0, "native-default" },
    { 610, 0, 0, "native-default" },
    { 612, 0, 0, "native-default" },
    { 613, 0, 0, "native-default" },
    { 614, 0, 0, "native-default" },
    { 617, 0, 0, "native-default" },
    { 618, 0, 0, "native-default" },
    { 628, 0, 0, "native-default" },
    { 633, 0, 0, "native-default" },
    { 634, 0, 0, "native-default" },
// <<< th13 rows

};

// The value that means "the measured table has no row for this opcode". The loader turns it into a named
// failure, because the interpreter's `default:` case skips a command it does not know without saying so.
constexpr u16 anm_opcode_unmapped = 0xffffu;

// First TH14 handlers verified against 1.00b machine-code execution. Missing
// entries must fail loading; raw TH14 IDs are not canonical TH12 instructions.
constexpr AnmOpcodeRow th14_anm_opcodes[] = {
    {508,anm_th13_callback_effect,0,"native"},
    {600,84,0,"native"}, {601,101,0,"native"}, {602,anm_th13_arc,0,"native"},
    {609,anm_th14_cylinder,0,"native"}, {610,anm_th14_world_arc,0,"native"},
    {603,anm_th13_rectangle,0,"native"}, {604,anm_th13_circle,0,"native"}, {605,anm_th13_circle_outline,0,"native"},
    {606,anm_th13_gradient_rectangle,0,"native"}, {607,anm_th13_soft_rectangle,0,"native"}, {608,anm_th13_soft_gradient_rectangle,0,"native"},
    {500,88,0,"native"}, {501,90,0,"native"}, {502,91,0,"native"}, {503,92,0,"native"},
    {504,95,0,"native"}, {505,96,0,"native"}, {506,97,0,"native"}, {509,anm_th14_copy_variables,0,"native"},
    {300,anm_th14_sprite,0,"native"},
    {301,anm_th14_random_sprite,0,"native"}, {420,100,0,"native"},
    {3,63,0,"native"}, {4,69,0,"native"}, {7,81,0,"native"},
    {100,6,0,"native"}, {101,7,0,"native"}, {102,8,0,"native"}, {103,9,0,"native"},
    {104,10,0,"native"}, {105,11,0,"native"}, {106,12,0,"native"}, {107,13,0,"native"},
    {108,14,0,"native"}, {109,15,0,"native"}, {110,16,0,"native"}, {111,17,0,"native"},
    {112,18,0,"native"}, {113,19,0,"native"}, {114,20,0,"native"}, {115,21,0,"native"},
    {116,22,0,"native"}, {117,23,0,"native"}, {118,24,0,"native"}, {119,25,0,"native"},
    {120,26,0,"native"}, {121,27,0,"native"}, {122,40,0,"native"}, {123,41,0,"native"},
    {124,42,0,"native"}, {125,43,0,"native"}, {126,44,0,"native"}, {127,45,0,"native"},
    {128,46,0,"native"}, {129,47,0,"native"},
    {130,anm_th14_polar,0,"native"}, {131,anm_th13_random_polar,0,"native"},
    {200,4,0,"native"}, {201,5,0,"native"}, {202,28,0,"native"}, {203,29,0,"native"},
    {204,30,0,"native"}, {205,31,0,"native"}, {206,32,0,"native"}, {207,33,0,"native"},
    {208,34,0,"native"}, {209,35,0,"native"}, {210,36,0,"native"}, {211,37,0,"native"},
    {212,38,0,"native"}, {213,39,0,"native"},
    {1,1,0,"native"}, {2,2,0,"native"}, {5,64,0,"native-label"}, {6,75,0,"native"},
    {302,67,0,"native"}, {304,anm_th14_layer,0,"native"},
    {306,anm_th13_flag15,0,"native"}, {309,62,0,"native"}, {310,72,0,"native"},
    {312,anm_th13_texture_address,0,"native"}, {403,51,0,"native"}, {405,77,0,"native"},
    {423,anm_th13_vertex_color_mode,0,"native"}, {424,anm_th13_flag6,0,"native"},
    {431,anm_th13_flag7,0,"native"}, {432,anm_th13_flag8,0,"native"},
    {437,anm_th13_rotation_order,0,"native"}, {438,anm_th14_layer_flag,0,"native"},
    {507,anm_th14_parent_independent,0,"native"},
    {303,anm_th14_blend,0,"native"}, {305,73,0,"native"},
    {307,anm_th14_visual_rng,0,"native"}, {308,61,0,"native"}, {311,anm_th14_rgb,0,"native"},
    {313,anm_th14_resolution,0,"native"}, {314,anm_th14_flag22,0,"native"}, {315,anm_th14_flag24,0,"native"},
    {421,65,0,"native"},
    {400,48,0,"native"}, {401,49,0,"native"}, {402,50,0,"native"},
    {404,52,0,"native"}, {406,76,0,"native"},
    {407,56,0,"native"}, {408,57,0,"native"}, {409,58,0,"native"},
    {410,59,0,"native"}, {412,60,0,"native"}, {413,78,0,"native"}, {414,79,0,"native"},
    {415,53,0,"native"}, {416,54,0,"native"}, {417,55,0,"native"}, {422,83,0,"native"},
    {425,70,0,"native"}, {426,71,0,"native"}, {427,93,0,"native"}, {428,94,0,"native"},
    {429,anm_th13_texture_scale,0,"native"}, {430,anm_th13_texture_scale_interpolate,0,"native"},
    {433,anm_th13_polar_interpolate,0,"native"}, {434,anm_th13_extra_scale,0,"native"},
    {435,anm_th13_extra_scale_interpolate,0,"native"}, {436,anm_th13_pivot,0,"native"},
    {0,0,0,"native-default"}, {439,0,0,"native-default"}, {440,0,0,"native-default"},
    {316,0,0,"native-default"}, {317,0,0,"native-default"}, {318,0,0,"native-default"},
    {441,0,0,"native-default"}, {611,0,0,"native-default"}, {612,0,0,"native-default"},
    {613,0,0,"native-default"}, {614,0,0,"native-default"}, {617,0,0,"native-default"},
    {618,0,0,"native-default"}, {628,0,0,"native-default"}, {633,0,0,"native-default"},
    {634,0,0,"native-default"},
};

// Rows the value matcher agreed on (tools/th16-anm-match.py, section 306): each is a TH16
// opcode whose body reproduces the original handler on all eight grid cases, measured by
// delta, flags, the title flag word, both timer quantities and integers[0]. The witnesses
// column is that case count. Numbers without a row keep this title's raw numbering.
constexpr AnmOpcodeRow th16_anm_opcodes[] = {
    // The one row these three titles were missing, and its absence is what stopped the title
    // compositor: their second texture's first script ends with 03 00 08 00 00 00 00 00, byte for
    // byte the command th13 and th14 write in that position, and th13's table measures that number
    // as 63 over 1388 witnesses (th14 carries the same row as native). Left unlisted, the raw 3
    // survives, and the interpreter reads the older titles' one-argument sprite: argument(0) then
    // comes from the command after it, whose bytes are the terminator 0xffff. Measured before the
    // row: "asks for sprite 65535 of 90". The three files carry the command at the same slot of
    // the same chunk (59 of the 96-byte script for 16 and 17, 66 for 18).
    {3, 63, 0, "native"},
    {6, 75, 8, "native"},
    // The other half of the pair above, and the number the dispatch ring walked into: raw 4 is the
    // generation's, 29 of them in their own text.anm and all in the eight-byte, argument-less form
    // th12 writes as 69 (98 of 69 in th12's text.anm, and its own raw 4 is one long-form
    // command). TH13's table measures this row 4 -> 69 over 48 witnesses and TH14 carries it
    // native. Unlisted, raw 4 reached th12's canonical 4, which is `jump(argument(0))`, and the
    // command has no argument of its own: the interpreter jumped to the value of the next
    // instruction's header and never came back.
    {4, 69, 0, "native"},
    // The label marker of these titles' scripts, and the one row the stream asks for first: their own
    // text.anm carries raw 5 with a label id 207 times across its 74 scripts (their ascii.anm 26,
    // st05enm.anm 34), while th12's own text.anm and ascii.anm carry 5 zero times - th12 spells
    // the marker 64, which is the number the interrupt scan walks for (`AnmInterpreter.cpp:27`).
    // TH13's table moves this same raw 5 onto 64 and TH14 carries the row as native; the titles'
    // own dispatchers read raw 5 as the default body (0x4629c8 for 16, 0x46a141 for 17, 0x47b6f3
    // for 18 by tools/th-anm-handlers.py), which is the no-op a label has. Left unlisted, the raw 5
    // reached th12's canonical 5, a decrement-and-jump, and the first update landed at +0x0005 of
    // the script - mid-instruction, on a zero word of length zero.
    {5, 64, 0, "native-label"},
    // The argument-less loop marker, and the row whose absence wrote over a live script: every raw 7
    // in these three titles' own files is the eight-byte form (754 in th16, 894 in th17, 652 in
    // th18, and th13 carries 715 of the same shape), so its own header has room for no argument at
    // all. Their own bodies are one shape - a twenty-byte block copied out of the vm, a call on
    // the vm's timer, then the instruction pointer taken from the vm's own word (0x45fcc1 for 16,
    // 0x467169 for 17, 0x478907 for 18, 0x462669 for 13, by tools/th-anm-handlers.py) - and TH13's
    // table measures that body as 7 -> 81 over 541 witnesses. Left unlisted, raw 7 reached th12's
    // canonical 7, the odd half of the expression band, which reads argument 1 and writes its
    // result at `instruction + 8` (its own machine code stores to `lea eax,[esi+8]`, 0x457243):
    // past an eight-byte command, over the next command's header. Measured: the next label read
    // `length 0`, the walk could not leave it, and the update's budget guard ended the frame
    // (sections 382 to 384).
    {7, 81, 0, "native"},
    {102, 8, 40, "native"}, // this title own 102: in place, argument(0) += argument(1)  // th12+0x4b
    {302, anm_th16_field25, 40, "native"}, // measured over every case: the original writes
                              // (argument&31) at slot 0x530 bits 25 to 29, which predicts 40 of
                              // 40 cases including argument 63, where the whole five bits move. The
                              // body the older tables name, case 67, writes only two of those bits
                              // at 22 and 23, so the all-case matcher rejected it.
    {312, anm_th16_visual_pair, 38, "native"}, // measured over every case: the original writes
                              // (argument2&3) at slot 0x530 bits 30 and 31, which is the rng and
                              // rgb selectors together. Every case fits: argument2 1 gives bit 30,
                              // 2 gives bit 31, 3 gives both, and the two cases where the slot
                              // did not move have argument2&3 zero. The row before this one did
                              // not exist for this title, so the number ran the default body, which
                              // wrote nothing here and agreed in only the 2 cases in which the
                              // original wrote nothing too.
    {303, anm_th16_blend, 34, "native"}, // measured over every case: the original writes the operand
                              // low nibble into slot 0x530 bits 5..8 (deltas 0x20 0x40 0x60 0x1e0
                              // over 34 writing cases and 6 that write nothing), and the all-case
                              // matcher reported no candidate body before this one: no body in the
                              // vocabulary held mask 0x1e0 at shift 5 in every case. The title-14
                              // table names this number blend with no witness.
    {305, anm_th16_flag13, 32, "native"}, // measured: the title flags word moves. The all-case
                              // matcher confirms case 74 with shift 0 and mask 0x2000 exactly, and
                              // this home holds that same write plus the numbered copy, flags2 bit
                              // 13, which no body wrote before.
    {304, anm_th16_layer, 30, "native"}, // measured over every case, and it is a branch, not a
                              // field: 40 cases, 30 set slot 0x534 bit 18 and 10 change that slot
                              // not at all. The condition is the low byte of the argument: 0, 1
                              // and 0xff stay quiet, every other value writes. The rows the corpus
                              // pairs with argument 3 all write and the unpaired rows carry the
                              // grid values, which is why the first reading looked like a pair
                              // effect. Two boundaries stay open for want of a case: no case has
                              // low byte 2, so 2 and 3 cannot be told apart, and no case
                              // distinguishes a clear of that bit from no write, since the slot
                              // starts clear in all 40. The body is the reading that fits: write
                              // when the byte is at least two and not 0xff. The title-14 table
                              // names this number layer, with no witness.
    {306, anm_th13_flag15, 32, "native"}, // measured: the same, raw flags bit 15
    {307, anm_th14_visual_rng, 32, "native"}, // measured here and named by two siblings: the corpus
                              // shows this title writing five single bits in a row at 0x534, at bits
                              // 7 8 9 10 11,
                              // by opcodes 424 431 432 307 311, and the named rows call the first
                              // three of them anm_th13_flag6 flag7 flag8 (0x700d 0x700e 0x700f), so
                              // the run is this title's flag array and 307 is its fourth flag.
                              // The named body's first line and plain 87 are the same write,
                              // field(0x40000000,30,argument<u8>(0)), which is the bit AnmVm.hpp
                              // reads as the rng selector, and title 13 measured 307 onto 87 with
                              // 368 witnesses; the row now uses the named body because it is the
                              // one that also keeps the numbered copy, flags2 bit 9, so the home
                              // the coverage matrix of section 325 asks for is written too.
    {310, 72, 32, "native"}, // measured in the 0x530 slot, where the two rows already standing are
                              // mask-identical: 305 writes 0x2000 and lands on case 74, whose mask
                              // is 0x2000; 306 writes 0x8000 and lands on the flag15 body at bit
                              // 15. The mask here is 0x1, and the one writer in the vocabulary
                              // whose mask is 0x1 is case 72. Both sibling tables name 72 for this
                              // number with the note visibility, and the port clears bit 0 when a
                              // child finishes.
    {311, anm_th14_rgb, 32, "native"}, // measured: the fifth flag of that run. The named body
                              // holds the write field(0x80000000,31,argument<u32>(0)), which plain
                              // 89 shares, so it moves the same flags bit 31 and also keeps the
                              // numbered copy, flags2 bit 10, which is the home the coverage
                              // matrix asks for. Title 13 measured 311 onto 89 with 187 witnesses.
                              // The row here said 73, which moves bit 11, before the run was
                              // measured.
    {313, 66, 34, "native"}, // measured: operand v<<20 with v in 1 2 3 7 (five sections of 319
                              // read it as payload 0x300000 0x100000 0x200000 0x700000 in 34
                              // writing cases); canonical 66 writes field(0x70,4,arg), so the
                              // field origins differ by 16 and the reported bits are its low
                              // three. The title-14 row named anm_th14_resolution with no
                              // witness; a fourth operand bit would separate 66 from
                              // anm_th14_blend, whose operand set here never sets it.
    {314, anm_th14_flag22, 32, "native"}, // named twice: this slot's bit 23 carries the ANM flag
                              // 22. The relation bit = N+1 is carried by five names already in the
                              // tables (424 431 432 at bits 7 8 9 are flags 6 7 8, and 314 and 315
                              // at bits 23 and 25 are flags 22 and 24), and the port's home for
                              // flag 22 is this handler, flags2 bit 22, which reads a one-bit
                              // integer. Measured here: 32 of 40 cases write one bit from the
                              // argument, which is the same shape.
    {315, anm_th14_flag24, 32, "native"}, // the same relation at bit 25, and this handler reads
                              // the byte argument, the channel the other members of this family
                              // read. The title-14 table names this number the same way with no
                              // witness.
    {419, anm_th16_flag12, 32, "native"}, // the relation bit = flag + 1 gives flag 12 for this
                              // slot's bit 13, and the port had no home for it: case 99 writes
                              // flags2 bit 13 for title 13 and bit 2 for this title, and nothing
                              // else wrote bit 12. The new body writes flags2 bit 12 from the byte
                              // argument, the channel measured here: 32 of 40 cases write the one
                              // bit, arg&1 predicting 40 of 40.
    {423, 28684, 8, "native"},  // th13+0x1a7
    {424, anm_th13_flag6, 32, "native"}, // the same relation on the run of five: 424 writes slot
                              // 0x534 bit 7, and its mask 0x80 is one below the flag6 home at
                              // flags2 bit 6, which is what the body already writes.
    {429, 28677, 8, "native"},  // th13+0x1ad
    {430, 28677, 8, "native"},  // th13+0x1ad
    {431, anm_th13_flag7, 32, "native"}, // bit 8 and mask 0x100, the flag7 home at flags2 bit 7
    {432, anm_th13_flag8, 32, "native"}, // bit 9 and mask 0x200, the flag8 home at flags2 bit 8
};

inline u16 anm_canonical_opcode(u16 code) {
    if constexpr(kPortTitle==16){
        if(code>=100&&code<=121)return code-94;
        if(code>=202&&code<=213)return code-174;
        switch(code){
        case 1:return 1;
        case 2:return 2;
        case 100:return 6;
        case 101:return 7;
        case 200:return 4;
        case 201:return 5;
        case 300:return anm_th14_sprite;
        case 122:return 40;
        case 123:return 41;
        case 124:return 42;
        case 125:return 43;
        case 126:return 44;
        case 127:return 45;
        case 128:return 46;
        case 130:return anm_th14_polar;
        case 131:return anm_th13_random_polar;
        case 301:return anm_th14_random_sprite;
        case 308:return 61;
        case 309:return 62;
        case 302:return anm_th16_native_mode;
        case 303:return anm_th14_blend;
        case 304:return anm_th16_native_layer;
        case 305:return anm_th16_native_flag13;
        case 306:return anm_th13_flag15;
        case 307:return anm_th16_field2_bit10;
        case 310:return anm_th16_visible;
        case 311:return anm_th16_field2_bit11;
        case 312:return anm_th13_texture_address;
        case 313:return anm_th16_native_resolution;
        case 314:return anm_th16_field2_bit23;
        case 315:return anm_th16_field2_bit25;
        case 419:return anm_th16_field2_bit13;
        case 420:return 100;
        case 421:return anm_th16_native_anchor;
        case 423:return anm_th13_vertex_color_mode;
        case 424:return anm_th16_field2_bit7;
        case 431:return anm_th16_field2_bit8;
        case 432:return anm_th16_field2_bit9;
        case 433:return anm_th13_polar_interpolate;
        case 437:return anm_th13_rotation_order;
        case 438:return anm_th16_native_origin;
        case 400:return 48;
        case 401:return 49;
        case 402:return 50;
        case 403:return 51;
        case 404:return 52;
        case 405:return 77;
        case 406:return 76;
        case 407:return 56;
        case 408:return 57;
        case 409:return 58;
        case 410:return 59;
        case 412:return anm_th16_scale_interpolate;
        case 413:return anm_th16_secondary_rgb_interpolate;
        case 414:return anm_th16_secondary_alpha_interpolate;
        case 415:return anm_th16_angular_velocity;
        case 416:return anm_th16_scale_velocity;
        case 425:return anm_th16_scroll_x_velocity;
        case 426:return anm_th16_scroll_y_velocity;
        case 429:return anm_th13_texture_scale;
        case 430:return anm_th13_texture_scale_interpolate;
        case 436:return anm_th13_pivot;
        case 500:return 88;
        case 501:return 90;
        case 502:return 91;
        case 503:return 92;
        case 504:return 95;
        case 505:return 96;
        case 506:return 97;
        case 507:return anm_th13_parent_independent;
        case 508:return anm_th13_callback_effect;
        case 509:return anm_th16_copy_variables;
        case 600:return anm_th16_ring;
        case 601:return anm_th16_centered_arc;
        case 602:return anm_th16_arc;
        case 603:return anm_th16_rectangle;
        case 604:return anm_th16_circle;
        case 605:return anm_th16_circle_outline;
        case 606:return anm_th16_gradient_rectangle;
        case 607:return anm_th16_soft_rectangle;
        case 608:return anm_th16_soft_gradient_rectangle;
        case 611:return anm_th16_ellipse;
        case 612:return anm_th16_rectangle_mode27;
        case 613:return anm_th16_rectangle_mode26;
        }
    }
    if (kPortTitle == 13) {
        for (const auto& row : th13_anm_opcodes)
            if (row.title_opcode == code) return row.canonical_opcode;
        return anm_opcode_unmapped;
    }
    if (kPortTitle == 14) {
        for (const auto& row : th14_anm_opcodes)
            if (row.title_opcode == code) return row.canonical_opcode;
        return anm_opcode_unmapped;
    }
    if (kPortTitle == 16 || kPortTitle == 17 || kPortTitle == 18) {
        // Titles 17 and 18 take the same rows as 16, and that is measured rather than assumed:
        // docs/porting/anm-flag-rows-16-18.json shares seven names across all three, records for
        // title 17 zero bit and zero slot differences against 16, and records for title 18 the same
        // seven at +2 bits and +4 on the slot. The values agree from the other side too: the
        // per-opcode solve of tools/anm-arrangement-search.mjs returns native bits 10, 25, 7 and 8
        // for title 17 exactly as for 16, and 12, 27, 9 and 10 for title 18, two higher. So the
        // rows are the ones below and only the arrangement differs, which the values now carry.
        // A TH16 native default is a no-op; an unimplemented real handler
        // must fail loading instead of executing a colliding TH12 number.
        for (const auto& row : th16_anm_opcodes)
            if (row.title_opcode == code) return row.canonical_opcode;
        if constexpr(kPortTitle==16)return th16_anm_native_noop(code)?0:anm_opcode_unmapped;
        return code;
    }
    return code;
}
}
