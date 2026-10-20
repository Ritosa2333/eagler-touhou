// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x470f2d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b3d60;

void sub_470f2d(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
    char v0;  // [bp+0x0]

    sub_4751de(g_4b3d60, &v0);
    if (!sub_4751de(g_4b3d60, &v0))
    {
        sub_47b3ff(2);
        sub_470dc6();
        return;
    }
    goto *((void *)(sub_4751de(g_4b3d60, &v0)));
}
