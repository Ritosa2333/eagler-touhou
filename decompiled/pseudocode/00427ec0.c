// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427ec0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427ec0_0 {
    char padding_0[20];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[4];
    unsigned int field_24;
    char padding_28[16];
    unsigned int field_38;
    char padding_3c[16];
    unsigned int field_4c;
    char padding_50[68];
    unsigned int field_94;
    char padding_98[944];
    unsigned int field_448;
} st_427ec0_0;

extern char g_4a05fc;
extern char g_4b2ed0;

st_427ec0_0 * sub_427ec0(unsigned int a0)
{
    st_427ec0_0 *idx;  // esi
    int v1;  // ecx
    st_427ec0_0 *iter;  // eax
    int v3;  // ecx
    unsigned int v5;  // eax

    *((char **)&idx->padding_0[0]) = &g_4a05fc;
    idx->field_24 = idx->field_24 & 0xfffffffe;
    idx->field_38 = idx->field_38 & 0xfffffffe;
    idx->field_4c = idx->field_4c & 0xfffffffe;
    v1 = 0x11;
    iter = &idx->field_94;
    do
    {
        v3 = v1;
        *((unsigned int *)&iter->padding_0[0]) = *((int *)&iter->padding_0[0]) & 0xfffffffe;
        iter = &iter->padding_28[12];
        v1 = v3 - 1;
    } while (v3 - 1 >= 0);
    idx->field_448 = idx->field_448 & 0xfffffffe;
    sub_477420(idx, 0, 1108);
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = idx->field_24;
    if (!((char)idx->field_24 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_1c = /* unsupported instruction */;
        else
            idx->field_1c = nan;
        idx->field_18 = 0;
        idx->field_14 = 0xfff0bdc1;
        *((char **)&idx->padding_20[0]) = &g_4b2ed0;
        idx->field_24 = v5 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_1c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_1c = nan;
        /* unsupported instruction */
    }
    idx->field_18 = 0;
    idx->field_14 = 0xffffffff;
    return idx;
}
