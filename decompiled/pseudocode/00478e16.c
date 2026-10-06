// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x478e16; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_478e16_5 {
    char padding_0[12];
    char field_c;
} st_478e16_5;

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_478e16_3 {
    char padding_0[172];
    int field_ac;
    char padding_b0[12];
    struct st_478e16_4 *field_bc;
} st_478e16_3;

typedef struct st_478e16_2 {
    char field_0;
    char field_1;
    char field_2;
} st_478e16_2;

typedef struct st_478e16_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[20];
    char field_24;
} st_478e16_1;

typedef struct st_478e16_4 {
    unsigned int field_0;
} st_478e16_4;

typedef struct st_478e16_6 {
    char padding_0[112];
    unsigned int field_70;
} st_478e16_6;

typedef struct st_478e16_0 {
    int field_0;
    char field_1;
    char field_2;
    char padding_3[1];
    int field_4;
} st_478e16_0;

extern st_478e16_1 g_4adbb0;
extern int g_4ade8c;
extern st_478e16_1 g_4d6320;

int sub_478e16(void)
{
    unsigned int v46;  // esi
    unsigned int v47;  // edi
    st_478e16_2 *iter;  // esi
    unsigned int v57;  // ebx
    unsigned int v58;  // ecx
    st_478e16_2 *v59;  // eax
    st_478e16_2 *v60;  // eax
    void* v61;  // eax
    st_478e16_0 *iter1;  // ebx
    unsigned int v63;  // edi
    unsigned int v64;  // eax
    unsigned int v65;  // ecx
    unsigned int v48;  // ebx
    int v66;  // eax
    int iter2;  // ebx
    unsigned int v68;  // eax
    unsigned int v70;  // ecx
    unsigned int v72;  // eax
    int v49;  // eax
    unsigned int v77;  // ecx
    unsigned int v78;  // eax
    unsigned int v79;  // eax
    unsigned int v81;  // eax
    unsigned int v82;  // eax
    unsigned int v83;  // eax
    unsigned int v84;  // eax
    st_478e16_1 *v50;  // edx
    st_478e16_2 *v87;  // esi
    char v88;  // dl
    char v89;  // al
    char v90;  // cl
    char v91;  // al
    unsigned int v92;  // edi
    unsigned int v93;  // edx
    unsigned int k;  // edx
    st_478e16_1 *v51;  // ecx
    st_478e16_5 *v96;  // esi
    st_478e16_5 *v97;  // ecx
    unsigned int v98;  // eax
    unsigned int v99;  // ecx
    unsigned int v101;  // edx
    int v102;  // eax
    unsigned int v103;  // eax
    unsigned int v104;  // 4105
    st_478e16_1 *v52;  // eax
    unsigned int v105;  // eax
    unsigned int v106;  // ecx
    st_478e16_2 *v107;  // eax
    st_478e16_5 *v108;  // edi, Other Possible Types: unsigned int
    unsigned int v109;  // ebx
    unsigned int v110;  // eax
    st_478e16_2 *v111;  // esi
    char v53;  // al
    st_478e16_2 *v55;  // esi
    unsigned int v0;  // [bp-0x210]
    unsigned int v1;  // [bp-0x20c]
    unsigned int v2;  // [bp-0x208]
    st_46d3b4_0 v3;  // [bp-0x204], Other Possible Types: st_478e16_3 *
    st_478e16_6 *idx;  // [bp-0x1fc]
    char v5;  // [bp-0x1f8]
    unsigned int v6;  // [bp-0x1f4]
    unsigned int v7;  // [bp-0x1f0]
    void* v8;  // [bp-0x1ec]
    unsigned int v9;  // [bp-0x1e8]
    void* v10;  // [bp-0x1e4]
    char v11;  // [bp-0x1e0]
    char v12;  // [bp-0x1df]
    unsigned int v13;  // [bp-0x1dc]
    char v14;  // [bp-0x1d5]
    unsigned int v15;  // [bp-0x1d4]
    unsigned int v16;  // [bp-0x1d0]
    int v17;  // [bp-0x1cc], Other Possible Types: unsigned int
    unsigned int v18;  // [bp-0x1c8]
    unsigned int v19;  // [bp-0x1c4]
    st_478e16_0 *v20;  // [bp-0x1c0]
    unsigned int v21;  // [bp-0x1bc], Other Possible Types: int
    int v22;  // [bp-0x1b8], Other Possible Types: unsigned int
    st_478e16_0 *v23;  // [bp-0x1b4], Other Possible Types: unsigned int
    st_478e16_2 *v24;  // [bp-0x1b0]
    unsigned int v25;  // [bp-0x1ac]
    char *v26;  // [bp-0x1a8]
    char v27;  // [bp-0x1a3]
    char v28;  // [bp-0x1a2]
    char v29;  // [bp-0x1a1]
    st_478e16_5 *v30;  // [bp-0x1a0]
    char v31;  // [bp-0x19a]
    char v32;  // [bp-0x199]
    int j;  // [bp-0x198]
    char v34;  // [bp-0x193]
    char v35;  // [bp-0x192]
    char i;  // [bp-0x191]
    int node;  // [bp-0x190]
    unsigned int v38;  // [bp-0x18c]
    char v39;  // [bp-0x188]
    char v40;  // [bp-0x28]
    char v41;  // [bp-0x1d]
    st_478e16_5 *v42;  // [bp+0x4]
    st_478e16_2 *v43;  // [bp+0x8]
    unsigned int *v44;  // [bp+0xc]
    void* v45;  // [bp+0x10]

    v2 = v46;
    v1 = v47;
    v10 = v45;
    v30 = v42;
    v24 = v43;
    v26 = &v39;
    v13 = 350;
    v15 = 0;
    v7 = 0;
    v38 = 0;
    v6 = 0;
    if (v24 && v30)
    {
        v0 = v48;
        if (!(v30->field_c & 64))
        {
            v49 = sub_47c1da(v30);
            v50 = &g_4adbb0.field_0;
            if (v49 != 0xffffffff && v49 != -0x2)
                v51 = (v49 & 31) * 64 + (&g_4d6320.field_0)[v49 >> 5];
            else
                v51 = &g_4adbb0.field_0;
            if (!(v51->field_24 & 127))
            {
                if (v49 != 0xffffffff && v49 != -0x2)
                    v52 = (v49 & 31) * 64 + (&g_4d6320.field_0)[v49 >> 5];
                else
                    v52 = &g_4adbb0.field_0;
                if (!(v52->field_24 & 128))
                    goto LABEL_478f21;
            }
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
        }
        else
        {
LABEL_478f21:
            sub_46d3b4(&v3, v50, v44);
            v53 = v24->field_0;
            v29 = 0;
            node = 0;
            v18 = 0;
            if (v24->field_0)
            {
                while (1)
                {
                    if (sub_48bb76(v53))
                    {
                        node -= 1;
                        sub_478dd9(sub_478dec(v30, v30));
                        v55 = v24;
                        do
                        {
                            v55 = &v55->field_1;
                        } while (sub_48bb76(v55->field_0));
                        v24 = v55;
                        goto LABEL_479e00;
                    }
                    iter = v24;
                    if (iter->field_0 == 37)
                        break;
LABEL_479d7c:
                    v108 = v30;
                    node += 1;
                    v109 = sub_478dc3(v53, v108);
                    v55 = &iter->field_1;
                    v38 = v109;
                    v24 = v55;
                    if (iter->field_0 != v109)
                    {
                        v108 = v108;
                        v109 = v109;
LABEL_479e1b:
                        sub_478dd9(v109, v108);
                        goto LABEL_479ea7;
                    }
                    if (sub_47c5db((char)v109))
                    {
                        node += 1;
                        v110 = sub_478dc3((char)v109, v108);
                        v111 = &v55->field_1;
                        v24 = v111;
                        if (v55->field_0 == v110)
                        {
                            node -= 1;
                            v55 = v111;
                            goto LABEL_479ddc;
                        }
                        else
                        {
                            sub_478dd9(v110, v108);
                            sub_478dd9(v109, v108);
                            goto LABEL_479ea7;
                        }
                    }
                    else
                    {
LABEL_479ddc:
                        if (v38 == 0xffffffff)
                        {
                            if (v55->field_0 != 37 || v24->field_1 != 110)
                                goto LABEL_479ea7;
                            v55 = v24;
                        }
LABEL_479e00:
                        v53 = v55->field_0;
                        if (!v55->field_0)
                            goto LABEL_479ea7;
                    }
                }
                if (iter->field_1 == iter->field_0 && iter->field_0 == 37)
                {
                    v107 = &iter->field_1;
                    if (v107->field_0 == 37)
                    {
                        iter = v107;
                        goto LABEL_479d7c;
                    }
                }
                v17 = 0;
                v14 = 0;
                v25 = 0;
                v19 = 0;
                j = 0;
                v16 = 0;
                v28 = 0;
                v27 = 0;
                v34 = 0;
                i = 0;
                v31 = 0;
                v35 = 0;
                v32 = 1;
                v23 = 0;
                do
                {
                    iter = &iter->field_1;
                    v57 = iter->field_0;
                    v58 = (char)v57;
                    if (sub_48ba71((char)v57))
                    {
                        v19 += 1;
                        j = j * 10 + v57 - 48;
                        continue;
                    }
                    if (v57 <= 78)
                    {
                        if (v57 == 78)
                            continue;
                        if (v57 == 42)
                        {
                            v34 += 1;
                            continue;
                        }
                        if (v57 == 70)
                            continue;
                        if (v57 != 73)
                        {
                            if (v57 == 76)
                            {
                                v32 += 1;
                                continue;
                            }
                        }
                        else
                        {
                            v58 = _INSERT(v58, 0, iter->field_1);
                            if (iter->field_1 == 54 && (v59 = iter + 2, v59->field_0 == 52))
                                goto LABEL_479089;
                            if ((char)v58 == 0x33 && (v60 = iter + 2, v60->field_0 == 50))
                            {
                                iter = v60;
                                continue;
                            }
                            else
                            {
                                if ((char)v58 == 100 || (char)v58 == 105 || (char)v58 == 111 || (char)v58 == 120 || (char)v58 == 88)
                                    continue;
                            }
                        }
                        goto LABEL_4790e2;
                    }
                    else if (v57 != 104)
                    {
                        if (v57 == 108)
                        {
                            v59 = &iter->field_1;
                            if (v59->field_0 != 108)
                            {
                                v32 += 1;
LABEL_4790f8:
                                v35 += 1;
                                continue;
                            }
LABEL_479089:
                            v23 += 1;
                            iter = v59;
                            v21 = 0;
                            v22 = 0;
                            continue;
                        }
                        else if (v57 == 0x77)
                        {
                            goto LABEL_4790f8;
                        }
LABEL_4790e2:
                        i += 1;
                        continue;
                    }
                    else
                    {
                        v32 -= 1;
                        v35 -= 1;
                    }
                } while (!i);
                v24 = iter;
                if (!v34)
                {
                    v61 = v10;
                    iter1 = *((int *)v61);
                    v8 = v61;
                    v10 = v61 + 4;
                }
                else
                {
                    iter1 = NULL;
                }
                v20 = iter1;
                i = 0;
                if (!v35 && (iter->field_0 == 83 || (v35 = 0xff, iter->field_0 == 67)))
                    v35 = 1;
                v63 = iter->field_0 | 32;
                v9 = v63;
                if (v63 != 110)
                {
                    if (v63 != 99 && v63 != 123)
                    {
                        v64 = sub_478dec(v30);
                    }
                    else
                    {
                        node += 1;
                        v64 = sub_478dc3(v58, v30);
                    }
                    v38 = v64;
                    if (v64 == 0xffffffff)
                        goto LABEL_479ea7;
                    iter1 = v20;
                    iter = v24;
                }
                v65 = v19;
                if (v65 && !j)
                {
                    v108 = v30;
                    v109 = v38;
                    v108 = v30;
                    v109 = v38;
                    goto LABEL_479e1b;
                }
                if (!v34 && (v63 == 99 || v63 == 115 || v63 == 123))
                {
                    iter1 = *((int *)v8);
                    v8 += 4;
                    v10 = v8 + 4;
                    v20 = iter1;
                    v16 = *((int *)((char *)v10 - 4));
                    if (*((int *)((char *)v10 - 4)) >= 1)
                        goto LABEL_47922d;
                    if (v35 > 0)
                        *((unsigned short *)&iter1->field_0) = 0;
                    else
                        *((char *)&iter1->field_0) = 0;
                    *((unsigned int *)sub_46e96f()) = 12;
                    goto LABEL_479ea7;
                }
LABEL_47922d:
                if (v63 <= 111)
                {
                    if (v63 == 111)
                        goto LABEL_479a4e;
                    if (v63 == 99 && !v65)
                    {
                        j += 1;
                        v19 = 1;
                        goto LABEL_47965e;
                    }
                    if (v63 == 100)
                        goto LABEL_479a4e;
                    if (v63 <= 100)
                        goto LABEL_4797df;
                    if (v63 > 103)
                    {
                        if (v63 == 105)
                        {
                            v108 = 100;
                            v63 = 100;
                            goto LABEL_479282;
                        }
                        if (v63 != 110)
                            goto LABEL_4797df;
                        v66 = node;
                        if (!v34)
                            goto LABEL_479c2a;
                        goto LABEL_479d59;
                    }
                    else
                    {
                        iter2 = 0;
                        if (v38 == 45)
                        {
                            v39 = 45;
                            iter2 = 1;
                        }
                        else if (!(v38 == 43))
                        {
                            goto LABEL_4792d8;
                        }
                        j -= 1;
                        node += 1;
                        v38 = sub_478dc3(v65, v30);
LABEL_4792d8:
                        if (!v19)
                            j = -0x1;
                        v68 = (char)v38;
                        while (1)
                        {
                            j = j;
                            if (!sub_48ba71(v68) || !(j = (int)(j - 1), j))
                                break;
                            v25 += 1;
                            v26[iter2] = v38;
                            iter2 += 1;
                            if (!sub_478d4c(iter2, &v39, &v15))
                                goto LABEL_479ea7;
                            node += 1;
                            v38 = sub_478dc3(v70, v30);
                            v68 = (char)v38;
                        }
                        v28 = *((char *)v3->field_bc->field_0);
                        if (*((char *)v3->field_bc->field_0) == (char)v38 && !(j = (int)(j - 1), j = j, !j))
                        {
                            node += 1;
                            v38 = sub_478dc3(v68, v30);
                            v26[iter2] = v28;
                            iter2 += 1;
                            if (!sub_478d4c(iter2, &v39, &v15))
                                goto LABEL_479ea7;
                            v72 = (char)v38;
                            while (1)
                            {
                                j = j;
                                if (!sub_48ba71(v72) || (j = (int)(j - 1), !j))
                                    break;
                                v25 += 1;
                                v26[iter2] = v38;
                                iter2 += 1;
                                if (!sub_478d4c(iter2, &v39, &v15))
                                    goto LABEL_479ea7;
                                node += 1;
                                v38 = sub_478dc3(v70, v30);
                                v72 = (char)v38;
                            }
                        }
                        if (v25 && (v38 == 101 || v38 == 69) && !(j = (int)(j - 1), !j))
                        {
                            v26[iter2] = 101;
                            iter2 += 1;
                            if (!sub_478d4c(iter2, &v39, &v15))
                                goto LABEL_479ea7;
                            node += 1;
                            v38 = sub_478dc3(v70, v30);
                            if (v38 == 45)
                            {
                                v26[iter2] = 45;
                                iter2 += 1;
                                if (!sub_478d4c(iter2, &v39, &v15))
                                    goto LABEL_479ea7;
                            }
                            else if (!(v38 == 43))
                            {
                                goto LABEL_47955b;
                            }
                            j -= 1;
                            if (!j)
                            {
                                j = 0;
                            }
                            else
                            {
                                node += 1;
                                v38 = sub_478dc3(v77, v30);
                            }
LABEL_47955b:
                            for (v78 = (char)v38; sub_48ba71(v78) && !(j = (int)(j - 1), !j); j = j)
                            {
                                v25 += 1;
                                v26[iter2] = v38;
                                v79 = sub_478d4c(iter2 + 1, &v39, &v15);
                                if (!v79)
                                    goto LABEL_479ea7;
                                node += 1;
                                v38 = sub_478dc3(v70, v30);
                                v78 = (char)v38;
                            }
                        }
                        node -= 1;
                        sub_478dd9(v38, v30);
                        if (!v25)
                            goto LABEL_479ea7;
                        if (!v34)
                        {
                            v18 += 1;
                            v26[iter2] = 0;
                            sub_4751de(g_4ade8c, v32 - 1, v20, v26, &v3)(v32 - 1, v20, v26, &v3);
                            goto LABEL_479d59;
                        }
                    }
                }
                v81 = v63;
                v82 = v81 - 112;
                if (v81 != 112)
                {
                    if (v82 == 3)
                    {
LABEL_47965e:
                        if (v35 > 0)
                        {
                            v31 = 1;
                            goto LABEL_47966e;
                        }
                    }
                    v83 = v82 - 4;
                    v84 = v83 - 1;
                    if (v83 == 1)
                        goto LABEL_479a4e;
                    if (v84 == 3)
                    {
LABEL_479282:
                        if (v38 == 45)
                        {
                            v27 = 1;
                        }
                        else if (!(v38 == 43))
                        {
                            goto LABEL_479914;
                        }
                        j -= 1;
                        if (j == 1 && v65)
                        {
                            i = 1;
                        }
                        else
                        {
                            node += 1;
                            v38 = sub_478dc3(v65, v30);
                        }
LABEL_479914:
                        if (v38 == 48)
                        {
                            node += 1;
                            v38 = sub_478dc3(v65, v30);
                            if ((char)v38 != 120 && (char)v38 != 88)
                            {
                                v25 = 1;
                                if (v63 != 120)
                                {
                                    j = j;
                                    if (v19)
                                    {
                                        j -= 1;
                                        if (j == 1)
                                            i += 1;
                                    }
                                    v108 = 111;
                                    goto LABEL_4799ca;
                                }
                                else
                                {
                                    node -= 1;
                                    sub_478dd9(v38, v30);
                                    v38 = 48;
                                    goto LABEL_479a95;
                                }
                            }
                            else
                            {
                                node += 1;
                                v38 = sub_478dc3(v70, v30);
                                if (v19)
                                {
                                    j -= 2;
                                    if (j < 1)
                                        i += 1;
                                }
                                v108 = 120;
LABEL_4799ca:
                                v63 = v108;
                                goto LABEL_479a95;
                            }
                        }
                    }
                    if (v84 != 6)
                    {
LABEL_4797df:
                        if (iter->field_0 == v38)
                        {
                            v29 -= 1;
                            if (!v34)
                            {
                                v10 = v8;
                                goto LABEL_479d59;
                            }
                        }
                        else
                        {
                            sub_478dd9(v38, v30);
                            v6 = 1;
                            goto LABEL_479ea7;
                        }
                    }
                    else
                    {
                        if (v35 > 0)
                            v31 = 1;
                        v87 = &iter->field_1;
                        if (v87->field_0 == 94)
                        {
                            v87 = &v87->field_1;
                            v28 = 0xff;
                        }
                        sub_477420(&v40, 0, 32);
                        if (v87->field_0 == 93)
                        {
                            v88 = 93;
                            v87 = &v87->field_1;
                            v41 = 32;
                        }
                        else
                        {
                            v88 = v14;
                        }
                        while (1)
                        {
                            v89 = v87->field_0;
                            if (v87->field_0 == 93)
                                break;
                            v87 = &v87->field_1;
                            if (v89 == 45 && v88 && !(v90 = v87->field_0, v87->field_0 == 93))
                            {
                                v87 = &v87->field_1;
                                if (v88 < v90)
                                {
                                    v91 = v90;
                                }
                                else
                                {
                                    v91 = v88;
                                    v88 = v90;
                                }
                                if (v88 <= v91)
                                {
                                    v92 = v88;
                                    v93 = v91 - v88 + 1;
                                    do
                                    {
                                        k = v93;
                                        (&v40)[v92 >> 3] = (&v40)[v92 >> 3] | (char)(1 << ((char)(v92 & 7) & 31));
                                        v92 += 1;
                                        v93 = k - 1;
                                    } while (k != 1);
                                    v63 = v9;
                                }
                                v88 = 0;
                            }
                            else
                            {
                                v63 = v9;
                                v88 = v89;
                                (&v40)[v89 >> 3] = (&v40)[v89 >> 3] | (char)(1 << (v89 & 7 & 31));
                            }
                        }
                        if (!v89)
                            goto LABEL_479ea7;
                        iter1 = v20;
                        v24 = v87;
LABEL_47966e:
                        v96 = v30;
                        node -= 1;
                        v23 = iter1;
                        sub_478dd9(v38, v96);
                        v97 = v96;
                        if (v63 != 99)
                        {
                            v16 -= 1;
                            v97 = v96;
                        }
                    }
                }
                else
                {
                    v32 = 1;
LABEL_479a4e:
                    if (v38 == 45)
                    {
                        v27 = 1;
                        goto LABEL_479a69;
                    }
                    else if (v38 == 43)
                    {
LABEL_479a69:
                        j -= 1;
                        if (j == 1 && v65)
                        {
                            i = 1;
                        }
                        else
                        {
                            node += 1;
                            v38 = sub_478dc3(v65, v30);
                        }
                    }
LABEL_479a95:
                    if (v23)
                    {
                        if (!i)
                        {
                            while (1)
                            {
                                if (v63 != 120 && v63 != 112)
                                {
                                    if (!sub_48ba71((char)v38))
                                        goto LABEL_479bb1;
                                    if (v63 == 111)
                                    {
                                        if (v38 >= 56)
                                            goto LABEL_479bb1;
                                        v99 = CONCAT(v22, v21) * 8 >> 32;
                                        v21 *= 8;
                                        v22 = v99;
                                    }
                                    else
                                    {
                                        v21 = sub_483220(v21, v22, 10, 0);
                                        v22 = v101;
                                    }
                                }
                                else if (sub_48baf5((char)v38))
                                {
                                    v102 = v21;
                                    v21 = v102 * 16;
                                    v22 = CONCAT(v22, v102) * 16 >> 32;
                                    v99 = v38;
                                    v38 = sub_478da3(v38);
                                }
                                else
                                {
LABEL_479bb1:
                                    node -= 1;
                                    sub_478dd9(v38, v30);
                                    break;
                                }
                                v25 += 1;
                                v103 = v38 - 48;
                                v104 = v21;
                                v21 = v104 + v103;
                                v22 = v22 + ((int)(v103) >> 31) + (v104 + v103 < v104);
                                if (v19 && (j = (int)(j - 1), j == 1))
                                    break;
                                node += 1;
                                v38 = sub_478dc3(v99, v30);
                            }
                        }
                        if (v27)
                        {
                            v21 = -(v21);
                            v22 = -(v22 + (0 < v21));
                            goto LABEL_479bf2;
                        }
                    }
                    else
                    {
                        if (!i)
                        {
                            while (1)
                            {
                                if (v63 != 120 && v63 != 112)
                                {
                                    if (!sub_48ba71((char)v38))
                                        goto LABEL_479d16;
                                    if (v63 == 111)
                                    {
                                        if (v38 >= 56)
                                            goto LABEL_479d16;
                                        v105 = v17 * 8;
                                    }
                                    else
                                    {
                                        v105 = v17 * 10;
                                    }
                                }
                                else if (sub_48baf5((char)v38))
                                {
                                    v17 *= 16;
                                    v38 = sub_478da3(v38);
                                    v105 = v17;
                                }
                                else
                                {
LABEL_479d16:
                                    node -= 1;
                                    sub_478dd9(v38, v30);
                                    break;
                                }
                                v106 = v38;
                                v25 += 1;
                                v17 = v105 + v106 - 48;
                                if (v19 && (j = (int)(j - 1), j == 1))
                                    break;
                                node += 1;
                                v38 = sub_478dc3(v106, v30);
                            }
                        }
                        if (!v27)
                        {
LABEL_479bf2:
                            v66 = v17;
                            goto LABEL_479bf8;
                        }
                        else
                        {
                            v66 = -(v17);
LABEL_479bf8:
                            if (v63 == 70)
                                v25 = 0;
                            if (!v25)
                                goto LABEL_479ea7;
                            if (!v34)
                            {
                                v18 += 1;
                                iter1 = v20;
LABEL_479c2a:
                                if (v23)
                                {
                                    iter1->field_0 = v21;
                                    *((int *)&iter1->field_1) = v22;
                                }
                                else if (v32)
                                {
                                    iter1->field_0 = v66;
                                }
                                else
                                {
                                    *((unsigned short *)&iter1->field_0) = v66;
                                }
                            }
                        }
                    }
LABEL_479d59:
                    v29 += 1;
                    v55 = &v24->field_1;
                    v24 = v55;
                    goto LABEL_479ddc;
                }
                while (1)
                {
                    while (1)
                    {
                        if (v19 && (j = (int)(j - 1), !j))
                        {
LABEL_4799f8:
                            if (v23 == iter1)
                                goto LABEL_479ea7;
                            if (!v34)
                            {
                                v18 += 1;
                                if (v63 != 99)
                                {
                                    if (v31)
                                    {
                                        *((unsigned short *)&v20->field_0) = 0;
                                        goto LABEL_479d59;
                                    }
                                    else
                                    {
                                        *((char *)&v20->field_0) = 0;
                                        goto LABEL_479d59;
                                    }
                                }
                            }
                        }
                        node += 1;
                        v98 = sub_478dc3(v97, v96);
                        v38 = v98;
                        if (v98 == 0xffffffff)
                        {
LABEL_4799e9:
                            node -= 1;
                            sub_478dd9(v98, v96);
                            goto LABEL_4799f8;
                        }
                        if (v63 != 99)
                        {
                            if (v63 == 115)
                            {
                                if (v98 >= 9 && v98 <= 13)
                                    goto LABEL_4799e9;
                                if (v98 != 32)
                                    goto LABEL_479723;
                            }
                            if (v63 != 123 || (v97 = (st_478e16_5 *)((int)(char)(&v40)[(int)(v98) >> 3] ^ (int)v28), v63 = v9, !(1 << (unsigned int)((char)(v98 & 7) & 31) & v97)))
                                goto LABEL_4799e9;
                        }
LABEL_479723:
                        if (!v34)
                            break;
                        v23 = (char *)&v23->field_0 + 1;
                    }
                    if (!v16)
                        break;
                    if (v31)
                    {
                        v11 = v98;
                        if (sub_47c5db((char)v98))
                        {
                            node += 1;
                            v12 = sub_478dc3((char)v98, v96);
                        }
                        v7 = 63;
                        sub_48c167(&v7, &v11, v3->field_ac, &v3);
                        *((unsigned short *)&iter1->field_0) = v7;
                        iter1 = (char *)&iter1->field_0 + 2;
                    }
                    else
                    {
                        *((char *)&iter1->field_0) = v98;
                        iter1 = (char *)&iter1->field_0 + 1;
                    }
                    v20 = iter1;
                    v16 -= 1;
                }
                *((unsigned int *)sub_46e96f()) = 12;
                if (v31)
                    *((unsigned short *)&v23->field_0) = 0;
                else
                    *((char *)&v23->field_0) = 0;
LABEL_479ea7:
                if (v15 == 1)
                    sub_46c941(v26);
                switch (v38)
                {
                case 4294967295:
                    if (v5)
                    {
                        idx->field_70 = idx->field_70 & 0xfffffffd;
                        goto LABEL_479f2b;
                    }
                    break;
                default:
                    if (v6 == 1)
                    {
                        *((unsigned int *)sub_46e96f()) = 22;
                        sub_470f2d(0, 0, 0, 0, 0);
                    }
                }
            }
            if (v5)
                idx->field_70 = idx->field_70 & 0xfffffffd;
        }
LABEL_479f2b:
        return;
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return;
}
