// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ce11; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_46ce11(int a0, unsigned int a1, int a2)
{
    unsigned int v1;  // 4119
    char v0;  // [bp+0x0]

    v1 = _ccall(15, 15, sub_472ac0(a2 + 9, a0 + 9, &v0), 0, 0);
    return v1 & 1;
}
