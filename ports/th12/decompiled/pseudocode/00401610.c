// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x401610; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_401610(int a0)
{
    int v5;  // eax
    unsigned int v6;  // edx
    unsigned int v7;  // edi
    int v8;  // eax
    int v9;  // edx
    unsigned int v0;  // [bp-0x114]
    unsigned int v1;  // [bp-0x110]
    unsigned int v2;  // [bp-0x10c]
    char v3;  // [bp-0x104]

    if (a0 < 1000)
    {
        sub_46cbbb(&v3, "%d", a0);
    }
    else
    {
        v5 = ((unsigned int)((int)(274877907 * a0 >> 38)) >> 31) + (int)(274877907 * a0 >> 38);
        v6 = v5 * 1000;
        if (a0 < 1000000)
        {
            sub_46cbbb(&v3, "%d,%.3d", v5, a0 - v6);
        }
        else
        {
            v2 = v7;
            v1 = a0 - v6;
            v0 = v5 % 1000;
            v8 = ((unsigned int)((int)(1125899907 * a0 >> 50)) >> 31) + (int)(1125899907 * a0 >> 50);
            v9 = v8 >> 31;
            if (a0 < 1000000000)
                sub_46cbbb(&v3, "%d,%.3d,%.3d", CONCAT(v9, v8) % 1000);
            else
                sub_46cbbb(&v3, "%d,%.3d,%.3d,%.3d", (((unsigned int)((int)(1152921505 * a0 >> 60)) >> 31) + (int)(1152921505 * a0 >> 60)) % 1000, CONCAT(v9, v8) % 1000);
        }
    }
    sub_4014f0();
    return;
}
