// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41e020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41e020_1 {
    unsigned int field_0;
} st_41e020_1;

typedef struct st_41e020_22 {
    unsigned int field_0;
    struct st_41e020_22 *field_4;
} st_41e020_22;

typedef struct st_41e020_7 {
    struct st_41e020_6 *field_0;
    struct st_41e020_7 *field_4;
} st_41e020_7;

typedef struct st_41e020_0 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    struct st_41e020_1 *field_10;
    char padding_14[44];
    unsigned int field_40;
    unsigned int field_44;
    unsigned int field_48;
    unsigned int field_4c;
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    unsigned int field_5c;
} st_41e020_0;

typedef struct st_41e020_12 {
    char padding_0[20];
    struct st_41e020_7 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_41e020_12;

typedef struct st_41e020_6 {
    char padding_0[20];
    unsigned int field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_41e020_6;

typedef struct st_41e020_3 {
    char padding_0[4212];
    unsigned int field_1074;
    char padding_1078[5584];
    unsigned int field_2648;
    char padding_264c[172];
    unsigned int field_26f8;
} st_41e020_3;

typedef struct st_41e020_5 {
    char padding_0[2428];
    unsigned int field_97c;
    unsigned int field_980;
} st_41e020_5;

typedef struct st_41e020_4 {
    char padding_0[116];
    unsigned int field_74;
} st_41e020_4;

typedef struct st_41e020_2 {
    char padding_0[28];
    struct st_41e020_3 *field_1c;
    char padding_20[28];
    char field_3c;
} st_41e020_2;

typedef struct st_41e020_63 {
    char padding_0[124];
    char field_7c;
} st_41e020_63;

extern unsigned int g_41ef8c[4];
extern unsigned int g_4b0cb0;
extern st_41e020_63 *g_4b43cc;
extern st_41e020_2 *g_4b43dc;
extern st_41e020_4 *g_4b44e8;
extern st_41e020_5 *g_4b4514;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee40;
extern unsigned int g_4cee78;

int sub_41e020(unsigned int a0)
{
    void* idx;  // edi
    int v9;  // esi
    unsigned int j;  // ebx
    unsigned int v108;  // ecx
    st_41e020_7 *iter;  // eax
    st_41e020_12 *v110;  // ebx
    st_41e020_7 *node;  // eax
    st_41e020_12 *idx1;  // eax
    st_41e020_7 *v113;  // eax
    st_41e020_7 *v114;  // eax
    st_41e020_0 *idx2;  // eax
    unsigned int v117;  // ecx
    void* v19;  // esi
    st_41e020_7 *iter1;  // eax
    st_41e020_12 *v119;  // ebx
    st_41e020_7 *iter2;  // eax
    st_41e020_12 *v121;  // eax
    st_41e020_7 *v122;  // eax
    st_41e020_7 *v123;  // eax
    st_41e020_0 *v125;  // eax
    unsigned int v126;  // ecx
    st_41e020_7 *v127;  // eax
    unsigned int v20;  // ebx
    st_41e020_12 *v128;  // ebx
    st_41e020_7 *v129;  // eax
    st_41e020_12 *v130;  // eax
    st_41e020_7 *v131;  // eax
    st_41e020_7 *v132;  // eax
    st_41e020_0 *v134;  // eax
    unsigned int v135;  // ebx
    unsigned int v136;  // eax
    unsigned int v137;  // eax
    unsigned int k;  // ebx
    void* v138;  // esi
    void* v139;  // esi
    void* v140;  // esi
    unsigned int v141;  // eax
    unsigned int v142;  // ftop
    unsigned int v144;  // ftop
    unsigned int v145;  // ftop
    unsigned int v146;  // fpround
    unsigned int v147;  // 4131
    void* v22;  // esi
    unsigned int v149;  // fc3210
    unsigned int v150;  // 4146
    unsigned int v151;  // edx
    unsigned int *v152;  // ecx
    unsigned short v154;  // ftop
    unsigned int v23;  // ebx
    void* v24;  // esi
    unsigned int l;  // ebx
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    int v10;  // ebx
    unsigned int *v28;  // edx
    unsigned int v31;  // ftop
    void* v32;  // esi
    unsigned int v33;  // ebx
    unsigned int v34;  // ebx
    unsigned int v36;  // ftop
    unsigned int v37;  // 4145
    unsigned int v39;  // 4134
    void* v40;  // esi
    unsigned int v41;  // ebx
    unsigned int v42;  // ebx
    unsigned int v43;  // ecx
    int v44;  // eax
    unsigned int v46;  // ftop
    unsigned int v47;  // 4237
    unsigned int *v12;  // esi
    unsigned int v49;  // ftop
    unsigned int v50;  // 4167
    unsigned int v51;  // fc3210
    unsigned int v52;  // 4146
    unsigned int v53;  // fc3210
    unsigned int v54;  // 4144
    int m;  // esi
    void* v13;  // esi
    int n;  // esi
    int v59;  // ebx
    void* v60;  // esi
    st_41e020_0 *index;  // esi
    unsigned int v62;  // edx
    unsigned int *v63;  // ecx
    unsigned int v66;  // ecx
    st_41e020_7 *v67;  // eax
    unsigned int v14;  // ebx
    st_41e020_6 *v68;  // ebx
    st_41e020_7 *v69;  // eax
    st_41e020_6 *v70;  // eax
    st_41e020_0 *v71;  // eax
    unsigned int v72;  // ecx
    st_41e020_7 *v73;  // eax
    st_41e020_12 *v74;  // ebx
    st_41e020_7 *v75;  // eax
    st_41e020_12 *v76;  // eax
    st_41e020_7 *v77;  // eax
    unsigned int i;  // ebx
    st_41e020_7 *v78;  // eax
    st_41e020_0 *v80;  // eax
    unsigned int v81;  // ecx
    st_41e020_7 *v82;  // eax
    unsigned int v83;  // ebx
    st_41e020_22 *v84;  // eax
    unsigned int v85;  // eax
    unsigned int v86;  // eax
    unsigned int v87;  // eax
    void* v16;  // esi
    st_41e020_0 *v89;  // eax
    unsigned int v90;  // ecx
    st_41e020_7 *v91;  // eax
    st_41e020_12 *v92;  // ebx
    st_41e020_7 *v93;  // eax
    st_41e020_12 *v94;  // eax
    st_41e020_7 *v95;  // eax
    st_41e020_7 *v96;  // eax
    unsigned int v17;  // ebx
    st_41e020_0 *v98;  // eax
    unsigned int v99;  // ecx
    st_41e020_7 *v100;  // eax
    st_41e020_12 *v101;  // ebx
    st_41e020_7 *v102;  // eax
    st_41e020_12 *v103;  // eax
    st_41e020_7 *v104;  // eax
    st_41e020_7 *v105;  // eax
    st_41e020_0 *v107;  // eax
    unsigned short v0;  // [bp-0x78]
    st_41e020_0 *v1;  // [bp-0x74], Other Possible Types: int, unsigned int
    int v2;  // [bp-0x70], Other Possible Types: unsigned int
    char v3;  // [bp-0x6c]
    unsigned int v4;  // [bp-0x60]
    int *v5;  // [bp-0x4c]
    void* v6;  // [bp-0x48], Other Possible Types: unsigned int

    if ((int)idx[27928] & 0x200)
        sub_464a80(v9, v10);
    if ((char)idx[27928] & 16)
    {
        *((unsigned int *)&idx[27980]) = (int)idx[27980] + 1;
        if ((int)idx[27980] == 180)
        {
            v12 = sub_46c9ea(0x44);
            if (v12)
            {
                v12[16] = v12[16] & 0xfffffffe;
                sub_477420(v12, 0, 0x44);
                *(v12) = *(v12) | 2;
                sub_452a60(v12, 5, 200, 0, 0, 0, 70);
            }
        }
        if ((int)idx[27980] >= 380)
        {
            if (g_4b44e8->field_74)
            {
                sub_432850();
            }
            else
            {
                a0 = (-(0 < (g_4cee78 & 0x2000)) & 0xfffffff3) + 15;
                g_4cee40 = a0;
            }
        }
    }
    if ((int)idx[27832] && !sub_461920(a0, g_4ce8cc, (int)idx[27828]))
    {
        *((unsigned int *)&idx[27828]) = 0;
        *((unsigned int *)&idx[27832]) = 0;
    }
    v13 = idx + 16;
    v14 = 8;
    do
    {
        i = v14;
        sub_455630(v13);
        v13 += 1204;
        v14 = i - 1;
    } while (i != 1);
    v16 = idx + 9648;
    v17 = 8;
    do
    {
        j = v17;
        sub_455630(v16);
        v16 += 1204;
        v17 = j - 1;
    } while (j != 1);
    v19 = idx + 19280;
    v20 = 2;
    do
    {
        k = v20;
        sub_455630(v19);
        v19 += 1204;
        v20 = k - 1;
    } while (k != 1);
    v22 = idx + 21688;
    v23 = 4;
    do
    {
        v24 = v22;
        l = v23;
        sub_455630(v24);
        v22 = v24 + 1204;
        v23 = l - 1;
    } while (l != 1);
    v27 = v26 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v28 = g_4b4514;
    if (!((char)idx[27928] & 1))
    {
        if (!g_4b4514)
            goto LABEL_41e269;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407a000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            goto LABEL_41e269;
        /* unsupported instruction */
        v31 = v27 + 1;
        if (!((char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4514->field_97c) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            v32 = idx + 22860;
            v33 = 4;
            do
            {
                v34 = v33;
                if (*((int *)v32))
                    (*((int *)v32))();
            } while ((*((unsigned short *)((char *)v32 - 208)) = 3, v32 += 1204, v33 = v34 - 1, v34 != 1));
            *((unsigned int *)&idx[27928]) = (int)idx[27928] | 1;
            v28 = g_4b4514;
        }
    }
    else if (g_4b4514)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v36 = v27;
        v37 = _ccall(10, 13, (char)((((unsigned short)v36 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4079000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v37 & 1)
        {
            /* unsupported instruction */
            v31 = v36 + 1;
            v39 = _ccall(10, 13, (char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4514->field_97c) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (v39 & 1)
                goto LABEL_41e26b;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v31 = v36 + 1;
        }
        v40 = idx + 22860;
        v41 = 4;
        do
        {
            v42 = v41;
            if (*((int *)v40))
                (*((int *)v40))();
        } while ((*((unsigned short *)((char *)v40 - 208)) = 2, v40 += 1204, v41 = v42 - 1, v42 != 1));
        *((unsigned int *)&idx[27928]) = (int)idx[27928] & 0xfffffffe;
        v28 = g_4b4514;
    }
    else
    {
LABEL_41e269:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v31 = v27 + 1;
    }
LABEL_41e26b:
    v43 = g_4b43dc;
    if (g_4b43dc)
    {
        v44 = (int)idx[27960];
        if (v44 >= 0 && g_4b43dc->field_1c && !(g_4b43dc->field_3c & 1) && !(int)idx[27952])
        {
            if (v44 < (int)idx[27968])
            {
                if (v44 < 5)
                {
                    if ((int)idx[20452])
                        (int)idx[20452]();
                    *((unsigned short *)&idx[20244]) = 9;
                    if ((int)idx[21656])
                        (int)idx[21656]();
                    *((unsigned short *)&idx[21448]) = 9;
                    sub_453d90();
                }
                else if (v44 < 10)
                {
                    if ((int)idx[20452])
                        (int)idx[20452]();
                    *((unsigned short *)&idx[20244]) = 8;
                    if ((int)idx[21656])
                        (int)idx[21656]();
                    *((unsigned short *)&idx[21448]) = 8;
                    sub_453d90();
                }
            }
            else
            {
                if (v44 > (int)idx[27968])
                {
                    if ((int)idx[20452])
                        (int)idx[20452]();
                    *((unsigned short *)&idx[20244]) = 7;
                    if ((int)idx[21656])
                        (int)idx[21656]();
                    *((unsigned short *)&idx[21448]) = 7;
                }
            }
            if ((int)idx[27960] != (int)idx[27968])
            {
                if ((int)idx[20296])
                    sub_454b80();
                if ((int)idx[0x53fc])
                    sub_454b80();
            }
            v43 = g_4b43dc;
            *((int *)&idx[27968]) = (int)idx[27960];
            v28 = g_4b4514;
        }
        if (!v43 || !*((int *)(v43 + 28)) || *((char *)(v43 + 60)) & 1 || (int)idx[27952])
            goto LABEL_41e6e3;
        v6 = *((int *)(*((int *)(v43 + 28)) + 9800));
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[27888]) = *((int *)(*((int *)(v43 + 28)) + 9800));
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[27884]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v46 = v31;
        v47 = _ccall(10, 13, (char)((((unsigned short)v46 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v47 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx[27880]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v49 = v46;
        v50 = _ccall(10, 13, (char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v50 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx[27880]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        if ((char)idx[27928] & 8)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v51 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v28[608]) * 0x100 & 0x4500;
            /* unsupported instruction */
            v31 = v49;
            v52 = _ccall(10, 13, (char)((((unsigned short)v31 & 7) * 0x800 | (unsigned short)v51 & 0x4700) >> 8) & 65, 0, 0);
            if (v52 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v53 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v28[607]) * 0x100 & 0x4500;
                /* unsupported instruction */
                v54 = _ccall(10, 13, (char)((((unsigned short)v31 & 7) * 0x800 | (unsigned short)v53 & 0x4700) >> 8) & 5, 0, 0);
                if (v54 & 1)
                    goto LABEL_41e5ba;
            }
            m = 0;
            if ((int)idx[27892] > 0)
            {
                v6 = idx + 27772;
                do
                {
                    sub_461970(*(v5));
                    v24 += 4;
                    m += 1;
                } while (m < (int)idx[27892]);
            }
            sub_461970((int)idx[27768]);
            *((unsigned int *)&idx[27928]) = (int)idx[27928] & 0xfffffff7;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v31 = v49;
            if (!((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v28[608]) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if (!((char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v28[607]) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    n = 0;
                    if ((int)idx[27892] > 0)
                    {
                        v6 = idx + 27772;
                        do
                        {
                            sub_461970(*(v5));
                            v24 += 4;
                            n += 1;
                        } while (n < (int)idx[27892]);
                    }
                    sub_461970((int)idx[27768]);
                    *((unsigned int *)&idx[27928]) = (int)idx[27928] | 8;
                }
            }
        }
LABEL_41e5ba:
        if (!(int)idx[27768])
        {
            if (g_4b0cb0 - 1 <= 6)
            {
                goto *((void *)(*((int *)((char *)&g_41ef8c[g_4b0cb0] - 4))));
            }
            else
            {
                sub_4615a0((int)idx[27972], &v24, 118, 0);
                *((int *)&idx[27768]) = 118;
            }
        }
        v59 = 0;
        v4 = 0;
        v60 = idx + 27772;
        do
        {
            if (v59 < (int)idx[27892])
            {
                if (!*((int *)v60))
                {
                    sub_4615a0((int)idx[27972], &v3, v59 + 53, 0);
                    *((int *)v60) = 0;
                }
            }
            else
            {
                if (*((int *)v60))
                {
                    sub_461970(*((int *)v60));
                    v59 = v1;
                    *((int *)v60) = 0;
                }
            }
        } while ((v59 = (int)(v59 + 1), v60 += 4, v2 = v59, v59 < 10));
    }
    else
    {
LABEL_41e6e3:
        if ((int)idx[27768])
        {
            sub_461970((int)idx[27768]);
            *((int *)&idx[27768]) = 0;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[27880]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)&idx[27896]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)&idx[0x6d00]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)&idx[27912]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)&idx[27920]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    index = (int)idx[27952];
    if (index)
    {
        if (!sub_41fcc0(index))
        {
            v62 = index->field_8;
            v63 = &index->field_10->field_0;
            index->field_4 = v62;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v63)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                    goto LABEL_41e77b;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                index->field_8 = v62 + 1;
                index->field_c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else
            {
LABEL_41e77b:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v31 -= 1;
                /* unsupported instruction */
                /* unsupported instruction */
                index->field_c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                index->field_8 = sub_4931e0();
            }
        }
        else
        {
            v1 = (int)idx[27952];
            if (v1)
            {
                v66 = v1->field_40;
                if (v66)
                {
                    v67 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v68 = v67->field_0;
                            if (*((int *)&v68->padding_0[0]) == v66)
                                goto LABEL_41e7f2;
                        } while ((v67 = (st_41e020_7 *)v67->field_4, v67));
                    }
                    v69 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v68 = v69->field_0;
                            if (*((int *)&v68->padding_0[0]) == v66)
                                break;
                            v69 = v69->field_4;
                            if (!v69)
                                goto LABEL_41e81f;
                        }
LABEL_41e7f2:
                        v70 = v68;
                        if (v70)
                        {
                            v70->field_47c = v70->field_47c | 0x10000000;
                            if (!v70->field_18 && v70->field_14)
                                return sub_41e810();
                        }
                    }
                }
LABEL_41e81f:
                v71 = v1;
                v71->field_40 = 0;
                v72 = v71->field_44;
                if (v72)
                {
                    v73 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v74 = v73->field_0;
                            if (*((int *)&v74->padding_0[0]) == v72)
                                goto LABEL_41e86f;
                        } while ((v73 = (st_41e020_7 *)v73->field_4, v73));
                    }
                    v75 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v74 = v75->field_0;
                            if (*((int *)&v74->padding_0[0]) == v72)
                                break;
                            v75 = v75->field_4;
                            if (!v75)
                                goto LABEL_41e89f;
                        }
LABEL_41e86f:
                        v76 = v74;
                        if (v76)
                        {
                            v76->field_47c = v76->field_47c | 0x10000000;
                            if (!v76->field_18)
                            {
                                v77 = v76->field_14;
                                if (v76->field_14)
                                {
                                    do
                                    {
                                        v78 = v77;
                                        v78->field_0->field_47c = v78->field_0->field_47c | 0x10000000;
                                        v77 = v78->field_4;
                                    } while (v78->field_4);
                                }
                            }
                        }
                    }
                }
LABEL_41e89f:
                v80 = v1;
                v80->field_44 = 0;
                v81 = v80->field_48;
                if (v81)
                {
                    v82 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v83 = v82->field_0;
                            if (*((int *)v83) == v81)
                                goto LABEL_41e8ef;
                        } while ((v82 = (st_41e020_7 *)v82->field_4, v82));
                    }
                    v84 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v83 = v84->field_0;
                            if (*((int *)v83) == v81)
                                break;
                            v84 = v84->field_4;
                            if (!v84)
                                goto LABEL_41e91f;
                        }
LABEL_41e8ef:
                        v85 = v83;
                        if (v85)
                        {
                            *((unsigned int *)(v85 + 1148)) = *((int *)(v85 + 1148)) | 0x10000000;
                            if (!*((int *)(v85 + 24)))
                            {
                                v86 = *((int *)(v85 + 20));
                                if (*((int *)(v85 + 20)))
                                {
                                    do
                                    {
                                        v87 = v86;
                                        *((unsigned int *)(*((int *)v87) + 1148)) = *((int *)(*((int *)v87) + 1148)) | 0x10000000;
                                        v86 = *((int *)(v87 + 4));
                                    } while (*((int *)(v87 + 4)));
                                }
                            }
                        }
                    }
                }
LABEL_41e91f:
                v89 = v1;
                v89->field_48 = 0;
                v90 = v89->field_4c;
                if (v90)
                {
                    v91 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v92 = v91->field_0;
                            if (*((int *)&v92->padding_0[0]) == v90)
                                goto LABEL_41e96f;
                        } while ((v91 = (st_41e020_7 *)v91->field_4, v91));
                    }
                    v93 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v92 = v93->field_0;
                            if (*((int *)&v92->padding_0[0]) == v90)
                                break;
                            v93 = v93->field_4;
                            if (!v93)
                                goto LABEL_41e99f;
                        }
LABEL_41e96f:
                        v94 = v92;
                        if (v94)
                        {
                            v94->field_47c = v94->field_47c | 0x10000000;
                            if (!v94->field_18)
                            {
                                v95 = v94->field_14;
                                if (v94->field_14)
                                {
                                    do
                                    {
                                        v96 = v95;
                                        v96->field_0->field_47c = v96->field_0->field_47c | 0x10000000;
                                        v95 = v96->field_4;
                                    } while (v96->field_4);
                                }
                            }
                        }
                    }
                }
LABEL_41e99f:
                v98 = v1;
                v98->field_4c = 0;
                v99 = v98->field_50;
                if (v99)
                {
                    v100 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v101 = v100->field_0;
                            if (*((int *)&v101->padding_0[0]) == v99)
                                goto LABEL_41e9ef;
                        } while ((v100 = (st_41e020_7 *)v100->field_4, v100));
                    }
                    v102 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v101 = v102->field_0;
                            if (*((int *)&v101->padding_0[0]) == v99)
                                break;
                            v102 = v102->field_4;
                            if (!v102)
                                goto LABEL_41ea1f;
                        }
LABEL_41e9ef:
                        v103 = v101;
                        if (v103)
                        {
                            v103->field_47c = v103->field_47c | 0x10000000;
                            if (!v103->field_18)
                            {
                                v104 = v103->field_14;
                                if (v103->field_14)
                                {
                                    do
                                    {
                                        v105 = v104;
                                        v105->field_0->field_47c = v105->field_0->field_47c | 0x10000000;
                                        v104 = v105->field_4;
                                    } while (v105->field_4);
                                }
                            }
                        }
                    }
                }
LABEL_41ea1f:
                v107 = v1;
                v107->field_50 = 0;
                v108 = v107->field_54;
                if (v108)
                {
                    iter = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v110 = iter->field_0;
                            if (*((int *)&v110->padding_0[0]) == v108)
                                goto LABEL_41ea6f;
                        } while ((iter = (st_41e020_7 *)iter->field_4, iter));
                    }
                    node = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v110 = node->field_0;
                            if (*((int *)&v110->padding_0[0]) == v108)
                                break;
                            node = node->field_4;
                            if (!node)
                                goto LABEL_41ea9f;
                        }
LABEL_41ea6f:
                        idx1 = v110;
                        if (idx1)
                        {
                            idx1->field_47c = idx1->field_47c | 0x10000000;
                            if (!idx1->field_18)
                            {
                                v113 = idx1->field_14;
                                if (idx1->field_14)
                                {
                                    do
                                    {
                                        v114 = v113;
                                        v114->field_0->field_47c = v114->field_0->field_47c | 0x10000000;
                                        v113 = v114->field_4;
                                    } while (v114->field_4);
                                }
                            }
                        }
                    }
                }
LABEL_41ea9f:
                idx2 = v1;
                idx2->field_54 = 0;
                v117 = idx2->field_58;
                if (v117)
                {
                    iter1 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v119 = iter1->field_0;
                            if (*((int *)&v119->padding_0[0]) == v117)
                                goto LABEL_41eaef;
                        } while ((iter1 = (st_41e020_7 *)iter1->field_4, iter1));
                    }
                    iter2 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v119 = iter2->field_0;
                            if (*((int *)&v119->padding_0[0]) == v117)
                                break;
                            iter2 = iter2->field_4;
                            if (!iter2)
                                goto LABEL_41eb1f;
                        }
LABEL_41eaef:
                        v121 = v119;
                        if (v121)
                        {
                            v121->field_47c = v121->field_47c | 0x10000000;
                            if (!v121->field_18)
                            {
                                v122 = v121->field_14;
                                if (v121->field_14)
                                {
                                    do
                                    {
                                        v123 = v122;
                                        v123->field_0->field_47c = v123->field_0->field_47c | 0x10000000;
                                        v122 = v123->field_4;
                                    } while (v123->field_4);
                                }
                            }
                        }
                    }
                }
LABEL_41eb1f:
                v125 = v1;
                v125->field_58 = 0;
                v126 = v125->field_5c;
                if (v126)
                {
                    v127 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v128 = v127->field_0;
                            if (*((int *)&v128->padding_0[0]) == v126)
                                goto LABEL_41eb6f;
                        } while ((v127 = (st_41e020_7 *)v127->field_4, v127));
                    }
                    v129 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (1)
                        {
                            v128 = v129->field_0;
                            if (*((int *)&v128->padding_0[0]) == v126)
                                break;
                            v129 = v129->field_4;
                            if (!v129)
                                goto LABEL_41eb9f;
                        }
LABEL_41eb6f:
                        v130 = v128;
                        if (v130)
                        {
                            v130->field_47c = v130->field_47c | 0x10000000;
                            if (!v130->field_18)
                            {
                                v131 = v130->field_14;
                                if (v130->field_14)
                                {
                                    do
                                    {
                                        v132 = v131;
                                        v132->field_0->field_47c = v132->field_0->field_47c | 0x10000000;
                                        v131 = v132->field_4;
                                    } while (v132->field_4);
                                }
                            }
                        }
                    }
                }
LABEL_41eb9f:
                v134 = v1;
                v134->field_5c = 0;
                sub_46ca4f(v134);
            }
            *((st_41e020_0 **)&idx[27952]) = NULL;
        }
    }
    if (g_4b43dc)
    {
        v135 = g_4b43dc->field_1c;
        if (v135 && (char)~((unsigned int)(*((int *)(v135 + 9976))) >> 5) & 1 && (char)~(*((int *)(v135 + 9976))) & 1)
        {
            v136 = (unsigned int)((int)idx[27928]) >> 1;
            if (g_4b43cc->field_7c & 1)
            {
                v137 = v136 & 3;
                if (v136 & 3)
                {
                    if (v137 != 1)
                    {
                        if (v137 != 2)
                        {
                            if (v137 == 3 && *((int *)(v135 + 9808)) > 400)
                                goto LABEL_41ee03;
                            goto LABEL_41ee2f;
                        }
                        else if (*((int *)(v135 + 9808)) < 400)
                        {
                            v140 = idx + 26508;
                            if (!(int)idx[27680])
                                goto LABEL_41eddd;
                            (int)idx[27680]();
                            *((unsigned short *)&v140[964]) = 9;
                            *((unsigned int *)&idx[27928]) = (int)idx[27928] | 6;
                            goto LABEL_41ee2f;
                        }
                    }
                    if (*((int *)(v135 + 9808)) >= 1000)
                        goto LABEL_41ee2f;
                    v139 = idx + 26508;
                    if (!(int)idx[27680])
                        goto LABEL_41ed90;
                    (int)idx[27680]();
                    *((unsigned short *)&v139[964]) = 8;
                    *((unsigned int *)&idx[27928]) = (int)idx[27928] & 0xfffffffd | 4;
                    goto LABEL_41ee2f;
                }
                if (*((int *)(v135 + 9808)) >= 2000)
                    goto LABEL_41ee2f;
                v138 = idx + 26508;
                if (!(int)idx[27680])
                    goto LABEL_41ed3f;
                (int)idx[27680]();
                *((unsigned short *)&v138[964]) = 7;
                *((unsigned int *)&idx[27928]) = (int)idx[27928] & 0xfffffffb | 2;
            }
            else
            {
                v141 = v136 & 3;
                if (v136 & 3)
                {
                    if (v141 != 1)
                    {
                        if (v141 == 2)
                        {
                            if (*((int *)(v135 + 9808)) < 200)
                            {
                                v140 = idx + 26508;
                                if ((int)idx[27680])
                                    (int)idx[27680]();
LABEL_41eddd:
                                *((unsigned short *)&v140[964]) = 9;
                                *((unsigned int *)&idx[27928]) = (int)idx[27928] | 6;
                                goto LABEL_41ee2f;
                            }
                        }
                        else
                        {
                            if (v141 == 3 && *((int *)(v135 + 9808)) > 200)
                            {
LABEL_41ee03:
                                if ((int)idx[27680])
                                    (int)idx[27680]();
                                *((unsigned short *)&idx[27472]) = 10;
                                *((unsigned int *)&idx[27928]) = (int)idx[27928] & 0xfffffff9;
                                goto LABEL_41ee2f;
                            }
                        }
                    }
                    if (*((int *)(v135 + 9808)) >= 400)
                        goto LABEL_41ee2f;
                    v139 = idx + 26508;
                    if ((int)idx[27680])
                        (int)idx[27680]();
LABEL_41ed90:
                    *((unsigned short *)&v139[964]) = 8;
                    *((unsigned int *)&idx[27928]) = (int)idx[27928] & 0xfffffffd | 4;
                    goto LABEL_41ee2f;
                }
                if (*((int *)(v135 + 9808)) >= 700)
                    goto LABEL_41ee2f;
                v138 = idx + 26508;
                if ((int)idx[27680])
                    (int)idx[27680]();
LABEL_41ed3f:
                *((unsigned short *)&v138[964]) = 7;
                *((unsigned int *)&idx[27928]) = (int)idx[27928] & 0xfffffffb | 2;
            }
LABEL_41ee2f:
            sub_455630(idx + 26508);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx[27580]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx[27584]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v142 = v31 - 2;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v144 = v142 + 1;
            if (!((((unsigned short)v142 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v145 = v144 + 1;
                *((char *)&idx[27467]) = 0xff;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v147 = _ccall(v146);
                v0 = v147;
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = v0 | 0xc00;
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v145 = v144 + 1;
                *((char *)&idx[27467]) = 64 - (char)v1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v31 = v145;
            if ((char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)(v135 + 4212))) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v149 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)(v135 + 4212))) * 0x100 & 0x4500;
                /* unsupported instruction */
                v150 = _ccall(10, 13, (char)((((unsigned short)v31 & 7) * 0x800 | (unsigned short)v149 & 0x4700) >> 8) & 5, 0, 0);
                if (v150 & 1)
                    goto LABEL_41ef0c;
            }
            *((char *)&idx[27467]) = 0;
        }
    }
LABEL_41ef0c:
    v151 = (int)idx[27844];
    v152 = (int)idx[27852];
    *((unsigned int *)&idx[27840]) = v151;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v154 = v31;
    if (!((char)(((v154 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v154 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v152)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&idx[27844]) = v151 + 1;
            *((int *)&idx[27848]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            return 1;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx[27848]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&idx[27844]) = sub_4931e0();
    return 1;
}
