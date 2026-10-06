// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4966a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b37a8[4];
extern unsigned int g_4b37ac[4];

unsigned int * sub_4966a0(unsigned int *i)
{
    int v0;  // eax

    v0 = 0;
    while (g_4b37a8[2 * v0] != i)
    {
        v0 += 1;
        if (v0 >= 29)
            return NULL;
    }
    return g_4b37ac[2 * v0];
}
