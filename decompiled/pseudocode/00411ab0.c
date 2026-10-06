// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411ab0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411ab0_0 {
    char padding_0[116];
    unsigned int field_74;
    char padding_78[116];
    unsigned int field_ec;
} st_411ab0_0;

typedef struct st_411ab0_1 {
    char padding_0[102332];
    int field_18fbc;
} st_411ab0_1;

extern st_411ab0_1 *g_4b43b8;

int sub_411ab0(void)
{
    unsigned int v1;  // esi
    int v2;  // ebx
    unsigned int v3;  // eax
    st_411ab0_0 *idx;  // eax

    v3 = sub_45fe60(v1, v2);
    *((unsigned int *)&idx->padding_78[8 + 4 * idx->field_ec]) = v3;
    idx->field_74 = idx->field_74 & 0xfffffffb;
    sub_461970(g_4b43b8->field_18fbc, v1);
    g_4b43b8->field_18fbc = 0;
    return;
}
