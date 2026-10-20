// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41c550; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_41c550(void)
{
    unsigned int *idx;  // eax
    unsigned int v2;  // 4098
    unsigned int v3;  // ecx

    v2 = idx[30];
    idx[30] = v3;
    if (v2 != v3)
        idx[32] = 0;
    return;
}
