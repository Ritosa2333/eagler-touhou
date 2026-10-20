// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c520; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47c520_0 {
    char padding_0[172];
    int field_ac;
} st_47c520_0;

typedef struct st_47c520_1 {
    char padding_0[112];
    unsigned int field_70;
} st_47c520_1;

unsigned int sub_47c520(int a0, int a1, int a2)
{
    st_47c520_0 *v0;  // [bp-0x18]
    st_47c520_1 *idx;  // [bp-0x10]
    char v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]

    v3 = 0xffffffff;
    sub_46d3b4(a2);
    if (sub_47c397(&v3, a0, v0->field_ac, a1, &v0))
        v3 = 0xffffffff;
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v3;
}
