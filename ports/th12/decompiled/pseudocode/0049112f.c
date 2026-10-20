// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49112f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_49112f(unsigned int *ptr)
{
    unsigned int v1;  // eax
    unsigned int v0;  // [bp-0x8]

    v1 = ptr[3];
    if (!((char)v1 & 131))
    {
        return v1;
    }
    else if ((char)v1 & 8)
    {
        sub_46c941(ptr[2], v0);
        ptr[3] = ptr[3] & 0xfffffbf7;
        memset(ptr, 0, 12);
        return 0;
    }
    else
    {
        return v1;
    }
}
