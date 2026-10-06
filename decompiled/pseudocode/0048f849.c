// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48f849; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_48f849(void)
{
    unsigned int v2;  // eax
    unsigned int v3;  // ebx
    unsigned int v4;  // ecx
    unsigned int v5;  // esi
    unsigned int v0;  // [bp-0x4]

    v2 = 0;
    if ((char)v3 & 16)
        v2 = 1;
    if ((char)v3 & 8)
        v2 |= 4;
    if ((char)v3 & 4)
        v2 |= 8;
    if ((char)v3 & 2)
        v2 |= 16;
    if ((char)v3 & 1)
        v2 |= 32;
    if (v3 & 0x80000)
        v2 |= 2;
    v4 = v3 & 0x300;
    v0 = v5;
    if (v3 & 0x300)
    {
        if (v4 == 0x100)
        {
            v2 |= 0x400;
        }
        else if (v4 == 0x200)
        {
            v2 |= 0x800;
        }
        else if (v4 == 0x300)
        {
            v2 |= 0xc00;
        }
    }
    if (!(v3 & 0x30000))
    {
        v2 |= 0x300;
    }
    else if ((v3 >> 16 & 3) == 1)
    {
        v2 |= 0x200;
    }
    if (v3 & 0x40000)
        v2 |= 0x1000;
    return v2;
}
