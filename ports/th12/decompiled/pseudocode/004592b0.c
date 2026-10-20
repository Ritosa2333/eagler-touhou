// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4592b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4592b0_0 {
    char padding_0[16];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    char padding_1c[4];
    unsigned int field_20;
} st_4592b0_0;

extern char g_4b2ed0;

int sub_4592b0(void)
{
    st_4592b0_0 *idx;  // eax
    unsigned int v2;  // ecx

    v2 = idx->field_20;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_20 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_18 = /* unsupported instruction */;
        else
            idx->field_18 = nan;
        idx->field_14 = 0;
        idx->field_10 = 0xfff0bdc1;
        *((char **)&idx->padding_1c[0]) = &g_4b2ed0;
        idx->field_20 = v2 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_18 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_18 = nan;
        /* unsupported instruction */
    }
    idx->field_14 = 0;
    idx->field_10 = 0xffffffff;
    return;
}
