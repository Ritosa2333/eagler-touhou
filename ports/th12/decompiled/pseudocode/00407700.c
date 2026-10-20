// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x407700; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_407700(unsigned int *a0, unsigned int a1, unsigned int a2)
{
    if (a0[1] == *(a0))
    {
        return 0;
    }
    else if (!(a0[1] % a2))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
