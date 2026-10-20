// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450810; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4d48c4;

void sub_450810(void)
{
    unsigned int v4;  // esi
    int v5;  // esi
    char v0;  // [bp-0x110]
    char v1;  // [bp-0x10c]
    unsigned int v2;  // [bp-0x8]

    v2 = g_4ad138 ^ &v0;
    sub_44f4b0(v4);
    if (g_4d48c4 & 0x40000)
    {
        sub_46d585("snapshot");
        v5 = 0;
        while (1)
        {
            sub_46cbbb(&v1, "snapshot/th%.3d.bmp", v5);
            if (!sub_463df0(&v1))
                break;
            v5 += 1;
            if (v5 >= 1000)
            {
                _security_check_cookie();
                return;
            }
        }
        if (v5 < 1000)
            sub_42fca0();
    }
    _security_check_cookie();
    return;
}
