// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485e71; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_485e71(unsigned int a0)
{
    void* idx;  // esi
    int v4;  // edi
    unsigned int v5;  // edi
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    unsigned int v0;  // [bp-0x8c]
    int v1;  // [bp-0x84]
    Byte v2[120];  // [bp-0x80]

    idx = sub_475467() + 156;
    v5 = sub_485b44(v4, v1);
    if (!GetLocaleInfoA(v5, (-(0 < (int)idx[16]) & 0xfffff002) + 4097, v2, 120))
    {
        *((unsigned int *)&idx[8]) = 0;
        return v6;
    }
    if (!sub_46e3f8(*((int *)idx), v2))
    {
        if (!(int)idx[16])
        {
            v0 = 1;
        }
        else
        {
            *((unsigned int *)&idx[8]) = (int)idx[8] | 4;
            *((unsigned int *)&idx[24]) = v5;
            *((unsigned int *)&idx[28]) = v5;
            return v7;
        }
    }
    else
    {
        if ((int)idx[16])
        {
            return v7;
        }
        else if (!(int)idx[12])
        {
            return v7;
        }
        else if (!sub_46e3f8(*((int *)idx), v2))
        {
            v0 = 0;
        }
        else
        {
            return v7;
        }
    }
    if (!sub_485c2b(v5))
        return v7;
    *((unsigned int *)&idx[8]) = (int)idx[8] | 4;
    *((unsigned int *)&idx[24]) = v5;
    *((unsigned int *)&idx[28]) = v5;
    return v7;
}
