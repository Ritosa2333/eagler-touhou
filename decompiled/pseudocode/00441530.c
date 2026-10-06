// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x441530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_441530_172 {
    char padding_0[16];
    struct st_441530_8 *field_10;
    struct st_441530_172 *field_14;
} st_441530_172;

typedef struct st_441530_4 {
    char padding_0[16];
    struct st_441530_5 *field_10;
    unsigned int field_14;
} st_441530_4;

typedef struct st_441530_6 {
    unsigned int field_0;
} st_441530_6;

typedef struct st_441530_20 {
    char padding_0[40];
    int field_28;
    char padding_2c[672];
    unsigned int field_2cc;
} st_441530_20;

typedef struct st_441530_7 {
    struct st_441530_4 *field_0;
    struct st_441530_7 *field_4;
} st_441530_7;

typedef struct st_441530_0 {
    unsigned int field_0;
    struct st_441530_0 *field_4;
} st_441530_0;

typedef struct st_441530_5 {
    char padding_0[1002];
    short field_3ea;
    char padding_3ec[168];
    struct st_441530_6 *field_494;
} st_441530_5;

typedef struct st_441530_21 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_441530_6 *field_494;
} st_441530_21;

typedef struct st_441530_8 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[36];
    short field_3ea;
    char padding_3ec[168];
    struct st_441530_6 *field_494;
} st_441530_8;

extern unsigned int g_4ce8cc;

void sub_441530(st_441530_20 *index)
{
    st_441530_20 *v3;  // eax
    unsigned int v4;  // ebx
    unsigned int iter;  // ecx
    int v103;  // ecx
    st_441530_7 *node;  // eax
    st_441530_4 *v105;  // eax
    st_441530_7 *iter1;  // eax
    unsigned int iter2;  // eax
    st_441530_8 *v108;  // esi
    int v109;  // ecx
    st_441530_7 *v110;  // eax
    st_441530_4 *v111;  // eax
    st_441530_7 *v112;  // eax
    void* v14;  // esi
    unsigned int v113;  // eax
    st_441530_8 *v114;  // esi
    int v115;  // ecx
    st_441530_7 *v116;  // eax
    st_441530_4 *v117;  // eax
    st_441530_7 *v118;  // eax
    unsigned int v119;  // eax
    st_441530_8 *v120;  // esi
    int v121;  // ecx
    st_441530_7 *v122;  // eax
    void* v15;  // esi
    st_441530_4 *v123;  // eax
    st_441530_7 *v124;  // eax
    unsigned int v125;  // eax
    st_441530_8 *v126;  // esi
    int v127;  // ecx
    st_441530_7 *v128;  // eax
    st_441530_4 *v129;  // eax
    st_441530_7 *v130;  // eax
    unsigned int v131;  // eax
    st_441530_8 *v132;  // esi
    unsigned int l;  // ecx
    int v133;  // ecx
    st_441530_7 *v134;  // eax
    st_441530_4 *v135;  // eax
    st_441530_7 *v136;  // eax
    unsigned int v137;  // eax
    st_441530_8 *v138;  // esi
    int v139;  // ecx
    st_441530_7 *v140;  // eax
    st_441530_4 *v141;  // eax
    st_441530_7 *v142;  // eax
    unsigned int v17;  // esi
    unsigned int v143;  // eax
    st_441530_8 *v144;  // esi
    int v145;  // ecx
    st_441530_7 *v146;  // eax
    st_441530_4 *v147;  // eax
    st_441530_7 *v148;  // eax
    unsigned int v149;  // eax
    st_441530_8 *v150;  // esi
    int v151;  // ecx
    st_441530_7 *v152;  // eax
    st_441530_7 *v18;  // eax
    st_441530_4 *v153;  // eax
    st_441530_7 *v154;  // eax
    unsigned int v155;  // eax
    st_441530_8 *v156;  // esi
    st_441530_172 *v157;  // eax
    st_441530_172 *v158;  // eax
    st_441530_8 *v159;  // esi
    st_441530_172 *v160;  // eax
    st_441530_172 *v161;  // eax
    st_441530_8 *v162;  // esi
    st_441530_4 *v19;  // eax
    st_441530_172 *v163;  // eax
    st_441530_172 *v164;  // eax
    st_441530_8 *v165;  // esi
    st_441530_172 *v166;  // eax
    st_441530_172 *v167;  // eax
    st_441530_8 *v168;  // esi
    st_441530_172 *v169;  // eax
    st_441530_172 *v170;  // eax
    st_441530_8 *v171;  // esi
    st_441530_0 *v20;  // eax
    unsigned int v21;  // ecx
    st_441530_8 *v22;  // esi
    unsigned int v5;  // esi
    st_441530_8 *v23;  // esi
    unsigned int v24;  // edi
    unsigned int v25;  // ebx
    unsigned int j;  // edi
    unsigned int m;  // ecx
    unsigned int v28;  // esi
    st_441530_7 *v29;  // eax
    st_441530_4 *v30;  // eax
    st_441530_0 *v31;  // eax
    unsigned int v32;  // ecx
    unsigned int v6;  // edi
    st_441530_8 *v33;  // esi
    st_441530_8 *v34;  // esi
    unsigned int n;  // ecx
    st_441530_7 *v36;  // eax
    st_441530_4 *v37;  // eax
    st_441530_0 *v38;  // eax
    unsigned int v39;  // ecx
    st_441530_8 *v40;  // esi
    st_441530_8 *v41;  // esi
    st_441530_20 *idx;  // edi
    unsigned int i;  // edi
    int v43;  // ecx
    st_441530_7 *v44;  // eax
    st_441530_4 *v45;  // eax
    st_441530_7 *v46;  // eax
    unsigned int v47;  // eax
    st_441530_21 *v48;  // esi
    int v49;  // ecx
    st_441530_7 *v50;  // eax
    st_441530_4 *v51;  // eax
    st_441530_7 *v52;  // eax
    unsigned int k;  // ecx
    unsigned int v53;  // eax
    st_441530_8 *v54;  // esi
    int v55;  // ecx
    st_441530_7 *v56;  // eax
    st_441530_4 *v57;  // eax
    st_441530_7 *v58;  // eax
    unsigned int v59;  // eax
    st_441530_8 *v60;  // esi
    int v61;  // ecx
    st_441530_7 *v62;  // eax
    unsigned int v9;  // esi
    unsigned int v63;  // eax
    st_441530_0 *v64;  // eax
    unsigned int v65;  // eax
    void* v66;  // esi
    int v67;  // ecx
    st_441530_7 *v68;  // eax
    st_441530_4 *v69;  // eax
    st_441530_7 *v70;  // eax
    unsigned int v71;  // eax
    st_441530_8 *v72;  // esi
    st_441530_7 *v10;  // eax
    int v73;  // ecx
    st_441530_7 *v74;  // eax
    st_441530_4 *v75;  // eax
    st_441530_7 *v76;  // eax
    unsigned int v77;  // eax
    st_441530_8 *v78;  // esi
    int v79;  // ecx
    st_441530_7 *v80;  // eax
    st_441530_4 *v81;  // eax
    st_441530_7 *v82;  // eax
    st_441530_4 *v11;  // eax
    unsigned int v83;  // eax
    st_441530_8 *v84;  // esi
    int v85;  // ecx
    st_441530_7 *v86;  // eax
    st_441530_4 *v87;  // eax
    st_441530_7 *v88;  // eax
    unsigned int v89;  // eax
    st_441530_8 *v90;  // esi
    int v91;  // ecx
    st_441530_7 *v92;  // eax
    st_441530_0 *v12;  // eax
    st_441530_4 *v93;  // eax
    st_441530_7 *v94;  // eax
    unsigned int v95;  // eax
    st_441530_8 *v96;  // esi
    int v97;  // ecx
    st_441530_7 *v98;  // eax
    st_441530_4 *v99;  // eax
    st_441530_7 *v100;  // eax
    unsigned int v101;  // eax
    st_441530_8 *v102;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v3 = index;
    v2 = v4;
    v1 = v5;
    v0 = v6;
    i = 0;
    if (v3->field_28 > NULL)
    {
        do
        {
            k = v3->field_2cc;
            v9 = i + 19;
            if (!k)
            {
LABEL_441552:
                v11 = 0;
                goto LABEL_44159b;
            }
            else
            {
                v10 = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)))
                {
                    do
                    {
                        if (*((int *)&v10->field_0->padding_0[0]) == k)
                        {
                            v11 = v10->field_0;
                            goto LABEL_441597;
                        }
                    } while ((v10 = (st_441530_7 *)v10->field_4, v10));
                }
                v12 = *((int *)(g_4ce8cc + 8935104));
                if (!*((int *)(g_4ce8cc + 8935104)))
                    goto LABEL_441552;
                while (*((int *)v12->field_0) != k)
                {
                    if (!v12->field_4)
                    {
                        v11 = 0;
                        goto LABEL_44159b;
                    }
                }
                v11 = v12->field_0;
LABEL_441597:
                if (v11)
                    goto LABEL_4415a5;
LABEL_44159b:
                index->field_2cc = 0;
LABEL_4415a5:
                iter = &v11->field_10;
                if (iter)
                {
                    do
                    {
                        if (*((short *)(*((int *)iter) + 1002)) == v9 || v9 == 0xffffffff && v14 != v11)
                        {
                            v14 = *((int *)iter);
                            v15 = v14;
                            goto LABEL_4415cf;
                        }
                    } while ((iter = (unsigned int)*((int *)(iter + 4)), iter));
                }
                v15 = NULL;
LABEL_4415cf:
                if ((int)v15[1172])
                    (int)v15[1172]();
                *((unsigned short *)&v15[964]) = 30;
                sub_455630(v15);
                l = index->field_2cc;
                v17 = i + 24;
                if (!l)
                {
LABEL_441605:
                    v19 = 0;
                    goto LABEL_441652;
                }
                else
                {
                    v18 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        l = l;
                        do
                        {
                            if (*((int *)&v18->field_0->padding_0[0]) == l)
                            {
                                v19 = v18->field_0;
                                goto LABEL_44164e;
                            }
                        } while ((v18 = (st_441530_7 *)v18->field_4, v18));
                    }
                    v20 = *((int *)(g_4ce8cc + 8935104));
                    if (!*((int *)(g_4ce8cc + 8935104)))
                        goto LABEL_441605;
                    while (*((int *)v20->field_0) != l)
                    {
                        if (!v20->field_4)
                        {
                            v19 = 0;
                            goto LABEL_441652;
                        }
                    }
                    v19 = v20->field_0;
LABEL_44164e:
                    if (v19)
                        goto LABEL_44165c;
LABEL_441652:
                    index->field_2cc = 0;
                }
            }
LABEL_44165c:
            v21 = &v19->field_10;
            if (v21)
            {
                do
                {
                    if (*((short *)(*((int *)v21) + 1002)) == v17 || v17 == 0xffffffff && v22 != v19)
                    {
                        v22 = *((int *)v21);
                        v23 = v22;
                        goto LABEL_441682;
                    }
                } while ((v21 = (unsigned int)*((int *)(v21 + 4)), v21));
            }
            v23 = NULL;
LABEL_441682:
            if (v23->field_494)
                v23->field_494();
            v23->field_3c4 = 30;
            sub_455630(v23);
            v3 = index;
            i += 1;
        } while (i < v3->field_28);
    }
    v24 = i + 1;
    v25 = 29;
    if (v24 < 5)
    {
        j = v24 + 24;
        do
        {
            m = index->field_2cc;
            v28 = j - 5;
            if (!m)
            {
LABEL_4416e1:
                v30 = 0;
                goto LABEL_44173d;
            }
            else
            {
                v29 = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)))
                {
                    do
                    {
                        if (*((int *)&v29->field_0->padding_0[0]) == m)
                        {
                            v30 = v29->field_0;
                            goto LABEL_441739;
                        }
                    } while ((v29 = (st_441530_7 *)v29->field_4, v29));
                }
                v31 = *((int *)(g_4ce8cc + 8935104));
                if (!*((int *)(g_4ce8cc + 8935104)))
                    goto LABEL_4416e1;
                v25 = v25;
                while (*((int *)v31->field_0) != m)
                {
                    if (!v31->field_4)
                    {
                        v30 = 0;
                        goto LABEL_44173d;
                    }
                }
                v30 = v31->field_0;
LABEL_441739:
                if (v30)
                    goto LABEL_441747;
LABEL_44173d:
                index->field_2cc = 0;
LABEL_441747:
                v32 = &v30->field_10;
                if (v32)
                {
                    j = j;
                    do
                    {
                        if (*((short *)(*((int *)v32) + 1002)) == v28 || v28 == 0xffffffff && v33 != v30)
                        {
                            v33 = *((int *)v32);
                            v34 = v33;
                            goto LABEL_441771;
                        }
                    } while ((v32 = (unsigned int)*((int *)(v32 + 4)), v32));
                }
                v34 = NULL;
LABEL_441771:
                if (v34->field_494)
                    v34->field_494();
                v34->field_3c4 = 31;
                sub_455630(v34);
                n = index->field_2cc;
                if (!n)
                {
LABEL_4417a4:
                    v37 = 0;
                    goto LABEL_4417f2;
                }
                else
                {
                    v36 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        j = j;
                        do
                        {
                            if (*((int *)&v36->field_0->padding_0[0]) == n)
                            {
                                v37 = v36->field_0;
                                goto LABEL_4417ee;
                            }
                        } while ((v36 = (st_441530_7 *)v36->field_4, v36));
                    }
                    v38 = *((int *)(g_4ce8cc + 8935104));
                    if (!*((int *)(g_4ce8cc + 8935104)))
                        goto LABEL_4417a4;
                    while (*((int *)v38->field_0) != n)
                    {
                        if (!v38->field_4)
                        {
                            v37 = 0;
                            goto LABEL_4417f2;
                        }
                    }
                    v37 = v38->field_0;
LABEL_4417ee:
                    if (v37)
                        goto LABEL_4417fc;
LABEL_4417f2:
                    index->field_2cc = 0;
                }
            }
LABEL_4417fc:
            v39 = &v37->field_10;
            if (v39)
            {
                do
                {
                    if (*((short *)(*((int *)v39) + 1002)) == j || j == 0xffffffff && v40 != v37)
                    {
                        v40 = *((int *)v39);
                        v41 = v40;
                        goto LABEL_441822;
                    }
                } while ((v39 = (unsigned int)*((int *)(v39 + 4)), v39));
            }
            v41 = NULL;
LABEL_441822:
            if (v41->field_494)
                v41->field_494();
            v41->field_3c4 = 31;
            sub_455630(v41);
            j += 1;
        } while (j < v25);
    }
    idx = index;
    if (idx->field_28 > NULL)
    {
        v43 = idx->field_2cc;
        if (!v43)
        {
LABEL_441867:
            v45 = NULL;
            goto LABEL_4418b2;
        }
        else
        {
            v44 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v44->field_0->padding_0[0]) == v43)
                    {
                        v45 = v44->field_0;
                        goto LABEL_4418ae;
                    }
                } while ((v44 = (st_441530_7 *)v44->field_4, v44));
            }
            v46 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441867;
        }
        while (1)
        {
            if (*((int *)&v46->field_0->padding_0[0]) != v43)
            {
                v46 = v46->field_4;
                if (!v46)
                {
                    v45 = NULL;
                    goto LABEL_4418b2;
                }
            }
            else
            {
                v45 = v46->field_0;
LABEL_4418ae:
                if (v45)
                    break;
LABEL_4418b2:
                idx->field_2cc = 0;
                break;
            }
        }
        v47 = &v45->field_10;
        if (v47)
        {
            do
            {
                if (*((short *)(*((int *)v47) + 1002)) == (unsigned short)v25)
                {
                    v48 = *((int *)v47);
                    goto LABEL_4418d4;
                }
            } while ((v47 = (unsigned int)*((int *)(v47 + 4)), v47));
        }
        v48 = NULL;
LABEL_4418d4:
        if (v48->field_494)
            v48->field_494();
        v48->field_3c4 = 30;
        sub_455630(v48);
        v49 = idx->field_2cc;
        if (!v49)
        {
LABEL_441905:
            v51 = NULL;
            goto LABEL_441952;
        }
        else
        {
            v50 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                v49 = v49;
                do
                {
                    if (*((int *)&v50->field_0->padding_0[0]) == v49)
                    {
                        v51 = v50->field_0;
                        goto LABEL_44194e;
                    }
                } while ((v50 = (st_441530_7 *)v50->field_4, v50));
            }
            v52 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441905;
        }
        while (1)
        {
            if (*((int *)&v52->field_0->padding_0[0]) != v49)
            {
                v52 = v52->field_4;
                if (!v52)
                {
                    v51 = NULL;
                    goto LABEL_441952;
                }
            }
            else
            {
                v51 = v52->field_0;
LABEL_44194e:
                if (v51)
                    break;
LABEL_441952:
                idx->field_2cc = 0;
                break;
            }
        }
        v53 = &v51->field_10;
        if (v53)
        {
            do
            {
                if (*((short *)(*((int *)v53) + 1002)) == 30)
                {
                    v54 = *((int *)v53);
                    goto LABEL_441974;
                }
            } while ((v53 = (unsigned int)*((int *)(v53 + 4)), v53));
        }
        v54 = NULL;
LABEL_441974:
        if (v54->field_494)
            v54->field_494();
        v54->field_3c4 = 30;
        sub_455630(v54);
        v55 = idx->field_2cc;
        if (!v55)
        {
LABEL_44199d:
            v57 = NULL;
LABEL_4419eb:
            idx->field_2cc = 0;
        }
        else
        {
            v56 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v56->field_0->padding_0[0]) == v55)
                    {
                        v57 = v56->field_0;
                        goto LABEL_4419e7;
                    }
                } while ((v56 = (st_441530_7 *)v56->field_4, v56));
            }
            v58 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_44199d;
            while (1)
            {
                if (*((int *)&v58->field_0->padding_0[0]) != v55)
                {
                    v58 = v58->field_4;
                    if (!v58)
                    {
                        v57 = NULL;
                        goto LABEL_4419eb;
                    }
                }
                else
                {
                    v57 = v58->field_0;
LABEL_4419e7:
                    if (v57)
                        break;
                    goto LABEL_4419eb;
                }
            }
        }
        v59 = &v57->field_10;
        if (v59)
        {
            do
            {
                if (*((short *)(*((int *)v59) + 1002)) == 31)
                {
                    v60 = *((int *)v59);
                    goto LABEL_441a15;
                }
            } while ((v59 = (unsigned int)*((int *)(v59 + 4)), v59));
        }
        v60 = NULL;
LABEL_441a15:
        if (v60->field_494)
            v60->field_494();
        v60->field_3c4 = 30;
        sub_455630(v60);
        v61 = idx->field_2cc;
        if (!v61)
        {
LABEL_441a3e:
            v63 = 0;
LABEL_441a8b:
            idx->field_2cc = 0;
        }
        else
        {
            v62 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v62->field_0->padding_0[0]) == v61)
                    {
                        v63 = v62->field_0;
                        goto LABEL_441a87;
                    }
                } while ((v62 = (st_441530_7 *)v62->field_4, v62));
            }
            v64 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441a3e;
            while (1)
            {
                if (*((int *)v64->field_0) != v61)
                {
                    v64 = v64->field_4;
                    if (!v64)
                    {
                        v63 = 0;
                        goto LABEL_441a8b;
                    }
                }
                else
                {
                    v63 = v64->field_0;
LABEL_441a87:
                    if (v63)
                        break;
                    goto LABEL_441a8b;
                }
            }
        }
        v65 = v63 + 16;
        if (v65)
        {
            do
            {
                if (*((short *)(*((int *)v65) + 1002)) == 32)
                {
                    v66 = *((int *)v65);
                    goto LABEL_441ab4;
                }
            } while ((v65 = (unsigned int)*((int *)(v65 + 4)), v65));
        }
        v66 = NULL;
LABEL_441ab4:
        if ((int)v66[1172])
            (int)v66[1172]();
        *((unsigned short *)&v66[964]) = 30;
        sub_455630(v66);
        v67 = idx->field_2cc;
        if (!v67)
        {
LABEL_441add:
            v69 = NULL;
LABEL_441b2b:
            idx->field_2cc = 0;
        }
        else
        {
            v68 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v68->field_0->padding_0[0]) == v67)
                    {
                        v69 = v68->field_0;
                        goto LABEL_441b27;
                    }
                } while ((v68 = (st_441530_7 *)v68->field_4, v68));
            }
            v70 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441add;
            while (1)
            {
                if (*((int *)&v70->field_0->padding_0[0]) != v67)
                {
                    v70 = v70->field_4;
                    if (!v70)
                    {
                        v69 = NULL;
                        goto LABEL_441b2b;
                    }
                }
                else
                {
                    v69 = v70->field_0;
LABEL_441b27:
                    if (v69)
                        break;
                    goto LABEL_441b2b;
                }
            }
        }
        v71 = &v69->field_10;
        if (v71)
        {
            do
            {
                if (*((short *)(*((int *)v71) + 1002)) == 33)
                {
                    v72 = *((int *)v71);
                    goto LABEL_441b54;
                }
            } while ((v71 = (unsigned int)*((int *)(v71 + 4)), v71));
        }
        v72 = NULL;
LABEL_441b54:
        if (v72->field_494)
            v72->field_494();
        v72->field_3c4 = 30;
        sub_455630(v72);
        v73 = idx->field_2cc;
        if (!v73)
        {
LABEL_441b7d:
            v75 = NULL;
LABEL_441bcb:
            idx->field_2cc = 0;
        }
        else
        {
            v74 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v74->field_0->padding_0[0]) == v73)
                    {
                        v75 = v74->field_0;
                        goto LABEL_441bc7;
                    }
                } while ((v74 = (st_441530_7 *)v74->field_4, v74));
            }
            v76 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441b7d;
            while (1)
            {
                if (*((int *)&v76->field_0->padding_0[0]) != v73)
                {
                    v76 = v76->field_4;
                    if (!v76)
                    {
                        v75 = NULL;
                        goto LABEL_441bcb;
                    }
                }
                else
                {
                    v75 = v76->field_0;
LABEL_441bc7:
                    if (v75)
                        break;
                    goto LABEL_441bcb;
                }
            }
        }
        v77 = &v75->field_10;
        if (v77)
        {
            do
            {
                if (*((short *)(*((int *)v77) + 1002)) == 0x22)
                {
                    v78 = *((int *)v77);
                    goto LABEL_441bf4;
                }
            } while ((v77 = (unsigned int)*((int *)(v77 + 4)), v77));
        }
        v78 = NULL;
LABEL_441bf4:
        if (v78->field_494)
            v78->field_494();
        v78->field_3c4 = 30;
        sub_455630(v78);
        v79 = idx->field_2cc;
        if (!v79)
        {
LABEL_441c1d:
            v81 = NULL;
LABEL_441c6b:
            idx->field_2cc = 0;
        }
        else
        {
            v80 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v80->field_0->padding_0[0]) == v79)
                    {
                        v81 = v80->field_0;
                        goto LABEL_441c67;
                    }
                } while ((v80 = (st_441530_7 *)v80->field_4, v80));
            }
            v82 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441c1d;
            while (1)
            {
                if (*((int *)&v82->field_0->padding_0[0]) != v79)
                {
                    v82 = v82->field_4;
                    if (!v82)
                    {
                        v81 = NULL;
                        goto LABEL_441c6b;
                    }
                }
                else
                {
                    v81 = v82->field_0;
LABEL_441c67:
                    if (v81)
                        break;
                    goto LABEL_441c6b;
                }
            }
        }
        v83 = &v81->field_10;
        if (v83)
        {
            do
            {
                if (*((short *)(*((int *)v83) + 1002)) == 35)
                {
                    v84 = *((int *)v83);
                    goto LABEL_441c94;
                }
            } while ((v83 = (unsigned int)*((int *)(v83 + 4)), v83));
        }
        v84 = NULL;
LABEL_441c94:
        if (v84->field_494)
            v84->field_494();
        v84->field_3c4 = 30;
        sub_455630(v84);
        v85 = idx->field_2cc;
        if (!v85)
        {
LABEL_441cbd:
            v87 = NULL;
LABEL_441d0b:
            idx->field_2cc = 0;
        }
        else
        {
            v86 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v86->field_0->padding_0[0]) == v85)
                    {
                        v87 = v86->field_0;
                        goto LABEL_441d07;
                    }
                } while ((v86 = (st_441530_7 *)v86->field_4, v86));
            }
            v88 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441cbd;
            while (1)
            {
                if (*((int *)&v88->field_0->padding_0[0]) != v85)
                {
                    v88 = v88->field_4;
                    if (!v88)
                    {
                        v87 = NULL;
                        goto LABEL_441d0b;
                    }
                }
                else
                {
                    v87 = v88->field_0;
LABEL_441d07:
                    if (v87)
                        break;
                    goto LABEL_441d0b;
                }
            }
        }
        v89 = &v87->field_10;
        if (v89)
        {
            do
            {
                if (*((short *)(*((int *)v89) + 1002)) == 36)
                {
                    v90 = *((int *)v89);
                    goto LABEL_441d34;
                }
            } while ((v89 = (unsigned int)*((int *)(v89 + 4)), v89));
        }
        v90 = NULL;
LABEL_441d34:
        if (v90->field_494)
            v90->field_494();
        v90->field_3c4 = 30;
        sub_455630(v90);
    }
    if (idx->field_28 > 1)
    {
        v91 = idx->field_2cc;
        if (!v91)
        {
LABEL_441d67:
            v93 = NULL;
            goto LABEL_441db2;
        }
        else
        {
            v92 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v92->field_0->padding_0[0]) == v91)
                    {
                        v93 = v92->field_0;
                        goto LABEL_441dae;
                    }
                } while ((v92 = (st_441530_7 *)v92->field_4, v92));
            }
            v94 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441d67;
        }
        while (1)
        {
            if (*((int *)&v94->field_0->padding_0[0]) != v91)
            {
                v94 = v94->field_4;
                if (!v94)
                {
                    v93 = NULL;
                    goto LABEL_441db2;
                }
            }
            else
            {
                v93 = v94->field_0;
LABEL_441dae:
                if (v93)
                    break;
LABEL_441db2:
                idx->field_2cc = 0;
                break;
            }
        }
        v95 = &v93->field_10;
        if (v95)
        {
            do
            {
                if (*((short *)(*((int *)v95) + 1002)) == 37)
                {
                    v96 = *((int *)v95);
                    goto LABEL_441dd8;
                }
            } while ((v95 = (unsigned int)*((int *)(v95 + 4)), v95));
        }
        v96 = NULL;
LABEL_441dd8:
        if (v96->field_494)
            v96->field_494();
        v96->field_3c4 = 30;
        sub_455630(v96);
        v97 = idx->field_2cc;
        if (!v97)
        {
LABEL_441e07:
            v99 = NULL;
            goto LABEL_441e52;
        }
        else
        {
            v98 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v98->field_0->padding_0[0]) == v97)
                    {
                        v99 = v98->field_0;
                        goto LABEL_441e4e;
                    }
                } while ((v98 = (st_441530_7 *)v98->field_4, v98));
            }
            v100 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441e07;
        }
        while (1)
        {
            if (*((int *)&v100->field_0->padding_0[0]) != v97)
            {
                v100 = v100->field_4;
                if (!v100)
                {
                    v99 = NULL;
                    goto LABEL_441e52;
                }
            }
            else
            {
                v99 = v100->field_0;
LABEL_441e4e:
                if (v99)
                    break;
LABEL_441e52:
                idx->field_2cc = 0;
                break;
            }
        }
        v101 = &v99->field_10;
        if (v101)
        {
            do
            {
                if (*((short *)(*((int *)v101) + 1002)) == 38)
                {
                    v102 = *((int *)v101);
                    goto LABEL_441e78;
                }
            } while ((v101 = (unsigned int)*((int *)(v101 + 4)), v101));
        }
        v102 = NULL;
LABEL_441e78:
        if (v102->field_494)
            v102->field_494();
        v102->field_3c4 = 30;
        sub_455630(v102);
        v103 = idx->field_2cc;
        if (!v103)
        {
LABEL_441ea7:
            v105 = NULL;
            goto LABEL_441ef2;
        }
        else
        {
            node = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&node->field_0->padding_0[0]) == v103)
                    {
                        v105 = node->field_0;
                        goto LABEL_441eee;
                    }
                } while ((node = (st_441530_7 *)node->field_4, node));
            }
            iter1 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441ea7;
        }
        while (1)
        {
            if (*((int *)&iter1->field_0->padding_0[0]) != v103)
            {
                iter1 = iter1->field_4;
                if (!iter1)
                {
                    v105 = NULL;
                    goto LABEL_441ef2;
                }
            }
            else
            {
                v105 = iter1->field_0;
LABEL_441eee:
                if (v105)
                    break;
LABEL_441ef2:
                idx->field_2cc = 0;
                break;
            }
        }
        iter2 = &v105->field_10;
        if (iter2)
        {
            do
            {
                if (*((short *)(*((int *)iter2) + 1002)) == 39)
                {
                    v108 = *((int *)iter2);
                    goto LABEL_441f18;
                }
            } while ((iter2 = (unsigned int)*((int *)(iter2 + 4)), iter2));
        }
        v108 = NULL;
LABEL_441f18:
        if (v108->field_494)
            v108->field_494();
        v108->field_3c4 = 30;
        sub_455630(v108);
        v109 = idx->field_2cc;
        if (!v109)
        {
LABEL_441f47:
            v111 = NULL;
            goto LABEL_441f92;
        }
        else
        {
            v110 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v110->field_0->padding_0[0]) == v109)
                    {
                        v111 = v110->field_0;
                        goto LABEL_441f8e;
                    }
                } while ((v110 = (st_441530_7 *)v110->field_4, v110));
            }
            v112 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441f47;
        }
        while (1)
        {
            if (*((int *)&v112->field_0->padding_0[0]) != v109)
            {
                v112 = v112->field_4;
                if (!v112)
                {
                    v111 = NULL;
                    goto LABEL_441f92;
                }
            }
            else
            {
                v111 = v112->field_0;
LABEL_441f8e:
                if (v111)
                    break;
LABEL_441f92:
                idx->field_2cc = 0;
                break;
            }
        }
        v113 = &v111->field_10;
        if (v113)
        {
            do
            {
                if (*((short *)(*((int *)v113) + 1002)) == 40)
                {
                    v114 = *((int *)v113);
                    goto LABEL_441fb8;
                }
            } while ((v113 = (unsigned int)*((int *)(v113 + 4)), v113));
        }
        v114 = NULL;
LABEL_441fb8:
        if (v114->field_494)
            v114->field_494();
        v114->field_3c4 = 30;
        sub_455630(v114);
        v115 = idx->field_2cc;
        if (!v115)
        {
LABEL_441fe7:
            v117 = NULL;
            goto LABEL_442032;
        }
        else
        {
            v116 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v116->field_0->padding_0[0]) == v115)
                    {
                        v117 = v116->field_0;
                        goto LABEL_44202e;
                    }
                } while ((v116 = (st_441530_7 *)v116->field_4, v116));
            }
            v118 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_441fe7;
        }
        while (1)
        {
            if (*((int *)&v118->field_0->padding_0[0]) != v115)
            {
                v118 = v118->field_4;
                if (!v118)
                {
                    v117 = NULL;
                    goto LABEL_442032;
                }
            }
            else
            {
                v117 = v118->field_0;
LABEL_44202e:
                if (v117)
                    break;
LABEL_442032:
                idx->field_2cc = 0;
                break;
            }
        }
        v119 = &v117->field_10;
        if (v119)
        {
            do
            {
                if (*((short *)(*((int *)v119) + 1002)) == 41)
                {
                    v120 = *((int *)v119);
                    goto LABEL_442058;
                }
            } while ((v119 = (unsigned int)*((int *)(v119 + 4)), v119));
        }
        v120 = NULL;
LABEL_442058:
        if (v120->field_494)
            v120->field_494();
        v120->field_3c4 = 30;
        sub_455630(v120);
        v121 = idx->field_2cc;
        if (!v121)
        {
LABEL_442087:
            v123 = NULL;
            goto LABEL_4420d2;
        }
        else
        {
            v122 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v122->field_0->padding_0[0]) == v121)
                    {
                        v123 = v122->field_0;
                        goto LABEL_4420ce;
                    }
                } while ((v122 = (st_441530_7 *)v122->field_4, v122));
            }
            v124 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_442087;
        }
        while (1)
        {
            if (*((int *)&v124->field_0->padding_0[0]) != v121)
            {
                v124 = v124->field_4;
                if (!v124)
                {
                    v123 = NULL;
                    goto LABEL_4420d2;
                }
            }
            else
            {
                v123 = v124->field_0;
LABEL_4420ce:
                if (v123)
                    break;
LABEL_4420d2:
                idx->field_2cc = 0;
                break;
            }
        }
        v125 = &v123->field_10;
        if (v125)
        {
            do
            {
                if (*((short *)(*((int *)v125) + 1002)) == 42)
                {
                    v126 = *((int *)v125);
                    goto LABEL_4420f8;
                }
            } while ((v125 = (unsigned int)*((int *)(v125 + 4)), v125));
        }
        v126 = NULL;
LABEL_4420f8:
        if (v126->field_494)
            v126->field_494();
        v126->field_3c4 = 30;
        sub_455630(v126);
        v127 = idx->field_2cc;
        if (!v127)
        {
LABEL_442127:
            v129 = NULL;
            goto LABEL_442172;
        }
        else
        {
            v128 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v128->field_0->padding_0[0]) == v127)
                    {
                        v129 = v128->field_0;
                        goto LABEL_44216e;
                    }
                } while ((v128 = (st_441530_7 *)v128->field_4, v128));
            }
            v130 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_442127;
        }
        while (1)
        {
            if (*((int *)&v130->field_0->padding_0[0]) != v127)
            {
                v130 = v130->field_4;
                if (!v130)
                {
                    v129 = NULL;
                    goto LABEL_442172;
                }
            }
            else
            {
                v129 = v130->field_0;
LABEL_44216e:
                if (v129)
                    break;
LABEL_442172:
                idx->field_2cc = 0;
                break;
            }
        }
        v131 = &v129->field_10;
        if (v131)
        {
            do
            {
                if (*((short *)(*((int *)v131) + 1002)) == 43)
                {
                    v132 = *((int *)v131);
                    goto LABEL_442198;
                }
            } while ((v131 = (unsigned int)*((int *)(v131 + 4)), v131));
        }
        v132 = NULL;
LABEL_442198:
        if (v132->field_494)
            v132->field_494();
        v132->field_3c4 = 30;
        sub_455630(v132);
        v133 = idx->field_2cc;
        if (!v133)
        {
LABEL_4421c7:
            v135 = NULL;
            goto LABEL_442212;
        }
        else
        {
            v134 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v134->field_0->padding_0[0]) == v133)
                    {
                        v135 = v134->field_0;
                        goto LABEL_44220e;
                    }
                } while ((v134 = (st_441530_7 *)v134->field_4, v134));
            }
            v136 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_4421c7;
        }
        while (1)
        {
            if (*((int *)&v136->field_0->padding_0[0]) != v133)
            {
                v136 = v136->field_4;
                if (!v136)
                {
                    v135 = NULL;
                    goto LABEL_442212;
                }
            }
            else
            {
                v135 = v136->field_0;
LABEL_44220e:
                if (v135)
                    break;
LABEL_442212:
                idx->field_2cc = 0;
                break;
            }
        }
        v137 = &v135->field_10;
        if (v137)
        {
            do
            {
                if (*((short *)(*((int *)v137) + 1002)) == 44)
                {
                    v138 = *((int *)v137);
                    goto LABEL_442238;
                }
            } while ((v137 = (unsigned int)*((int *)(v137 + 4)), v137));
        }
        v138 = NULL;
LABEL_442238:
        if (v138->field_494)
            v138->field_494();
        v138->field_3c4 = 30;
        sub_455630(v138);
        return;
    }
    else if (idx->field_28 < 1)
    {
        v139 = idx->field_2cc;
        if (!v139)
        {
LABEL_442278:
            v141 = NULL;
            goto LABEL_4422c2;
        }
        else
        {
            v140 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v140->field_0->padding_0[0]) == v139)
                    {
                        v141 = v140->field_0;
                        goto LABEL_4422be;
                    }
                } while ((v140 = (st_441530_7 *)v140->field_4, v140));
            }
            v142 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_442278;
        }
        while (1)
        {
            if (*((int *)&v142->field_0->padding_0[0]) != v139)
            {
                v142 = v142->field_4;
                if (!v142)
                {
                    v141 = NULL;
                    goto LABEL_4422c2;
                }
            }
            else
            {
                v141 = v142->field_0;
LABEL_4422be:
                if (v141)
                    break;
LABEL_4422c2:
                idx->field_2cc = 0;
                break;
            }
        }
        v143 = &v141->field_10;
        if (v143)
        {
            do
            {
                if (*((short *)(*((int *)v143) + 1002)) == 37)
                {
                    v144 = *((int *)v143);
                    goto LABEL_4422e8;
                }
            } while ((v143 = (unsigned int)*((int *)(v143 + 4)), v143));
        }
        v144 = NULL;
LABEL_4422e8:
        if (v144->field_494)
            v144->field_494();
        v144->field_3c4 = 31;
        sub_455630(v144);
        v145 = idx->field_2cc;
        if (!v145)
        {
LABEL_442317:
            v147 = NULL;
            goto LABEL_442362;
        }
        else
        {
            v146 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v146->field_0->padding_0[0]) == v145)
                    {
                        v147 = v146->field_0;
                        goto LABEL_44235e;
                    }
                } while ((v146 = (st_441530_7 *)v146->field_4, v146));
            }
            v148 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_442317;
        }
        while (1)
        {
            if (*((int *)&v148->field_0->padding_0[0]) != v145)
            {
                v148 = v148->field_4;
                if (!v148)
                {
                    v147 = NULL;
                    goto LABEL_442362;
                }
            }
            else
            {
                v147 = v148->field_0;
LABEL_44235e:
                if (v147)
                    break;
LABEL_442362:
                idx->field_2cc = 0;
                break;
            }
        }
        v149 = &v147->field_10;
        if (v149)
        {
            do
            {
                if (*((short *)(*((int *)v149) + 1002)) == 38)
                {
                    v150 = *((int *)v149);
                    goto LABEL_442388;
                }
            } while ((v149 = (unsigned int)*((int *)(v149 + 4)), v149));
        }
        v150 = NULL;
LABEL_442388:
        if (v150->field_494)
            v150->field_494();
        v150->field_3c4 = 31;
        sub_455630(v150);
        v151 = idx->field_2cc;
        if (!v151)
        {
LABEL_4423b7:
            v153 = NULL;
            goto LABEL_442402;
        }
        else
        {
            v152 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&v152->field_0->padding_0[0]) == v151)
                    {
                        v153 = v152->field_0;
                        goto LABEL_4423fe;
                    }
                } while ((v152 = (st_441530_7 *)v152->field_4, v152));
            }
            v154 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_4423b7;
        }
        while (1)
        {
            if (*((int *)&v154->field_0->padding_0[0]) != v151)
            {
                v154 = v154->field_4;
                if (!v154)
                {
                    v153 = NULL;
                    goto LABEL_442402;
                }
            }
            else
            {
                v153 = v154->field_0;
LABEL_4423fe:
                if (v153)
                    break;
LABEL_442402:
                idx->field_2cc = 0;
                break;
            }
        }
        v155 = &v153->field_10;
        if (v155)
        {
            do
            {
                if (*((short *)(*((int *)v155) + 1002)) == 39)
                {
                    v156 = *((int *)v155);
                    goto LABEL_442438;
                }
            } while ((v155 = (unsigned int)*((int *)(v155 + 4)), v155));
        }
        v156 = NULL;
LABEL_442438:
        if (v156->field_494)
            v156->field_494();
        v156->field_3c4 = 31;
        sub_455630(v156);
        v157 = sub_461920(idx->field_2cc);
        if (!v157)
            idx->field_2cc = 0;
        v158 = &v157->field_10;
        if (v158)
        {
            do
            {
                if (*((short *)(*((int *)&v158->padding_0[0]) + 1002)) == 40)
                {
                    v159 = *((int *)&v158->padding_0[0]);
                    goto LABEL_4424a8;
                }
            } while ((v158 = (st_441530_172 *)*((int *)&v158->padding_0[4]), v158));
        }
        v159 = NULL;
LABEL_4424a8:
        if (v159->field_494)
            v159->field_494();
        v159->field_3c4 = 31;
        sub_455630(v159);
        v160 = sub_461920(idx->field_2cc);
        if (!v160)
            idx->field_2cc = 0;
        v161 = &v160->field_10;
        if (v161)
        {
            do
            {
                if (*((short *)(*((int *)&v161->padding_0[0]) + 1002)) == 41)
                {
                    v162 = *((int *)&v161->padding_0[0]);
                    goto LABEL_442518;
                }
            } while ((v161 = (st_441530_172 *)*((int *)&v161->padding_0[4]), v161));
        }
        v162 = NULL;
LABEL_442518:
        if (v162->field_494)
            v162->field_494();
        v162->field_3c4 = 31;
        sub_455630(v162);
        v163 = sub_461920(idx->field_2cc);
        if (!v163)
            idx->field_2cc = 0;
        v164 = &v163->field_10;
        if (v164)
        {
            do
            {
                if (*((short *)(*((int *)&v164->padding_0[0]) + 1002)) == 42)
                {
                    v165 = *((int *)&v164->padding_0[0]);
                    goto LABEL_442588;
                }
            } while ((v164 = (st_441530_172 *)*((int *)&v164->padding_0[4]), v164));
        }
        v165 = NULL;
LABEL_442588:
        if (v165->field_494)
            v165->field_494();
        v165->field_3c4 = 31;
        sub_455630(v165);
        v166 = sub_461920(idx->field_2cc);
        if (!v166)
            idx->field_2cc = 0;
        v167 = &v166->field_10;
        if (v167)
        {
            do
            {
                if (*((short *)(*((int *)&v167->padding_0[0]) + 1002)) == 43)
                {
                    v168 = *((int *)&v167->padding_0[0]);
                    goto LABEL_4425f8;
                }
            } while ((v167 = (st_441530_172 *)*((int *)&v167->padding_0[4]), v167));
        }
        v168 = NULL;
LABEL_4425f8:
        if (v168->field_494)
            v168->field_494();
        v168->field_3c4 = 31;
        sub_455630(v168);
        v169 = sub_461920(idx->field_2cc);
        if (!v169)
            idx->field_2cc = 0;
        v170 = &v169->field_10;
        if (v170)
        {
            do
            {
                if (*((short *)(*((int *)&v170->padding_0[0]) + 1002)) == 44)
                {
                    v171 = *((int *)&v170->padding_0[0]);
                    goto LABEL_442659;
                }
            } while ((v170 = (st_441530_172 *)*((int *)&v170->padding_0[4]), v170));
        }
        v171 = NULL;
LABEL_442659:
        if (v171->field_494)
            v171->field_494();
        v171->field_3c4 = 31;
        sub_455630(v171);
        return;
    }
    else
    {
        return;
    }
}
