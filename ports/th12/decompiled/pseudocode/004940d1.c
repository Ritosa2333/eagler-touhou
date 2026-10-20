// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4940d1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4940d1(int a0, int a1)
{
    unsigned int v3;  // fpround
    unsigned int v4;  // 4109
    int v0;  // [bp-0x2d8]
    char v1;  // [bp-0x2cc]
    unsigned short v2;  // [bp-0xa8]

    sub_494104(a0, a1, v0);
    v4 = _ccall(v3);
    v2 = v4;
    v1 &= 253;
    sub_494140(v0);
    sub_493f83();
    return;
}
