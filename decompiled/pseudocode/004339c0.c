// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4339c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4339c0_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[8];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    char padding_1c[4];
    unsigned int field_20;
    char padding_24[452];
    int field_1e8;
    unsigned int field_1ec;
} st_4339c0_0;

extern char g_4b2ed0;

void sub_4339c0(void)
{
    st_4339c0_0 *idx;  // esi
    unsigned int v4;  // eax
    unsigned int v5;  // ebx
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4 = 19;
    v4 = idx->field_20;
    if (!((char)idx->field_20 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_18 = /* unsupported instruction */;
        else
            idx->field_18 = nan;
        idx->field_14 = 0;
        idx->field_10 = 0xfff0bdc1;
        *((char **)&idx->padding_1c[0]) = &g_4b2ed0;
        idx->field_20 = v4 | 1;
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
    v1 = v5;
    idx->field_14 = 0;
    idx->field_10 = 0xffffffff;
    v0 = idx->field_1ec;
    sub_461970();
    sub_461970(idx->field_1e8);
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
    if (!/* unsupported instruction */)
    {
        *((unsigned int *)&g_4b2ed0) = nan;
        /* unsupported instruction */
        return;
    }
    *((int *)&g_4b2ed0) = /* unsupported instruction */;
    /* unsupported instruction */
    return;
}
