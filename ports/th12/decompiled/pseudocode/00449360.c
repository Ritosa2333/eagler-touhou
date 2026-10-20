// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x449360; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_449360_12 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    unsigned int field_28;
    struct st_449360_13 *field_2c;
    struct st_449360_13 *field_30;
    char padding_34[640];
    unsigned int field_2b4;
    int field_2b8;
    unsigned int field_2bc;
    char padding_2c0[4];
    unsigned int field_2c4;
    char padding_2c8[932];
    struct st_449360_12 *field_66c;
    char padding_670[64];
    int field_6b0;
    char padding_6b4[76];
    int field_700;
    char padding_704[32];
    struct st_449360_13 *field_724;
    int field_728;
    unsigned int field_72c;
    unsigned int field_730;
    char padding_734[21056];
    struct st_449360_13 *field_5974;
    char padding_5978[664];
    int field_5c10;
} st_449360_12;

typedef struct st_449360_13 {
    struct st_449360_0 *field_0;
} st_449360_13;

typedef struct st_449360_1 {
    unsigned int field_0;
} st_449360_1;

typedef struct st_449360_19 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[94];
    unsigned int field_424;
    unsigned int field_428;
    unsigned int field_42c;
    char padding_430[76];
    unsigned int field_47c;
    char padding_480[20];
    struct st_449360_1 *field_494;
} st_449360_19;

typedef struct st_449360_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[182];
    unsigned int field_47c;
    char padding_480[20];
    struct st_449360_1 *field_494;
} st_449360_0;

typedef struct st_449360_18 {
    char padding_0[125402];
    char field_1e9da;
} st_449360_18;

typedef struct st_449360_17 {
    char padding_0[102324];
    int field_18fb4;
} st_449360_17;

extern int g_4a20a8;
extern char g_4b2ed0;
extern unsigned int g_4b33c0[4];
extern st_449360_17 *g_4b43b8;
extern st_449360_18 *g_4b451c;
extern unsigned int g_4ce8cc;
extern char g_4ceae8;
extern int g_4cee70;
extern char g_4d48c0;
extern void g_4d48c4;

unsigned int sub_449360(st_449360_12 *a0)
{
    unsigned int v30;  // ebx
    st_449360_12 *idx;  // ebx
    unsigned int v40;  // eax
    int v41;  // esi
    st_449360_12 *iter;  // edi
    unsigned int v43;  // eax
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v32;  // esi
    unsigned int v50;  // eax
    unsigned int v51;  // ecx
    unsigned int v52;  // ftop
    unsigned int v53;  // eax
    unsigned int v54;  // ecx
    st_449360_0 *idx1;  // esi
    unsigned int v56;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v33;  // edi
    unsigned int v60;  // ftop
    unsigned int v61;  // ftop
    unsigned int v62;  // eax
    unsigned int v63;  // eax
    unsigned int v64;  // 4101
    unsigned int v65;  // ecx
    unsigned int v66;  // 4101
    st_449360_12 *v67;  // esi
    unsigned int v68;  // eax
    unsigned int v69;  // ftop
    st_449360_12 *v34;  // ecx
    unsigned int v70;  // ftop
    unsigned int v71;  // ftop
    st_449360_0 **v72;  // edi
    unsigned int v73;  // ftop
    unsigned int v74;  // ftop
    unsigned int v75;  // ftop
    unsigned int v76;  // ftop
    st_449360_19 *idx2;  // esi
    unsigned int v78;  // ftop
    unsigned int v79;  // ftop
    st_449360_0 **v35;  // eax
    unsigned int v80;  // ftop
    unsigned int v81;  // ftop
    unsigned int v82;  // ftop
    unsigned int v83;  // ftop
    unsigned int v84;  // ftop
    unsigned int v86;  // ftop
    unsigned int v87;  // 4172
    unsigned int v88;  // ftop
    unsigned int v89;  // ftop
    unsigned int v36;  // 4101
    unsigned int v90;  // ftop
    unsigned int v91;  // ftop
    st_449360_12 *v92;  // esi
    unsigned int v93;  // edi
    unsigned int v94;  // edi
    st_449360_12 *index;  // esi
    unsigned int v96;  // eax
    st_449360_0 **v97;  // esi
    st_449360_12 *v98;  // edi
    st_449360_12 *v99;  // esi
    unsigned int v37;  // eax
    unsigned int v100;  // edi
    unsigned int v101;  // edi
    st_449360_12 *v102;  // eax
    unsigned int v103;  // ecx
    st_449360_12 *v38;  // esi
    char *v39;  // ecx
    unsigned int v0;  // [bp-0xa8]
    unsigned int v1;  // [bp-0xa0]
    unsigned int v2;  // [bp-0x94]
    st_449360_0 *v3;  // [bp-0x90]
    int v4;  // [bp-0x8c]
    st_449360_0 **v5;  // [bp-0x88]
    unsigned int v6;  // [bp-0x84]
    unsigned int v7;  // [bp-0x80]
    unsigned int v8;  // [bp-0x70]
    unsigned int v9;  // [bp-0x64]
    unsigned int v10;  // [bp-0x60]
    int v11;  // [bp-0x5c], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x58]
    unsigned int v13;  // [bp-0x54]
    int v14;  // [bp-0x50], Other Possible Types: unsigned int
    char *v15;  // [bp-0x4c]
    int v16;  // [bp-0x48]
    int v17;  // [bp-0x44], Other Possible Types: unsigned int
    st_449360_0 **v18;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int *node;  // [bp-0x3c], Other Possible Types: int, unsigned int
    int v20;  // [bp-0x38], Other Possible Types: unsigned int
    st_449360_12 *v21;  // [bp-0x34]
    unsigned int v22;  // [bp-0x2c]
    unsigned int v23;  // [bp-0x28]
    st_449360_12 *v24;  // [bp-0x24], Other Possible Types: unsigned int
    unsigned int v25;  // [bp-0x20]
    unsigned int v26;  // [bp-0x1c]
    unsigned int v27;  // [bp-0x18]
    char v28;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v29;  // [bp-0x10]

    v22 = v30;
    idx = a0;
    *((unsigned int *)&(&v20)[8]) = v32;
    *((unsigned int *)&(&v20)[4]) = v33;
    switch (idx->field_24)
    {
    case 0:
        if (idx->field_2b8 == 1)
        {
            v34 = &idx->field_28;
            idx->field_30 = 0x6;
            v35 = *((int *)&v34->padding_0[8]);
            v24 = v34;
            if (v35)
            {
                v36 = _ccall(14, 15, v35, 0, 0);
                if (v36 & 1)
                {
                    *((st_449360_0 ***)&v24->padding_0[0]) = (char *)v35 - 1;
                    break;
                }
                else
                {
                    *((unsigned int *)&v24->padding_0[0]) = 0;
                    break;
                }
            }
            else
            {
                *((unsigned int *)&v24->padding_0[0]) = 0;
                break;
            }
            if (!idx->field_66c)
            {
                *((unsigned int *)&v20) = 0;
                node = 20;
                v18 = &v28;
                sub_4615a0(g_4b43b8->field_18fb4, &v28, 20, *((unsigned int *)&v20));
                idx->field_66c = v24;
            }
            sub_43efa0();
            v16 = 0;
            v15 = &v20;
            v23 = 0;
            v37 = sub_463c10(&v20, 0);
            idx->field_5c10 = v37;
            if (!v37)
                goto LABEL_449c27;
            if (v18 > 0)
            {
                v38 = &idx->padding_734[4160];
                v21 = &idx->padding_734[0x800];
                *((st_449360_12 **)&v20) = idx->padding_734;
                do
                {
                    if (*((char *)v37) != 35 && *((char *)v37) == 64)
                    {
                        sub_449dc0(v16);
                        sub_449dc0(v16);
                        v17 = 8;
                        do
                        {
                            v37 = sub_449dc0(v38);
                        } while ((v38 += 66, node -= 1, node != 1));
                        v18 = (char *)v18 + 1;
                        v16 += 64;
                        v17 = 74;
                    }
                    else
                    {
                        v37 = sub_449d80();
                    }
                } while (v14 > 0);
            }
            v39 = &v20;
            idx->field_30 = v18;
            v40 = (int)(&v20)[8];
            v41 = 0;
            if (!v40)
            {
                v20 = 0;
            }
            else if (v40 <= 0)
            {
                v20 = v40 - 1;
            }
            else
            {
                v20 = 0;
            }
            idx->field_5974 = NULL;
            idx->field_724 = v18;
            iter = &idx->field_700;
            do
            {
                sub_4615a0(g_4cee70, &v15, v41 + 41, 0);
            } while ((*((int *)&iter->padding_0[0]) = v11, v41 = (int)(v41 + 1), iter += 4, v41 < 8));
            memset(&idx->field_728, 0, 12);
        }
        v43 = idx->field_2b8;
        if (v43 < 10)
        {
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
                v12 = /* unsupported instruction */;
                /* unsupported instruction */
                v47 = v46 + 1;
            }
            else
            {
                v12 = nan;
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
            v11 = v43 * 40 - 40;
            v50 = v43 * 2;
            v51 = v50 - 2;
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
            v8 = v51;
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
                v13 = /* unsupported instruction */;
                /* unsupported instruction */
                v52 = v49 + 1;
            }
            else
            {
                v13 = nan;
                /* unsupported instruction */
                v52 = v49 + 1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            if (v51 < v50)
            {
                v53 = v51;
                v9 = &idx->padding_734[0x800 + 64 * v51 + 2 * v53];
                v10 = &idx->padding_0[4 * v53 + 1712];
                do
                {
                    if (v5 >= idx->field_724)
                        break;
                    sub_4615a0(idx->field_14, &v7, (st_449360_0 **)((char *)&v5[52] + 3), 0);
                    *(v5) = v3;
                    idx1 = sub_461920(v54, g_4ce8cc, v3);
                    if (!idx1)
                        *(v5) = idx1;
                    if (g_4b451c->field_1e9da)
                    {
                        sub_460760(0xffffff, 0, 0, 0, v4);
                    }
                    else
                    {
                        v0 = 1;
                        sub_460760(0xffffff, 0, 0, 0, &g_4a20a8);
                    }
                    if (0 >= idx->field_5974 && 0 < (char *)&idx->field_5974[2].field_0 + 2)
                        idx1->field_47c = idx1->field_47c | 2;
                    else
                        idx1->field_47c = idx1->field_47c & 0xfffffffd;
                    if (!idx->field_28)
                    {
                        v56 = v52 - 1;
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
                            v7 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v52 = v57 + 1;
                        }
                        else
                        {
                            v7 = nan;
                            /* unsupported instruction */
                            v52 = v57 + 1;
                        }
                    }
                    sub_427b90(4, 0);
                    if (!idx->field_28)
                    {
                        v58 = v52 - 1;
                        if (/* unsupported instruction */)
                        {
                            v59 = v58 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v59 = v58 - 1;
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
                            v5 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v52 = v59 + 1;
                        }
                        else
                        {
                            v5 = (unsigned int)nan;
                            /* unsupported instruction */
                            v52 = v59 + 1;
                        }
                    }
                    v60 = v52 - 1;
                    if (/* unsupported instruction */)
                    {
                        v61 = v60 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v61 = v60 - 1;
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
                        v6 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v52 = v61 + 1;
                    }
                    else
                    {
                        v6 = nan;
                        /* unsupported instruction */
                        v52 = v61 + 1;
                    }
                    v4 = (unsigned short)(unsigned int)((NULL != idx->field_28) + 2);
                    if (idx1->field_494)
                        idx1->field_494();
                    v3 = &v3->padding_0[4];
                    v2 += 66;
                    idx1->field_3c4 = v4;
                    v1 = 1;
                } while (1 < idx->field_2b8 * 2);
            }
        }
        if (idx->field_2b8 < 10)
            return 0;
        idx->field_24 = 1;
        v62 = idx->field_2c4;
        if (!((char)idx->field_2c4 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_2bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx->field_2b8 = 0;
            idx->field_2b4 = 0xfff0bdc1;
            *((char **)&idx->padding_2c0[0]) = &g_4b2ed0;
            idx->field_2c4 = v62 | 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_2b8 = 0;
        idx->field_2bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_2b4 = 0xffffffff;
        return 0;
    case 1:
        v63 = idx->field_2b8;
        if ((v63 & 0x80000001) < 0)
        {
            v64 = _ccall(4, 18, ((v63 & 0x80000001) - 1 | 0xfffffffe) + 1, 0, 0);
            if (v64 & 1)
                goto LABEL_44977c;
        }
        else if (!(v63 & 0x80000001))
        {
LABEL_44977c:
            if (idx->field_728 < 8)
            {
                sub_461c50();
                if (!(&g_4b451c->field_1e9da)[idx->field_72c] && idx->field_730)
                {
                    *((unsigned int *)&v20) = g_4b33c0[idx->field_728];
                    node = 0;
                    v18 = 0;
                    v17 = 0;
                    v16 = 0x8080ff;
                }
                else
                {
                    *((st_449360_12 **)&v20) = &idx->padding_734[4160 + 528 * idx->field_72c + 66 * idx->field_728];
                    node = 0;
                    v18 = 0;
                    v17 = 0;
                    v16 = 0xffffff;
                }
                sub_460760(v16, v17, v18, node, *((unsigned int *)&v20));
                sub_40d6e0();
                idx->field_728 = idx->field_728 + 1;
            }
        }
        if (idx->field_2b8 <= 4)
            return 0;
        sub_43ef40();
        return 0;
    case 2:
        v65 = idx->field_2b8;
        if ((v65 & 0x80000001) < 0)
        {
            v66 = _ccall(4, 18, ((v65 & 0x80000001) - 1 | 0xfffffffe) + 1, 0, 0);
            if (v66 & 1)
                goto LABEL_44984d;
        }
        else if (!(v65 & 0x80000001))
        {
LABEL_44984d:
            if (idx->field_728 < 8)
            {
                sub_461c50();
                if (!(&g_4b451c->field_1e9da)[idx->field_72c] && idx->field_730)
                {
                    *((unsigned int *)&v20) = g_4b33c0[idx->field_728];
                    node = NULL;
                    v18 = 0;
                    v17 = 0;
                    v16 = 0x8080ff;
                }
                else
                {
                    *((st_449360_12 **)&v20) = &idx->padding_734[4160 + 528 * idx->field_72c + 66 * idx->field_728];
                    node = NULL;
                    v18 = 0;
                    v17 = 0;
                    v16 = 0xffffff;
                }
                sub_460760(v16, v17, v18, node, *((unsigned int *)&v20));
                sub_40d6e0();
                idx->field_728 = idx->field_728 + 1;
            }
        }
        v67 = &idx->field_28;
        *((unsigned int *)&v67->padding_0[4]) = idx->field_28;
        v24 = v67;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
        {
            *((unsigned int *)&v20) = 0xffffffff;
            sub_464970(*((unsigned int *)&v20));
        }
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
        {
            node = 0x1;
            sub_464970(1);
        }
        if (*((int *)&v24->padding_0[4]) != *((int *)&v24->padding_0[0]))
        {
            sub_453d90();
            v68 = *((int *)&v24->padding_0[0]);
            if (v68 >= idx->field_5974)
            {
                if (v68 < (char *)&idx->field_5974[2].field_0 + 2)
                    goto LABEL_449957;
                v68 -= 9;
            }
            idx->field_5974 = v68;
LABEL_449957:
            v70 = v69 - 1;
            if (/* unsupported instruction */)
            {
                v71 = v70 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v71 = v70 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v72 = NULL;
            if (/* unsupported instruction */)
            {
                v27 = /* unsupported instruction */;
                /* unsupported instruction */
                v73 = v71 + 1;
            }
            else
            {
                v27 = nan;
                /* unsupported instruction */
                v73 = v71 + 1;
            }
            v74 = v73 - 1;
            if (/* unsupported instruction */)
            {
                v75 = v74 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v75 = v74 - 1;
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
                v28 = /* unsupported instruction */;
                /* unsupported instruction */
                v76 = v75 + 1;
            }
            else
            {
                v28 = nan;
                /* unsupported instruction */
                v76 = v75 + 1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v29 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            if (idx->field_724 > 0)
            {
                *((st_449360_12 **)&(&v20)[8]) = &idx->field_6b0;
                do
                {
                    idx2 = sub_461920(*((unsigned int *)&v20), g_4ce8cc, *((int *)*((unsigned int *)&v20)));
                    if (!idx2)
                        *((unsigned int *)*((unsigned int *)&v20)) = 0;
                    if (v72 >= idx->field_5974 && v72 < (char *)&idx->field_5974[2].field_0 + 2)
                        idx2->field_47c = idx2->field_47c | 2;
                    else
                        idx2->field_47c = idx2->field_47c & 0xfffffffd;
                    if (v72 == *((int *)*((unsigned int *)(&v20 + 4))))
                    {
                        v78 = v76 - 1;
                        if (/* unsupported instruction */)
                        {
                            v79 = v78 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v79 = v78 - 1;
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
                            v25 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v76 = v79 + 1;
                        }
                        else
                        {
                            v25 = nan;
                            /* unsupported instruction */
                            v76 = v79 + 1;
                        }
                    }
                    v80 = v76 - 1;
                    if (/* unsupported instruction */)
                    {
                        v81 = v80 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v81 = v80 - 1;
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
                        v24 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v82 = v81 + 1;
                    }
                    else
                    {
                        v24 = nan;
                        /* unsupported instruction */
                        v82 = v81 + 1;
                    }
                    v83 = v82 - 1;
                    if (/* unsupported instruction */)
                    {
                        v84 = v83 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v84 = v83 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v86 = v84 + 1;
                    v87 = _ccall(10, 13, (char)((((unsigned short)v86 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x42200000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                    if (!(v87 & 1))
                    {
                        sub_427b90(4, 0);
                    }
                    else
                    {
                        idx2->field_424 = v25;
                        idx2->field_428 = v26;
                        idx2->field_42c = v27;
                    }
                    if (v72 == *((int *)*((unsigned int *)(&v20 + 4))))
                    {
                        v88 = v86 - 1;
                        if (/* unsupported instruction */)
                        {
                            v89 = v88 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v89 = v88 - 1;
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
                            v25 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v86 = v89 + 1;
                        }
                        else
                        {
                            v25 = nan;
                            /* unsupported instruction */
                            v86 = v89 + 1;
                        }
                    }
                    v90 = v86 - 1;
                    if (/* unsupported instruction */)
                    {
                        v91 = v90 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v91 = v90 - 1;
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
                        v26 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v76 = v91 + 1;
                    }
                    else
                    {
                        v26 = nan;
                        /* unsupported instruction */
                        v76 = v91 + 1;
                    }
                    v24 = (unsigned short)(unsigned int)((v72 != *((int *)*((unsigned int *)(&v20 + 4)))) + 2);
                    if (idx2->field_494)
                        idx2->field_494();
                    *((unsigned int *)&v20) = *((unsigned int *)&v20) + 4;
                    v72 = (char *)v72 + 1;
                    idx2->field_3c4 = v24;
                } while (v72 < idx->field_724);
            }
            if (idx->field_728 >= 8)
                idx->field_730 = 0;
        }
        if (*((int *)&g_4d48c4) & 524289)
        {
            v92 = &idx->field_700;
            v93 = 8;
            do
            {
                v94 = v93;
                sub_461970(*((int *)&v92->padding_0[0]));
            } while ((v92 += 4, v93 = v94 - 1, v94 != 1));
            index = a0;
            index->field_72c = *(node);
            index->field_728 = 0;
            v96 = index->field_2c4;
            if (!((char)index->field_2c4 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                index->field_2bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                index->field_2b8 = 0;
                index->field_2b4 = 0xfff0bdc1;
                *((char **)&index->padding_2c0[0]) = &g_4b2ed0;
                index->field_2c4 = v96 | 1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            index->field_2bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            index->field_2b8 = 0;
            index->field_2b4 = 0xffffffff;
            if (!(&g_4b451c->field_1e9da)[index->field_72c] && !index->field_730)
            {
                v14 = 0;
                if (g_4ceae8 & 16)
                {
                    sub_454960(4);
                    index->field_730 = 1;
                }
                else
                {
                    sub_454960(3);
                    index->field_730 = 1;
                }
                return 0;
            }
            sub_4300d0(0, &index->padding_734[64 * *(node)]);
            if (g_4ceae8 & 16)
                sub_454960(4, 0);
            sub_454960(2, 0);
            g_4b451c->field_1e9da = 1;
            index->field_730 = 0;
            return 0;
        }
        else if (!(*((int *)&g_4d48c4) & 258))
        {
            return 0;
        }
LABEL_449c27:
        v97 = NULL;
        if (idx->field_5c10)
        {
            sub_46c941(idx->field_5c10);
            idx->field_5c10 = 0;
        }
        idx->field_5c10 = 0;
        sub_464940();
        if (idx->field_724 > 0)
        {
            v98 = &idx->field_6b0;
            do
            {
                sub_461970(*((int *)&v98->padding_0[0]));
            } while ((v97 += 1, v98 += 4, v97 < a0->field_724));
            idx = a0;
        }
        v99 = &idx->field_700;
        v100 = 8;
        do
        {
            v101 = v100;
            sub_461970(*((int *)&v99->padding_0[0]));
        } while ((v99 += 4, v100 = v101 - 1, v101 != 1));
        sub_453d90();
        v102 = a0;
        v102->field_24 = 3;
        v103 = v102->field_2c4;
        if (!((char)v102->field_2c4 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v102->field_2bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v102->field_2b8 = v100;
            v102->field_2b4 = 0xfff0bdc1;
            *((char **)&v102->padding_2c0[0]) = &g_4b2ed0;
            v102->field_2c4 = v103 | 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v102->field_2b8 = 0;
        v102->field_2bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v102->field_2b4 = 0xffffffff;
        return 0;
    case 3:
        if (idx->field_2b8 < 10)
            return 0;
        sub_43efd0();
        sub_461970(idx->field_66c);
        idx->field_66c = NULL;
        sub_43eee0(idx->field_66c);
        sub_4300d0(0, "bgm/th12_01.wav");
        sub_430150(0, 0);
        sub_464940(0, 0);
        return 0;
    default:
        return 0;
    }
}
