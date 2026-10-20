// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4079d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4079d0_0 {
    char padding_0[24];
    int field_18;
} st_4079d0_0;

unsigned int sub_4079d0(unsigned int a0, st_4079d0_0 *a1, unsigned int a2)
{
    unsigned int v1;  // ecx
    unsigned int v2;  // ftop
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v12;  // ftop
    unsigned int v14;  // 4139
    unsigned int v3;  // ftop
    unsigned short v4;  // ftop
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned int v0;  // [bp-0x8]

    v1 = 0;
    if (a1->field_18 < 30)
        return 0;
    if (a1->field_18 < 90)
    {
        v3 = v2 - 1;
        if (/* unsupported instruction */)
        {
            v4 = v3 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v4 = v3 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
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
        v5 = v4 - 1;
        if (/* unsupported instruction */)
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = v6 + 1;
    }
    else
    {
        v7 = v2 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
        v8 = v7 + 1;
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
        v8 = v7 + 1;
    }
    v9 = v8 - 1;
    if (/* unsupported instruction */)
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v11 = v10 - 1;
    if (/* unsupported instruction */)
    {
        v12 = v11 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v12 = v11 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = _ccall(10, 13, (char)(((v12 + 2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (!(v14 & 1))
        v1 = 16;
    return v1;
}
