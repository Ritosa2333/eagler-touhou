// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42b100; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42b100_0 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[1552];
    unsigned int field_68c;
    char padding_690[1012];
    unsigned int field_a84;
    unsigned int field_a88;
    unsigned int field_a8c;
    char padding_a90[76];
    unsigned int field_adc;
    char padding_ae0[1112];
    unsigned int field_f38;
    unsigned int field_f3c;
    unsigned int field_f40;
} st_42b100_0;

extern unsigned int g_4ce8cc;

unsigned int sub_42b100(st_42b100_0 *idx)
{
    unsigned int v7;  // esi
    unsigned int v8;  // ftop
    unsigned int v17;  // ftop
    unsigned short v18;  // ftop
    unsigned short v19;  // ftop
    unsigned short v20;  // ftop
    unsigned short v21;  // ftop
    unsigned short v22;  // ftop
    unsigned short v23;  // ftop
    unsigned short v24;  // ftop
    unsigned short v25;  // ftop
    unsigned short v26;  // ftop
    unsigned int v9;  // ftop
    unsigned int v27;  // fc3210
    unsigned int v28;  // 4144
    unsigned int v10;  // ftop
    unsigned int v11;  // edi
    st_42b100_0 *index;  // edi
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    v5 = v7;
    v9 = v8 - 1;
    if (/* unsupported instruction */)
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v4 = v11;
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
    index = &idx->padding_7c[1508];
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
    if (/* unsupported instruction */)
    {
        idx->field_a84 = /* unsupported instruction */;
        /* unsupported instruction */
        v13 = v10 + 1;
    }
    else
    {
        idx->field_a84 = nan;
        /* unsupported instruction */
        v13 = v10 + 1;
    }
    v14 = v13 - 1;
    if (/* unsupported instruction */)
    {
        v15 = v14 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v15 = v14 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
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
    if (/* unsupported instruction */)
    {
        idx->field_a88 = /* unsupported instruction */;
        /* unsupported instruction */
        v16 = v15 + 1;
    }
    else
    {
        idx->field_a88 = nan;
        /* unsupported instruction */
        v16 = v15 + 1;
    }
    v17 = v16 - 1;
    if (/* unsupported instruction */)
    {
        v18 = v17 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v18 = v17 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        idx->field_a8c = /* unsupported instruction */;
        /* unsupported instruction */
        v19 = v18 + 1;
    }
    else
    {
        idx->field_a8c = nan;
        /* unsupported instruction */
        v19 = v18 + 1;
    }
    v20 = v19 - 1;
    if (/* unsupported instruction */)
    {
        v21 = v20 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v21 = v20 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
        v22 = v21 + 1;
    }
    else
    {
        v3 = nan;
        /* unsupported instruction */
        v22 = v21 + 1;
    }
    v23 = v22 - 1;
    if (/* unsupported instruction */)
    {
        v24 = v23 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v24 = v23 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
        v25 = v24 + 1;
    }
    else
    {
        v2 = nan;
        /* unsupported instruction */
        v25 = v24 + 1;
    }
    sub_464640();
    if (/* unsupported instruction */)
    {
        *((int *)&index->padding_0[44]) = /* unsupported instruction */;
        /* unsupported instruction */
        v26 = v25 + 1;
    }
    else
    {
        *((unsigned int *)&index->padding_0[44]) = nan;
        /* unsupported instruction */
        v26 = v25 + 1;
    }
    *((unsigned int *)&index->padding_7c[0x400]) = *((int *)&index->padding_7c[0x400]) | 4;
    v1 = g_4ce8cc;
    sub_45c900();
    /* unsupported instruction */
    /* unsupported instruction */
    v27 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_78) * 0x100 & 0x4500;
    /* unsupported instruction */
    v28 = _ccall(10, 13, (char)(((v26 & 7) * 0x800 | (unsigned short)v27 & 0x4700) >> 8) & 0x44, 0, 0);
    if (v28 & 1)
        return 0;
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
    v0 = g_4ce8cc;
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
    if (/* unsupported instruction */)
    {
        idx->field_f38 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_f38 = nan;
        /* unsupported instruction */
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
    if (/* unsupported instruction */)
    {
        idx->field_f3c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_f3c = nan;
        /* unsupported instruction */
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
    if (/* unsupported instruction */)
    {
        idx->field_f40 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_f40 = nan;
        /* unsupported instruction */
    }
    sub_45c900();
    return 0;
}
