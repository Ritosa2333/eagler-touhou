// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d50a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d50a_0 {
    char padding_0[112];
    unsigned int field_70;
} st_46d50a_0;

unsigned int sub_46d50a(int a0, int a1, int a2, int a3, int a4, int a5, int a6)
{
    unsigned int v3;  // eax
    char v0;  // [bp-0x14]
    st_46d50a_0 *idx;  // [bp-0xc]
    char v2;  // [bp-0x8]

    sub_46d3b4(0);
    v3 = sub_476077(a0, a1, a2, a3, a4, a5, a6, &v0);
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v3;
}
