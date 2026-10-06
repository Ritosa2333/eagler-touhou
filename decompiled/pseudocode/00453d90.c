// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453d90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned short g_4ae5aa[4];
extern unsigned int g_4cf508[4];
extern unsigned int g_4cf538[4];
extern unsigned int g_4cf568;
extern unsigned int g_4d0e74;

int sub_453d90(unsigned int a0, int index)
{
    unsigned int v1;  // edi
    int v2;  // eax
    unsigned int *v3;  // ecx
    int idx;  // eax

    v1 = g_4ae5aa[10 * index];
    v2 = 0;
    v3 = &g_4cf508[0];
    while (1)
    {
        idx = v2;
        if (*(v3) < 0)
        {
            if (idx >= 12)
                return idx;
            g_4cf508[idx] = index;
            (&g_4cf568)[128 * idx] = 0;
            g_4cf538[idx] = g_4cf538[idx] + 1;
            (&g_4d0e74)[6 * index] = v1;
            return idx;
        }
        if (*(v3) == index)
            break;
        v3 += 1;
        v2 = idx + 1;
        if (v3 >= &g_4cf538[0])
            return idx + 1;
    }
    if (g_4cf538[idx] < 0x80)
    {
        (&g_4cf568)[128 * idx + g_4cf538[idx]] = 0;
        g_4cf538[idx] = g_4cf538[idx] + 1;
    }
    return idx;
}
