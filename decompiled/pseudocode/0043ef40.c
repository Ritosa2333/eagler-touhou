// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43ef40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43ef40_0 {
    char padding_0[36];
    unsigned int field_24;
    char padding_28[652];
    unsigned int field_2b4;
    unsigned int field_2b8;
    unsigned int field_2bc;
    char padding_2c0[4];
    unsigned int field_2c4;
} st_43ef40_0;

extern char g_4b2ed0;

int sub_43ef40(void)
{
    st_43ef40_0 *idx;  // eax
    unsigned int v2;  // ecx
    unsigned int v3;  // ecx

    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_24 = v2;
    v3 = idx->field_2c4;
    if (!((char)idx->field_2c4 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_2bc = /* unsupported instruction */;
        else
            idx->field_2bc = nan;
        idx->field_2b8 = 0;
        idx->field_2b4 = 0xfff0bdc1;
        *((char **)&idx->padding_2c0[0]) = &g_4b2ed0;
        idx->field_2c4 = v3 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_2bc = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_2bc = nan;
        /* unsupported instruction */
    }
    idx->field_2b8 = 0;
    idx->field_2b4 = 0xffffffff;
    return;
}
