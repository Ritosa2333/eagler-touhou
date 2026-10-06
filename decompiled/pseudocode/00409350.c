// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409350; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_409350_0 {
    char padding_0[1252];
    unsigned int field_4e4;
    unsigned int field_4e8;
    unsigned int field_4ec;
    char padding_4f0[4];
    unsigned int field_4f4;
    unsigned int field_4f8;
    unsigned int field_4fc;
    unsigned int field_500;
    char padding_504[4];
    unsigned int field_508;
    char padding_50c[38];
    unsigned short field_532;
} st_409350_0;

extern char g_4b2ed0;

int sub_409350(void)
{
    st_409350_0 *idx;  // eax
    unsigned int v2;  // ecx
    unsigned int v3;  // ecx

    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_532 = 0;
    v2 = idx->field_4f4;
    if (!((char)idx->field_4f4 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_4ec = /* unsupported instruction */;
        else
            idx->field_4ec = nan;
        idx->field_4e8 = 0;
        idx->field_4e4 = 0xfff0bdc1;
        *((char **)&idx->padding_4f0[0]) = &g_4b2ed0;
        idx->field_4f4 = v2 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_4ec = /* unsupported instruction */;
    else
        idx->field_4ec = nan;
    idx->field_4e8 = 0;
    idx->field_4e4 = 0xffffffff;
    v3 = idx->field_508;
    if (!((char)idx->field_508 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_500 = /* unsupported instruction */;
        else
            idx->field_500 = nan;
        idx->field_4fc = 0;
        idx->field_4f8 = 0xfff0bdc1;
        *((char **)&idx->padding_504[0]) = &g_4b2ed0;
        idx->field_508 = v3 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_500 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_500 = nan;
        /* unsupported instruction */
    }
    idx->field_4fc = 0;
    idx->field_4f8 = 0xffffffff;
    return;
}
