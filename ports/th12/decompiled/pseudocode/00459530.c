// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x459530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_459530_0 {
    char padding_0[488];
    unsigned int field_1e8;
    unsigned int field_1ec;
    unsigned int field_1f0;
    unsigned int field_1f4;
    char padding_1f8[16];
    unsigned int field_208;
    unsigned int field_20c;
    unsigned int field_210;
    char padding_214[4];
    unsigned int field_218;
    unsigned int field_21c;
    unsigned int field_220;
} st_459530_0;

extern char g_4b2ed0;

int sub_459530(void)
{
    unsigned int v3;  // esi
    st_459530_0 *idx;  // eax
    unsigned int *v5;  // edx
    unsigned int *v6;  // ecx
    unsigned int v7;  // ecx
    unsigned int v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]
    char v2;  // [bp+0x8]

    /* unsupported instruction */
    /* unsupported instruction */
    v0 = v3;
    idx->field_21c = v1;
    idx->field_220 = v2;
    idx->field_1e8 = *(v5);
    idx->field_1ec = v5[1];
    idx->field_1f0 = *(v6);
    idx->field_1f4 = v6[1];
    v7 = idx->field_218;
    if (!((char)idx->field_218 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_210 = /* unsupported instruction */;
        else
            idx->field_210 = nan;
        idx->field_20c = 0;
        idx->field_208 = 0xfff0bdc1;
        *((char **)&idx->padding_214[0]) = &g_4b2ed0;
        idx->field_218 = v7 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_210 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_210 = nan;
        /* unsupported instruction */
    }
    idx->field_20c = 0;
    idx->field_208 = 0xffffffff;
    return;
}
