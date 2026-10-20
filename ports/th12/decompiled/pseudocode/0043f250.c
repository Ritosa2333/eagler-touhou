// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f250; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f250_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[32];
    unsigned int field_28;
    char padding_2c[4];
    unsigned int field_30;
    char padding_34[128];
    unsigned int field_b4;
    char padding_b8[64];
    unsigned int field_f8;
    unsigned int field_fc;
    unsigned int field_100;
    char padding_104[4];
    unsigned int field_108;
    char padding_10c[128];
    unsigned int field_18c;
    char padding_190[64];
    unsigned int field_1d0;
    unsigned int field_1d4;
    unsigned int field_1d8;
    char padding_1dc[4];
    unsigned int field_1e0;
    char padding_1e4[128];
    unsigned int field_264;
    char padding_268[64];
    unsigned int field_2a8;
    unsigned int field_2ac;
    char padding_2b0[20];
    unsigned int field_2c4;
    char padding_2c8[22216];
    unsigned int field_5990;
    char padding_5994[4];
    unsigned int field_5998;
    char padding_599c[128];
    unsigned int field_5a1c;
    char padding_5a20[64];
    unsigned int field_5a60;
    unsigned int field_5a64;
} st_43f250_0;

extern char g_4a20cc;
extern char g_4a3738;
extern unsigned int g_4b4530;

st_43f250_0 * sub_43f250(void)
{
    st_43f250_0 *idx;  // esi

    *((char **)&idx->padding_0[0]) = &g_4a20cc;
    idx->field_b4 = 0;
    idx->field_28 = 0;
    idx->field_fc = 0;
    idx->field_f8 = 1;
    idx->field_30 = 999;
    idx->field_18c = 0;
    idx->field_100 = 0;
    idx->field_1d4 = 0;
    idx->field_1d0 = 1;
    idx->field_108 = 999;
    idx->field_264 = 0;
    idx->field_1d8 = 0;
    idx->field_2ac = 0;
    idx->field_2a8 = 1;
    idx->field_1e0 = 999;
    idx->field_2c4 = idx->field_2c4 & 0xfffffffe;
    idx->field_5a1c = 0;
    idx->field_5990 = 0;
    idx->field_5a64 = 0;
    idx->field_5a60 = 1;
    idx->field_5998 = 999;
    *((char **)&idx[1].padding_190[36]) = &g_4a3738;
    memset(&idx[1].padding_190[40], 0, 16);
    sub_477420(idx, 0, 23608);
    idx->field_4 = idx->field_4 | 2;
    g_4b4530 = idx;
    return idx;
}
