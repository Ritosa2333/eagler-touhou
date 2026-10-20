// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48f9ba; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_48f9ba(unsigned int a0, unsigned int a1)
{
    unsigned int v4;  // eax
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // edi
    unsigned int v8;  // ecx
    unsigned int v9;  // edx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v4 = 0;
    if ((char)a1 & 16)
        v4 = 128;
    v2 = v5;
    v1 = v6;
    v0 = v7;
    if ((char)a1 & 8)
        v4 |= 0x200;
    if ((char)a1 & 4)
        v4 |= 0x400;
    if ((char)a1 & 2)
        v4 |= 0x800;
    if ((char)a1 & 1)
        v4 |= 0x1000;
    if (a1 & 0x80000)
        v4 |= 0x100;
    v8 = a1 & 0x300;
    if (a1 & 0x300)
    {
        if (v8 == 0x100)
        {
            v4 |= 0x2000;
        }
        else if (v8 == 0x200)
        {
            v4 |= 0x4000;
        }
        else if (v8 == 0x300)
        {
            v4 |= 0x6000;
        }
    }
    v9 = a1 & 0x3000000;
    if (v9 == 0x1000000)
    {
        return v4 | 32832;
    }
    else if (v9 == 0x2000000)
    {
        return v4 | 64;
    }
    else if (v9 == 0x3000000)
    {
        return v4 | 0x8000;
    }
    else
    {
        return v4;
    }
}
