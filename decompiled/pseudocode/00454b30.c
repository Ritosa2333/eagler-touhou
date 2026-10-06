// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454b30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ae5a0;
extern int g_4cf4e8;
extern unsigned int g_4d0e70[4];
extern unsigned int g_4d1410[4];

int sub_454b30(void)
{
    unsigned int v1;  // edx
    unsigned int *iter;  // eax
    unsigned int v3;  // ecx
    unsigned int v4;  // ecx

    sub_477420(&g_4cf4e8, 0, 21152);
    v1 = 0;
    iter = &g_4d0e70[0];
    do
    {
        iter[1] = 0xffffffff;
        v3 = &g_4ae5a0;
        do
        {
            v4 = v3;
            v3 = v4;
        } while (*((int *)v3) != v1 && (v3 = v4 + 20, v4 != 0xffffffec));
        iter[3] = v1;
        iter[2] = v3;
        iter += 6;
        v1 += 1;
    } while (iter < &g_4d1410[0]);
    return &g_4cf4e8;
}
