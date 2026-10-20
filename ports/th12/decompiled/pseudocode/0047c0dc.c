// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c0dc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b3d98;
extern unsigned int g_4d52e0;

void sub_47c0dc(void)
{
    unsigned int v0;  // [bp+0x0]

    sub_48d307();
    if (g_4b3d98)
        sub_48d08b();
    sub_46c941(g_4d52e0, v0);
    return;
}
