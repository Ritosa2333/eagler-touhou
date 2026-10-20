// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x438d70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_438d70_1 {
    char padding_0[84];
    int field_54;
    unsigned int field_58;
    char padding_5c[76];
    unsigned int field_a8;
    char padding_ac[44];
    unsigned int field_d8;
} st_438d70_1;

typedef struct st_438d70_0 {
    char padding_0[2440];
    unsigned int field_988;
    unsigned int field_98c;
    char padding_990[48136];
    unsigned int field_c598;
} st_438d70_0;

extern st_438d70_0 *g_4b4514;

void sub_438d70(void)
{
    unsigned int v6;  // edi
    unsigned int v7;  // eax
    st_438d70_1 *idx;  // esi
    unsigned int v9;  // eax
    unsigned int v10;  // ecx
    int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]

    if (!g_4b4514->field_c598)
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
    }
    else
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
    }
    if (/* unsupported instruction */)
    {
        v4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v4 = nan;
        /* unsupported instruction */
    }
    v3 = v6;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
    }
    sub_4393d0();
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = sub_4931e0();
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
    idx->field_54 = g_4b4514->field_988 - v7;
    v9 = sub_4931e0();
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
    v0 = v10;
    if (/* unsupported instruction */)
    {
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
        /* unsupported instruction */
    }
    idx->field_58 = g_4b4514->field_98c - v9;
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
    sub_4646e0();
    v11 = idx->field_54;
    if (/* unsupported instruction */)
    {
        idx->field_a8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_a8 = nan;
        /* unsupported instruction */
    }
    if (v11 < -0x6600)
    {
        idx->field_d8 = idx->field_d8 | 1;
        *((unsigned int *)&idx->padding_ac[40]) = 1;
        idx->field_54 = v11 + 0xcc00;
        return;
    }
    else if (v11 > 0x6600)
    {
        idx->field_d8 = idx->field_d8 | 1;
        *((unsigned int *)&idx->padding_ac[40]) = 1;
        idx->field_54 = v11 - 0xcc00;
        return;
    }
    else
    {
        v12 = idx->field_d8;
        if ((char)v12 & 1)
            *((unsigned int *)&idx->padding_ac[40]) = 1;
        idx->field_d8 = v12 & 0xfffffffe;
        return;
    }
}
