// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410ba0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410ba0_0 {
    char padding_0[116];
    unsigned int field_74;
    char padding_78[116];
    unsigned int field_ec;
} st_410ba0_0;

typedef struct st_410ba0_1 {
    char padding_0[24];
    struct st_410ba0_0 *field_18;
} st_410ba0_1;

typedef struct st_410ba0_2 {
    char padding_0[102332];
    int field_18fbc;
} st_410ba0_2;

extern st_410ba0_2 *g_4b43b8;
extern st_410ba0_1 *g_4b43d8;

unsigned int sub_410ba0(void)
{
    st_410ba0_0 *idx;  // esi
    unsigned int v2;  // esi
    int v3;  // ebx
    unsigned int v4;  // eax

    idx = g_4b43d8->field_18;
    v4 = sub_45fe60(v2, v3);
    *((unsigned int *)&idx->padding_78[8 + 4 * idx->field_ec]) = v4;
    idx->field_74 = idx->field_74 & 0xfffffffb;
    sub_461970(g_4b43b8->field_18fbc, v2);
    g_4b43b8->field_18fbc = 0;
    return 0;
}
