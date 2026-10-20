// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4287c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4287c0_0 {
    char padding_0[12];
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[20];
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    char padding_34[4];
    unsigned int field_38;
    unsigned int field_3c;
    unsigned int field_40;
    unsigned int field_44;
    char padding_48[4];
    unsigned int field_4c;
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[12];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    unsigned int field_74;
    unsigned int field_78;
    char padding_7c[956];
    unsigned int field_438;
    unsigned int field_43c;
    unsigned int field_440;
    char padding_444[4];
    unsigned int field_448;
    char padding_44c[4];
    unsigned short field_450;
    short field_452;
    unsigned int field_454;
    unsigned int field_458;
    unsigned int field_45c;
    char padding_460[24];
    unsigned short field_478;
    short field_47a;
    unsigned int field_47c;
    char padding_480[436];
    int field_634;
    char padding_638[968];
    unsigned short field_a00;
    char padding_a02[182];
    unsigned int field_ab8;
    char padding_abc[20];
    struct st_4287c0_1 *field_ad0;
    char padding_ad4[24];
    struct st_4287c0_0 *field_aec;
    char padding_af0[964];
    unsigned short field_eb4;
    char padding_eb6[182];
    unsigned int field_f6c;
    char padding_f70[20];
    struct st_4287c0_1 *field_f84;
    char padding_f88[4];
    char field_f8c;
    char field_f8d;
} st_4287c0_0;

typedef struct st_4287c0_1 {
    unsigned int field_0;
} st_4287c0_1;

typedef struct st_4287c0_3 {
    char padding_0[1160];
    unsigned int field_488;
} st_4287c0_3;

extern char g_4b2ed0;
extern st_4287c0_3 *g_4b44f4;

unsigned int sub_4287c0(st_4287c0_0 *idx, unsigned int a1, unsigned int *i)
{
    unsigned int v8;  // ebx
    unsigned int v9;  // esi
    unsigned int v18;  // eax
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ecx
    unsigned int v24;  // ftop
    unsigned int v25;  // eax
    unsigned int v26;  // eax
    unsigned int v27;  // fc3210
    unsigned int v10;  // edi
    unsigned int v28;  // edx
    unsigned int v29;  // eax
    unsigned int v30;  // 4140
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    st_4287c0_0 *iter;  // edi
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned short v46;  // ftop
    unsigned short v47;  // ftop
    unsigned int v12;  // ecx
    unsigned short v48;  // ftop
    unsigned short v49;  // ftop
    unsigned short v50;  // ftop
    unsigned short v51;  // ftop
    unsigned short v52;  // ftop
    unsigned short v53;  // ftop
    unsigned short v54;  // ftop
    unsigned short v13;  // ax
    unsigned short v14;  // cx
    st_4287c0_0 *idx1;  // esi
    unsigned int v16;  // edi
    st_4287c0_0 *index;  // esi
    unsigned int v0;  // [bp-0x48]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x38]
    unsigned int v3;  // [bp-0x30]
    unsigned int v4;  // [bp-0x24]
    unsigned int v5;  // [bp-0x20]
    unsigned int v6;  // [bp-0x1c]
    unsigned int v7;  // [bp-0x18]

    v6 = v8;
    v5 = v9;
    v4 = v10;
    iter = &idx->field_454;
    for (v12 = 122; v12; i += 1)
    {
        v12 -= 1;
        *((unsigned int *)&iter->padding_0[0]) = *(i);
        iter = &iter->padding_0[4];
    }
    v13 = idx->field_478;
    v14 = idx->field_47a;
    idx1 = &idx->padding_638[4];
    idx->field_c = 2;
    idx->field_10 = 0;
    idx->field_450 = v13;
    idx->field_452 = v14;
    sub_402520();
    *((void* *)&idx->padding_ad4[20]) = sub_428ac0;
    idx->field_aec = idx;
    sub_454ee0();
    if (*((int *)&idx1->padding_480[20]))
        (*((int *)&idx1->padding_480[20]))();
    *((unsigned short *)&idx1->padding_7c[840]) = 2;
    sub_455630(idx1);
    idx1->field_47c = idx1->field_47c & 0xffffff3f | 32;
    v16 = idx->field_47a;
    idx->field_ab8 = idx->field_ab8 & 0xf0c7ffff | 0xc00000;
    index = idx->padding_af0;
    v7 = g_4b44f4->field_488;
    sub_402520(idx1);
    index->padding_480[29] = 16;
    index->padding_480[28] = 16;
    sub_454d10(index, v16 + 53);
    if (*((int *)&index->padding_480[20]))
        (*((int *)&index->padding_480[20]))();
    *((unsigned short *)&index->padding_7c[840]) = 2;
    sub_455630(index);
    /* unsupported instruction */
    /* unsupported instruction */
    index->field_47c = index->field_47c & 0xffffff3f | 32;
    idx->field_f6c = idx->field_f6c & 0xf0ffffff | 0x800000;
    v18 = idx->field_448;
    if (!((char)idx->field_448 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_440 = /* unsupported instruction */;
        else
            idx->field_440 = nan;
        idx->field_43c = 0;
        idx->field_438 = 0xfff0bdc1;
        *((char **)&idx->padding_444[0]) = &g_4b2ed0;
        idx->field_448 = v18 | 1;
    }
    v20 = v19 - 2;
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
    idx->field_43c = 30;
    if (/* unsupported instruction */)
    {
        idx->field_440 = /* unsupported instruction */;
        /* unsupported instruction */
        v22 = v21 + 1;
    }
    else
    {
        idx->field_440 = nan;
        /* unsupported instruction */
        v22 = v21 + 1;
    }
    idx->field_438 = 29;
    if (idx->field_634 >= 0)
    {
        v2 = v23;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
            v24 = v22 + 1;
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
            v24 = v22 + 1;
        }
        sub_453e20();
        v22 = v24 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v25 = idx->field_38;
    if (!((char)idx->field_38 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_30 = /* unsupported instruction */;
        else
            idx->field_30 = nan;
        idx->field_2c = 0;
        idx->field_28 = 0xfff0bdc1;
        *((char **)&idx->padding_34[0]) = &g_4b2ed0;
        idx->field_38 = v25 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_30 = /* unsupported instruction */;
    else
        idx->field_30 = nan;
    idx->field_2c = 0;
    idx->field_28 = 0xffffffff;
    v26 = idx->field_4c;
    if (!((char)idx->field_4c & 1))
    {
        if (/* unsupported instruction */)
            idx->field_44 = /* unsupported instruction */;
        else
            idx->field_44 = nan;
        idx->field_40 = 0;
        idx->field_3c = 0xfff0bdc1;
        *((char **)&idx->padding_48[0]) = &g_4b2ed0;
        idx->field_4c = v26 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_44 = /* unsupported instruction */;
    else
        idx->field_44 = nan;
    idx->field_40 = 0;
    idx->field_3c = 0xffffffff;
    v27 = (/* unsupported instruction */ ? CmpF(/* unsupported instruction */, idx->field_47c) * 0x100 & 0x4500 : CmpF(nan, idx->field_47c) * 0x100 & 0x4500);
    v28 = idx->field_458;
    v29 = idx->field_45c;
    idx->field_50 = idx->field_454;
    idx->field_54 = v28;
    idx->field_58 = v29;
    v30 = _ccall(10, 13, (char)((((unsigned short)v22 & 7) * 0x800 | (unsigned short)v27 & 0x4700) >> 8) & 0x44, 0, 0);
    if (v30 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v31 = v22 - 0;
        if (/* unsupported instruction */)
        {
            v32 = v31 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v32 = v31 - 1;
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
            v33 = v32 + 1;
        }
        else
        {
            idx->field_50 = nan;
            /* unsupported instruction */
            v33 = v32 + 1;
        }
        v34 = v33 - 1;
        if (/* unsupported instruction */)
        {
            v35 = v34 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v35 = v34 - 1;
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
            v36 = v35 + 1;
        }
        else
        {
            idx->field_54 = nan;
            /* unsupported instruction */
            v36 = v35 + 1;
        }
        v22 = v36 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v37 = v22 - 1;
    if (/* unsupported instruction */)
    {
        v38 = v37 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v38 = v37 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
        v39 = v38 + 1;
    }
    else
    {
        v3 = nan;
        /* unsupported instruction */
        v39 = v38 + 1;
    }
    v40 = v39 - 1;
    if (/* unsupported instruction */)
    {
        v41 = v40 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v41 = v40 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
        idx->field_6c = /* unsupported instruction */;
    else
        idx->field_6c = nan;
    v42 = v41 - 1;
    if (/* unsupported instruction */)
    {
        v43 = v42 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v43 = v42 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        idx->field_70 = /* unsupported instruction */;
        /* unsupported instruction */
        v44 = v43 + 1;
    }
    else
    {
        idx->field_70 = nan;
        /* unsupported instruction */
        v44 = v43 + 1;
    }
    v45 = v44 - 1;
    if (/* unsupported instruction */)
    {
        v46 = v45 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v46 = v45 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
        v47 = v46 + 1;
    }
    else
    {
        v3 = nan;
        /* unsupported instruction */
        v47 = v46 + 1;
    }
    v48 = v47 - 1;
    if (/* unsupported instruction */)
    {
        v49 = v48 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v49 = v48 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
        idx->field_74 = /* unsupported instruction */;
    else
        idx->field_74 = nan;
    v50 = v49 - 1;
    if (/* unsupported instruction */)
    {
        v51 = v50 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v51 = v50 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
        v52 = v51 + 1;
    }
    else
    {
        v3 = nan;
        /* unsupported instruction */
        v52 = v51 + 1;
    }
    v53 = v52 - 1;
    if (/* unsupported instruction */)
    {
        v54 = v53 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v54 = v53 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
        idx->field_68 = /* unsupported instruction */;
    else
        idx->field_68 = nan;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)(((v54 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    sub_42e880();
    return 0;
}
