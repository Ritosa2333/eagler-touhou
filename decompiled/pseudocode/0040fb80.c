// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40fb80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b43d4;

int sub_40fb80(void)
{
    unsigned int v2;  // eax
    int v0;  // [bp-0x4]

    if (!g_4b43d4)
        return v2;
    sub_40fa30(v0);
    return sub_46ca4f(g_4b43d4);
}
