// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x483f19; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4adf68;
extern char g_4adf6c;
extern char g_4adf70;

void sub_483f19(int *a0)
{
    if (!a0)
        return;
    if (*(a0) != *((int *)&g_4adf68))
        sub_46c941(*(a0));
    if (a0[1] != *((int *)&g_4adf6c))
        sub_46c941(a0[1]);
    if (a0[2] == *((int *)&g_4adf70))
        return;
    sub_46c941(a0[2]);
    return;
}
