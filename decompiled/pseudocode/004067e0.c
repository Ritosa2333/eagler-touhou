// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4067e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4067e0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[4];
    unsigned int field_10;
} st_4067e0_0;

extern char g_4b2ed0;

int sub_4067e0(void)
{
    st_4067e0_0 *idx;  // eax
    unsigned int v2;  // ecx
    unsigned int v0;  // [bp+0x4]

    v2 = idx->field_10;
    if (!((char)v2 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_4 = 0;
        idx->field_0 = 0xfff0bdc1;
        *((char **)&idx->padding_c[0]) = &g_4b2ed0;
        idx->field_10 = v2 | 1;
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
    idx->field_4 = v0;
    idx->field_0 = v0 - 1;
    if (!/* unsupported instruction */)
    {
        idx->field_8 = nan;
        /* unsupported instruction */
        return;
    }
    idx->field_8 = /* unsupported instruction */;
    /* unsupported instruction */
    return;
}
