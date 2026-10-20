// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x496a9d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_496a9d(unsigned int a0, int a1)
{
    unsigned short v1;  // cx
    unsigned int v0;  // [bp-0x8]

    if (*((unsigned int *)&a1) == 0x7ff00000)
    {
        if (!a0)
            return 1;
    }
    else
    {
        if (*((unsigned int *)&a1) == 0xfff00000 && !a0)
        {
            v0 = 2;
            return v0;
        }
    }
    v1 = (unsigned short)*((unsigned int *)(&a1 + 2)) & 0x7ff8;
    if (v1 == 0x7ff8)
    {
        v0 = 3;
        return v0;
    }
    else if (v1 == 0x7ff0)
    {
        if (!(*((unsigned int *)&a1) & 0x7ffff) && !a0)
            return 0;
        v0 = 4;
        return v0;
    }
    else
    {
        return 0;
    }
}
