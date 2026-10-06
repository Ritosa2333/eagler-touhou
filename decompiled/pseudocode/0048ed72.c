// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ed72; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48ed72_0 {
    char padding_0[112];
    unsigned int field_70;
} st_48ed72_0;

int sub_48ed72(int a0, unsigned int a1, LPArray a2, int a3)
{
    int v3;  // eax
    st_48ed72_0 *idx;  // [bp-0xc]
    char v1;  // [bp-0x8]
    unsigned short *v2;  // [bp+0x14]

    sub_46d3b4(a0);
    v3 = (a3 < -0x1 ? 0 : GetStringTypeW(a1, a2, a3, v2));
    if (v1)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v3;
}
