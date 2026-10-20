// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425790; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_425790_0 {
    char padding_0[428];
    unsigned int field_1ac;
    unsigned int field_1b0;
    unsigned int field_1b4;
    unsigned int field_1b8;
    char padding_1bc[16];
    unsigned int field_1cc;
    unsigned int field_1d0;
    unsigned int field_1d4;
    char padding_1d8[4];
    unsigned int field_1dc;
    unsigned int field_1e0;
    unsigned int field_1e4;
} st_425790_0;

extern char g_4b2ed0;

int sub_425790(void)
{
    unsigned int v3;  // esi
    st_425790_0 *idx;  // eax
    unsigned int *v5;  // edx
    unsigned int *v6;  // ecx
    unsigned int v7;  // ecx
    unsigned int v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]
    char v2;  // [bp+0x8]

    /* unsupported instruction */
    /* unsupported instruction */
    v0 = v3;
    idx->field_1e0 = v1;
    idx->field_1e4 = v2;
    idx->field_1ac = *(v5);
    idx->field_1b0 = v5[1];
    idx->field_1b4 = *(v6);
    idx->field_1b8 = v6[1];
    v7 = idx->field_1dc;
    if (!((char)idx->field_1dc & 1))
    {
        if (/* unsupported instruction */)
            idx->field_1d4 = /* unsupported instruction */;
        else
            idx->field_1d4 = nan;
        idx->field_1d0 = 0;
        idx->field_1cc = 0xfff0bdc1;
        *((char **)&idx->padding_1d8[0]) = &g_4b2ed0;
        idx->field_1dc = v7 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_1d4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_1d4 = nan;
        /* unsupported instruction */
    }
    idx->field_1d0 = 0;
    idx->field_1cc = 0xffffffff;
    return;
}
