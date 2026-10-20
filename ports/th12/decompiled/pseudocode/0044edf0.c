// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44edf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_44edf0(unsigned int *idx, unsigned int a1, unsigned int a2)
{
    unsigned int v0;  // 4098

    v0 = idx[6];
    idx[5] = a2;
    if (v0 >= 16)
        *((char *)(idx[1] + a2)) = 0;
    else
        *((char *)idx + a2 + 4) = 0;
    return;
}
