// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491181; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_491181(unsigned int a0, unsigned int a1)
{
    unsigned int v1;  // ecx
    unsigned int v2;  // fpround
    unsigned int v3;  // 4134
    unsigned int v0;  // [bp-0x8]

    v0 = v1;
    v3 = _ccall(v2);
    *((unsigned short *)&v0) = v3;
    return (short)v0;
}
