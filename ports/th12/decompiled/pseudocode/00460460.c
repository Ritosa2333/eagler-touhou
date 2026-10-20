// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460460; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b50c0;

unsigned int sub_460460(unsigned int a0, unsigned int a1)
{
    unsigned int v1;  // ecx
    unsigned int iter;  // edx
    unsigned int *v3;  // eax

    v1 = 0;
    iter = &(&g_4b50c0)[a1];
    while (1)
    {
        v3 = *((int *)iter);
        if (v3 && (v3[74] || v3[73]))
            break;
        v1 += 1;
        iter += 4;
        if (v1 >= 32)
            return 1;
    }
    return 0;
}
