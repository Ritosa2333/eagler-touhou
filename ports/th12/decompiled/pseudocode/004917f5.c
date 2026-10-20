// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4917f5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49f3a8;

unsigned int sub_4917f5(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v2;  // esi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    v0 = a2;
    sub_491781(a0);
    *((char **)a0) = &g_49f3a8;
    return a0;
}
