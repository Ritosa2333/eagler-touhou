// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428b00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428b00_0 {
    char padding_0[20];
    struct st_428b00_1 *field_14;
} st_428b00_0;

typedef struct st_428b00_2 {
    struct st_428b00_0 *field_0;
    char padding_4[124];
    int field_80;
    char padding_84[36];
    unsigned int field_a8;
    char padding_ac[32];
    unsigned int field_cc;
    unsigned int field_d0;
    char padding_d4[12];
    int field_e0;
    char padding_e4[28];
    unsigned int field_100;
    unsigned int field_104;
    char padding_108[12];
    int field_114;
    char padding_118[8];
    unsigned int field_120;
    unsigned int field_124;
    unsigned int field_128;
    char padding_12c[4];
    unsigned int field_130;
    unsigned int field_134;
    unsigned int field_138;
    char padding_13c[12];
    int field_148;
    unsigned int field_14c;
    unsigned int field_150;
    char padding_154[20];
    unsigned int field_168;
    char padding_16c[16];
    unsigned int field_17c;
    int field_180;
    unsigned int field_184;
    char padding_188[8];
    unsigned int field_190;
    char padding_194[4];
    unsigned int field_198;
    char padding_19c[136];
    unsigned int field_224;
    unsigned int field_228;
    unsigned int field_22c;
    char padding_230[4];
    unsigned int field_234;
    unsigned int field_238;
    unsigned int field_23c;
    char padding_240[12];
    int field_24c;
    char padding_250[476];
    int field_42c;
    unsigned int field_430;
    char padding_434[24];
    int field_44c;
    char padding_450[52];
    unsigned int field_484;
    char padding_488[432];
    int field_638;
    char padding_63c[1148];
    unsigned int field_ab8;
} st_428b00_2;

typedef struct st_428b00_1 {
    unsigned int field_0;
} st_428b00_1;

extern char g_400000;
extern char g_4b2ed0;

int sub_428b00(void)
{
    unsigned int v22;  // ebx
    unsigned int v23;  // esi
    void* index;  // esi
    unsigned int v32;  // eax
    unsigned int v34;  // ftop
    unsigned int v35;  // 4191
    unsigned int v36;  // ftop
    unsigned int v38;  // fc3210
    unsigned int v39;  // ftop
    unsigned int v40;  // 4138
    unsigned int v24;  // edi
    unsigned int v41;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // eax
    st_428b00_2 *idx;  // esi
    unsigned int v47;  // eax
    unsigned int v48;  // eax
    unsigned short v49;  // cx
    void* idx1;  // esi
    st_428b00_2 *idx2;  // ecx
    unsigned short v51;  // dx
    unsigned short v52;  // ax
    st_428b00_2 *i;  // esi
    unsigned int v54;  // ecx
    unsigned int *iter;  // edi
    unsigned int v56;  // eax
    unsigned int v26;  // ftop
    int v27;  // edx
    unsigned int v28;  // ftop
    unsigned int v29;  // eax
    unsigned int v30;  // ecx
    unsigned int v0;  // [bp-0x240]
    unsigned int v1;  // [bp-0x234]
    unsigned int v2;  // [bp-0x230]
    char *v3;  // [bp-0x22c], Other Possible Types: unsigned short
    unsigned short v4;  // [bp-0x22a]
    unsigned int v5;  // [bp-0x228]
    char v6;  // [bp-0x224]
    unsigned int v7;  // [bp-0x224]
    unsigned int v8;  // [bp-0x220]
    unsigned int v9;  // [bp-0x21c]
    unsigned int v10;  // [bp-0x218]
    unsigned int v11;  // [bp-0x214]
    unsigned int v12;  // [bp-0x210]
    char v13;  // [bp-0x208]
    unsigned short v14;  // [bp-0x34]
    unsigned short v15;  // [bp-0x32]
    unsigned short v16;  // [bp-0x30]
    unsigned int v17;  // [bp-0x2c]
    unsigned int v18;  // [bp-0x20]
    unsigned int v19;  // [bp-0x1c]
    char v20;  // [bp-0x4]

    v5 = v22;
    v3 = &v20;
    v2 = v23;
    v1 = v24;
    if (idx2->field_42c >= 18)
        return;
    while (1)
    {
        v27 = idx2->field_42c;
        v28 = v26 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v29 = v27 * 3;
        v30 = *((int *)&idx2->padding_488[12 + 8 * v29]);
        index = &(&idx2->field_0)[2 * v29 + 289];
        if (!v30 || !(int)index[20] && idx2->field_430)
            break;
        if (v30 == 0x80000000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v26 = v28 + 1;
            idx2->field_42c = v27 + 1;
        }
        else
        {
            v32 = v30;
            if (v32 <= 0x1000)
            {
                if (v32 == 0x1000)
                {
                    idx2->field_430 = idx2->field_430 | v30;
                    v47 = idx2->field_198;
                    v5 = (int)index[8];
                    if (!((char)v47 & 1))
                    {
                        idx2->field_190 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        *((int *)&idx2->padding_188[4]) = 0;
                        *((unsigned int *)&idx2->padding_188[0]) = 0xfff0bdc1;
                        *((char **)&idx2->padding_194[0]) = &g_4b2ed0;
                        idx2->field_198 = v47 | 1;
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v26 = v28 + 1;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&idx2->padding_188[4]) = v5;
                    *((unsigned int *)&idx2->padding_188[0]) = v5 - 1;
                    idx2->field_190 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    idx2->field_42c = idx2->field_42c + 1;
                }
                else if (v32 <= 32)
                {
                    if (v32 == 32)
                        goto LABEL_428d2a;
                    switch (v32)
                    {
                    case 1:
                        idx2->field_430 = idx2->field_430 | 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        sub_4067e0(0);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_a8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        idx2->field_42c = idx2->field_42c + 1;
                        break;
                    case 4:
                        idx2->field_430 = idx2->field_430 | 4;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_cc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v34 = v28 + 1;
                        v35 = _ccall(10, 13, (char)((((unsigned short)v34 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc08ef00000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (!(v35 & 1))
                        {
                            v36 = v34 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v36 = v34;
                            if (!((((unsigned short)v36 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x408ef00000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                            {
                                sub_4377a0();
                            }
                            else
                            {
                                v36 -= 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                        }
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v36 + 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_d0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        sub_4067e0(0);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_e0 = (int)index[8];
                        /* unsupported instruction */
                        sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        if (idx2->field_42c)
                        {
                            if (idx2->field_638 >= 0)
                            {
                                sub_453d90();
                                idx2->field_42c = idx2->field_42c + 1;
                                break;
                            }
                            else
                            {
                                idx2->field_42c = idx2->field_42c + 1;
                                break;
                            }
                        }
                        else
                        {
                            idx2->field_42c = idx2->field_42c + 1;
                            break;
                        }
                    case 8:
                        idx2->field_430 = idx2->field_430 | 8;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_100 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_104 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        sub_4067e0(0);
                        idx2->field_114 = (int)index[8];
                        if (idx2->field_42c)
                        {
                            if (idx2->field_638 >= 0)
                            {
                                sub_453d90();
                                idx2->field_42c = idx2->field_42c + 1;
                                break;
                            }
                            else
                            {
                                idx2->field_42c = idx2->field_42c + 1;
                                break;
                            }
                        }
                        else
                        {
                            idx2->field_42c = idx2->field_42c + 1;
                            break;
                        }
                    case 16:
LABEL_428d2a:
                        idx2->field_430 = idx2->field_430 | v30;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_138 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v43 = v28;
                        if (!((char)((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc08f380000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                        {
                            v44 = v43 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v44 = v43 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_134 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v45 = idx2->field_130;
                        if (!((char)v45 & 1))
                        {
                            idx2->field_128 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            idx2->field_124 = 0;
                            idx2->field_120 = 0xfff0bdc1;
                            *((char **)&idx2->padding_12c[0]) = &g_4b2ed0;
                            idx2->field_130 = v45 | 1;
                        }
                        idx2->field_128 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v44 + 2;
                        idx2->field_124 = 0;
                        idx2->field_120 = 0xffffffff;
                        idx2->field_148 = (int)index[8];
                        idx2->field_14c = (int)index[12];
                        idx2->field_150 = 0;
                        idx2->field_42c = idx2->field_42c + 1;
                        break;
                    default:
LABEL_428dcd:
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        idx2->field_42c = idx2->field_42c + 1;
                        break;
                    }
                }
                else if (v32 > 0x200)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v32 == 0x800)
                    {
                        idx = idx2->padding_63c;
                        sub_402520();
                        idx->padding_488[21] = 16;
                        idx->padding_488[20] = 16;
                        sub_454d10(idx, *((int *)(208 * index[8] + 4911744)) + (int)index[12]);
                        idx2->field_42c = idx2->field_42c + 1;
                    }
                    else
                    {
                        idx2->field_42c = idx2->field_42c + 1;
                    }
                }
                else if (v32 != 0x200)
                {
                    if (v32 == 64)
                        goto LABEL_428d2a;
                    if (v32 != 0x100 || (int)index[8] <= 0)
                        goto LABEL_428dcd;
                    idx2->field_430 = idx2->field_430 | v30;
                    v38 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)index)) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    v39 = v28 + 1;
                    v40 = _ccall(10, 13, (char)((((unsigned short)v39 & 7) * 0x800 | (unsigned short)v38 & 0x4700) >> 8) & 65, 0, 0);
                    if (!(v40 & 1))
                    {
                        v41 = v39 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v41 = v39 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    idx2->field_168 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v26 = v41 + 1;
                    *((unsigned int *)&index[8]) = (int)index[8] - 1;
                    idx2->field_180 = (int)index[8];
                    idx2->field_17c = 0;
                    idx2->field_184 = (int)index[12];
                    idx2->field_42c = idx2->field_42c + 1;
                }
                else
                {
                    idx2->field_44c = (int)index[8];
                    goto LABEL_428dcd;
                }
            }
            else
            {
                if (v32 <= 0x80000)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v32 != 0x80000)
                    {
                        if (v32 <= 0x20000)
                        {
                            if (v32 != 0x20000)
                            {
                                if (v32 != 0x2000)
                                {
                                    if (v32 == 0x4000)
                                    {
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v0 = v30;
                                        /* unsupported instruction */
                                        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        idx2->field_42c = idx2->field_42c + 1;
                                    }
                                    else
                                    {
                                        idx2->field_42c = idx2->field_42c + 1;
                                    }
                                }
                                else
                                {
                                    *((unsigned int *)&idx2->padding_4[8]) = 3;
                                    idx2->field_42c = idx2->field_42c + 1;
                                }
                            }
                            else
                            {
                                idx2->field_430 = idx2->field_430 | v30;
                                sub_4067e0((int)index[8]);
                                idx2->field_42c = idx2->field_42c + 1;
                            }
                        }
                        else
                        {
                            if (v32 == 0x40000)
                            {
                                idx2->field_430 = idx2->field_430 | v30;
                                sub_4067e0((int)index[8]);
                                idx2->field_42c = idx2->field_42c + 1;
                            }
                            else
                            {
                                idx2->field_42c = idx2->field_42c + 1;
                            }
                        }
                    }
                    else
                    {
                        sub_477420(&v6, 0, 532);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v19 = 0xffffffff;
                        /* unsupported instruction */
                        sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v48 = (int)index[8];
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v49 = (char)index[10];
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1 = index + 24;
                        v16 = v48 >> 24 & 127;
                        v51 = *((char *)idx1 - 15);
                        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v18 = v48 & 0xff;
                        v52 = *((short *)((char *)idx1 - 12));
                        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx2->field_42c = idx2->field_42c + 1;
                        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v3 = v49;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v4 = v51;
                        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v15 = (short)idx1[8];
                        v14 = v52;
                        v17 = (int)idx1[12];
                        i = &idx2->field_484;
                        v54 = 108;
                        for (iter = &v13; v54; i = i->padding_4)
                        {
                            v54 -= 1;
                            *(iter) = i->field_0;
                            iter += 1;
                        }
                        sub_40b5e0();
                        idx2->field_42c = idx2->field_42c + 1;
                        if (v48 & 0x80000000)
                        {
                            idx2->field_0->field_14(0, 0);
                            idx2->field_42c = idx2->field_42c + 1;
                        }
                    }
                }
                else if (v32 > 0x800000)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v32 == 0x10000000)
                    {
                        if ((int)index[8])
                        {
                            idx2->field_ab8 = idx2->field_ab8 & 0xffffff3f | 32;
                            idx2->field_42c = idx2->field_42c + 1;
                        }
                        else
                        {
                            idx2->field_ab8 = idx2->field_ab8 & 0xffffff1f;
                            idx2->field_42c = idx2->field_42c + 1;
                        }
                    }
                    else
                    {
                        idx2->field_42c = idx2->field_42c + 1;
                    }
                }
                else if (v32 != 0x800000)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v32 == 0x200000)
                    {
                        idx2->field_80 = (int)index[8];
                        idx2->field_42c = v27 + 1;
                    }
                    else if (v32 == &g_400000)
                    {
                        idx2->field_42c = (int)index[8];
                    }
                    else
                    {
                        idx2->field_42c = idx2->field_42c + 1;
                    }
                }
                else
                {
                    idx2->field_430 = idx2->field_430 | 0x800000;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx2->field_238 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx2->field_23c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v56 = idx2->field_234;
                    if (!((char)v56 & 1))
                    {
                        idx2->field_22c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        idx2->field_228 = 0;
                        idx2->field_224 = 0xfff0bdc1;
                        *((char **)&idx2->padding_230[0]) = &g_4b2ed0;
                        idx2->field_234 = v56 | 1;
                    }
                    idx2->field_22c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    idx2->field_228 = 0;
                    idx2->field_224 = 0xffffffff;
                    idx2->field_24c = (int)index[8];
                    idx2->field_42c = idx2->field_42c + 1;
                }
            }
        }
        if (idx2->field_42c >= 18)
            return;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return;
}
