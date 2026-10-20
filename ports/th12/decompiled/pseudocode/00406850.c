// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406850; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4027e0_0 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[96];
    unsigned int field_dc;
    char padding_e0[72];
    unsigned int field_128;
    char padding_12c[40];
    unsigned int field_154;
    char padding_158[72];
    unsigned int field_1a0;
    char padding_1a4[56];
    unsigned int field_1dc;
    char padding_1e0[56];
    unsigned int field_218;
    char padding_21c[72];
    unsigned int field_264;
    char padding_268[40];
    unsigned int field_290;
    char padding_294[40];
    unsigned int field_2bc;
    char padding_2c0[40];
    unsigned int field_2e8;
    char padding_2ec[236];
    unsigned int field_3d8;
    char padding_3dc[8];
    unsigned short field_3e4;
} st_4027e0_0;

extern unsigned int g_4b43c4;

st_4027e0_0 * sub_406850(void)
{
    st_4027e0_0 *idx;  // esi

    *((unsigned int *)&idx->padding_0[36]) = *((int *)&idx->padding_0[36]) & 0xfffffffe;
    *((unsigned int *)&idx->padding_0[56]) = *((int *)&idx->padding_0[56]) & 0xfffffffe;
    sub_4027e0(&idx->padding_0[76]);
    sub_477420(idx, 0, 1316);
    *((unsigned int *)&idx->padding_0[0]) = *((int *)&idx->padding_0[0]) | 2;
    g_4b43c4 = idx;
    return idx;
}
