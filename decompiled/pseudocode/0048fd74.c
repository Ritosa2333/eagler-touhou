// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fd74; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48fd74(unsigned int *a0, unsigned int *a1)
{
    unsigned short v0;  // ftop
    unsigned int v1;  // fc3210
    unsigned int v2;  // eax
    unsigned int v3;  // ecx
    unsigned int v4;  // eax

    if (a0)
    {
        a0 = (v0 & 7) * 0x800 | (unsigned short)v1 & 0x4700;
        v2 = _INSERT(0, 0, a0);
        v3 = 0;
        if ((char)a0 & 63)
        {
            if ((char)v2 & 1)
                v3 = 16;
            if ((char)v2 & 4)
                v3 |= 8;
            if ((char)v2 & 8)
                v3 |= 4;
            if ((char)v2 & 16)
                v3 |= 2;
            if ((char)v2 & 32)
                v3 |= 1;
            if ((char)v2 & 2)
                v3 |= 0x80000;
        }
        *(a0) = v3;
    }
    if (!a1)
        return v2;
    v4 = sub_48fb73();
    *(a1) = v4;
    return v4;
}
