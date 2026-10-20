// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x403020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_403020_3 {
    char padding_0[124];
    char field_7c;
} st_403020_3;

typedef struct st_403020_0 {
    unsigned int field_0;
    int field_4;
    char padding_8[8];
    void* field_10;
    unsigned int field_14;
} st_403020_0;

typedef struct st_403020_1 {
    char padding_0[8];
    unsigned int field_8;
} st_403020_1;

extern unsigned int g_4b2ed0;
extern st_403020_3 *g_4b43cc;
extern unsigned int g_4ced1c;

unsigned int sub_403020(unsigned int a0)
{
    void* idx;  // ebx
    char v24;  // al
    unsigned int v31;  // ecx
    unsigned int *iter;  // edi
    void* node;  // esi
    st_403020_0 *v61;  // ecx
    st_403020_0 *v34;  // ecx
    st_403020_0 *v35;  // edi
    st_403020_1 *v36;  // ecx
    void* iter1;  // esi, Other Possible Types: int, unsigned int
    unsigned int v25;  // 4112
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // 4208
    void* iter2;  // esi
    unsigned int v43;  // fpround
    int v44;  // edi
    unsigned int v45;  // ftop
    unsigned int v46;  // ecx
    unsigned int v47;  // ftop
    void* v26;  // esi
    unsigned int v49;  // ftop
    unsigned int v50;  // 4289
    unsigned long long v51;  // 4392
    unsigned int v52;  // 4413
    unsigned long long v53;  // 4496
    unsigned int v54;  // 4517
    unsigned long long v55;  // 4621
    unsigned int v56;  // ftop
    unsigned int v27;  // edi
    st_403020_0 *v57;  // ecx
    st_403020_0 *v59;  // ecx
    unsigned int i;  // edi
    unsigned int v29;  // ftop
    void* j;  // esi
    st_403020_0 *v0;  // [bp-0x8c]
    st_403020_0 *v1;  // [bp-0x88]
    st_403020_0 *v2;  // [bp-0x84]
    st_403020_0 *v3;  // [bp-0x80], Other Possible Types: st_403020_1 *, unsigned int
    st_403020_0 *v4;  // [bp-0x7c], Other Possible Types: st_403020_1 *, unsigned short
    unsigned int v5;  // [bp-0x78]
    unsigned int v6;  // [bp-0x78]
    unsigned int v7;  // [bp-0x74]
    unsigned int v8;  // [bp-0x70]
    st_403020_1 *v9;  // [bp-0x6c], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x68]
    st_403020_1 *v11;  // [bp-0x60], Other Possible Types: unsigned int
    unsigned long v12;  // [bp-0x5c], Other Possible Types: unsigned int
    st_403020_1 *v13;  // [bp-0x58], Other Possible Types: unsigned int
    unsigned int v14;  // [bp-0x54]
    unsigned int v15;  // [bp-0x50]
    unsigned int v16;  // [bp-0x4c]
    unsigned int v17;  // [bp-0x48]
    unsigned int v18;  // [bp-0x44]
    unsigned int v19;  // [bp-0x40]
    unsigned int v20;  // [bp-0x10]
    unsigned int v21;  // [bp-0xc]
    unsigned int v22;  // [bp-0x8]

    v24 = (int)idx[13756];
    if (v24 & 8)
        return 1;
    if (v24 & 4 && (int)idx[13764] >= 60)
        return 1;
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx[14068]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx[14072]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)&idx[14076]) = v20;
    *((unsigned int *)&idx[0x3700]) = v21;
    *((unsigned int *)&idx[14084]) = v22;
    D3DXVec3Normalize(idx + 13872, idx + 13848);
    v25 = (char)idx[13756] & 4;
    *((unsigned int *)&idx[10100]) = 0x808080;
    if (!(char)v25 || (int)idx[13764] < 30)
    {
        sub_404620(idx);
        sub_4049a0(idx);
        v26 = idx + 460;
        v27 = 8;
        do
        {
            i = v27;
            iter1 = v26;
            sub_455630(iter1);
            v26 = iter1 + 1204;
            v27 = i - 1;
        } while (i != 1);
        if ((int)idx[10104] != v27)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_455630(idx + 10132);
            sub_455630(idx + 11336);
            sub_455630(idx + 12540);
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
    }
    j = idx + 13836;
    v31 = 70;
    iter = &g_4ced1c;
    for (*((unsigned int *)&idx[10104]) = 0; v31; j += 4)
    {
        v31 -= 1;
        *(iter) = *((int *)j);
        iter += 1;
    }
    if ((int)idx[13788])
    {
        v14 = (int)idx[13804];
        v13 = *((int *)((int)idx[13788] + 20));
        v15 = (int)idx[13808];
        if ((int)idx[13812] == 1)
        {
            if (g_4b43cc && g_4b43cc->field_7c & 1)
                goto LABEL_4036dd;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_410020((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            node = *((int *)((int)idx[13788] + 16));
            v9 = NULL;
            if (*((int *)(int)idx[13788]) > NULL)
            {
                while (1)
                {
                    iter1 = 0;
                    if (*((int *)((int)idx[13788] + 4)) > NULL)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        sub_4938c0();
                        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        do
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            *((char *)&node[19]) = 192;
                            v35 = (int)idx[13788];
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v13 = v35->field_4 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            sub_4938c0();
                            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            if (v8)
                            {
                                v36 = v11;
                                if (v36 && v8 != v35->field_0 - 1 && v36 != v35->field_4 - 1)
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v36 = v9;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    *((int *)node) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    *((int *)&node[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    *((int *)&node[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    v36->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                }
                            }
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v4 = v36;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v8 += 12;
                            iter1 += 1;
                            node += 28;
                        } while (iter1 < *((int *)((int)idx[13788] + 4)));
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v4 = v34;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v34 = (int)idx[13788];
                    v7 += 1;
                    if (v7 >= v34->field_0)
                        break;
                }
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = v34;
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&idx[13804]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_4036ce;
        }
        else if ((int)idx[13812] == 2)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v39 = v29;
            v40 = v39 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v41 = _ccall(10, 13, (char)((((unsigned short)v39 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v41 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx[13796]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_410020((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            iter2 = *((int *)((int)idx[13788] + 16));
            v14 = 0;
            if (*((int *)(int)idx[13788]) > NULL)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                do
                {
                    v44 = 0;
                    if (*((int *)((int)idx[13788] + 4)) > NULL)
                    {
                        v45 = v40 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v6 = v5;
                        while (1)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v46 = v18;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v14 = v17;
                            v15 = v46;
                            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v16 = v19;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v47 = v45 - 2;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v49 = v47 + 1;
                            if (!((((unsigned short)v47 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                *((unsigned int *)&iter2[16]) = 0xffffffff;
                                v11 = 0xff - (char)iter2[18];
                                *((char *)&iter2[19]) = 96;
                                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v50 = _ccall(v43);
                                v4 = v50;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v11 = v50 & 0xffff | 0xc00;
                                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v11 = 0xff - (char)iter2[0x11];
                                *((char *)&iter2[18]) = v11;
                                v51 = _ccall(v50 & 0xffff);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v52 = _ccall((unsigned int)v51);
                                v4 = v52;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v11 = v52 & 0xffff | 0xc00;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v11 = 0xff - (char)iter2[16];
                                *((char *)&iter2[0x11]) = v11;
                                v53 = _ccall(v52 & 0xffff);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v54 = _ccall((unsigned int)v53);
                                v4 = v54;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v11 = v54 & 0xffff | 0xc00;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                *((char *)&iter2[16]) = v11;
                                v55 = _ccall(v54 & 0xffff);
                                v43 = v55;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                D3DXVec3Normalize(&v14, &v14);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v56 = v49 + 3;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                sub_4938c0(&v14, &v14);
                                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                sub_4938c0();
                                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                *((int *)iter2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                *((int *)&iter2[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                *((int *)&iter2[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                v3->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                            }
                            else
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                *((char *)&iter2[19]) = 0;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v56 = v49 + 3;
                            }
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v3 = v46;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v2 = v61;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v6 += 12;
                            v40 = v56 + 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v44 += 1;
                            iter2 += 28;
                            if (v44 >= *((int *)((int)idx[13788] + 4)))
                                break;
                            v45 = v40 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    }
                    v57 = (int)idx[13788];
                    iter1 += 1;
                } while (iter1 < v57->field_0);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = v57;
            /* unsupported instruction */
            /* unsupported instruction */
            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&idx[13804]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            iter1 = sub_464440((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            if (iter1 < NULL)
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            iter1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            iter1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            iter1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_4036ce:
            v0 = v59;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&idx[13808]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
    }
LABEL_4036dd:
    *((unsigned int *)&idx[13784]) = (int)idx[13784] + 1;
    return 1;
}
