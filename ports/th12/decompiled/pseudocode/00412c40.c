// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412c40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43dc;

unsigned int * sub_412c40(void)
{
    unsigned int *idx;  // esi

    idx[24] = idx[24] & 0xfffffffe;
    sub_477420(idx, 0, 124);
    *(idx) = *(idx) | 2;
    g_4b43dc = idx;
    return idx;
}
