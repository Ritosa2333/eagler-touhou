// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f180; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49cd00;

int sub_44f180(unsigned int a0, unsigned int a1, int a2)
{
    int v0;  // esi

    sub_46dfce(a2, v0);
    *((char **)a0) = &g_49cd00;
    return a0;
}
