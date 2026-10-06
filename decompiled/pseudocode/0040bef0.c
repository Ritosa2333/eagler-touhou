// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40bef0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40bef0_0 {
    char padding_0[1236];
    unsigned int field_4d4;
    char padding_4d8[80];
    unsigned int field_528;
    char padding_52c[24];
    int field_544;
    char padding_548[688];
    int field_7f8;
    char padding_7fc[4];
    char field_800;
} st_40bef0_0;

int sub_40bef0(void)
{
    int v3;  // esi
    unsigned int v4;  // edi
    st_40bef0_0 *idx;  // eax
    int v6;  // edi
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned int v10;  // fc3210
    unsigned short v11;  // ftop
    int v12;  // edx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!sub_40d560((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), v3))
        return;
    v4 = 0;
    if (idx->field_800 & 1 && sub_40be50(v6))
        v4 = 1;
    if (idx->field_800 & 2 && sub_40be90())
        v4 = 1;
    if (idx->field_800 & 8 && sub_40bde0())
        v4 = 1;
    if (idx->field_800 & 4 && sub_40bd70())
        v4 = 1;
    v8 = v7 - 1;
    if (/* unsupported instruction */)
    {
        v9 = v8 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v9 = v8 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v10 = CmpF(/* unsupported instruction */, 0xc08ef00000000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v11 = v9 + 1;
    }
    else
    {
        v10 = CmpF(nan, 0xc08ef00000000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v11 = v9 + 1;
    }
    if (!((char)(((v11 & 7) * 0x800 | (unsigned short)v10 & 0x4700) >> 8) & 65))
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
            idx->field_4d4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_4d4 = nan;
            /* unsupported instruction */
        }
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
    if (/* unsupported instruction */)
    {
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
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
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    sub_40d640();
    if (v4)
    {
        v12 = idx->field_544;
        idx->field_7f8 = idx->field_7f8 + 1;
        if (v12 >= 0)
            sub_453d90();
    }
    if (idx->field_7f8 < idx->padding_7fc)
        return;
    idx->field_528 = idx->field_528 & 0xfffffeff;
    return;
}
