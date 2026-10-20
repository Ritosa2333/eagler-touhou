// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x405fd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_405fd0_0 {
    char padding_0[112];
    unsigned int field_70;
    unsigned int field_74;
    unsigned int field_78;
    char padding_7c[4];
    unsigned int field_80;
} st_405fd0_0;

extern char g_4b2ed0;

int sub_405fd0(void)
{
    st_405fd0_0 *idx;  // eax
    unsigned int v2;  // ecx

    v2 = idx->field_80;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_80 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_78 = /* unsupported instruction */;
        else
            idx->field_78 = nan;
        idx->field_74 = 0;
        idx->field_70 = 0xfff0bdc1;
        *((char **)&idx->padding_7c[0]) = &g_4b2ed0;
        idx->field_80 = v2 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_78 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_78 = nan;
        /* unsupported instruction */
    }
    idx->field_74 = 0;
    idx->field_70 = 0xffffffff;
    return;
}
