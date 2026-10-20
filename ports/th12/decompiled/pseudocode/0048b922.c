// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b922; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48b922_0 {
    char padding_0[172];
    int field_ac;
    char padding_b0[24];
    unsigned int field_c8;
} st_48b922_0;

typedef struct st_48b922_1 {
    char padding_0[112];
    unsigned int field_70;
} st_48b922_1;

unsigned int sub_48b922(int a0, int a1)
{
    unsigned int v3;  // eax
    st_48b922_0 *v0;  // [bp-0x14]
    st_48b922_1 *idx;  // [bp-0xc]
    char v2;  // [bp-0x8]

    sub_46d3b4(a1);
    v3 = (v0->field_ac <= 1 ? *((short *)(v0->field_c8 + a0 * 2)) & 1 : sub_4758eb(a0, 1, &v0));
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v3;
}
