// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e0c2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_47e0c2(int a0)
{
    char v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    char v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    char v4;  // [bp+0x0]

    sub_481a74(a0, &v0, 0, &v2, 0, 0, v1 & 0xffff0000, 0, v3 & 0xffff0000, &v4);
    return a0;
}
