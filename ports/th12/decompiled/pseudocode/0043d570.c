// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d570; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43d570_0 {
    unsigned int field_0;
    char padding_4[16];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    char padding_24[16];
    unsigned int field_34;
    char padding_38[4];
    unsigned int field_3c;
    char padding_40[128];
    unsigned int field_c0;
    char padding_c4[64];
    unsigned int field_104;
    unsigned int field_108;
    unsigned int field_10c;
    char padding_110[4];
    unsigned int field_114;
    char padding_118[128];
    unsigned int field_198;
    char padding_19c[64];
    unsigned int field_1dc;
    unsigned int field_1e0;
    unsigned int field_1e4;
    char padding_1e8[4];
    unsigned int field_1ec;
    char padding_1f0[128];
    unsigned int field_270;
    char padding_274[64];
    unsigned int field_2b4;
    unsigned int field_2b8;
} st_43d570_0;

extern char g_4a3738;
extern unsigned int g_4b4520;

st_43d570_0 * sub_43d570(void)
{
    st_43d570_0 *idx;  // esi

    *((char **)&idx->padding_4[12]) = &g_4a3738;
    idx->field_14 = 0;
    idx->field_18 = 0;
    idx->field_1c = 0;
    idx->field_20 = 0;
    idx->field_c0 = 0;
    idx->field_34 = 0;
    idx->field_108 = 0;
    idx->field_104 = 1;
    idx->field_3c = 999;
    idx->field_198 = 0;
    idx->field_10c = 0;
    idx->field_1e0 = 0;
    idx->field_1dc = 1;
    idx->field_114 = 999;
    idx->field_270 = 0;
    idx->field_1e4 = 0;
    idx->field_2b8 = 0;
    idx->field_2b4 = 1;
    idx->field_1ec = 999;
    sub_477420(idx, 0, 700);
    idx->field_0 = idx->field_0 | 2;
    g_4b4520 = idx;
    return idx;
}
