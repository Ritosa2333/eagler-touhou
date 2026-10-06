// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f4b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4a092c;
extern unsigned int g_4a0954;
extern unsigned int g_4ad138;
extern unsigned int g_4cf288;
extern unsigned int g_4cf28c;

void sub_42f4b0(unsigned int a0)
{
    int v5;  // edi
    int v0;  // [bp-0x98]
    unsigned int v1;  // [bp-0x90]
    char v2;  // [bp-0x88]
    char v3;  // [bp-0x84]
    unsigned int v4;  // [bp-0x4]

    v4 = g_4ad138 ^ &v2;
    if ((char)sub_44b610(v5))
    {
        sub_46cbbb(&v3, "th12_%.4x%c.ver", 0x100, 98);
        g_4cf28c = sub_463c10(&v2, 0);
        g_4cf288 = 0;
        if (g_4cf28c)
        {
            _security_check_cookie();
            return;
        }
        v0 = &g_4a092c;
    }
    else
    {
        v1 = &g_4a0954;
    }
    sub_464300(v0);
    _security_check_cookie();
    return;
}
