// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44a230; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4534;

unsigned int * sub_44a230(void)
{
    unsigned int *idx;  // esi
    int v3;  // edi
    int v4;  // ebx
    int v0;  // [bp-0x4]

    idx = sub_46c9ea(108, v0);
    if (idx)
    {
        idx[8] = idx[8] & 0xfffffffe;
        sub_477420(idx, 0, 108);
        *(idx) = *(idx) | 2;
        g_4b4534 = idx;
    }
    else
    {
        idx = NULL;
    }
    if (!sub_449fa0(v3))
        return idx;
    if (!idx)
        return NULL;
    sub_44a120(v4);
    sub_46ca4f(idx);
    return NULL;
}
