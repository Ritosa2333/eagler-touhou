// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d840; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4af208;

unsigned int sub_40d840(unsigned int a0, unsigned int a1)
{
    unsigned int v1;  // eax
    int v2;  // ecx

    v1 = 0;
    v2 = 0;
    do
    {
        if ((&g_4af208)[v2] == a1)
            v1 += 1;
    } while ((v2 = (int)(v2 + 1), v2 < 113));
    return v1;
}
