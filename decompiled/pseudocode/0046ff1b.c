// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ff1b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b3d5c;

unsigned int sub_46ff1b(int a0)
{
    unsigned int v2;  // eax
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    sub_46ec9a(4, v0, &v1);
    v2 = sub_4751de(g_4b3d5c);
    g_4b3d5c = sub_475163(a0);
    sub_46eba8(4);
    return v2;
}
