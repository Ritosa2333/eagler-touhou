// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423250; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423250_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned short field_14;
    unsigned short field_16;
    unsigned short field_18;
    char field_1a;
    char field_1b;
    char field_1c;
    char field_1d;
    char field_1e;
    char field_1f;
    char field_20;
    char field_21;
    char field_22;
    char field_23;
    char padding_24[4];
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[8];
    unsigned int field_38;
} st_423250_0;

extern unsigned int g_4d49ec;
extern unsigned int g_4d49f0;
extern unsigned int g_4d49f4;
extern unsigned int g_4d49f8;
extern unsigned short g_4d49fc;

void sub_423250(unsigned int a0)
{
    st_423250_0 *idx;  // esi
    int v0;  // [bp-0x4]

    sub_477420(idx, 0, 60, v0);
    idx->field_38 = idx->field_38 | 0x100;
    idx->field_16 = 600;
    idx->field_1a = 0;
    idx->field_1d = 0;
    idx->field_1e = 0;
    idx->field_0 = 1179649;
    idx->field_18 = 600;
    idx->field_1b = 1;
    idx->field_1c = 1;
    idx->field_4 = g_4d49ec;
    idx->field_8 = g_4d49f0;
    idx->field_c = g_4d49f4;
    idx->field_10 = g_4d49f8;
    idx->field_14 = g_4d49fc;
    idx->field_1f = 2;
    idx->field_23 = 2;
    idx->field_22 = 0;
    idx->field_20 = 100;
    idx->field_21 = 80;
    idx->field_28 = 0x80000000;
    idx->field_2c = 0x80000000;
    return;
}
