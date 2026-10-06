// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40daa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43cc;

int sub_40daa0(void)
{
    int v1;  // esi
    unsigned int *idx;  // esi

    idx = sub_46c9ea(192, v1);
    if (idx)
    {
        idx[13] = idx[13] & 0xfffffffe;
        sub_477420(idx, 0, 192);
        *(idx) = *(idx) | 2;
        g_4b43cc = idx;
    }
    return sub_40dad5();
}
