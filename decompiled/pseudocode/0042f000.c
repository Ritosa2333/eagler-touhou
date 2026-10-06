// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f000; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4cee40;
extern unsigned int g_4cf428;
extern int g_4cf42c;
extern unsigned int g_4d48c4;

unsigned int sub_42f000(unsigned int *a0)
{
    unsigned int v2;  // eax
    int v0;  // [bp-0x4]

    if ((a0[356] & 384) == 128 && !a0[0x200])
        g_4cee40 = 3;
    sub_453fa0(v0);
    sub_4319f0();
    sub_462ec0();
    if (sub_4603d0())
        return 4;
    if ((!g_4cf42c || (g_4cf42c = (int)(g_4cf42c - 1), g_4cf42c <= 0)) && g_4d48c4 & 0x800000)
    {
        v2 = g_4cf428 | 2;
        g_4cf428 = v2 ^ ((v2 & 0xfffffffc) + 4 ^ v2) & 12;
    }
    if (!a0[515])
    {
        if (sub_4311e0() != 1)
            goto LABEL_42f0a7;
LABEL_42f0a7:
        return 1;
    }
    return (-(0 < a0[515] - 2) & 0xfffffffd) + 4;
}
