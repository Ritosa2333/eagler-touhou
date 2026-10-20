// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431d00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431d00_0 {
    unsigned int field_0;
    char padding_4[28];
    unsigned int field_20;
    char padding_24[16];
    unsigned int field_34;
    unsigned int field_38;
    char padding_3c[4];
    unsigned int field_40;
    char padding_44[128];
    unsigned int field_c4;
    char padding_c8[64];
    unsigned int field_108;
    unsigned int field_10c;
    unsigned int field_110;
    char padding_114[4];
    unsigned int field_118;
    char padding_11c[128];
    unsigned int field_19c;
    char padding_1a0[64];
    unsigned int field_1e0;
    unsigned int field_1e4;
} st_431d00_0;

extern unsigned int g_4b4510;

st_431d00_0 * sub_431d00(unsigned int a0)
{
    st_431d00_0 *idx;  // esi

    idx->field_20 = idx->field_20 & 0xfffffffe;
    idx->field_34 = idx->field_34 & 0xfffffffe;
    idx->field_c4 = 0;
    idx->field_38 = 0;
    idx->field_10c = 0;
    idx->field_108 = 1;
    idx->field_40 = 999;
    idx->field_19c = 0;
    idx->field_110 = 0;
    idx->field_1e4 = 0;
    idx->field_1e0 = 1;
    idx->field_118 = 999;
    sub_477420(idx, 0, 736);
    idx->field_0 = idx->field_0 | 2;
    g_4b4510 = idx;
    return idx;
}
