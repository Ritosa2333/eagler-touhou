// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4597f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4597f0_0 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    unsigned int field_38;
    char padding_3c[4];
    unsigned int field_40;
} st_4597f0_0;

extern char g_4b2ed0;

int sub_4597f0(void)
{
    st_4597f0_0 *idx;  // eax
    unsigned int v2;  // ecx

    v2 = idx->field_40;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_40 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_38 = /* unsupported instruction */;
        else
            idx->field_38 = nan;
        idx->field_34 = 0;
        idx->field_30 = 0xfff0bdc1;
        *((char **)&idx->padding_3c[0]) = &g_4b2ed0;
        idx->field_40 = v2 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_38 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_38 = nan;
        /* unsupported instruction */
    }
    idx->field_34 = 0;
    idx->field_30 = 0xffffffff;
    return;
}
