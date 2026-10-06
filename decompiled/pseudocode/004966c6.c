// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4966c6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_4966c6(char a0)
{
    unsigned int v0;  // [bp-0x8]

    if (a0 & 32)
    {
        v0 = 5;
        return v0;
    }
    else if (a0 & 8)
    {
        return 1;
    }
    else if (a0 & 4)
    {
        v0 = 2;
        return v0;
    }
    else if (a0 & 1)
    {
        v0 = 3;
        return v0;
    }
    else
    {
        return (a0 & 2) * 2;
    }
}
