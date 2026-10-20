// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41c350; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41c350_0 {
    char padding_0[32];
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    char padding_2c[4];
    unsigned int field_30;
} st_41c350_0;

extern char g_4b2ed0;

int sub_41c350(void)
{
    st_41c350_0 *idx;  // eax
    unsigned int v2;  // ecx

    v2 = idx->field_30;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_30 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_28 = /* unsupported instruction */;
        else
            idx->field_28 = nan;
        idx->field_24 = 0;
        idx->field_20 = 0xfff0bdc1;
        *((char **)&idx->padding_2c[0]) = &g_4b2ed0;
        idx->field_30 = v2 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_28 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_28 = nan;
        /* unsupported instruction */
    }
    idx->field_24 = 0;
    idx->field_20 = 0xffffffff;
    return;
}
