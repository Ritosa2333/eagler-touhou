// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473138; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad3f8[4];
extern unsigned int g_4ad3fc[4];

unsigned int * sub_473138(unsigned int *i)
{
    unsigned int v0;  // eax

    v0 = 0;
    while (i != g_4ad3f8[2 * v0])
    {
        v0 += 1;
        if (v0 >= 23)
            return NULL;
    }
    return g_4ad3fc[2 * v0];
}
