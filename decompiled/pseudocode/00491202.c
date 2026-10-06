// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491202; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

unsigned int sub_491202(void)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // sseround
    unsigned int v4;  // 4101
    unsigned int v0;  // [bp-0x8]

    v0 = v2;
    if (!g_4d52dc)
    {
        v0 = 0;
        return v0;
    }
    v4 = _ccall(v3 & 3);
    v0 = v4;
    return v0;
}
