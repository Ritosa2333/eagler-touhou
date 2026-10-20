// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477d8f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477d8f_3 {
    char padding_0[12];
    char field_c;
} st_477d8f_3;

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_477d8f_1 {
    char padding_0[172];
    int field_ac;
    char padding_b0[12];
    struct st_477d8f_2 *field_bc;
} st_477d8f_1;

typedef struct st_477d8f_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[20];
    char field_24;
} st_477d8f_0;

typedef struct st_477d8f_2 {
    unsigned int field_0;
} st_477d8f_2;

typedef struct st_477d8f_4 {
    char padding_0[112];
    unsigned int field_70;
} st_477d8f_4;

extern st_477d8f_0 g_4adbb0;
extern int g_4ade8c;
extern st_477d8f_0 g_4d6320;

int sub_477d8f(void)
{
    unsigned int v45;  // esi
    unsigned int v46;  // edi
    int v55;  // ebx
    unsigned int v56;  // ecx
    char *v57;  // eax
    char *v58;  // eax
    void* *v59;  // eax
    void* iter;  // esi
    char iter1;  // bl
    int v62;  // eax
    unsigned int v63;  // ecx
    char *iter2;  // edi
    int v64;  // ebx
    unsigned int v65;  // eax
    unsigned int v67;  // ecx
    unsigned int v69;  // eax
    unsigned int v48;  // ebx
    unsigned int v74;  // ecx
    unsigned int v75;  // eax
    unsigned int v76;  // eax
    unsigned int v78;  // eax
    unsigned int v79;  // eax
    unsigned int v80;  // eax
    int v49;  // eax
    char *v83;  // edi
    char v84;  // dl
    char v85;  // al
    char v86;  // cl
    char v87;  // al
    unsigned int v88;  // edi
    unsigned int v89;  // edx
    unsigned int k;  // edx
    st_477d8f_3 *v92;  // edi
    st_477d8f_0 *v50;  // edx
    st_477d8f_3 *v93;  // ecx
    int v94;  // eax
    unsigned int v95;  // ecx
    unsigned int v96;  // edi
    unsigned int v97;  // esi
    unsigned int v99;  // edx
    int v100;  // esi
    int v101;  // eax
    unsigned int v102;  // edi
    st_477d8f_0 *v51;  // ecx
    char *v103;  // edi
    char *v104;  // eax
    int v105;  // ebx, Other Possible Types: unsigned int
    int v106;  // eax
    char *v107;  // edi
    st_477d8f_3 *v108;  // esi, Other Possible Types: unsigned int
    st_477d8f_0 *v52;  // eax
    char v53;  // al
    unsigned int v0;  // [bp-0x20c]
    unsigned int v1;  // [bp-0x208]
    unsigned int v2;  // [bp-0x204]
    void* *v3;  // [bp-0x200]
    st_46d3b4_0 v4;  // [bp-0x1fc], Other Possible Types: st_477d8f_1 *
    st_477d8f_4 *idx;  // [bp-0x1f4]
    char v6;  // [bp-0x1f0]
    unsigned int v7;  // [bp-0x1ec]
    void* *v8;  // [bp-0x1e8]
    char v9;  // [bp-0x1e4]
    char v10;  // [bp-0x1e3]
    unsigned int v11;  // [bp-0x1e0]
    unsigned int v12;  // [bp-0x1dc]
    char v13;  // [bp-0x1d5]
    unsigned int v14;  // [bp-0x1d4]
    int v15;  // [bp-0x1d0]
    int v16;  // [bp-0x1d0]
    int v17;  // [bp-0x1cc]
    unsigned int v18;  // [bp-0x1c8]
    void* v19;  // [bp-0x1c4], Other Possible Types: int
    void* v20;  // [bp-0x1c0]
    char *v21;  // [bp-0x1bc]
    unsigned int v22;  // [bp-0x1b8]
    char i;  // [bp-0x1b1]
    char *v24;  // [bp-0x1b0]
    unsigned int v25;  // [bp-0x1ac]
    unsigned int v26;  // [bp-0x1a8]
    char v27;  // [bp-0x1a4]
    char v28;  // [bp-0x1a3]
    char v29;  // [bp-0x1a2]
    char v30;  // [bp-0x1a1]
    st_477d8f_3 *v31;  // [bp-0x1a0]
    char v32;  // [bp-0x19a]
    char v33;  // [bp-0x199]
    int j;  // [bp-0x198]
    char v35;  // [bp-0x191]
    int node;  // [bp-0x190]
    int v37;  // [bp-0x18c]
    char v38;  // [bp-0x188]
    char v39;  // [bp-0x28]
    char v40;  // [bp-0x1d]
    st_477d8f_3 *v41;  // [bp+0x4]
    char *v42;  // [bp+0x8]
    unsigned int *v43;  // [bp+0xc]
    void* *v44;  // [bp+0x10]

    v2 = v45;
    v1 = v46;
    iter2 = v42;
    v8 = v44;
    v31 = v41;
    v24 = &v38;
    v11 = 350;
    v14 = 0;
    v7 = 0;
    v37 = 0;
    if (iter2 && v31)
    {
        v0 = v48;
        if (!(v31->field_c & 64))
        {
            v49 = sub_47c1da(v31);
            v50 = &g_4adbb0.field_0;
            if (v49 != -0x1 && v49 != -0x2)
                v51 = (v49 & 31) * 64 + (&g_4d6320.field_0)[v49 >> 5];
            else
                v51 = &g_4adbb0.field_0;
            if (!(v51->field_24 & 127))
            {
                if (v49 != -0x1 && v49 != -0x2)
                    v52 = (v49 & 31) * 64 + (&g_4d6320.field_0)[v49 >> 5];
                else
                    v52 = &g_4adbb0.field_0;
                if (!(v52->field_24 & 128))
                    goto LABEL_477e8e;
            }
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
        }
        else
        {
LABEL_477e8e:
            sub_46d3b4(&v4, v50, v43);
            v53 = *(iter2);
            v30 = 0;
            node = 0;
            v18 = 0;
            if (*(iter2))
            {
                do
                {
                    v108 = v31;
                    if (sub_48bb76(v53))
                    {
                        node -= 1;
                        sub_477d52(sub_477d65(v108, v108));
                        do
                        {
                            iter2 += 1;
                        } while (sub_48bb76(*(iter2)));
                    }
                    if (*(iter2) == 37)
                    {
                        if (iter2[1] == *(iter2) && *(iter2) == 37)
                        {
                            v104 = iter2 + 1;
                            if (*(v104) == 37)
                            {
                                iter2 = v104;
                                goto LABEL_478c38;
                            }
                        }
                        v19 = 0;
                        v13 = 0;
                        v25 = 0;
                        v22 = 0;
                        j = 0;
                        v28 = 0;
                        v27 = 0;
                        v32 = 0;
                        i = 0;
                        v29 = 0;
                        v35 = 0;
                        v33 = 1;
                        v12 = 0;
                        do
                        {
                            iter2 += 1;
                            v55 = *(iter2);
                            v56 = (char)v55;
                            if (sub_48ba71((char)v55))
                            {
                                v22 += 1;
                                j = j * 10 + v55 - 48;
                                continue;
                            }
                            if (v55 <= 78)
                            {
                                if (v55 == 78)
                                    continue;
                                if (v55 == 42)
                                {
                                    v32 += 1;
                                    continue;
                                }
                                if (v55 == 70)
                                    continue;
                                if (v55 != 73)
                                {
                                    if (v55 == 76)
                                    {
                                        v33 += 1;
                                        continue;
                                    }
                                }
                                else
                                {
                                    v56 = _INSERT(v56, 0, iter2[1]);
                                    if (iter2[1] == 54 && (v57 = iter2 + 2, *(v57) == 52))
                                        goto LABEL_477fda;
                                    if ((char)v56 == 0x33 && (v58 = iter2 + 2, *(v58) == 50))
                                    {
                                        iter2 = v58;
                                        continue;
                                    }
                                    else
                                    {
                                        if ((char)v56 == 100 || (char)v56 == 105 || (char)v56 == 111 || (char)v56 == 120 || (char)v56 == 88)
                                            continue;
                                    }
                                }
                                goto LABEL_478033;
                            }
                            else if (v55 != 104)
                            {
                                if (v55 == 108)
                                {
                                    v57 = iter2 + 1;
                                    if (*(v57) != 108)
                                    {
                                        v33 += 1;
LABEL_478049:
                                        v35 += 1;
                                        continue;
                                    }
LABEL_477fda:
                                    v12 += 1;
                                    iter2 = v57;
                                    v15 = 0;
                                    v17 = 0;
                                    continue;
                                }
                                else if (v55 == 0x77)
                                {
                                    goto LABEL_478049;
                                }
LABEL_478033:
                                i += 1;
                                continue;
                            }
                            else
                            {
                                v33 -= 1;
                                v35 -= 1;
                            }
                        } while (!i);
                        v21 = iter2;
                        if (!v32)
                        {
                            v59 = v8;
                            iter = *(v59);
                            v3 = v59;
                            v8 = v59 + 1;
                        }
                        else
                        {
                            iter = NULL;
                        }
                        iter1 = 0;
                        v20 = iter;
                        if (!v35 && (*(iter2) == 83 || (v35 = 0xff, *(iter2) == 67)))
                            v35 = 1;
                        v26 = *(iter2) | 32;
                        if (v26 != 110)
                        {
                            if (v26 != 99 && v26 != 123)
                            {
                                v62 = sub_477d65(v31);
                            }
                            else
                            {
                                node += 1;
                                v62 = sub_477d3c(v56, v31);
                            }
                            v37 = v62;
                            if (v62 == -0x1)
                                break;
                            iter = v20;
                            iter2 = v21;
                        }
                        v63 = v22;
                        if (v63 && !j)
                        {
LABEL_478cb2:
                            v108 = v31;
                            v105 = v37;
                            v108 = v31;
                            v105 = v37;
                            goto LABEL_478cbe;
                        }
                        if (v26 > 111)
                        {
                            v78 = v26 - 112;
                            if (v26 == 112)
                            {
                                v33 = 1;
LABEL_478944:
                                if (v37 == 45)
                                {
                                    v27 = 1;
                                }
                                else if (!(v37 == 43))
                                {
                                    goto LABEL_478986;
                                }
                                j -= 1;
                                if (j == 1 && v63)
                                {
                                    iter1 = 1;
                                }
                                else
                                {
                                    node += 1;
                                    v37 = sub_477d3c(v63, v31);
                                }
LABEL_478986:
                                if (v12)
                                {
                                    if (!iter1)
                                    {
                                        while (1)
                                        {
                                            if (v26 != 120 && v26 != 112)
                                            {
                                                v95 = (char)v37;
                                                if (!sub_48ba71((char)v37))
                                                    goto LABEL_478a8d;
                                                if (v26 == 111)
                                                {
                                                    if (v37 >= 56)
                                                        goto LABEL_478a8d;
                                                    v96 = CONCAT(v17, v15) * 8 >> 32;
                                                    v97 = v15 * 8;
                                                }
                                                else
                                                {
                                                    v97 = sub_483220(v15, v17, 10, 0);
                                                    v96 = v99;
                                                }
                                            }
                                            else if (sub_48baf5((char)v37))
                                            {
                                                v100 = v15;
                                                v96 = CONCAT(v17, v100) * 16 >> 32;
                                                v97 = v100 * 16;
                                                v95 = v37;
                                                v37 = sub_477d1c(v37);
                                            }
                                            else
                                            {
LABEL_478a8d:
                                                node -= 1;
                                                sub_477d52(v37, v31);
                                                break;
                                            }
                                            v25 += 1;
                                            v101 = v37 - 48;
                                            v15 = v97 + v101;
                                            v17 = v96 + (v101 >> 31) + (v97 + v101 < v97);
                                            if (v22 && (j = (int)(j - 1), j == 1))
                                                break;
                                            node += 1;
                                            v37 = sub_477d3c(v95, v31);
                                        }
                                    }
                                    if (v27)
                                    {
                                        v16 = -(v15);
                                        v17 = -(v17 + (0 < v15));
                                        v15 = v16;
                                    }
                                }
                                else
                                {
                                    v19 = v19;
                                    if (!iter1)
                                    {
                                        while (1)
                                        {
                                            if (v26 != 120 && v26 != 112)
                                            {
                                                v108 = (char)v37;
                                                if (!sub_48ba71((char)v37))
                                                    goto LABEL_478b93;
                                                if (v26 == 111)
                                                {
                                                    if (v37 >= 56)
                                                        goto LABEL_478b93;
                                                    v102 = v19 * 8;
                                                }
                                                else
                                                {
                                                    v102 = v19 * 10;
                                                }
                                            }
                                            else if (sub_48baf5((char)v37))
                                            {
                                                v102 = v19 * 16;
                                                v108 = v37;
                                                v37 = sub_477d1c(v37);
                                            }
                                            else
                                            {
LABEL_478b93:
                                                node -= 1;
                                                sub_477d52(v37, v31);
                                                break;
                                            }
                                            v25 += 1;
                                            v19 = v102 + v37 - 48;
                                            if (v22 && (j = (int)(j - 1), j == 1))
                                                break;
                                            node += 1;
                                            v37 = sub_477d3c(v108, v31);
                                        }
                                    }
                                    if (v27)
                                        v19 = -(v19);
                                }
                                if (v26 == 70)
                                    v25 = 0;
                                if (!v25)
                                    break;
                                if (v32)
                                    goto LABEL_478c15;
                                v18 += 1;
                                iter = v20;
LABEL_478be9:
                                if (v12)
                                {
                                    *((int *)iter) = v15;
                                    *((int *)&iter[4]) = v17;
                                }
                                else if (v33)
                                {
                                    *((int *)iter) = v19;
                                }
                                else
                                {
                                    *((unsigned short *)iter) = v19;
                                }
LABEL_478c15:
                                v30 += 1;
                                v103 = v21 + 1;
                                v21 = v103;
                                goto LABEL_478c8a;
                            }
                            if (v78 != 3)
                            {
                                v79 = v78 - 4;
                                v80 = v79 - 1;
                                if (v79 == 1)
                                    goto LABEL_478944;
                                if (v80 != 3)
                                {
                                    if (v80 == 6)
                                    {
                                        if (v35 > 0)
                                            v29 = 1;
                                        v83 = iter2 + 1;
                                        if (*(v83) == 94)
                                        {
                                            v83 += 1;
                                            v28 = 0xff;
                                        }
                                        sub_477420(&v39, 0, 32);
                                        if (*(v83) == 93)
                                        {
                                            v84 = 93;
                                            v83 += 1;
                                            v40 = 32;
                                        }
                                        else
                                        {
                                            v84 = v13;
                                        }
                                        while (1)
                                        {
                                            v85 = *(v83);
                                            if (*(v83) == 93)
                                                break;
                                            v83 += 1;
                                            if (v85 == 45 && v84 && !(v86 = *(v83), *(v83) == 93))
                                            {
                                                v83 += 1;
                                                if (v84 < v86)
                                                {
                                                    v87 = v86;
                                                }
                                                else
                                                {
                                                    v87 = v84;
                                                    v84 = v86;
                                                }
                                                if (v84 <= v87)
                                                {
                                                    v88 = v84;
                                                    v89 = v87 - v84 + 1;
                                                    do
                                                    {
                                                        k = v89;
                                                        (&v39)[v88 >> 3] = (&v39)[v88 >> 3] | (char)(1 << ((char)(v88 & 7) & 31));
                                                        v88 += 1;
                                                        v89 = k - 1;
                                                    } while (k != 1);
                                                }
                                                v84 = 0;
                                            }
                                            else
                                            {
                                                v84 = v85;
                                                (&v39)[v85 >> 3] = (&v39)[v85 >> 3] | (char)(1 << (v85 & 7 & 31));
                                            }
                                        }
                                        if (!v85)
                                            break;
                                        v21 = v83;
                                        iter = v20;
LABEL_47857a:
                                        v92 = v31;
                                        node -= 1;
                                        v19 = iter;
                                        sub_477d52(v37, v92);
                                        v93 = v92;
                                        while (1)
                                        {
                                            if (v22 && (j = (int)(j - 1), !j))
                                                break;
                                            node += 1;
                                            v94 = sub_477d3c(v93, v92);
                                            v37 = v94;
                                            if (v94 == -0x1)
                                            {
LABEL_4788db:
                                                node -= 1;
                                                sub_477d52(v94, v92);
                                                break;
                                            }
                                            if (v26 != 99)
                                            {
                                                if (v26 == 115)
                                                {
                                                    if (v94 >= 9 && v94 <= 13)
                                                        goto LABEL_4788db;
                                                    if (v94 != 32)
                                                        goto LABEL_47862a;
                                                }
                                                if (v26 != 123 || (v93 = (st_477d8f_3 *)((int)(char)(&v39)[v94 >> 3] ^ (int)v28), !(1 << (unsigned int)((char)(v94 & 7) & 31) & v93)))
                                                    goto LABEL_4788db;
                                            }
LABEL_47862a:
                                            if (!v32)
                                            {
                                                if (v29)
                                                {
                                                    v9 = v94;
                                                    if (sub_47c5db((char)v94))
                                                    {
                                                        node += 1;
                                                        v10 = sub_477d3c((char)v94, v92);
                                                    }
                                                    v7 = 63;
                                                    sub_48c167(&v7, &v9, v4->field_ac, &v4);
                                                    *((unsigned short *)iter) = v7;
                                                    iter += 2;
                                                }
                                                else
                                                {
                                                    *((char *)iter) = v94;
                                                    iter += 1;
                                                }
                                                v20 = iter;
                                            }
                                            else
                                            {
                                                v19 += 1;
                                            }
                                        }
                                        if (v19 == iter)
                                            break;
                                        if (!v32)
                                        {
                                            v18 += 1;
                                            if (v26 != 99)
                                            {
                                                if (v29)
                                                {
                                                    *((unsigned short *)v20) = 0;
                                                    goto LABEL_478c15;
                                                }
                                                else
                                                {
                                                    *((char *)v20) = 0;
                                                    goto LABEL_478c15;
                                                }
                                            }
                                        }
                                    }
LABEL_4786d7:
                                    if (*(iter2) != v37)
                                        goto LABEL_478cb2;
                                    v30 -= 1;
                                    if (!v32)
                                    {
                                        v8 = v3;
                                        goto LABEL_478c15;
                                    }
                                }
LABEL_47818e:
                                if (v37 == 45)
                                {
                                    v27 = 1;
                                }
                                else if (!(v37 == 43))
                                {
                                    goto LABEL_4787ff;
                                }
                                j -= 1;
                                if (j == 1 && v63)
                                {
                                    iter1 = 1;
                                }
                                else
                                {
                                    node += 1;
                                    v37 = sub_477d3c(v63, v31);
                                }
LABEL_4787ff:
                                v108 = 48;
                                if (v37 == 48)
                                {
                                    node += 1;
                                    v37 = sub_477d3c(v63, v31);
                                    if ((char)v37 != 120 && (char)v37 != 88)
                                    {
                                        v25 = 1;
                                        if (v26 != 120)
                                        {
                                            j = j;
                                            if (v22)
                                            {
                                                j -= 1;
                                                if (j == 1)
                                                    iter1 += 1;
                                            }
                                            v26 = 111;
                                            goto LABEL_478986;
                                        }
                                        else
                                        {
                                            node -= 1;
                                            sub_477d52(v37, v31);
                                            v37 = 48;
                                            goto LABEL_478986;
                                        }
                                    }
                                    else
                                    {
                                        node += 1;
                                        v37 = sub_477d3c(v67, v31);
                                        if (v22)
                                        {
                                            j -= 2;
                                            if (j < 1)
                                                iter1 += 1;
                                        }
                                        v26 = 120;
                                        goto LABEL_478986;
                                    }
                                }
                            }
LABEL_47856a:
                            if (v35 > 0)
                            {
                                v29 = 1;
                                goto LABEL_47857a;
                            }
                        }
                        if (v26 == 111)
                            goto LABEL_478944;
                        if (v26 == 99 && !v63)
                        {
                            j += 1;
                            v22 = 1;
                            goto LABEL_47856a;
                        }
                        v108 = 100;
                        if (v26 == 100)
                            goto LABEL_478944;
                        if (v26 <= 100)
                            goto LABEL_4786d7;
                        switch (v26)
                        {
                        case 105:
                            v26 = 100;
                            goto LABEL_47818e;
                            break;
                        case 110:
                            v19 = node;
                            if (v32)
                                goto LABEL_478c15;
                            goto LABEL_478be9;
                            break;
                        }
                        v64 = 0;
                        if (v37 == 45)
                        {
                            v38 = 45;
                            v64 = 1;
                        }
                        else if (!(v37 == 43))
                        {
                            goto LABEL_4781e4;
                        }
                        j -= 1;
                        node += 1;
                        v37 = sub_477d3c(v63, v31);
LABEL_4781e4:
                        if (!v22)
                            j = -0x1;
                        v65 = (char)v37;
                        while (1)
                        {
                            j = j;
                            if (!sub_48ba71(v65) || !(j = (int)(j - 1), j))
                                break;
                            v25 += 1;
                            v24[v64] = v37;
                            v64 += 1;
                            if (!sub_477cb3(v64, &v38, &v14))
                                goto LABEL_478cdc;
                            node += 1;
                            v37 = sub_477d3c(v67, v31);
                            v65 = (char)v37;
                        }
                        v28 = *((char *)v4->field_bc->field_0);
                        if (*((char *)v4->field_bc->field_0) == (char)v37 && !(j = (int)(j - 1), j = j, !j))
                        {
                            node += 1;
                            v37 = sub_477d3c(v65, v31);
                            v24[v64] = v28;
                            v64 += 1;
                            if (!sub_477cb3(v64, &v38, &v14))
                                break;
                            v69 = (char)v37;
                            while (1)
                            {
                                j = j;
                                if (!sub_48ba71(v69) || (j = (int)(j - 1), !j))
                                    break;
                                v25 += 1;
                                v24[v64] = v37;
                                v64 += 1;
                                if (!sub_477cb3(v64, &v38, &v14))
                                    goto LABEL_478cdc;
                                node += 1;
                                v37 = sub_477d3c(v67, v31);
                                v69 = (char)v37;
                            }
                        }
                        if (v25 && (v37 == 101 || v37 == 69) && !(j = (int)(j - 1), !j))
                        {
                            v24[v64] = 101;
                            v64 += 1;
                            if (!sub_477cb3(v64, &v38, &v14))
                                break;
                            node += 1;
                            v37 = sub_477d3c(v67, v31);
                            if (v37 == 45)
                            {
                                v24[v64] = 45;
                                v64 += 1;
                                if (!sub_477cb3(v64, &v38, &v14))
                                    break;
                            }
                            else if (!(v37 == 43))
                            {
                                goto LABEL_478467;
                            }
                            j -= 1;
                            if (!j)
                            {
                                j = 0;
                            }
                            else
                            {
                                node += 1;
                                v37 = sub_477d3c(v74, v31);
                            }
LABEL_478467:
                            for (v75 = (char)v37; sub_48ba71(v75) && !(j = (int)(j - 1), !j); j = j)
                            {
                                v25 += 1;
                                v24[v64] = v37;
                                v76 = sub_477cb3(v64 + 1, &v38, &v14);
                                if (!v76)
                                    goto LABEL_478cdc;
                                node += 1;
                                v37 = sub_477d3c(v67, v31);
                                v75 = (char)v37;
                            }
                        }
                        node -= 1;
                        sub_477d52(v37, v31);
                        if (!v25)
                            break;
                        if (!v32)
                        {
                            v18 += 1;
                            v24[v64] = 0;
                            sub_4751de(g_4ade8c, v33 - 1, v20, v24, &v4)(v33 - 1, v20, v24, &v4);
                            goto LABEL_478c15;
                        }
                    }
                    else
                    {
LABEL_478c38:
                        node += 1;
                        v105 = sub_477d3c(v53, v108);
                        v103 = iter2 + 1;
                        v37 = v105;
                        v21 = v103;
                        if (*(iter2) != v105)
                        {
                            v108 = v108;
                            v105 = v105;
LABEL_478cbe:
                            sub_477d52(v105, v108);
                            v37 = v37;
                            break;
                        }
                        else if (sub_47c5db((char)v105))
                        {
                            node += 1;
                            v106 = sub_477d3c((char)v105, v108);
                            v107 = v103 + 1;
                            v21 = v107;
                            if (*(v103) == v106)
                            {
                                node -= 1;
                                v103 = v107;
                            }
                            else
                            {
                                sub_477d52(v106, v108);
                                sub_477d52(v105, v108);
                                break;
                            }
                        }
LABEL_478c8a:
                        iter2 = v103;
                        if (v37 != -0x1)
                            continue;
                        if (*(v103) != 37 || v21[1] != 110)
                            break;
                        iter2 = v21;
                    }
                    v53 = *(iter2);
                } while (*(iter2));
LABEL_478cdc:
                if (v14 == 1)
                    sub_46c941(v24);
                switch (v37)
                {
                default:
                    goto LABEL_478d24;
                }
                if (v6)
                    idx->field_70 = idx->field_70 & 0xfffffffd;
            }
            else
            {
LABEL_478d24:
                if (v6)
                    idx->field_70 = idx->field_70 & 0xfffffffd;
            }
        }
        return;
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return;
}
