// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40c000; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40c000_0 {
    char padding_0[1320];
    unsigned int field_528;
    char padding_52c[24];
    int field_544;
    char padding_548[756];
    int field_83c;
} st_40c000_0;

unsigned int sub_40c000(void)
{
    unsigned int v5;  // ftop
    unsigned int v6;  // ftop
    unsigned int v15;  // esi
    unsigned int *v16;  // ecx
    unsigned short v18;  // ftop
    unsigned short v19;  // ftop
    unsigned short v20;  // ftop
    unsigned int v21;  // fc3210
    unsigned int v22;  // 4132
    st_40c000_0 *idx;  // edi
    unsigned int v7;  // ftop
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v13;  // ftop
    unsigned short v14;  // ftop
    unsigned int *v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    v6 = v5 - 1;
    if (/* unsupported instruction */)
    {
        v7 = v6 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v7 = v6 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
        v8 = (unsigned short)v7 + 1;
    }
    else
    {
        v3 = nan;
        /* unsupported instruction */
        v8 = (unsigned short)v7 + 1;
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
    if (/* unsupported instruction */)
    {
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
        v11 = v10 + 1;
    }
    else
    {
        v2 = nan;
        /* unsupported instruction */
        v11 = v10 + 1;
    }
    if (!sub_409970())
        return 0;
    v13 = v11 - 1;
    if (/* unsupported instruction */)
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v1 = v15;
    /* unsupported instruction */
    v18 = v14 + 1;
    if (!((char)(((v18 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v16)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
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
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v19 = v18 - 1;
        if (/* unsupported instruction */)
        {
            v20 = v19 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v20 = v19 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v21 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v16)) * 0x100 & 0x4500;
        /* unsupported instruction */
        v22 = _ccall(10, 13, (char)(((v20 + 1 & 7) * 0x800 | (unsigned short)v21 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v22 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else if (idx->field_83c <= 0)
        {
            idx->field_528 = idx->field_528 ^ 0x20000;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    *(v16) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v0 = v16;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    if (idx->field_544 >= 0)
    {
        sub_453d90();
        if (idx->field_83c > 0)
            return 0;
        idx->field_528 = idx->field_528 ^ 0x20000;
        return 1;
    }
    else if (idx->field_83c <= 0)
    {
        idx->field_528 = idx->field_528 ^ 0x20000;
        return 1;
    }
    else
    {
        return 0;
    }
}
