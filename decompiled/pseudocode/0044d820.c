// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d820; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4aeb20[4];

unsigned int sub_44d820(unsigned int a0, unsigned int a1)
{
    unsigned int v1;  // ecx
    unsigned int idx;  // eax

    v1 = g_4aeb20;
    idx = 0;
    if (g_4aeb20 != 0xffffffff)
    {
        do
        {
        } while (v1 != a1 && (idx += 1, v1 = g_4aeb20[6 * idx], g_4aeb20[6 * idx] != 0xffffffff));
    }
    if (a1 != 0xffffffff)
        return &g_4aeb20[6 * idx];
    return 0;
}
