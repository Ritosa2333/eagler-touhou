// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43caa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_43caa0(void)
{
    unsigned int *idx;  // eax
    unsigned int v2;  // edx

    v2 = idx[2];
    idx[1] = *(idx);
    idx[3] = v2;
    idx[5] = 0xffffffff;
    return;
}
