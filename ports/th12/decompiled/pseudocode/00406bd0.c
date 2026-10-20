// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406bd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4b43c4;

void sub_406bd0(void)
{
    if (*((int *)&g_4b43c4))
    {
        sub_406930(*((int *)&g_4b43c4));
        sub_46ca4f(*((int *)&g_4b43c4));
    }
    return;
}
