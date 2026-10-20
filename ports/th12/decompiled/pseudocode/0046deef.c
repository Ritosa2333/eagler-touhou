// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46deef; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b41dc;

void sub_46deef(void)
{
    unsigned int *v1;  // eax

    v1 = sub_4751de(g_4b41dc);
    if (v1)
        v1();
    sub_472f8d(25);
    sub_47a10a(0, 1);
    sub_479ff3(); /* do not return */
}
