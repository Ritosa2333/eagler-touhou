// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406b20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43c4;

unsigned int * sub_406b20(void)
{
    int v1;  // esi
    unsigned int *idx;  // esi

    idx = sub_46c9ea(1316, v1);
    if (idx)
    {
        idx[9] = idx[9] & 0xfffffffe;
        idx[14] = idx[14] & 0xfffffffe;
        sub_4027e0();
        sub_477420(idx, 0, 1316);
        *(idx) = *(idx) | 2;
        g_4b43c4 = idx;
    }
    else
    {
        idx = NULL;
    }
    if (!sub_406880(idx))
        return idx;
    if (!idx)
        return NULL;
    sub_406930(idx);
    sub_46ca4f(idx);
    return NULL;
}
