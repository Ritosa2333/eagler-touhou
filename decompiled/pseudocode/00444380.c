// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x444380; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_444380_0 {
    struct st_444380_0 *field_0;
    struct st_444380_0 *field_4;
    char padding_8[8];
    struct st_444380_1 *field_10;
    struct st_444380_0 *field_14;
} st_444380_0;

typedef struct st_444380_2 {
    unsigned int field_0;
} st_444380_2;

typedef struct st_444380_9 {
    char padding_0[720];
    struct st_444380_0 *field_2d0;
} st_444380_9;

typedef struct st_444380_18 {
    char padding_0[964];
    unsigned short field_3c4;
} st_444380_18;

typedef struct st_444380_1 {
    char padding_0[40];
    int field_28;
    char padding_2c[676];
    struct st_444380_0 *field_2d0;
    char padding_2d4[278];
    short field_3ea;
    char padding_3ec[168];
    struct st_444380_2 *field_494;
} st_444380_1;

extern unsigned int g_4ce8cc;

void sub_444380(st_444380_1 *a0)
{
    unsigned int v10;  // ebx
    st_444380_1 *idx;  // ebp
    st_444380_0 *iter;  // eax
    st_444380_0 *node;  // eax
    st_444380_0 *iter1;  // ecx
    st_444380_18 *v112;  // esi
    st_444380_18 *v113;  // esi
    st_444380_0 *iter2;  // ecx
    unsigned int v22;  // esi
    unsigned int v23;  // esi
    st_444380_0 *k;  // ecx
    int v25;  // esi
    st_444380_0 *v26;  // eax
    st_444380_0 *v27;  // eax
    st_444380_0 *v28;  // eax
    st_444380_0 *v29;  // ecx
    st_444380_0 *v12;  // esi
    st_444380_18 *v30;  // esi
    st_444380_18 *index;  // esi
    st_444380_0 *v32;  // ecx
    unsigned int v33;  // esi
    st_444380_0 *v34;  // eax
    st_444380_0 *v35;  // eax
    st_444380_0 *v36;  // eax
    st_444380_0 *v37;  // ecx
    st_444380_18 *v38;  // esi
    st_444380_18 *v39;  // esi
    unsigned int v13;  // edi
    st_444380_0 *v40;  // ecx
    unsigned int v41;  // esi
    st_444380_0 *v42;  // eax
    st_444380_0 *v43;  // eax
    st_444380_0 *v44;  // eax
    st_444380_0 *v45;  // ecx
    st_444380_18 *v46;  // esi
    st_444380_18 *v47;  // esi
    st_444380_0 *v48;  // ecx
    unsigned int v49;  // esi
    st_444380_0 *v14;  // ebx
    st_444380_0 *v50;  // eax
    st_444380_0 *v51;  // eax
    st_444380_0 *v52;  // eax
    st_444380_0 *v53;  // ecx
    st_444380_18 *v54;  // esi
    st_444380_18 *v55;  // esi
    st_444380_0 *v56;  // ecx
    unsigned int v57;  // esi
    st_444380_0 *v58;  // eax
    st_444380_0 *v59;  // eax
    int v15;  // edi
    st_444380_0 *v60;  // eax
    st_444380_0 *v61;  // ecx
    st_444380_18 *v62;  // esi
    st_444380_18 *v63;  // esi
    int v64;  // edi
    int i;  // edi
    st_444380_0 *l;  // ecx
    int v67;  // esi
    st_444380_0 *v68;  // eax
    st_444380_0 *v69;  // eax
    st_444380_0 *j;  // ecx
    st_444380_0 *v70;  // eax
    st_444380_0 *v71;  // ecx
    int v72;  // esi
    int idx1;  // esi
    st_444380_0 *m;  // ecx
    st_444380_0 *v75;  // eax
    st_444380_0 *v76;  // eax
    st_444380_0 *v77;  // eax
    st_444380_0 *v78;  // ecx
    int v79;  // esi
    int v17;  // esi
    int idx2;  // esi
    unsigned int v81;  // edi
    st_444380_0 *n;  // ecx
    unsigned int v83;  // esi
    st_444380_0 *v84;  // eax
    st_444380_0 *v85;  // eax
    st_444380_0 *v86;  // eax
    st_444380_0 *v87;  // ecx
    st_444380_18 *v88;  // esi
    st_444380_18 *v89;  // esi
    st_444380_0 *v18;  // eax
    st_444380_0 *i0;  // ecx
    unsigned int v91;  // esi
    st_444380_0 *v92;  // eax
    st_444380_0 *v93;  // eax
    st_444380_0 *v94;  // eax
    st_444380_0 *v95;  // ecx
    st_444380_18 *v96;  // esi
    st_444380_18 *v97;  // esi
    st_444380_0 *i1;  // ecx
    unsigned int v99;  // esi
    st_444380_0 *v19;  // eax
    st_444380_0 *v100;  // eax
    st_444380_0 *v101;  // eax
    st_444380_0 *v102;  // eax
    st_444380_0 *v103;  // ecx
    st_444380_18 *v104;  // esi
    st_444380_18 *v105;  // esi
    st_444380_0 *i2;  // ecx
    unsigned int v107;  // esi
    st_444380_0 *v108;  // eax
    st_444380_0 *v109;  // eax
    st_444380_1 *v0;  // [bp-0x3c]
    st_444380_9 *v1;  // [bp-0x28]
    st_444380_9 *v2;  // [bp-0x24]
    st_444380_1 *v3;  // [bp-0x20]
    st_444380_1 *v4;  // [bp-0x1c]
    st_444380_9 *v5;  // [bp-0x18]
    st_444380_1 *v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0x10]
    st_444380_0 *v8;  // [bp-0xc]
    unsigned int v9;  // [bp-0x4]

    v9 = v10;
    idx = a0;
    v8 = v12;
    v7 = v13;
    v14 = NULL;
    v15 = 0;
    if (idx->field_28 > 0)
    {
        do
        {
            j = idx->field_2d0;
            v17 = v15 + 45;
            if (j == v14)
            {
                v18 = NULL;
                goto LABEL_4443ef;
            }
            v19 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)) != v14)
            {
                do
                {
                    if (v19->field_0->field_0 == j)
                    {
                        v18 = v19->field_0;
                        goto LABEL_4443e7;
                    }
                } while ((v19 = (st_444380_0 *)v19->field_4, v19 != v14));
            }
            iter = *((int *)(g_4ce8cc + 8935104));
            if (*((int *)(g_4ce8cc + 8935104)) != v14)
            {
                v15 = v15;
            }
            else
            {
LABEL_4443e1:
                v18 = NULL;
                goto LABEL_4443eb;
            }
            while (iter->field_0->field_0 != j)
            {
                if (iter->field_4 == v14)
                    goto LABEL_4443e1;
            }
            v18 = iter->field_0;
LABEL_4443e7:
            if (v18 != v14)
                goto LABEL_4443f5;
LABEL_4443eb:
            idx = v6;
LABEL_4443ef:
            idx->field_2d0 = v14;
LABEL_4443f5:
            iter2 = &v18->field_10;
            if (iter2 != v14)
            {
                do
                {
                    if (*((short *)((char *)&iter2->field_0[41].field_10 + 2)) == v17 || v17 == 0xffffffff && v22 != v18)
                    {
                        v22 = iter2->field_0;
                        v23 = v22;
                        goto LABEL_44441f;
                    }
                } while ((iter2 = (st_444380_0 *)iter2->field_4, iter2 != v14));
            }
            v23 = 0;
LABEL_44441f:
            if (*((int *)(v23 + 1172)) != v14)
                (*((int *)(v23 + 1172)))();
            *((unsigned short *)(v23 + 964)) = 30;
            sub_455630(v23);
            k = v5->field_2d0;
            v25 = v15 + 52;
            if (k == v14)
            {
LABEL_444455:
                v27 = NULL;
                goto LABEL_4444ab;
            }
            else
            {
                v26 = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)) != v14)
                {
                    do
                    {
                        if (v26->field_0->field_0 == k)
                        {
                            v27 = v26->field_0;
                            goto LABEL_4444a7;
                        }
                    } while ((v26 = (st_444380_0 *)v26->field_4, v26 != v14));
                }
                v28 = *((int *)(g_4ce8cc + 8935104));
                if (*((int *)(g_4ce8cc + 8935104)) == v14)
                    goto LABEL_444455;
                while (v28->field_0->field_0 != k)
                {
                    if (v28->field_4 == v14)
                    {
                        v27 = NULL;
                        goto LABEL_4444ab;
                    }
                }
                v27 = v28->field_0;
LABEL_4444a7:
                if (v27 != v14)
                    goto LABEL_4444b5;
LABEL_4444ab:
                v5->field_2d0 = v14;
LABEL_4444b5:
                v29 = &v27->field_10;
                if (v29 != v14)
                {
                    do
                    {
                        if (*((short *)((char *)&v29->field_0[41].field_10 + 2)) == v25 || v25 == 0xffffffff && v30 != v27)
                        {
                            v30 = v29->field_0;
                            index = v30;
                            goto LABEL_4444df;
                        }
                    } while ((v29 = (st_444380_0 *)v29->field_4, v29 != v14));
                }
                index = NULL;
LABEL_4444df:
                if (*((int *)&index[1].padding_0[206]) != v14)
                    (*((int *)&index[1].padding_0[206]))();
                index->field_3c4 = 30;
                sub_455630(index);
                if (v15 < 5)
                {
                    v32 = v4->field_2d0;
                    v33 = v15 * 2 + 59;
                    if (v32 != v14)
                    {
                        v34 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)) != v14)
                        {
                            do
                            {
                                if (v34->field_0->field_0 == v32)
                                {
                                    v35 = v34->field_0;
                                    goto LABEL_444567;
                                }
                            } while ((v34 = (st_444380_0 *)v34->field_4, v34 != v14));
                        }
                        v36 = *((int *)(g_4ce8cc + 8935104));
                        if (*((int *)(g_4ce8cc + 8935104)) != v14)
                        {
                            while (1)
                            {
                                if (v36->field_0->field_0 != v32)
                                {
                                    v36 = v36->field_4;
                                    if (v36 == v14)
                                    {
                                        v35 = NULL;
                                        goto LABEL_44456b;
                                    }
                                }
                                else
                                {
                                    v35 = v36->field_0;
LABEL_444567:
                                    if (v35 != v14)
                                        goto LABEL_444575;
                                    goto LABEL_44456b;
                                }
                            }
                        }
                    }
                    v35 = NULL;
LABEL_44456b:
                    v4->field_2d0 = v14;
LABEL_444575:
                    v37 = &v35->field_10;
                    if (v37 != v14)
                    {
                        do
                        {
                            if (*((short *)((char *)&v37->field_0[41].field_10 + 2)) == v33 || v33 == 0xffffffff && v38 != v35)
                            {
                                v38 = v37->field_0;
                                v39 = v38;
                                goto LABEL_44459f;
                            }
                        } while ((v37 = (st_444380_0 *)v37->field_4, v37 != v14));
                    }
                    v39 = NULL;
LABEL_44459f:
                    if (*((int *)&v39[1].padding_0[206]) != v14)
                        (*((int *)&v39[1].padding_0[206]))();
                    v39->field_3c4 = 30;
                    sub_455630(v39);
                    v40 = v3->field_2d0;
                    v41 = v15 * 2 + 60;
                    if (v40 != v14)
                    {
                        v42 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)) != v14)
                        {
                            v15 = v15;
                            do
                            {
                                if (v42->field_0->field_0 == v40)
                                {
                                    v43 = v42->field_0;
                                    goto LABEL_44461f;
                                }
                            } while ((v42 = (st_444380_0 *)v42->field_4, v42 != v14));
                        }
                        v44 = *((int *)(g_4ce8cc + 8935104));
                        if (*((int *)(g_4ce8cc + 8935104)) != v14)
                        {
                            while (1)
                            {
                                if (v44->field_0->field_0 != v40)
                                {
                                    v44 = v44->field_4;
                                    if (v44 == v14)
                                    {
                                        v43 = NULL;
                                        goto LABEL_444623;
                                    }
                                }
                                else
                                {
                                    v43 = v44->field_0;
LABEL_44461f:
                                    if (v43 != v14)
                                        goto LABEL_44462d;
                                    goto LABEL_444623;
                                }
                            }
                        }
                    }
                    v43 = NULL;
LABEL_444623:
                    v3->field_2d0 = v14;
LABEL_44462d:
                    v45 = &v43->field_10;
                    if (v45 != v14)
                    {
                        do
                        {
                            if (*((short *)((char *)&v45->field_0[41].field_10 + 2)) == v41 || v41 == 0xffffffff && v46 != v43)
                            {
                                v46 = v45->field_0;
                                v47 = v46;
                                goto LABEL_444653;
                            }
                        } while ((v45 = (st_444380_0 *)v45->field_4, v45 != v14));
                    }
                    v47 = NULL;
LABEL_444653:
                    if (*((int *)&v47[1].padding_0[206]) != v14)
                        (*((int *)&v47[1].padding_0[206]))();
                    v47->field_3c4 = 30;
                    sub_455630(v47);
                    v48 = v2->field_2d0;
                    v49 = v15 * 2 + 69;
                    if (v48 != v14)
                    {
                        v50 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)) != v14)
                        {
                            do
                            {
                                if (v50->field_0->field_0 == v48)
                                {
                                    v51 = v50->field_0;
                                    goto LABEL_4446d7;
                                }
                            } while ((v50 = (st_444380_0 *)v50->field_4, v50 != v14));
                        }
                        v52 = *((int *)(g_4ce8cc + 8935104));
                        if (*((int *)(g_4ce8cc + 8935104)) != v14)
                        {
                            v14 = v14;
                            while (1)
                            {
                                if (v52->field_0->field_0 != v48)
                                {
                                    v52 = v52->field_4;
                                    if (v52 == v14)
                                    {
                                        v51 = NULL;
                                        goto LABEL_4446db;
                                    }
                                }
                                else
                                {
                                    v51 = v52->field_0;
LABEL_4446d7:
                                    if (v51 != v14)
                                        goto LABEL_4446e5;
                                    goto LABEL_4446db;
                                }
                            }
                        }
                    }
                    v51 = NULL;
LABEL_4446db:
                    v2->field_2d0 = v14;
LABEL_4446e5:
                    v53 = &v51->field_10;
                    if (v53 != v14)
                    {
                        do
                        {
                            if (*((short *)((char *)&v53->field_0[41].field_10 + 2)) == v49 || v49 == 0xffffffff && v54 != v51)
                            {
                                v54 = v53->field_0;
                                v55 = v54;
                                goto LABEL_44470f;
                            }
                        } while ((v53 = (st_444380_0 *)v53->field_4, v53 != v14));
                    }
                    v55 = NULL;
LABEL_44470f:
                    if (*((int *)&v55[1].padding_0[206]) != v14)
                        (*((int *)&v55[1].padding_0[206]))();
                    v55->field_3c4 = 30;
                    sub_455630(v55);
                    v56 = v1->field_2d0;
                    v57 = v15 * 2 + 70;
                    if (v56 != v14)
                    {
                        v58 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)) != v14)
                        {
                            v15 = v15;
                            do
                            {
                                if (v58->field_0->field_0 == v56)
                                {
                                    v59 = v58->field_0;
                                    goto LABEL_44478f;
                                }
                            } while ((v58 = (st_444380_0 *)v58->field_4, v58 != v14));
                        }
                        v60 = *((int *)(g_4ce8cc + 8935104));
                        if (*((int *)(g_4ce8cc + 8935104)) != v14)
                        {
                            while (1)
                            {
                                if (v60->field_0->field_0 != v56)
                                {
                                    v60 = v60->field_4;
                                    if (v60 == v14)
                                    {
                                        v59 = NULL;
                                        goto LABEL_444793;
                                    }
                                }
                                else
                                {
                                    v59 = v60->field_0;
LABEL_44478f:
                                    if (v59 != v14)
                                        goto LABEL_44479d;
                                    goto LABEL_444793;
                                }
                            }
                        }
                    }
                    v59 = NULL;
LABEL_444793:
                    v1->field_2d0 = v14;
LABEL_44479d:
                    v61 = &v59->field_10;
                    if (v61 != v14)
                    {
                        do
                        {
                            if (*((short *)((char *)&v61->field_0[41].field_10 + 2)) == v57 || v57 == 0xffffffff && v62 != v59)
                            {
                                v62 = v61->field_0;
                                v63 = v62;
                                goto LABEL_4447c3;
                            }
                        } while ((v61 = (st_444380_0 *)v61->field_4, v61 != v14));
                    }
                    v63 = NULL;
LABEL_4447c3:
                    if (*((int *)&v63[1].padding_0[206]) != v14)
                        (*((int *)&v63[1].padding_0[206]))();
                    v63->field_3c4 = 30;
                    sub_455630(v63);
                }
            }
        } while ((idx = v6, v15 = (int)(v15 + 1), v15 < idx->field_28));
    }
    v64 = v15 + 1;
    if (v64 < 7)
    {
        i = v64 + 52;
        do
        {
            l = idx->field_2d0;
            v67 = i - 7;
            if (l == v14)
            {
                v68 = NULL;
                goto LABEL_44485f;
            }
            v69 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)) != v14)
            {
                do
                {
                    if (v69->field_0->field_0 == l)
                    {
                        v68 = v69->field_0;
                        goto LABEL_444857;
                    }
                } while ((v69 = (st_444380_0 *)v69->field_4, v69 != v14));
            }
            v70 = *((int *)(g_4ce8cc + 8935104));
            if (*((int *)(g_4ce8cc + 8935104)) == v14)
            {
LABEL_444851:
                v68 = NULL;
                goto LABEL_44485b;
            }
            while (v70->field_0->field_0 != l)
            {
                if (v70->field_4 == v14)
                    goto LABEL_444851;
            }
            v68 = v70->field_0;
LABEL_444857:
            if (v68 != v14)
                goto LABEL_444865;
LABEL_44485b:
            idx = v4;
LABEL_44485f:
            idx->field_2d0 = v14;
LABEL_444865:
            v71 = &v68->field_10;
            if (v71 != v14)
            {
                do
                {
                    if (*((short *)((char *)&v71->field_0[41].field_10 + 2)) == v67 || v67 == 0xffffffff && v72 != v68)
                    {
                        v72 = v71->field_0;
                        idx1 = v72;
                        goto LABEL_44488f;
                    }
                } while ((v71 = (st_444380_0 *)v71->field_4, v71 != v14));
            }
            idx1 = 0;
LABEL_44488f:
            if (*((int *)(idx1 + 1172)) != v14)
                (*((int *)(idx1 + 1172)))();
            *((unsigned short *)(idx1 + 964)) = 31;
            sub_455630(idx1);
            idx = v3;
            m = idx->field_2d0;
            if (m == v14)
            {
LABEL_4448c2:
                v76 = NULL;
                goto LABEL_444912;
            }
            else
            {
                v75 = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)) != v14)
                {
                    i = i;
                    do
                    {
                        if (v75->field_0->field_0 == m)
                        {
                            v76 = v75->field_0;
                            goto LABEL_44490e;
                        }
                    } while ((v75 = (st_444380_0 *)v75->field_4, v75 != v14));
                }
                v77 = *((int *)(g_4ce8cc + 8935104));
                if (*((int *)(g_4ce8cc + 8935104)) == v14)
                    goto LABEL_4448c2;
                while (v77->field_0->field_0 != m)
                {
                    if (v77->field_4 == v14)
                    {
                        v76 = NULL;
                        goto LABEL_444912;
                    }
                }
                v76 = v77->field_0;
LABEL_44490e:
                if (v76 != v14)
                    goto LABEL_444918;
LABEL_444912:
                idx->field_2d0 = v14;
            }
LABEL_444918:
            v78 = &v76->field_10;
            if (v78 != v14)
            {
                do
                {
                    if (*((short *)((char *)&v78->field_0[41].field_10 + 2)) == i || i == 0xffffffff && v79 != v76)
                    {
                        v79 = v78->field_0;
                        idx2 = v79;
                        goto LABEL_44493f;
                    }
                } while ((v78 = (st_444380_0 *)v78->field_4, v78 != v14));
            }
            idx2 = 0;
LABEL_44493f:
            if (*((int *)(idx2 + 1172)) != v14)
                (*((int *)(idx2 + 1172)))();
            *((unsigned short *)(idx2 + 964)) = 31;
            sub_455630(idx2);
            i += 1;
        } while (i < 59);
    }
    v81 = idx->field_28 + 1;
    if (v81 >= 5)
        return;
    while (1)
    {
        n = idx->field_2d0;
        v83 = v81 * 2 + 59;
        if (n == v14)
        {
            v84 = NULL;
            goto LABEL_4449df;
        }
        v85 = *((int *)(g_4ce8cc + 8935096));
        if (*((int *)(g_4ce8cc + 8935096)) != v14)
        {
            do
            {
                if (v85->field_0->field_0 == n)
                {
                    v84 = v85->field_0;
                    goto LABEL_4449d7;
                }
            } while ((v85 = (st_444380_0 *)v85->field_4, v85 != v14));
        }
        v86 = *((int *)(g_4ce8cc + 8935104));
        if (*((int *)(g_4ce8cc + 8935104)) == v14)
        {
LABEL_4449d1:
            v84 = NULL;
            goto LABEL_4449db;
        }
        while (v86->field_0->field_0 != n)
        {
            if (v86->field_4 == v14)
                goto LABEL_4449d1;
        }
        v84 = v86->field_0;
LABEL_4449d7:
        if (v84 != v14)
            goto LABEL_4449e5;
LABEL_4449db:
        idx = v23;
LABEL_4449df:
        idx->field_2d0 = v14;
LABEL_4449e5:
        v87 = &v84->field_10;
        if (v87 != v14)
        {
            do
            {
                if (*((short *)((char *)&v87->field_0[41].field_10 + 2)) == v83 || v83 == 0xffffffff && v88 != v84)
                {
                    v88 = v87->field_0;
                    v89 = v88;
                    goto LABEL_444a0f;
                }
            } while ((v87 = (st_444380_0 *)v87->field_4, v87 != v14));
        }
        v89 = NULL;
LABEL_444a0f:
        if (*((int *)&v89[1].padding_0[206]) != v14)
            (*((int *)&v89[1].padding_0[206]))();
        v89->field_3c4 = 31;
        sub_455630(v89);
        i0 = *((int *)&index->padding_0[720]);
        v91 = v81 * 2 + 60;
        if (i0 == v14)
        {
LABEL_444a46:
            v93 = NULL;
            goto LABEL_444a9b;
        }
        v92 = *((int *)(g_4ce8cc + 8935096));
        if (*((int *)(g_4ce8cc + 8935096)) != v14)
        {
            do
            {
                if (v92->field_0->field_0 == i0)
                {
                    v93 = v92->field_0;
                    goto LABEL_444a97;
                }
            } while ((v92 = (st_444380_0 *)v92->field_4, v92 != v14));
        }
        v94 = *((int *)(g_4ce8cc + 8935104));
        if (*((int *)(g_4ce8cc + 8935104)) == v14)
            goto LABEL_444a46;
        v14 = v14;
        while (v94->field_0->field_0 != i0)
        {
            if (v94->field_4 == v14)
            {
                v93 = NULL;
                goto LABEL_444a9b;
            }
        }
        v93 = v94->field_0;
LABEL_444a97:
        if (v93 != v14)
            goto LABEL_444aa5;
LABEL_444a9b:
        *((st_444380_0 **)&index->padding_0[720]) = v14;
LABEL_444aa5:
        v95 = &v93->field_10;
        if (v95 != v14)
        {
            do
            {
                if (*((short *)((char *)&v95->field_0[41].field_10 + 2)) == v91 || v91 == 0xffffffff && v96 != v93)
                {
                    v96 = v95->field_0;
                    v97 = v96;
                    goto LABEL_444acf;
                }
            } while ((v95 = (st_444380_0 *)v95->field_4, v95 != v14));
        }
        v97 = NULL;
LABEL_444acf:
        if (*((int *)&v97[1].padding_0[206]) != v14)
            (*((int *)&v97[1].padding_0[206]))();
        v97->field_3c4 = 31;
        sub_455630(v97);
        i1 = *((int *)(idx1 + 720));
        v99 = v81 * 2 + 69;
        if (i1 == v14)
        {
LABEL_444b06:
            v101 = NULL;
            goto LABEL_444b53;
        }
        v100 = *((int *)(g_4ce8cc + 8935096));
        if (*((int *)(g_4ce8cc + 8935096)) != v14)
        {
            v81 = v81;
            do
            {
                if (v100->field_0->field_0 == i1)
                {
                    v101 = v100->field_0;
                    goto LABEL_444b4f;
                }
            } while ((v100 = (st_444380_0 *)v100->field_4, v100 != v14));
        }
        v102 = *((int *)(g_4ce8cc + 8935104));
        if (*((int *)(g_4ce8cc + 8935104)) == v14)
            goto LABEL_444b06;
        while (v102->field_0->field_0 != i1)
        {
            if (v102->field_4 == v14)
            {
                v101 = NULL;
                goto LABEL_444b53;
            }
        }
        v101 = v102->field_0;
LABEL_444b4f:
        if (v101 != v14)
            goto LABEL_444b5d;
LABEL_444b53:
        *((st_444380_0 **)(idx1 + 720)) = v14;
LABEL_444b5d:
        v103 = &v101->field_10;
        if (v103 != v14)
        {
            do
            {
                if (*((short *)((char *)&v103->field_0[41].field_10 + 2)) == v99 || v99 == 0xffffffff && v104 != v101)
                {
                    v104 = v103->field_0;
                    v105 = v104;
                    goto LABEL_444b83;
                }
            } while ((v103 = (st_444380_0 *)v103->field_4, v103 != v14));
        }
        v105 = NULL;
LABEL_444b83:
        if (*((int *)&v105[1].padding_0[206]) != v14)
            (*((int *)&v105[1].padding_0[206]))();
        v105->field_3c4 = 31;
        sub_455630(v105);
        i2 = *((int *)(idx2 + 720));
        v107 = v81 * 2 + 70;
        if (i2 == v14)
        {
LABEL_444bba:
            v109 = NULL;
            goto LABEL_444c0b;
        }
        v108 = *((int *)(g_4ce8cc + 8935096));
        if (*((int *)(g_4ce8cc + 8935096)) != v14)
        {
            do
            {
                if (v108->field_0->field_0 == i2)
                {
                    v109 = v108->field_0;
                    goto LABEL_444c07;
                }
            } while ((v108 = (st_444380_0 *)v108->field_4, v108 != v14));
        }
        node = *((int *)(g_4ce8cc + 8935104));
        if (*((int *)(g_4ce8cc + 8935104)) == v14)
            goto LABEL_444bba;
        v14 = v14;
        while (node->field_0->field_0 != i2)
        {
            if (node->field_4 == v14)
            {
                v109 = NULL;
                goto LABEL_444c0b;
            }
        }
        v109 = node->field_0;
LABEL_444c07:
        if (v109 != v14)
            goto LABEL_444c15;
LABEL_444c0b:
        *((st_444380_0 **)(idx2 + 720)) = v14;
LABEL_444c15:
        iter1 = &v109->field_10;
        if (iter1 != v14)
        {
            do
            {
                if (*((short *)((char *)&iter1->field_0[41].field_10 + 2)) == v107 || v107 == 0xffffffff && v112 != v109)
                {
                    v112 = iter1->field_0;
                    v113 = v112;
                    goto LABEL_444c3f;
                }
            } while ((iter1 = (st_444380_0 *)iter1->field_4, iter1 != v14));
        }
        v113 = NULL;
LABEL_444c3f:
        if (*((int *)&v113[1].padding_0[206]) != v14)
            (*((int *)&v113[1].padding_0[206]))();
        v113->field_3c4 = 31;
        sub_455630(v113);
        v81 += 1;
        if (v81 >= 5)
            break;
        idx = v0;
    }
    return;
}
