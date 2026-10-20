// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4654c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned short * sub_4654c0(unsigned short *idx)
{
    idx[154] = 0;
    idx[155] = 1;
    idx[156] = 2;
    idx[162] = 3;
    idx[157] = 4;
    memset(idx + 158, -0x1, 8);
    return idx;
}
