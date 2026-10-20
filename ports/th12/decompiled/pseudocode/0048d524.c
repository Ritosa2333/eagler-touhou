// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d524; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d524_0 {
    char padding_0[112];
    unsigned int field_70;
} st_48d524_0;

extern unsigned int g_4adea4;

unsigned int sub_48d524(unsigned short a0, unsigned short a1, int a2)
{
    char v0;  // [bp-0x18]
    st_48d524_0 *idx;  // [bp-0x10]
    char v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]

    if (a0 == 0xffff)
    {
        v3 = 0;
    }
    else if (a0 < 0x100)
    {
        v3 = *((short *)(g_4adea4 + a0 * 2)) & a1;
    }
    else
    {
        sub_46d3b4(a2);
        if (!sub_48ed72(&v0, 1, &a0, 1, &v3, *((int *)(v0 + 4)), *((int *)(v0 + 20))))
            v3 = 0;
        if (v2)
            idx->field_70 = idx->field_70 & 0xfffffffd;
    }
    return (unsigned short)v3 & a1;
}
