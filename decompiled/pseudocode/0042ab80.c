// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42ab80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42ab80_0 {
    char padding_0[12];
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[60];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[12];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    unsigned int field_74;
    char padding_78[8];
    unsigned int field_80;
    char padding_84[972];
    unsigned short field_450;
    short field_452;
    unsigned int field_454;
    unsigned int field_458;
    unsigned int field_45c;
    char padding_460[52];
    int field_494;
    char padding_498[4];
    unsigned int field_49c;
    unsigned int field_4a0;
    unsigned short field_4a4;
    short field_4a6;
    char padding_4a8[436];
    unsigned int field_65c;
    char padding_660[964];
    unsigned short field_a24;
    char padding_a26[182];
    unsigned int field_adc;
    char padding_ae0[20];
    struct st_42ab80_1 *field_af4;
    char padding_af8[24];
    struct st_42ab80_0 *field_b10;
    char padding_b14[964];
    unsigned short field_ed8;
    char padding_eda[182];
    unsigned int field_f90;
    char padding_f94[20];
    struct st_42ab80_1 *field_fa8;
    char padding_fac[4];
    char field_fb0;
    char field_fb1;
} st_42ab80_0;

typedef struct st_42ab80_1 {
    unsigned int field_0;
} st_42ab80_1;

typedef struct st_42ab80_3 {
    char padding_0[1160];
    unsigned int field_488;
} st_42ab80_3;

extern st_42ab80_3 *g_4b44f4;

unsigned int sub_42ab80(st_42ab80_0 *idx, unsigned int a1, unsigned int *i)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // esi
    unsigned int v17;  // ecx
    unsigned short v18;  // ftop
    unsigned int v19;  // fc3210
    unsigned int v20;  // edx
    unsigned int v21;  // eax
    unsigned int v22;  // 4168
    unsigned int v23;  // ecx
    unsigned int v9;  // edi
    st_42ab80_0 *iter;  // edi
    unsigned int v11;  // ecx
    unsigned short v12;  // ax
    unsigned short v13;  // cx
    st_42ab80_0 *idx1;  // esi
    unsigned int v15;  // edi
    st_42ab80_0 *index;  // esi
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x38]
    unsigned int v3;  // [bp-0x24]
    unsigned int v4;  // [bp-0x20]
    unsigned int v5;  // [bp-0x1c]
    unsigned int v6;  // [bp-0x18]

    v5 = v7;
    v4 = v8;
    v3 = v9;
    iter = &idx->field_454;
    for (v11 = 130; v11; i += 1)
    {
        v11 -= 1;
        *((unsigned int *)&iter->padding_0[0]) = *(i);
        iter = &iter->padding_0[4];
    }
    v12 = idx->field_4a4;
    v13 = idx->field_4a6;
    idx1 = idx->padding_660;
    idx->field_c = 3;
    idx->field_10 = 1;
    idx->field_450 = v12;
    idx->field_452 = v13;
    sub_402520();
    *((void* *)&idx->padding_af8[20]) = sub_428ac0;
    idx->field_b10 = idx;
    sub_454ee0();
    if (idx1->field_494)
        idx1->field_494();
    *((unsigned short *)&idx1->padding_84[832]) = 2;
    sub_455630(idx1);
    *((unsigned int *)&idx1->padding_460[28]) = *((int *)&idx1->padding_460[28]) & 0xffffff3f | 32;
    v15 = idx->field_4a6;
    idx->field_adc = idx->field_adc & 0xf0c7ffff | 0xc00000;
    index = idx->padding_b14;
    v6 = g_4b44f4->field_488;
    sub_402520(idx1);
    *((char *)&index->field_49c + 1) = 16;
    *((char *)&index->field_49c) = 16;
    sub_454d10(index, v15 + 53);
    if (index->field_494)
        index->field_494();
    *((unsigned short *)&index->padding_84[832]) = 2;
    sub_455630(index);
    *((unsigned int *)&index->padding_460[28]) = *((int *)&index->padding_460[28]) & 0xffffff3f | 32;
    idx->field_f90 = idx->field_f90 & 0xf0ffffff | 0x800000;
    if (idx->field_494 >= 0)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v17;
        /* unsupported instruction */
        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_4a0) * 0x100 & 0x4500;
    /* unsupported instruction */
    v20 = idx->field_458;
    v21 = idx->field_45c;
    idx->field_50 = idx->field_454;
    idx->field_54 = v20;
    idx->field_58 = v21;
    v22 = _ccall(10, 13, (char)(((v18 & 7) * 0x800 | (unsigned short)v19 & 0x4700) >> 8) & 0x44, 0, 0);
    if (v22 & 1)
    {
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
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
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
            v0 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v0 = nan;
            /* unsupported instruction */
        }
        sub_42e880();
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
            idx->field_50 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_50 = nan;
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
            idx->field_54 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_54 = nan;
            /* unsupported instruction */
        }
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
    v23 = idx->field_49c;
    if (/* unsupported instruction */)
    {
        idx->field_6c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_6c = nan;
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
        idx->field_70 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_70 = nan;
        /* unsupported instruction */
    }
    idx->field_80 = v23;
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
    idx->field_65c = 0;
    if (/* unsupported instruction */)
    {
        idx->field_74 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_74 = nan;
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
    if (!/* unsupported instruction */)
    {
        idx->field_68 = nan;
        /* unsupported instruction */
        return 0;
    }
    idx->field_68 = /* unsupported instruction */;
    /* unsupported instruction */
    return 0;
}
