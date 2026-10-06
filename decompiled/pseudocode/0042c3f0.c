// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42c3f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42c3f0_0 {
    char padding_0[12];
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[20];
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    char padding_34[4];
    unsigned int field_38;
    char padding_3c[20];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[12];
    unsigned int field_68;
    char padding_6c[4];
    unsigned int field_70;
    unsigned int field_74;
    char padding_78[960];
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
    char padding_460[12];
    unsigned short field_46c;
    short field_46e;
    int field_470;
    unsigned int field_474;
    char padding_478[436];
    int field_62c;
    char padding_630[968];
    unsigned short field_9f8;
    char padding_9fa[182];
    unsigned int field_ab0;
    char padding_ab4[20];
    struct st_42c3f0_1 *field_ac8;
    char padding_acc[24];
    struct st_42c3f0_0 *field_ae4;
    char padding_ae8[964];
    unsigned short field_eac;
    char padding_eae[182];
    unsigned int field_f64;
    char padding_f68[20];
    struct st_42c3f0_1 *field_f7c;
    char padding_f80[4];
    char field_f84;
    char field_f85;
    char padding_f86[22];
    unsigned int field_f9c;
    unsigned int field_fa0;
} st_42c3f0_0;

typedef struct st_42c3f0_1 {
    unsigned int field_0;
} st_42c3f0_1;

typedef struct st_42c3f0_3 {
    char padding_0[1160];
    unsigned int field_488;
} st_42c3f0_3;

extern char g_4b2ed0;
extern st_42c3f0_3 *g_4b44f4;

unsigned int sub_42c3f0(st_42c3f0_0 *idx, unsigned int a1, unsigned int *i)
{
    unsigned int v8;  // ebx
    unsigned int v9;  // esi
    unsigned int v18;  // eax
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // fc3210
    unsigned int v22;  // ecx
    unsigned int v23;  // edx
    unsigned int v24;  // 4172
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v10;  // edi
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    int j;  // edx
    unsigned int v32;  // eax
    unsigned int *idx1;  // eax
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    st_42c3f0_0 *iter;  // edi
    unsigned int v38;  // ftop
    unsigned int v39;  // eax
    unsigned int v40;  // eax
    unsigned int v12;  // ecx
    unsigned short v13;  // ax
    unsigned short v14;  // cx
    st_42c3f0_0 *idx2;  // esi
    unsigned int v16;  // edi
    st_42c3f0_0 *index;  // esi
    unsigned int v0;  // [bp-0x48]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x40]
    unsigned int v3;  // [bp-0x30]
    unsigned int v4;  // [bp-0x24]
    unsigned int v5;  // [bp-0x20]
    unsigned int v6;  // [bp-0x1c]
    unsigned int v7;  // [bp-0x18]

    v6 = v8;
    v5 = v9;
    v4 = v10;
    iter = &idx->field_454;
    for (v12 = 120; v12; i += 1)
    {
        v12 -= 1;
        *((unsigned int *)&iter->padding_0[0]) = *(i);
        iter = &iter->padding_0[4];
    }
    v13 = idx->field_46c;
    v14 = idx->field_46e;
    idx2 = &idx->padding_630[4];
    idx->field_c = 2;
    idx->field_10 = 2;
    idx->field_450 = v13;
    idx->field_452 = v14;
    sub_402520();
    *((void* *)&idx->padding_acc[20]) = sub_42e6d0;
    idx->field_ae4 = idx;
    sub_454ee0();
    if (*((int *)&idx2->padding_478[28]))
        (*((int *)&idx2->padding_478[28]))();
    *((unsigned short *)&idx2->padding_78[844]) = 2;
    sub_455630(idx2);
    *((unsigned int *)&idx2->padding_478[4]) = *((int *)&idx2->padding_478[4]) & 0xffffff3f | 32;
    v16 = idx->field_46e;
    idx->field_ab0 = idx->field_ab0 & 0xf0c7ffff | 0xc00000;
    index = idx->padding_ae8;
    v7 = g_4b44f4->field_488;
    sub_402520(idx2);
    index->padding_478[37] = 16;
    index->padding_478[36] = 16;
    sub_454d10(index, v16 + 53);
    if (*((int *)&index->padding_478[28]))
        (*((int *)&index->padding_478[28]))();
    *((unsigned short *)&index->padding_78[844]) = 2;
    sub_455630(index);
    *((unsigned int *)&index->padding_478[4]) = *((int *)&index->padding_478[4]) & 0xffffff3f | 32;
    idx->field_f64 = idx->field_f64 & 0xf0ffffff | 0x800000;
    idx->field_fa0 = sub_46d04a(idx->field_470 * 56);
    v18 = sub_46d04a(idx->field_470 * 20);
    v20 = v19 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v21 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_474) * 0x100 & 0x4500;
    v22 = idx->field_458;
    v23 = idx->field_45c;
    idx->field_f9c = v18;
    idx->field_50 = idx->field_454;
    idx->field_54 = v22;
    idx->field_58 = v23;
    v24 = _ccall(10, 13, (char)((((unsigned short)v20 & 7) * 0x800 | (unsigned short)v21 & 0x4700) >> 8) & 0x44, 0, 0);
    if (v24 & 1)
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
        v25 = v20 - 0;
        if (/* unsupported instruction */)
        {
            v26 = v25 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v26 = v25 - 1;
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
            v27 = v26 + 1;
        }
        else
        {
            idx->field_50 = nan;
            /* unsupported instruction */
            v27 = v26 + 1;
        }
        v28 = v27 - 1;
        if (/* unsupported instruction */)
        {
            v29 = v28 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v29 = v28 - 1;
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
            v30 = v29 + 1;
        }
        else
        {
            idx->field_54 = nan;
            /* unsupported instruction */
            v30 = v29 + 1;
        }
        v20 = v30 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    j = 0;
    if (idx->field_470 > 0)
    {
        v22 = 0;
        do
        {
            v32 = idx->field_f9c;
            *((unsigned int *)(v32 + v22)) = idx->field_50;
            idx1 = v32 + v22;
            idx1[1] = idx->field_54;
            idx1[2] = idx->field_58;
            v34 = v20 - 1;
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
                *((int *)(v22 + idx->field_f9c + 12)) = /* unsupported instruction */;
                /* unsupported instruction */
                v36 = v35 + 1;
            }
            else
            {
                *((unsigned int *)(v22 + idx->field_f9c + 12)) = nan;
                /* unsupported instruction */
                v36 = v35 + 1;
            }
            v37 = v36 - 1;
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
            j += 1;
            if (/* unsupported instruction */)
            {
                *((int *)(v22 + idx->field_f9c + 16)) = /* unsupported instruction */;
                /* unsupported instruction */
                v20 = v38 + 1;
            }
            else
            {
                *((unsigned int *)(v22 + idx->field_f9c + 16)) = nan;
                /* unsupported instruction */
                v20 = v38 + 1;
            }
            v22 += 20;
        } while (j < idx->field_470);
    }
    v39 = idx->field_448;
    if (!((char)idx->field_448 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_440 = /* unsupported instruction */;
        else
            idx->field_440 = nan;
        idx->field_43c = 0;
        idx->field_438 = 0xfff0bdc1;
        *((char **)&idx->padding_444[0]) = &g_4b2ed0;
        idx->field_448 = v39 | 1;
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
    idx->field_43c = 30;
    if (/* unsupported instruction */)
    {
        idx->field_440 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_440 = nan;
        /* unsupported instruction */
    }
    idx->field_438 = 29;
    if (idx->field_62c >= 0)
    {
        v2 = v22;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        sub_453e20();
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v40 = idx->field_38;
    if (!((char)idx->field_38 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_30 = /* unsupported instruction */;
        else
            idx->field_30 = nan;
        idx->field_2c = 0;
        idx->field_28 = 0xfff0bdc1;
        *((char **)&idx->padding_34[0]) = &g_4b2ed0;
        idx->field_38 = v40 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_30 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_30 = nan;
        /* unsupported instruction */
    }
    idx->field_2c = 0;
    idx->field_28 = 0xffffffff;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
        idx->field_68 = /* unsupported instruction */;
    else
        idx->field_68 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
        idx->field_74 = /* unsupported instruction */;
    else
        idx->field_74 = nan;
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
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    sub_42e880();
    return 0;
}
