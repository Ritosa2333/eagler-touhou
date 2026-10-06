// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d99f9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d99f9(void)
{
    char v2;  // ch
    unsigned int v3;  // 4132
    unsigned int v4;  // eax
    unsigned int v0;  // [bp-0x2]

    v3 = _ccall(25, v2 >> 21, v2 >> 20, 0);
    *((unsigned short *)&v0) = (unsigned short)v3 | 0x202;
    if ((v4 & 1577663448) < 0)
        goto LABEL_0x4d9a83;
    return;
}
