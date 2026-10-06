// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451c70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ce90c[4];

unsigned int sub_451c70(void)
{
    int v1;  // eax

    v1 = 0;
    while (g_4ce90c[v1])
    {
        v1 += 1;
        if (v1 >= 1)
            return 0;
    }
    return sub_451c98(0);
}
