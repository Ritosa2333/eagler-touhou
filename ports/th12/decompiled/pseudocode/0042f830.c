// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f830; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b43d8;
extern unsigned int g_4b44e8;
extern int g_4b44f8;
extern int g_4b4518;
extern int g_4b4530;
extern unsigned int g_4cf468;

void sub_42f830(unsigned int a0)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    v3 = a0;
    v2 = v5;
    v1 = v6;
    g_4cf468 = 0;
    if (g_4b44e8)
    {
        v0 = g_4b44e8;
        sub_422290();
        sub_46ca4f(g_4b44e8);
    }
    if (g_4b4530)
    {
        sub_43f4f0(g_4b4530);
        sub_46ca4f(g_4b4530);
    }
    if (g_4b44f8)
    {
        sub_42ec00(g_4b44f8);
        sub_46ca4f(g_4b44f8);
    }
    if (g_4b43d8)
    {
        sub_410ea0();
        sub_46ca4f(g_4b43d8);
    }
    if (g_4b4518)
    {
        sub_43b450(g_4b4518);
        sub_46ca4f(g_4b4518);
    }
    return;
}
