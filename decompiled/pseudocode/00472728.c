// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472728; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_472728(int a0, int a1, int a2, int a3, int a4)
{
    int v1;  // eax
    char v0;  // [bp+0x0]

    v1 = sub_472414(sub_471154, a0, a1, a2, a3, a4, &v0);
    if (v1 >= 0)
        return v1;
    return -0x1;
}
