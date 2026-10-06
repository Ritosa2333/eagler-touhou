// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x459830; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_459830_0 {
    char padding_0[232];
    unsigned int field_e8;
    unsigned int field_ec;
    unsigned int field_f0;
    unsigned int field_f4;
    unsigned int field_f8;
    unsigned int field_fc;
    unsigned int field_100;
    unsigned int field_104;
    unsigned int field_108;
    unsigned int field_10c;
    unsigned int field_110;
    unsigned int field_114;
    unsigned int field_118;
    unsigned int field_11c;
    unsigned int field_120;
    char padding_124[4];
    unsigned int field_128;
    unsigned int field_12c;
    unsigned int field_130;
} st_459830_0;

extern char g_4b2ed0;

int sub_459830(void)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v15;  // edx
    unsigned int v16;  // ecx
    st_459830_0 *idx;  // eax
    unsigned int v8;  // edi
    char *v9;  // ecx
    unsigned int v10;  // edi
    unsigned int v11;  // ebx
    char *v12;  // edx
    unsigned int v13;  // ebp
    unsigned int v14;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]
    unsigned int v3;  // [bp+0x4]
    char v4;  // [bp+0x8]

    v2 = v5;
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = v6;
    idx->field_12c = v3;
    idx->field_100 = 0;
    idx->field_10c = 0;
    v0 = v8;
    idx->field_104 = 0;
    idx->field_110 = 0;
    idx->field_130 = v4;
    idx->field_108 = 0;
    idx->field_114 = 0;
    v10 = v9[2];
    v11 = v9[1];
    v13 = v12[2];
    v14 = v12[1];
    v15 = *(v12);
    idx->field_f4 = *(v9);
    idx->field_f8 = v11;
    idx->field_e8 = v15;
    idx->field_fc = v10;
    idx->field_ec = v14;
    idx->field_f0 = v13;
    v16 = idx->field_128;
    if (!((char)idx->field_128 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_120 = /* unsupported instruction */;
        else
            idx->field_120 = nan;
        idx->field_11c = 0;
        idx->field_118 = 0xfff0bdc1;
        *((char **)&idx->padding_124[0]) = &g_4b2ed0;
        idx->field_128 = v16 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_120 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_120 = nan;
        /* unsupported instruction */
    }
    idx->field_11c = 0;
    idx->field_118 = 0xffffffff;
    return;
}
