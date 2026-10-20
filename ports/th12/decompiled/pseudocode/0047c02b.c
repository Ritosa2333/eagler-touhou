// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c02b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4adbf0;
extern int g_4adc00;
extern int g_4adc60;
extern int g_4ade70;
extern unsigned int g_4d52e0;
extern int g_4d6300;
extern unsigned int g_4d6320[4];

unsigned int sub_47c02b(void)
{
    int v2;  // eax
    int v3;  // esi
    unsigned int v4;  // eax
    unsigned int v5;  // edx
    int v6;  // ecx
    int v7;  // edx
    int iter;  // ecx
    unsigned int v9;  // edi
    unsigned int v0;  // [bp-0x8]

    v2 = g_4d6300;
    v0 = 20;
    v3 = 20;
    if (!v2)
    {
        v3 = 0x200;
    }
    else if (!(v2 < 20))
    {
        goto LABEL_47c04a;
    }
    g_4d6300 = v3;
    v2 = v3;
LABEL_47c04a:
    v4 = sub_477813(v2, 4);
    g_4d52e0 = v4;
    if (!v4)
    {
        g_4d6300 = 20;
        v4 = sub_477813(20, 4);
        g_4d52e0 = v4;
        if (!v4)
            return 26;
    }
    v5 = 0;
    v6 = &g_4adbf0;
    while (1)
    {
        *((int *)(v5 + v4)) = v6;
        v6 += 32;
        v5 += 4;
        if (v6 >= &g_4ade70)
            break;
        v4 = g_4d52e0;
    }
    v0 = 0xfffffffe;
    v7 = 0;
    iter = &g_4adc00;
    v0 = v9;
    do
    {
        if (*((int *)((v7 & 31) * 64 + g_4d6320[v7 >> 5])) == 0xffffffff || *((int *)((v7 & 31) * 64 + g_4d6320[v7 >> 5])) == 0xfffffffe || !*((int *)((v7 & 31) * 64 + g_4d6320[v7 >> 5])))
            *((unsigned int *)iter) = 0xfffffffe;
    } while ((iter = (int)(iter + 32), v7 = (int)(v7 + 1), iter < &g_4adc60));
    return 0;
}
