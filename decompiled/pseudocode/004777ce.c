// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4777ce; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b41d0;

unsigned int sub_4777ce(int a0)
{
    unsigned int v1;  // edi
    unsigned int v2;  // esi
    unsigned int v3;  // eax
    unsigned int v0;  // [bp-0xc]

    v0 = v1;
    v2 = 0;
    while (1)
    {
        v3 = sub_46d04a(a0);
        if (v3)
        {
            return v3;
        }
        else if (*((int *)&g_4b41d0) > v3)
        {
            Sleep(v2);
            v2 += 1000;
            if (v2 > *((int *)&g_4b41d0))
                v2 = 0xffffffff;
            if (v2 == 0xffffffff)
                return v3;
        }
        else
        {
            return v3;
        }
    }
}
