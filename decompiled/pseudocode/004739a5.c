// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4739a5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aac40;
extern unsigned int g_4ad4b0;
extern char g_4ad8d8;
extern unsigned int g_4ad9d4;

int sub_4739a5(unsigned int a0)
{
    unsigned int *idx;  // edi
    unsigned int v2;  // esi
    void* v3;  // ebp
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aac40, 12);
    idx = sub_475467(&g_4aac40, 12);
    if (g_4ad9d4 & idx[28] && idx[27])
    {
        v2 = idx[26];
    }
    else
    {
        sub_46ec9a(13);
        *((unsigned int *)((char *)v3 - 4)) = 0;
        v2 = idx[26];
        *((unsigned int *)((char *)v3 - 28)) = v2;
        if (v2 != *((int *)&g_4ad8d8))
        {
            if (v2 && !InterlockedDecrement(v2) && v2 != &g_4ad4b0)
                sub_46c941(v2, &g_4aac40);
            idx[26] = *((int *)&g_4ad8d8);
            v2 = *((int *)&g_4ad8d8);
            *((int *)((char *)v3 - 28)) = *((int *)&g_4ad8d8);
            InterlockedIncrement(*((int *)&g_4ad8d8));
        }
        *((unsigned int *)((char *)v3 - 4)) = 0xfffffffe;
        sub_473a40();
    }
    if (!v2)
        sub_472c0d(32);
    return (unsigned int)sub_46fd31(&g_4aac40, 12, v0, a0);
}
