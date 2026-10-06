// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48dbec; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48dbec(int a0, int a1, unsigned int a2, int a3, int a4)
{
    int v0;  // eax
    int v1;  // edi

    v0 = 0;
    if (a4 == 10 && a1 <= 0 && (a1 < 0 || a0 < 0))
        v0 = 1;
    return sub_48daf4(a0, a1, a3, a4, v0, v1);
}
