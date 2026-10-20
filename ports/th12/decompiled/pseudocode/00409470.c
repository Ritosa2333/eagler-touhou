// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409470; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43c8;

int sub_409470(void)
{
    int v1;  // esi

    sub_46ce49(v1 + 100, 2552, 2001, sub_4094b0, sub_4094f0);
    sub_477420(v1, 0, 5106656);
    g_4b43c8 = v1;
    return v1;
}
