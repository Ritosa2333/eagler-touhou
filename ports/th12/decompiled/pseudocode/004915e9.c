// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4915e9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49f39c;

int sub_4915e9(unsigned int a0, unsigned int a1, int a2)
{
    int v1;  // esi
    char v0;  // [bp+0x0]

    sub_491517(a2, v1, &v0);
    *((char **)a0) = &g_49f39c;
    return a0;
}
