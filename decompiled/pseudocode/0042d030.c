// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42d030; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42d030_0 {
    char padding_0[20];
    struct st_42d030_1 *field_14;
} st_42d030_0;

typedef struct st_42d030_2 {
    struct st_42d030_0 *field_0;
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
    char padding_16c[20];
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
    char padding_250[204];
    unsigned int field_31c;
    char padding_320[268];
    int field_42c;
    unsigned int field_430;
    char padding_434[24];
    int field_44c;
    char padding_450[44];
    unsigned int field_47c;
    char padding_480[432];
    int field_630;
    char padding_634[1148];
    unsigned int field_ab0;
} st_42d030_2;

typedef struct st_42d030_1 {
    unsigned int field_0;
} st_42d030_1;

extern char g_400000;
extern char g_4b2ed0;

int sub_42d030(void)
{
    unsigned int v22;  // ebx
    unsigned int v23;  // esi
    void* index;  // esi
    unsigned int v32;  // eax
    unsigned int v33;  // ecx
    unsigned int v35;  // ftop
    unsigned int v36;  // 4192
    unsigned int v37;  // ftop
    unsigned int v40;  // ftop
    unsigned int v24;  // edi
    unsigned int v41;  // ftop
    unsigned int v42;  // eax
    unsigned int v43;  // fc3210
    unsigned int v44;  // ftop
    unsigned int v45;  // 4139
    unsigned int v46;  // ftop
    st_42d030_2 *idx;  // esi
    unsigned int v48;  // eax
    unsigned int v49;  // eax
    unsigned short v50;  // cx
    st_42d030_2 *idx1;  // ecx
    void* idx2;  // esi
    unsigned short v52;  // dx
    st_42d030_2 *i;  // esi
    unsigned int v54;  // ecx
    unsigned int *iter;  // edi
    unsigned int v56;  // eax
    unsigned int v26;  // ftop
    int v27;  // edi
    unsigned int v28;  // ftop
    unsigned int v29;  // eax
    unsigned int v30;  // edx
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
    if (idx1->field_42c >= 18)
        return;
    while (1)
    {
        v27 = idx1->field_42c;
        v28 = v26 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v29 = v27 * 3;
        v30 = *((int *)&idx1->padding_480[12 + 8 * v29]);
        index = &(&idx1->field_0)[2 * v29 + 287];
        if (!v30 || !(int)index[20] && idx1->field_430 || !(v32 = idx1->field_430, !(v32 & v30)))
            break;
        if (v30 == 0x80000000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v26 = v28 + 1;
            idx1->field_42c = v27 + 1;
        }
        else
        {
            v33 = v30;
            if (v33 <= 0x1000)
            {
                if (v33 == 0x1000)
                {
                    idx1->field_430 = v32 | v30;
                    v48 = idx1->field_198;
                    v5 = (int)index[8];
                    if (!((char)v48 & 1))
                    {
                        idx1->field_190 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        *((int *)&idx1->padding_188[4]) = 0;
                        *((unsigned int *)&idx1->padding_188[0]) = 0xfff0bdc1;
                        *((char **)&idx1->padding_194[0]) = &g_4b2ed0;
                        idx1->field_198 = v48 | 1;
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
                    *((unsigned int *)&idx1->padding_188[4]) = v5;
                    *((unsigned int *)&idx1->padding_188[0]) = v5 - 1;
                    idx1->field_190 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    idx1->field_42c = idx1->field_42c + 1;
                }
                else if (v33 <= 64)
                {
                    if (v33 == 64)
                        goto LABEL_42d209;
                    switch (v33)
                    {
                    case 1:
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        idx1->field_430 = v32 | 1;
                        sub_4067e0(0);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_a8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        idx1->field_42c = idx1->field_42c + 1;
                        break;
                    case 4:
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_430 = v32 | 4;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_cc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v35 = v28 + 1;
                        v36 = _ccall(10, 13, (char)((((unsigned short)v35 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc08ef00000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (!(v36 & 1))
                        {
                            v37 = v35 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v37 = v35;
                            if (!((((unsigned short)v37 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x408ef00000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                            {
                                sub_4377a0();
                            }
                            else
                            {
                                v37 -= 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                        }
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v37 + 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_d0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        sub_4067e0(0);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_e0 = (int)index[8];
                        /* unsupported instruction */
                        sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        if (idx1->field_42c)
                        {
                            if (idx1->field_630 >= 0)
                            {
                                sub_453d90();
                                idx1->field_42c = idx1->field_42c + 1;
                                break;
                            }
                            else
                            {
                                idx1->field_42c = idx1->field_42c + 1;
                                break;
                            }
                        }
                        else
                        {
                            idx1->field_42c = idx1->field_42c + 1;
                            break;
                        }
                    case 8:
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_430 = v32 | 8;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_100 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_104 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        sub_4067e0(0);
                        idx1->field_114 = (int)index[8];
                        if (idx1->field_42c)
                        {
                            if (idx1->field_630 >= 0)
                            {
                                sub_453d90();
                                idx1->field_42c = idx1->field_42c + 1;
                                break;
                            }
                            else
                            {
                                idx1->field_42c = idx1->field_42c + 1;
                                break;
                            }
                        }
                        else
                        {
                            idx1->field_42c = idx1->field_42c + 1;
                            break;
                        }
                    case 16: case 32:
LABEL_42d209:
                        idx1->field_430 = v32 | v30;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_138 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v40 = v28;
                        if (!((char)((((unsigned short)v40 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc08f380000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_134 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v42 = idx1->field_130;
                        if (!((char)v42 & 1))
                        {
                            idx1->field_128 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            idx1->field_124 = 0;
                            idx1->field_120 = 0xfff0bdc1;
                            *((char **)&idx1->padding_12c[0]) = &g_4b2ed0;
                            idx1->field_130 = v42 | 1;
                        }
                        idx1->field_128 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v26 = v41 + 2;
                        idx1->field_124 = 0;
                        idx1->field_120 = 0xffffffff;
                        idx1->field_148 = (int)index[8];
                        idx1->field_14c = (int)index[12];
                        idx1->field_150 = 0;
                        idx1->field_42c = idx1->field_42c + 1;
                        break;
                    default:
LABEL_42d678:
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v26 = v28 + 1;
                        idx1->field_42c = idx1->field_42c + 1;
                        break;
                    }
                }
                else if (v33 > 0x400)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v33 == 0x800)
                    {
                        idx = idx1->padding_634;
                        sub_402520();
                        idx->padding_480[29] = 16;
                        idx->padding_480[28] = 16;
                        sub_454d10(idx, *((int *)(208 * index[8] + 4911744)) + (int)index[12]);
                        idx1->field_42c = idx1->field_42c + 1;
                    }
                    else
                    {
                        idx1->field_42c = idx1->field_42c + 1;
                    }
                }
                else if (v33 == 0x400)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    idx1->field_430 = v32 | v30;
                    sub_4067e0((int)index[8]);
                    idx1->field_31c = (int)index[12];
                    idx1->field_42c = idx1->field_42c + 1;
                }
                else if (v33 != 0x100)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v33 == 0x200)
                    {
                        idx1->field_44c = (int)index[8];
                        idx1->field_42c = idx1->field_42c + 1;
                    }
                    else
                    {
                        idx1->field_42c = idx1->field_42c + 1;
                    }
                }
                else
                {
                    if ((int)index[8] <= 0)
                        goto LABEL_42d678;
                    idx1->field_430 = v32 | v30;
                    v43 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)index)) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    v44 = v28 + 1;
                    v45 = _ccall(10, 13, (char)((((unsigned short)v44 & 7) * 0x800 | (unsigned short)v43 & 0x4700) >> 8) & 65, 0, 0);
                    if (!(v45 & 1))
                    {
                        v46 = v44 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v46 = v44 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    idx1->field_168 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v26 = v46 + 1;
                    *((unsigned int *)&index[8]) = (int)index[8] - 1;
                    idx1->field_180 = (int)index[8];
                    *((unsigned int *)&idx1->padding_16c[16]) = 0;
                    idx1->field_184 = (int)index[12];
                    idx1->field_42c = idx1->field_42c + 1;
                }
            }
            else
            {
                if (v33 <= 0x80000)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v33 != 0x80000)
                    {
                        if (v33 <= 0x20000)
                        {
                            if (v33 != 0x20000)
                            {
                                if (v33 != 0x2000)
                                {
                                    if (v33 == 0x4000)
                                    {
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v0 = 0x4000;
                                        /* unsupported instruction */
                                        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        idx1->field_42c = idx1->field_42c + 1;
                                    }
                                    else
                                    {
                                        idx1->field_42c = idx1->field_42c + 1;
                                    }
                                }
                                else
                                {
                                    *((unsigned int *)&idx1->padding_4[8]) = 3;
                                    idx1->field_42c = idx1->field_42c + 1;
                                }
                            }
                            else
                            {
                                idx1->field_430 = v32 | v30;
                                sub_4067e0((int)index[8]);
                                idx1->field_42c = idx1->field_42c + 1;
                            }
                        }
                        else
                        {
                            if (v33 == 0x40000)
                            {
                                idx1->field_430 = v32 | v30;
                                sub_4067e0((int)index[8]);
                                idx1->field_42c = idx1->field_42c + 1;
                            }
                            else
                            {
                                idx1->field_42c = idx1->field_42c + 1;
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
                        v49 = (int)index[8];
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v16 = v49 >> 24 & 127;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v50 = (char)index[9];
                        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        idx2 = index + 24;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v3 = (char)index[10];
                        v52 = *((short *)((char *)idx2 - 12));
                        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1->field_42c = idx1->field_42c + 1;
                        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v4 = v50;
                        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v18 = v49 & 0xff;
                        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v17 = (int)idx2[12];
                        v14 = v52;
                        v15 = (short)idx2[8];
                        i = &idx1->field_47c;
                        v54 = 108;
                        for (iter = &v13; v54; i = i->padding_4)
                        {
                            v54 -= 1;
                            *(iter) = i->field_0;
                            iter += 1;
                        }
                        sub_40b5e0();
                        idx1->field_42c = idx1->field_42c + 1;
                        if (v49 & 0x80000000)
                        {
                            idx1->field_0->field_14(0, 0);
                            idx1->field_42c = idx1->field_42c + 1;
                        }
                    }
                }
                else if (v33 > 0x800000)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v33 == 0x10000000)
                    {
                        if ((int)index[8])
                        {
                            idx1->field_ab0 = idx1->field_ab0 & 0xffffff3f | 32;
                            idx1->field_42c = idx1->field_42c + 1;
                        }
                        else
                        {
                            idx1->field_ab0 = idx1->field_ab0 & 0xffffff1f;
                            idx1->field_42c = idx1->field_42c + 1;
                        }
                    }
                    else
                    {
                        idx1->field_42c = idx1->field_42c + 1;
                    }
                }
                else if (v33 != 0x800000)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    if (v33 == 0x200000)
                    {
                        idx1->field_80 = (int)index[8];
                        idx1->field_42c = v27 + 1;
                    }
                    else if (v33 == &g_400000)
                    {
                        idx1->field_42c = (int)index[8];
                    }
                    else
                    {
                        idx1->field_42c = idx1->field_42c + 1;
                    }
                }
                else
                {
                    idx1->field_430 = v32 | 0x800000;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx1->field_238 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx1->field_23c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v56 = idx1->field_234;
                    if (!((char)v56 & 1))
                    {
                        idx1->field_22c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        idx1->field_228 = 0;
                        idx1->field_224 = 0xfff0bdc1;
                        *((char **)&idx1->padding_230[0]) = &g_4b2ed0;
                        idx1->field_234 = v56 | 1;
                    }
                    idx1->field_22c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v26 = v28 + 1;
                    idx1->field_228 = 0;
                    idx1->field_224 = 0xffffffff;
                    idx1->field_24c = (int)index[8];
                    idx1->field_42c = idx1->field_42c + 1;
                }
            }
        }
        if (idx1->field_42c >= 18)
            return;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return;
}
