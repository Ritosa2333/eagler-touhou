// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c5cb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48c5cb_0 {
    char padding_0[200];
    unsigned int field_c8;
} st_48c5cb_0;

typedef struct st_48c5cb_1 {
    char padding_0[112];
    unsigned int field_70;
} st_48c5cb_1;

unsigned int sub_48c5cb(int a0, char a1, unsigned int a2, char a3)
{
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    st_48c5cb_0 *v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    st_48c5cb_1 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]

    sub_46d3b4(a0);
    v4 = a1;
    if (a3 & *((char *)(v1 + v4 + 29)) || (v5 = (!a2 ? 0 : (unsigned int)(unsigned short)*((short *)(v0->field_c8 + v4 * 2)) & a2), v5))
        v5 = 1;
    if (v3)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v5;
}
