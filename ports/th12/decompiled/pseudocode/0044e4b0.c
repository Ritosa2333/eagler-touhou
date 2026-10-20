// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e4b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ce8f0;

int sub_44e4b0(unsigned int a0, int a1)
{
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    D3DXCreateTexture(g_4ce8f0, a1, a0, 1, 0, 25, 1, &v0, a0);
    return g_4ce8f0;
}
