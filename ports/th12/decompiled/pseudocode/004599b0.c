// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4599b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4599b0_0 {
    char padding_0[308];
    unsigned int field_134;
    unsigned int field_138;
    unsigned int field_13c;
    unsigned int field_140;
    unsigned int field_144;
    unsigned int field_148;
    unsigned int field_14c;
    char padding_150[4];
    unsigned int field_154;
    unsigned int field_158;
    unsigned int field_15c;
} st_4599b0_0;

extern char g_4b2ed0;

int sub_4599b0(void)
{
    st_4599b0_0 *idx;  // eax
    unsigned int v4;  // ecx
    unsigned int v5;  // ecx
    char v0;  // [bp+0x4]
    char v1;  // [bp+0x8]
    char v2;  // [bp+0xc]

    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_158 = v4;
    idx->field_15c = v0;
    idx->field_134 = v1;
    idx->field_13c = 0;
    idx->field_140 = 0;
    idx->field_138 = v2;
    v5 = idx->field_154;
    if (!((char)idx->field_154 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_14c = /* unsupported instruction */;
        else
            idx->field_14c = nan;
        idx->field_148 = 0;
        idx->field_144 = 0xfff0bdc1;
        *((char **)&idx->padding_150[0]) = &g_4b2ed0;
        idx->field_154 = v5 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_14c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_14c = nan;
        /* unsupported instruction */
    }
    idx->field_148 = 0;
    idx->field_144 = 0xffffffff;
    return;
}
