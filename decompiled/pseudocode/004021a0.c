// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4021a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4021a0(int a0, unsigned int a1)
{
    int v2;  // eax
    int v3;  // eax
    char v0;  // [bp-0x10c]

    if (a0 < 1000)
    {
        sub_46cbbb(&v0, "%d", a0);
    }
    else if (a0 < 1000000)
    {
        v2 = ((unsigned int)((int)(274877907 * a0 >> 38)) >> 31) + (int)(274877907 * a0 >> 38);
        sub_46cbbb(&v0, "%d,%.3d", v2, a0 - v2 * 1000);
    }
    else if (a0 < 1000000000)
    {
        v3 = ((unsigned int)((int)(274877907 * a0 >> 38)) >> 31) + (int)(274877907 * a0 >> 38);
        sub_46cbbb(&v0, "%d,%.3d,%.3d", (((unsigned int)((int)(1125899907 * a0 >> 50)) >> 31) + (int)(1125899907 * a0 >> 50)) % 1000, v3 % 1000, a0 - v3 * 1000);
    }
    sub_4020a0(&v0);
    return;
}
