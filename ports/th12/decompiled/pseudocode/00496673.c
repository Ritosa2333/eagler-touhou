// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x496673; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_496673(int a0)
{
    char v0;  // [bp+0x0]

    if (a0 == 1)
    {
        *((unsigned int *)sub_46e96f(&v0)) = 33;
        return;
    }
    else if (a0 > 1)
    {
        if (a0 > 3)
            return;
        *((unsigned int *)sub_46e96f()) = 0x22;
        return;
    }
    else
    {
        return;
    }
}
