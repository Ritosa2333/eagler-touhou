// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dc0b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_49dc30[4];
extern unsigned int g_4b4328;

unsigned int * sub_47dc0b(unsigned int idx)
{
    unsigned int *v0;  // eax

    v0 = g_49dc30[idx];
    if (!((char)~(g_4b4328) & 1))
        v0 = g_49dc30[idx] + 2;
    return v0;
}
