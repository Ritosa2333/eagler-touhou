// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x446c20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_446c20_2 {
    char padding_0[184];
    unsigned int field_b8;
    char padding_bc[32];
    struct st_446c20_3 *field_dc;
} st_446c20_2;

typedef struct st_446c20_0 {
    char padding_0[36];
    unsigned int field_24;
    unsigned int field_28;
    char padding_2c[428];
    unsigned int field_1d8;
    char padding_1dc[220];
    int field_2b8;
    char padding_2bc[22460];
    int field_5a78;
} st_446c20_0;

typedef struct st_446c20_6 {
    char padding_0[23168];
    struct st_446c20_5 *field_5a80;
} st_446c20_6;

typedef struct st_446c20_1 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    int field_14;
} st_446c20_1;

typedef struct st_446c20_5 {
    char padding_0[28];
    struct st_446c20_4 *field_1c;
    char padding_20[455];
    unsigned int field_1e7;
} st_446c20_5;

typedef struct st_446c20_3 {
    char padding_0[12];
    unsigned int field_c;
    char padding_10[40];
    unsigned int field_38;
} st_446c20_3;

typedef struct st_446c20_4 {
    char padding_0[12];
    char field_c;
    char padding_d[79];
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
} st_446c20_4;

typedef struct st_446c20_8 {
    char padding_0[12];
    char field_c;
    char padding_d[7];
    unsigned int field_14;
    char padding_18[68];
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
} st_446c20_8;

extern unsigned int g_4aee20[4];
extern unsigned int g_4aee38[4];
extern unsigned int g_4aee60[4];
extern unsigned int g_4aee88[4];
extern unsigned int g_4b33b0[4];
extern unsigned int g_4b33b8;
extern unsigned int g_4b43b8;
extern unsigned int g_4d4f30;
extern char g_4d4f34;

unsigned int sub_446c20(void)
{
    st_446c20_0 *idx;  // edi
    unsigned int v18;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    st_446c20_1 *idx1;  // eax
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int *v34;  // edx
    unsigned int v35;  // ftop
    unsigned int v36;  // ecx
    unsigned int v19;  // ftop
    unsigned int v37;  // edx
    unsigned int v38;  // ecx
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    int v43;  // ebp
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v20;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    st_446c20_2 *v48;  // eax
    unsigned int *idx2;  // eax
    unsigned int v50;  // ecx
    unsigned int v51;  // edx
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // esi
    unsigned int v55;  // ftop
    unsigned int v21;  // ftop
    unsigned int v56;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // ebp
    unsigned int v62;  // ftop
    st_446c20_6 *v63;  // ebx
    unsigned int v64;  // edx
    st_446c20_4 *v65;  // esi
    unsigned int v22;  // ftop
    st_446c20_1 *index;  // eax
    unsigned int v67;  // ftop
    unsigned int v68;  // ftop
    unsigned int v69;  // ecx
    unsigned int v70;  // edx
    unsigned int v71;  // ecx
    unsigned int v72;  // edx
    unsigned int v73;  // ftop
    unsigned int v74;  // ftop
    unsigned int v23;  // ftop
    unsigned int v75;  // ftop
    unsigned int v76;  // ftop
    st_446c20_8 *v24;  // esi
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v0;  // [bp-0x58]
    st_446c20_4 *v1;  // [bp-0x50], Other Possible Types: st_446c20_8 *
    int v2;  // [bp-0x4c]
    unsigned int v3;  // [bp-0x48]
    unsigned int v4;  // [bp-0x44]
    unsigned int v5;  // [bp-0x40]
    unsigned int v6;  // [bp-0x3c]
    unsigned int *v7;  // [bp-0x38]
    unsigned int *v8;  // [bp-0x34]
    double v9;  // [bp-0x2c]
    unsigned int iter;  // [bp-0x18]
    st_446c20_6 *v11;  // [bp-0x14], Other Possible Types: unsigned int
    st_446c20_8 *v12;  // [bp-0x10]
    unsigned int v13;  // [bp-0xc]
    unsigned int v14;  // [bp-0x8]
    unsigned int v15;  // [bp-0x4]

    if (idx->field_24 == 2)
    {
        v54 = g_4b43b8;
        v56 = v55 - 1;
        if (/* unsupported instruction */)
        {
            v57 = v56 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v57 = v56 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *((unsigned int *)(g_4b43b8 + 102292)) = 1;
        if (/* unsupported instruction */)
        {
            v13 = /* unsupported instruction */;
            /* unsupported instruction */
            v58 = v57 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v58 = v57 + 1;
        }
        v59 = v58 - 1;
        if (/* unsupported instruction */)
        {
            v60 = v59 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v60 = v59 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v61 = idx->field_1d8 * 25;
        if (/* unsupported instruction */)
        {
            v14 = /* unsupported instruction */;
            /* unsupported instruction */
            v62 = v60 + 1;
        }
        else
        {
            v14 = nan;
            /* unsupported instruction */
            v62 = v60 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (v61 < v61 + 25)
        {
            v63 = &idx->padding_0[4 * v61 + 23168];
            iter = v61 + 1;
            v11 = v63;
            while (1)
            {
                v64 = 1374389535 * v61 >> 35;
                *((unsigned int *)(v54 + 102272)) = (-(0 < idx->field_28 - (v61 - ((v64 >> 31) + v64) * 25)) & 0xff808180) - 0x100;
                if (*((int *)&v63->padding_0[0]))
                {
                    v65 = *((int *)(*((int *)&v63->padding_0[0]) + 28));
                    index = sub_46d8e4(&v65->field_c);
                    if (!idx->field_1d8)
                    {
                        v67 = v62 - 1;
                        if (/* unsupported instruction */)
                        {
                            v68 = v67 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v68 = v67 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        v34 = g_4aee88[v65->field_68];
                        if (/* unsupported instruction */)
                        {
                            v9 = (double)/* unsupported instruction */;
                            /* unsupported instruction */
                            v62 = v68 + 1;
                        }
                        else
                        {
                            v9 = (double)nan;
                            /* unsupported instruction */
                            v62 = v68 + 1;
                        }
                        v34 = g_4aee88[v65->field_68];
                        v8 = g_4aee38[v65->field_64];
                        v69 = index->field_4;
                        v7 = g_4aee20[2 * v65->field_5c + v65->field_60];
                        v70 = index->field_8;
                        v6 = index->field_4;
                        v71 = index->field_c;
                        v5 = index->field_8;
                        v4 = index->field_c;
                        v3 = index->field_10 + 1;
                        v2 = index->field_14 % 100;
                        v72 = iter;
                        v1 = v65;
                        v72 = iter;
                        v0 = g_4b33b0;
                    }
                    else
                    {
                        g_4d4f30 = *((int *)(*((int *)&v63->padding_0[0]) + 487));
                        g_4d4f34 = 0;
                        v73 = v62 - 1;
                        if (/* unsupported instruction */)
                        {
                            v74 = v73 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v74 = v73 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                        {
                            v9 = (double)/* unsupported instruction */;
                            /* unsupported instruction */
                            v62 = v74 + 1;
                        }
                        else
                        {
                            v9 = (double)nan;
                            /* unsupported instruction */
                            v62 = v74 + 1;
                        }
                        v34 = g_4aee88[v65->field_68];
                        v8 = g_4aee38[v65->field_64];
                        v69 = index->field_4;
                        v7 = g_4aee20[2 * v65->field_5c + v65->field_60];
                        v70 = index->field_8;
                        v6 = index->field_4;
                        v71 = index->field_c;
                        v5 = index->field_8;
                        v4 = index->field_c;
                        v3 = index->field_10 + 1;
                        v2 = index->field_14 % 100;
                        v1 = v65;
                        v72 = &g_4d4f30;
                        v0 = g_4b33b8;
                    }
                    sub_4015c0(v0, v72, v65, v2, v3, v71, v70, v69, v7, v8, v34, *((unsigned int *)&v9));
                }
                else
                {
                    sub_4015c0(g_4b33b0[1 + 2 * (idx->field_1d8)], iter);
                }
                v75 = v62 - 1;
                if (/* unsupported instruction */)
                {
                    v76 = v75 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v76 = v75 - 1;
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
                v11 = &v11->padding_0[4];
                iter += 1;
                v54 = g_4b43b8;
                if (/* unsupported instruction */)
                {
                    v14 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v62 = v76 + 1;
                }
                else
                {
                    v14 = nan;
                    /* unsupported instruction */
                    v62 = v76 + 1;
                }
                v61 += 1;
                if (v61 >= (idx->field_1d8 + 1) * 25)
                    break;
                v63 = v11;
            }
        }
        *((unsigned int *)(v54 + 102272)) = 0xffffffff;
        *((unsigned int *)(v54 + 102292)) = 0;
        return 1;
    }
    else if (idx->field_24 == 4)
    {
        v19 = v18 - 1;
        if (/* unsupported instruction */)
        {
            v20 = v19 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v20 = v19 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v13 = /* unsupported instruction */;
            /* unsupported instruction */
            v21 = v20 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v21 = v20 + 1;
        }
        v22 = v21 - 1;
        if (/* unsupported instruction */)
        {
            v23 = v22 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v23 = v22 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v24 = *((int *)(*((int *)&idx[1].padding_0[4 + 4 * idx->field_5a78]) + 28));
        if (/* unsupported instruction */)
        {
            v14 = /* unsupported instruction */;
            /* unsupported instruction */
            v25 = v23 + 1;
        }
        else
        {
            v14 = nan;
            /* unsupported instruction */
            v25 = v23 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v26 = v25;
        v12 = v24;
        if (idx->field_2b8 < 10)
        {
            v27 = v26 - 1;
            if (/* unsupported instruction */)
            {
                v28 = v27 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v28 = v27 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v29 = v28 - 1;
            if (/* unsupported instruction */)
            {
                v30 = v29 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v30 = v29 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v11 = (int)(idx->field_5a78 % 25) * 15;
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
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v26 = v30 + 2;
        }
        *((unsigned int *)(g_4b43b8 + 102292)) = 1;
        idx1 = sub_46d8e4(&v24->field_c);
        if (!idx->field_1d8)
        {
            v32 = v26 - 1;
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
            v34 = g_4aee88[v24->field_68];
            if (/* unsupported instruction */)
            {
                v9 = (double)/* unsupported instruction */;
                /* unsupported instruction */
                v35 = v33 + 1;
            }
            else
            {
                v9 = (double)nan;
                /* unsupported instruction */
                v35 = v33 + 1;
            }
            v34 = g_4aee88[v24->field_68];
            v8 = g_4aee38[v24->field_64];
            v36 = idx1->field_4;
            v7 = g_4aee20[2 * v24->field_5c + v24->field_60];
            v37 = idx1->field_8;
            v6 = idx1->field_4;
            v38 = idx1->field_c;
            v5 = idx1->field_8;
            v4 = idx1->field_c;
            v3 = idx1->field_10 + 1;
            v2 = idx1->field_14 % 100;
            v1 = v24;
            v72 = idx->field_5a78 + 1;
            v0 = g_4b33b0;
        }
        else
        {
            g_4d4f30 = *((int *)(*((int *)&idx[1].padding_0[4 + 4 * idx->field_5a78]) + 487));
            g_4d4f34 = 0;
            v39 = v26 - 1;
            if (/* unsupported instruction */)
            {
                v40 = v39 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v40 = v39 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v9 = (double)/* unsupported instruction */;
                /* unsupported instruction */
                v35 = v40 + 1;
            }
            else
            {
                v9 = (double)nan;
                /* unsupported instruction */
                v35 = v40 + 1;
            }
            v34 = g_4aee88[v24->field_68];
            v8 = g_4aee38[v24->field_64];
            v36 = idx1->field_4;
            v7 = g_4aee20[2 * v24->field_5c + v24->field_60];
            v37 = idx1->field_8;
            v6 = idx1->field_4;
            v38 = idx1->field_c;
            v5 = idx1->field_8;
            v4 = idx1->field_c;
            v3 = idx1->field_10 + 1;
            v2 = idx1->field_14 % 100;
            v1 = v24;
            v72 = &g_4d4f30;
            v0 = g_4b33b8;
        }
        sub_4015c0(v0, v72, v24, v2, v3, v38, v37, v36, v7, v8, v34, *((unsigned int *)&v9));
        if (idx->field_2b8 >= 10)
        {
            v41 = v35 - 1;
            if (/* unsupported instruction */)
            {
                v42 = v41 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v42 = v41 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v43 = 1;
            if (/* unsupported instruction */)
            {
                v13 = /* unsupported instruction */;
                /* unsupported instruction */
                v44 = v42 + 1;
            }
            else
            {
                v13 = nan;
                /* unsupported instruction */
                v44 = v42 + 1;
            }
            iter = 36;
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
                v14 = /* unsupported instruction */;
                /* unsupported instruction */
                v47 = v46 + 1;
            }
            else
            {
                v14 = nan;
                /* unsupported instruction */
                v47 = v46 + 1;
            }
            while (1)
            {
                *((unsigned int *)(g_4b43b8 + 102272)) = (-(0 < idx->field_28 - (v43 - 1)) & 0xff808180) - 0x100;
                v48 = *((int *)&idx[1].padding_0[4 + 4 * idx->field_5a78]) + iter;
                if (!v48->field_b8)
                {
                    sub_4015c0("%s  ---------", g_4aee60[v43]);
                }
                else
                {
                    if (v43 < 6 && !(idx2 = (unsigned int *)v48->field_dc, !idx2))
                    {
                        v50 = idx2[14];
                        v51 = idx2[3];
                    }
                    else
                    {
                        v50 = v12->field_6c;
                        v51 = v12->field_14;
                    }
                    sub_4015c0("%s  %.8d%d", g_4aee60[v43], v51, v50);
                }
                v52 = v47 - 1;
                if (/* unsupported instruction */)
                {
                    v53 = v52 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v53 = v52 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                iter += 36;
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
                v43 += 1;
                if (/* unsupported instruction */)
                {
                    v14 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v47 = v53 + 1;
                    if (v43 >= 8)
                        break;
                }
                else
                {
                    v14 = nan;
                    /* unsupported instruction */
                    v47 = v53 + 1;
                    if (v43 >= 8)
                        break;
                }
            }
        }
        *((unsigned int *)(g_4b43b8 + 102272)) = 0xffffffff;
        *((unsigned int *)(g_4b43b8 + 102292)) = 0;
        return 1;
    }
    else
    {
        return 1;
    }
}
