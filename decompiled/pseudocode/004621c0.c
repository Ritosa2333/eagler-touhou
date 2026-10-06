// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4621c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b40bc;
extern char g_4b50bc;
extern unsigned int g_4ce8cc;

unsigned int sub_4621c0(void)
{
    unsigned int v1;  // eax
    unsigned int v2;  // ebp
    unsigned int v3;  // eax
    unsigned int v4;  // eax
    unsigned int v5;  // ecx
    unsigned int v6;  // ecx

    v1 = *((int *)&(&g_4b50bc)[g_4ce8cc]);
    v2 = v1 * 1204 + g_4ce8cc + 188;
    if ((&g_4b40bc)[g_4ce8cc + v1])
    {
        v3 = v1 + 1;
        v4 = v3 & 0x80000fff;
        if ((v3 & 0x80000fff) < 0)
            v4 = (v4 - 1 | 0xfffff000) + 1;
        *((unsigned int *)&(&g_4b50bc)[g_4ce8cc]) = v4;
        v2 = v4 * 1204 + g_4ce8cc + 188;
        if (!(&g_4b40bc)[g_4ce8cc + v4])
        {
            sub_402520();
            (&g_4b40bc)[*((int *)&(&g_4b50bc)[g_4ce8cc]) + g_4ce8cc] = 1;
        }
        else if (sub_46c9ea(1204))
        {
            v2 = sub_4027e0();
            sub_402520();
        }
        else
        {
            v2 = 0;
            sub_402520();
        }
    }
    else
    {
        (&g_4b40bc)[g_4ce8cc + v1] = 1;
    }
    v5 = *((int *)&(&g_4b50bc)[g_4ce8cc]) + 1;
    v6 = v5 & 0x80000fff;
    if ((v5 & 0x80000fff) < 0)
        v6 = (v6 - 1 | 0xfffff000) + 1;
    *((unsigned int *)&(&g_4b50bc)[g_4ce8cc]) = v6;
    return v2;
}
