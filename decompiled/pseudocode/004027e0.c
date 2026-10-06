// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4027e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

void sub_4027e0(st_4027e0_0 *idx)
{
    int v0;  // [bp-0x4]

    idx->field_78 = idx->field_78 & 0xfffffffe;
    idx->field_dc = idx->field_dc & 0xfffffffe;
    idx->field_128 = idx->field_128 & 0xfffffffe;
    idx->field_154 = idx->field_154 & 0xfffffffe;
    idx->field_1a0 = idx->field_1a0 & 0xfffffffe;
    idx->field_1dc = idx->field_1dc & 0xfffffffe;
    idx->field_218 = idx->field_218 & 0xfffffffe;
    idx->field_264 = idx->field_264 & 0xfffffffe;
    idx->field_290 = idx->field_290 & 0xfffffffe;
    idx->field_2bc = idx->field_2bc & 0xfffffffe;
    idx->field_2e8 = idx->field_2e8 & 0xfffffffe;
    idx->field_3d8 = idx->field_3d8 & 0xfffffffe;
    sub_477420(idx, 0, 1204, v0);
    idx->field_3e4 = 0xffff;
    return;
}
