// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48dab3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48dab3(int a0, unsigned int a1, int a2, int a3)
{
    int v0;  // eax

    v0 = 0;
    if (a3 == 10 && a0 < 0)
        v0 = 1;
    return sub_48d9ac(a2, a3, v0);
}
