// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4912da; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

unsigned int sub_4912da(unsigned int a0, unsigned int a1)
{
    unsigned int v2;  // esi
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    if (!g_4d52dc)
        return 0;
    v2 = sub_491202(v0, &v1);
    sub_491220((~(a1) | 0xffff807f) & v2 | a0 & a1);
    return v2;
}
