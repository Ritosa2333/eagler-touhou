// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40f790; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_40f790(int a0, int *a1)
{
    int v1;  // eax

    v1 = a1[2];
    if (!v1)
    {
        *(a1) = a0;
        return a0;
    }
    else if (a0 >= v1)
    {
        *(a1) = v1 - 1;
        return v1 - 1;
    }
    else
    {
        *(a1) = (unsigned int)((a0 < 0) + 1) & a0;
        return (unsigned int)((a0 < 0) + 1) & a0;
    }
}
