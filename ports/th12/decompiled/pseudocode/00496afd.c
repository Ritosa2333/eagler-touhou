// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x496afd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_496afd(unsigned int a0, unsigned int a1)
{
    unsigned short v4;  // ftop
    unsigned short v5;  // ftop
    unsigned int v7;  // 4146
    unsigned int v8;  // edx
    unsigned int v9;  // fc3210
    unsigned short v10;  // ftop
    unsigned int v11;  // eax
    unsigned int v12;  // esi
    unsigned int v13;  // edx
    double v0;  // [bp-0x10], Other Possible Types: unsigned int, unsigned long
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    int i;  // [bp+0x4]

    v5 = v4 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = _ccall(10, 13, (char)(((v5 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((unsigned long long *)&i)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
    if (!(v7 & 1))
    {
        v8 = 0;
    }
    else if (!(*((unsigned int *)(&i + 6)) & 0x7ff0) && (*((unsigned int *)(&i + 4)) & 0xfffff || *((unsigned int *)&i)))
    {
        if (/* unsupported instruction */)
        {
            v9 = CmpF(/* unsupported instruction */, *((unsigned long long *)&i)) * 0x100 & 0x4500;
            /* unsupported instruction */
            v10 = v5 + 1;
        }
        else
        {
            v9 = CmpF(nan, *((unsigned long long *)&i)) * 0x100 & 0x4500;
            /* unsupported instruction */
            v10 = v5 + 1;
        }
        for (v11 = (!((char)(((v10 & 7) * 0x800 | (unsigned short)v9 & 0x4700) >> 8) & 65) ? 1 : 0); !(*((char *)(&i + 6)) & 16); *((unsigned int *)&i) = *((unsigned int *)&i) * 2)
        {
            *((unsigned int *)&(&i)[4]) = *((unsigned int *)(&i + 4)) * 2;
            if (*((unsigned int *)&i) & 0x80000000)
                *((unsigned int *)&(&i)[4]) = *((unsigned int *)(&i + 4)) | 1;
        }
        v2 = v12;
        *((unsigned short *)&(&i)[6]) = *((unsigned short *)(&i + 6)) & 0xffef;
        if (v11)
            *((unsigned short *)&(&i)[6]) = *((unsigned short *)(&i + 6)) | 0x8000;
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v2 = 0;
        v1 = 0;
        v0 = 0;
        if (/* unsupported instruction */)
        {
            v0 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v0 = (double)nan;
            /* unsupported instruction */
        }
        sub_496a05();
    }
    else
    {
        v2 = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = 0;
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_496a05((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v8 = (v13 >> 4 & 0x7ff) - 0x3fe;
    }
    *((unsigned int *)*((unsigned int *)(&i + 8))) = v8;
    return;
}
