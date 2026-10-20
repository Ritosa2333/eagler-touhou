// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46eb06; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad2b8[4];
extern unsigned int g_4ad2bc[4];
extern char g_4b3c08;

unsigned int sub_46eb06(void)
{
    unsigned int v2;  // edi
    int idx;  // esi
    unsigned int v4;  // edi
    int *v5;  // eax
    unsigned int v0;  // [bp-0x8]

    v0 = v2;
    idx = 0;
    v4 = &g_4b3c08;
    while (1)
    {
        if (g_4ad2bc[2 * idx] == 0x1)
        {
            v5 = &g_4ad2b8[2 * idx];
            *(v5) = v4;
            v4 += 24;
            if (!sub_47b416(*(v5), 4000))
            {
                g_4ad2b8[2 * idx] = 0;
                return 0;
            }
        }
        idx += 1;
        if (idx >= 36)
            return 1;
    }
}
