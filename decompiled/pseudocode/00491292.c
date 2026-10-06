// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491292; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

void sub_491292(void)
{
    unsigned int v2;  // sseround
    unsigned int v3;  // 4108
    unsigned int v0;  // [bp-0x8]

    if (g_4d52dc)
    {
        v3 = _ccall(v2 & 3);
        v0 = v3;
        v0 &= 0xffffffc0;
    }
    return;
}
