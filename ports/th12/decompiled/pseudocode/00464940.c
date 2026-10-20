// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464940; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_464940(void)
{
    unsigned int *idx;  // eax
    unsigned int v2;  // 4098
    unsigned int v3;  // 4107
    unsigned int v4;  // ecx

    v2 = idx[35];
    idx[35] = idx[35] - 1;
    v3 = _ccall(8, 3, v2, 0xffffffff, 0);
    if (v3 & 1)
        idx[35] = 0;
    v4 = idx[35];
    *(idx) = idx[3 + v4];
    idx[2] = idx[19 + v4];
    idx[53] = 0;
    return;
}
