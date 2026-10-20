// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da242; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4da242(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned short a6)
{
    unsigned int v0;  // 4131
    unsigned int v1;  // 4132
    unsigned int v2;  // 4141

    v0 = _ccall(15, a1 & a4, 0, 0);
    v1 = _ccall(CONCAT((unsigned short)v0, a6), 55);
    v2 = _ccall(12, 0, v1 >> 16 & 2261, 0, 0);
    if (!(v2 & 1))
        goto LABEL_0x4da29d;
}
