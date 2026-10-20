// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40c3b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40c3b0_0 {
    char padding_0[1212];
    unsigned int field_4bc;
    unsigned int field_4c0;
    unsigned int field_4c4;
    char padding_4c8[96];
    unsigned int field_528;
    char padding_52c[988];
    unsigned int field_908;
    int field_90c;
    unsigned int field_910;
    char padding_914[4];
    unsigned int field_918;
} st_40c3b0_0;

extern char g_4b2ed0;

int sub_40c3b0(void)
{
    st_40c3b0_0 *idx;  // eax
    unsigned int v5;  // ecx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    if (idx->field_90c >= *((int *)&idx[1].padding_0[20]))
    {
        idx->field_528 = idx->field_528 & 0xfffffff5;
        return;
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4c0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4c4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v5 = idx->field_918;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_918 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_910 = /* unsupported instruction */;
        else
            idx->field_910 = nan;
        idx->field_90c = 0;
        idx->field_908 = 0xfff0bdc1;
        *((char **)&idx->padding_914[0]) = &g_4b2ed0;
        idx->field_918 = v5 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_910 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_910 = nan;
        /* unsupported instruction */
    }
    idx->field_90c = 0;
    idx->field_908 = 0xffffffff;
    return;
}
