// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48be53; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48be53(int a0, int a1)
{
    char v0;  // [bp+0x0]

    if (sub_48b89c(a0, a1, &v0))
    {
        return 1;
    }
    else if (a0 != 95)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
