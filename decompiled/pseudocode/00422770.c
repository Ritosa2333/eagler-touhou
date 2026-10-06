// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422770; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b44e8;
extern unsigned int g_4cf468;

void sub_422770(void)
{
    unsigned int v3;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v3;
    g_4cf468 = 0;
    if (g_4b44e8)
    {
        v0 = g_4b44e8;
        sub_422290();
        sub_46ca4f(g_4b44e8);
    }
    return;
}
