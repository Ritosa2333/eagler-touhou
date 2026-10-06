// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453e20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned short g_4ae5aa[4];
extern unsigned int g_4cf508[4];
extern unsigned int g_4cf538[4];
extern unsigned int g_4cf568;
extern unsigned int g_4d0e74;

int sub_453e20(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    int index;  // eax
    unsigned int v7;  // ebx
    unsigned int v8;  // edi
    unsigned int v9;  // eax
    int idx;  // ecx
    unsigned int *v11;  // edx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v2 = v4;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v1 = v5;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v7 = g_4ae5aa[10 * index];
    v0 = v8;
    v9 = sub_4931e0();
    idx = 0;
    v11 = &g_4cf508[0];
    while (1)
    {
        if (*(v11) < 0)
        {
            if (idx >= 12)
                return;
            g_4cf508[idx] = index;
            (&g_4cf568)[128 * idx] = v9;
            g_4cf538[idx] = g_4cf538[idx] + 1;
            (&g_4d0e74)[6 * index] = v7;
            return;
        }
        if (*(v11) == index)
            break;
        v11 += 1;
        idx += 1;
        if (v11 >= &g_4cf538[0])
            return;
    }
    if (g_4cf538[idx] < 0x80)
    {
        (&g_4cf568)[128 * idx + g_4cf538[idx]] = v9;
        g_4cf538[idx] = g_4cf538[idx] + 1;
    }
    return;
}
