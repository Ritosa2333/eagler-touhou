// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46caf2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_46caf2(unsigned int *a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0x18]

    if (a1)
    {
        v2 = 0x7fffffff;
        v4 = 66;
        v3 = 0;
        v1 = 0;
        return a0(&v1, a1, a2, a3);
    }
    *((unsigned int *)sub_46e96f(v0)) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0xffffffff;
}
