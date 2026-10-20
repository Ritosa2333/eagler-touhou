// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493f3b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_493f3b(int a0, int a1, int a2, int a3)
{
    unsigned int v4;  // fpround
    unsigned int v5;  // 4110
    int v0;  // [bp-0x2d8]
    char v1;  // [bp-0x2cc]
    unsigned short v2;  // [bp-0xa8]
    char v3;  // [bp-0x93]

    sub_494104(a0, a1, v0);
    sub_494104(a2, a3);
    v5 = _ccall(v4);
    v2 = v5;
    v1 |= 2;
    v3 = 1;
    sub_4941a7();
    sub_493f83();
    return;
}
