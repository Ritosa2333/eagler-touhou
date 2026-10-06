// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48abc6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_48abc6(unsigned int a0, unsigned int a1, int a2, unsigned int a3, int a4, int a5, int a6)
{
    if (a3 != 101 && a3 != 69)
    {
        if (a3 == 0x66)
        {
            sub_48a9d4(a0, a1, a2, a4, a6);
            return;
        }
        if (a3 != 97 && a3 != 65)
        {
            sub_48aaac(a0, a1, a2, a4, a5, a6);
            return;
        }
        sub_48a54a(a0, a1, a2, a4, a5, a6);
        return;
    }
    sub_48a45a(a0, a1, a2, a4, a5, a6);
    return;
}
