// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4046c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4046c0_6 {
    char padding_0[20];
    unsigned int field_14;
    struct st_4046c0_4 *field_18;
    char padding_1c[428];
    unsigned int field_1c8;
    char padding_1cc[13284];
    unsigned int field_35b0;
    unsigned int field_35b4;
    unsigned int field_35b8;
} st_4046c0_6;

typedef struct st_4046c0_4 {
    unsigned short field_0;
    unsigned short field_2;
    char padding_4[12];
    char field_10;
} st_4046c0_4;

typedef struct st_4046c0_0 {
    char padding_0[188];
    struct st_4046c0_1 *field_bc;
    char padding_c0[36];
    struct st_4046c0_1 *field_e4;
} st_4046c0_0;

typedef struct st_4046c0_1 {
    unsigned int field_0;
} st_4046c0_1;

typedef struct st_4046c0_9 {
    char padding_0[2];
    char field_2;
    char field_3;
    char padding_4[26];
    unsigned short field_1e;
    char padding_20[2];
    unsigned short field_22;
    char padding_24[12];
    unsigned int field_30;
    unsigned int field_34;
} st_4046c0_9;

typedef struct st_4046c0_8 {
    char padding_0[64];
    unsigned int field_40;
    unsigned int field_44;
    char padding_48[1000];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[64];
    unsigned int field_47c;
} st_4046c0_8;

typedef struct st_4046c0_3 {
    struct st_4046c0_0 *field_0;
} st_4046c0_3;

extern char g_4b5644;
extern st_4046c0_4 *g_4ce8cc;
extern st_4046c0_3 *g_4ce8f0;
extern char g_4ced1c;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;
extern unsigned int g_4cf278;

unsigned int sub_4046c0(st_4046c0_6 *index, unsigned int a1)
{
    st_4046c0_4 *iter;  // esi
    int v11;  // edi
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    short i;  // ax
    int v12;  // esi
    short *v29;  // ebx
    st_4046c0_8 *idx;  // edi
    unsigned int v31;  // ecx
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    int v13;  // ebx
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // fc3210
    unsigned int v43;  // 4141
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v46;  // fc3210
    unsigned int v47;  // 4116
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // eax
    unsigned int v51;  // ecx
    unsigned int v14;  // ftop
    st_4046c0_9 *idx1;  // ebx
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    st_4046c0_6 *idx2;  // edi
    unsigned int v0;  // [bp-0x50]
    st_4046c0_4 *v1;  // [bp-0x4c]
    st_4046c0_4 *v2;  // [bp-0x4c]
    st_4046c0_4 *v3;  // [bp-0x3c]
    unsigned int v4;  // [bp-0x38]
    unsigned int v5;  // [bp-0x34]
    st_4046c0_4 *v6;  // [bp-0x24]
    unsigned int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0x18]
    unsigned int v9;  // [bp-0x14]

    iter = index->field_18;
    g_4cee34 = &g_4ced1c;
    sub_430a70(v11, v12, v13, iter, 1);
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 2;
    *((char *)(g_4ce8cc + &g_4b5644)) = 1;
    if (iter->field_0 < 0)
        return 0;
    do
    {
        idx1 = *((int *)(index->field_14 + iter->field_0 * 4));
        if (idx1->field_2 == a1)
        {
            v16 = v14 - 1;
            if (/* unsupported instruction */)
            {
                v17 = v16 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v17 = v16 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            idx2 = index;
            if (/* unsupported instruction */)
            {
                v7 = /* unsupported instruction */;
                /* unsupported instruction */
                v19 = v17 + 1;
            }
            else
            {
                v7 = nan;
                /* unsupported instruction */
                v19 = v17 + 1;
            }
            v5 = index->field_14;
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
                v8 = /* unsupported instruction */;
                /* unsupported instruction */
                v22 = v21 + 1;
            }
            else
            {
                v8 = nan;
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
                v9 = /* unsupported instruction */;
                /* unsupported instruction */
                v25 = v24 + 1;
            }
            else
            {
                v9 = nan;
                /* unsupported instruction */
                v25 = v24 + 1;
            }
            v26 = v25 - 1;
            if (/* unsupported instruction */)
            {
                v27 = v26 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v27 = v26 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v5 = /* unsupported instruction */;
                /* unsupported instruction */
                v14 = v27 + 1;
            }
            else
            {
                v5 = nan;
                /* unsupported instruction */
                v14 = v27 + 1;
            }
            if (sub_403f10())
            {
                idx2->field_35b4 = idx2->field_35b4 + 1;
                iter->field_2 = iter->field_2 & 0xfffe;
            }
            else
            {
                idx1->field_3 = idx1->field_3 | 2;
                i = *((short *)&idx1->padding_4[24]);
                v29 = &idx1->padding_4[24];
                v2 = v1;
                if (i >= 0)
                {
                    do
                    {
                        idx = v29[3] * 1204 + index->field_1c8;
                        if (i)
                            continue;
                        v31 = idx->field_47c;
                        if ((v31 >> 23 & 31) >= 4)
                        {
                            v32 = v14 - 1;
                            if (/* unsupported instruction */)
                            {
                                v33 = v32 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v33 = v32 - 1;
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
                                idx->field_430 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v34 = v33 + 1;
                            }
                            else
                            {
                                idx->field_430 = nan;
                                /* unsupported instruction */
                                v34 = v33 + 1;
                            }
                            v35 = v34 - 1;
                            if (/* unsupported instruction */)
                            {
                                v36 = v35 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v36 = v35 - 1;
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
                                idx->field_434 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v37 = v36 + 1;
                            }
                            else
                            {
                                idx->field_434 = nan;
                                /* unsupported instruction */
                                v37 = v36 + 1;
                            }
                            v38 = v37 - 1;
                            if (/* unsupported instruction */)
                            {
                                v39 = v38 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v39 = v38 - 1;
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
                                idx->field_438 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v40 = v39 + 1;
                            }
                            else
                            {
                                idx->field_438 = nan;
                                /* unsupported instruction */
                                v40 = v39 + 1;
                            }
                            v41 = v40 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v42 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&v29[10])) * 0x100 & 0x4500;
                            v43 = _ccall(10, 13, (char)((((unsigned short)v41 & 7) * 0x800 | (unsigned short)v42 & 0x4700) >> 8) & 0x44, 0, 0);
                            if (v43 & 1)
                            {
                                v44 = v41 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v45 = v44 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v45 = v44 - 1;
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
                                idx->field_47c = v31 | 8;
                                if (/* unsupported instruction */)
                                {
                                    idx->field_40 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v41 = v45 + 1;
                                }
                                else
                                {
                                    idx->field_40 = nan;
                                    /* unsupported instruction */
                                    v41 = v45 + 1;
                                }
                            }
                            if (/* unsupported instruction */)
                            {
                                v46 = CmpF(/* unsupported instruction */, *((int *)&v29[12])) * 0x100 & 0x4500;
                                /* unsupported instruction */
                                v14 = v41 + 1;
                            }
                            else
                            {
                                v46 = CmpF(nan, *((int *)&v29[12])) * 0x100 & 0x4500;
                                /* unsupported instruction */
                                v14 = v41 + 1;
                            }
                            v47 = _ccall(10, 13, (char)((((unsigned short)v14 & 7) * 0x800 | (unsigned short)v46 & 0x4700) >> 8) & 0x44, 0, 0);
                            if (v47 & 1)
                            {
                                v48 = v14 - 1;
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
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                idx->field_47c = idx->field_47c | 8;
                                if (/* unsupported instruction */)
                                {
                                    idx->field_44 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v14 = v49 + 1;
                                }
                                else
                                {
                                    idx->field_44 = nan;
                                    /* unsupported instruction */
                                    v14 = v49 + 1;
                                }
                            }
                        }
                        if ((idx->field_47c & 0xf800000) == 0x4000000)
                        {
                            if (g_4cf278 == 1)
                                goto LABEL_4048a8;
                            sub_45a3c0();
                            g_4cf278 = 1;
                            v0 = 1;
                        }
                        else
                        {
                            if (!g_4cf278)
                                goto LABEL_4048a8;
                            sub_45a3c0();
                            g_4cf278 = 0;
                            v0 = 0;
                        }
                        g_4ce8f0->field_0->field_e4(g_4ce8f0, 28);
                        iter = v2;
LABEL_4048a8:
                        v50 = idx->field_47c >> 12;
                        if (v50 & 1 && g_4ce8f0)
                        {
                            sub_45a3c0();
                            g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
                            v4 = 0;
                        }
                        else
                        {
                            if (v50 & 1 || g_4ce8f0)
                                goto LABEL_40491b;
                            sub_45a3c0();
                            g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 1);
                            v4 = 1;
                        }
                        iter = v3;
LABEL_40491b:
                        v2 = g_4ce8cc;
                        sub_45c900(g_4ce8cc);
                        index->field_35b8 = index->field_35b8 + 1;
                        v51 = v29[1];
                        i = *((short *)((char *)v29 + v51));
                        v29 = (char *)v29 + v51;
                    } while (i >= 0);
                    idx2 = index;
                }
                iter->field_2 = iter->field_2 | 1;
                idx2->field_35b0 = idx2->field_35b0 + 1;
            }
        }
    } while ((iter += 16, v6 = iter, iter->field_0 >= 0));
    if (v12)
        return 0;
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 1);
    return 0;
}
