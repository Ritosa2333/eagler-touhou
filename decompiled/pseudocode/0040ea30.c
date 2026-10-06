// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40ea30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40ea30_0 {
    unsigned int field_0;
    char padding_4[16];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    char padding_24[24];
    unsigned int field_3c;
    char padding_40[4];
    unsigned int field_44;
    char padding_48[128];
    unsigned int field_c8;
    char padding_cc[64];
    unsigned int field_10c;
    unsigned int field_110;
    unsigned int field_114;
    char padding_118[4];
    unsigned int field_11c;
    char padding_120[128];
    unsigned int field_1a0;
    char padding_1a4[64];
    unsigned int field_1e4;
    unsigned int field_1e8;
    unsigned int field_1ec;
    char padding_1f0[4];
    unsigned int field_1f4;
    char padding_1f8[128];
    unsigned int field_278;
    char padding_27c[64];
    unsigned int field_2bc;
    unsigned int field_2c0;
} st_40ea30_0;

extern char g_4a3738;
extern unsigned int g_4b43d0;

st_40ea30_0 * sub_40ea30(void)
{
    st_40ea30_0 *idx;  // esi
    int v0;  // [bp-0x4]

    *((char **)&idx->padding_4[12]) = &g_4a3738;
    idx->field_14 = 0;
    idx->field_18 = 0;
    idx->field_1c = 0;
    idx->field_20 = 0;
    idx->field_c8 = 0;
    idx->field_3c = 0;
    idx->field_110 = 0;
    idx->field_10c = 1;
    idx->field_44 = 999;
    idx->field_1e4 = 1;
    idx->field_1a0 = 0;
    idx->field_114 = 0;
    idx->field_1e8 = 0;
    idx->field_11c = 999;
    idx->field_2bc = 1;
    idx->field_278 = 0;
    idx->field_1ec = 0;
    idx->field_2c0 = 0;
    idx->field_1f4 = 999;
    sub_4027e0(v0);
    sub_477420(idx, 0, 1936);
    idx->field_0 = idx->field_0 | 2;
    g_4b43d0 = idx;
    return idx;
}
