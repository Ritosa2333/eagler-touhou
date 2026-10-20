// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48f7aa; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48f7aa(unsigned int a0)
{
    unsigned short v3;  // cx
    unsigned int v4;  // eax
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // ecx
    unsigned int v8;  // ecx
    unsigned int v9;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v3 = a0;
    v4 = 0;
    if (v3 & 1)
        v4 = 16;
    if ((char)v3 & 4)
        v4 |= 8;
    if ((char)v3 & 8)
        v4 |= 4;
    if ((char)v3 & 16)
        v4 |= 2;
    if ((char)v3 & 32)
        v4 |= 1;
    if ((char)v3 & 2)
        v4 |= 0x80000;
    v2 = v5;
    v1 = v6;
    v7 = v3;
    v8 = v7 & 0xc00;
    v0 = v9;
    if (v7 & 0xc00)
    {
        if (v8 == 0x400)
        {
            v4 |= 0x100;
        }
        else if (v8 == 0x800)
        {
            v4 |= 0x200;
        }
        else if (v8 == 0xc00)
        {
            v4 |= 0x300;
        }
    }
    if (!(v7 & 0x300))
    {
        v4 |= 0x20000;
    }
    else if ((v7 >> 8 & 3) == 2)
    {
        v4 |= 0x10000;
    }
    if (a0 & 0x1000)
        v4 |= 0x40000;
    return v4;
}
