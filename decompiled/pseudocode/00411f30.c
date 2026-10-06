// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411f30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49fc28;

unsigned int * sub_411f30(void)
{
    unsigned int *idx;  // esi

    *(idx) = &g_49fc28;
    idx[1060] = 0;
    idx[1061] = 0;
    sub_477420(idx, 0, 4248);
    return idx;
}
