// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e19b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49cd48;

int sub_46e19b(unsigned int a0, unsigned int a1, char a2)
{
    *((char **)a0) = &g_49cd48;
    sub_46e086(a0);
    if (a2 & 1)
        sub_46ca4f(a0);
    return a0;
}
