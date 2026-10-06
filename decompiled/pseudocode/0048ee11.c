// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ee11; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48ee11_1 {
    unsigned int field_0;
    unsigned short field_4;
} st_48ee11_1;

typedef struct st_48ee11_3 {
    char field_0;
    char padding_1[3];
    unsigned short field_4;
} st_48ee11_3;

typedef struct st_48ee11_0 {
    unsigned short field_0;
    char field_2;
    char field_3;
    char field_4;
    char field_5;
} st_48ee11_0;

extern unsigned int g_4adf80;
extern unsigned int g_4ae0e0;

int sub_48ee11(unsigned int a0, unsigned int a1, unsigned short a2, int a3, char a4, st_48ee11_0 *idx)
{
    unsigned int v29;  // edi
    unsigned short v30;  // dx
    void* v39;  // esi
    unsigned int v40;  // edx
    unsigned int v41;  // ecx
    unsigned short v42;  // cx
    unsigned short v43;  // dx
    unsigned int iter;  // edi
    st_48ee11_1 *node;  // esi
    unsigned int v46;  // ecx
    unsigned int v47;  // eax
    unsigned int iter1;  // edi
    unsigned int v31;  // esi
    unsigned int v49;  // 4108
    unsigned int v50;  // eax
    unsigned int v51;  // 4198
    unsigned int v52;  // 4105
    unsigned int v53;  // eax
    unsigned int v54;  // eax
    unsigned short v55;  // cx
    unsigned int v56;  // ecx
    unsigned short v57;  // cx
    unsigned short v58;  // dx
    unsigned int v32;  // edi
    unsigned int iter2;  // esi
    unsigned int v60;  // eax
    st_48ee11_1 *v61;  // edi
    unsigned short *v62;  // eax
    unsigned int v63;  // ecx
    unsigned int v64;  // ebx
    unsigned int v65;  // esi
    unsigned int v66;  // 4108
    unsigned int v67;  // edi
    unsigned int v68;  // eax
    unsigned int v33;  // eax
    unsigned int v69;  // 4196
    unsigned int v70;  // 4105
    unsigned int v71;  // eax
    unsigned int v72;  // eax
    st_48ee11_0 *idx1;  // edx
    unsigned short v74;  // ax
    int v75;  // edi
    unsigned int v76;  // esi
    char v77;  // sil
    unsigned int k;  // esi
    unsigned int v34;  // eax
    st_48ee11_0 *v79;  // ebx
    unsigned int v80;  // eax
    unsigned int v81;  // ecx
    unsigned int v82;  // ecx
    unsigned int v83;  // esi
    unsigned int v84;  // edi
    unsigned int v85;  // eax
    unsigned int v86;  // edx
    st_48ee11_0 *v87;  // ebx
    st_48ee11_0 *m;  // ebx
    int i;  // ebx
    st_48ee11_0 *index;  // eax
    unsigned int v90;  // eax
    unsigned int v91;  // eax
    unsigned int v36;  // ecx
    void* v37;  // eax
    void* v38;  // esi
    char *v0;  // [bp-0x88], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x84]
    unsigned int v2;  // [bp-0x78]
    unsigned int v3;  // [bp-0x74]
    unsigned short *v4;  // [bp-0x70]
    void* v5;  // [bp-0x6c]
    unsigned int v6;  // [bp-0x68]
    unsigned int v7;  // [bp-0x60]
    unsigned int v8;  // [bp-0x5c]
    int n;  // [bp-0x58], Other Possible Types: unsigned int
    int i0;  // [bp-0x54], Other Possible Types: unsigned int
    int l;  // [bp-0x50], Other Possible Types: unsigned int
    st_48ee11_3 *v12;  // [bp-0x4c], Other Possible Types: unsigned int
    void* j;  // [bp-0x48], Other Possible Types: unsigned int
    st_48ee11_0 *v14;  // [bp-0x44], Other Possible Types: int
    int v15;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int v16;  // [bp-0x3c]
    unsigned int v17;  // [bp-0x38]
    unsigned int v18;  // [bp-0x34]
    int v19;  // [bp-0x30]
    int v20;  // [bp-0x24], Other Possible Types: unsigned short, unsigned int
    unsigned int v21;  // [bp-0x22]
    unsigned int v22;  // [bp-0x20]
    unsigned int v23;  // [bp-0x1e]
    unsigned int v24;  // [bp-0x1c]
    unsigned short v25;  // [bp-0x1a]
    char v26;  // [bp-0x19]
    int v27;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v28;  // [bp-0x10]

    v1 = v29;
    *((unsigned int *)&v27) = a0;
    v28 = a1;
    *((unsigned short *)&(&v27)[8]) = a2;
    v30 = (unsigned short)*((unsigned int *)(&v27 + 8)) & 0x7fff;
    *((char *)&v18) = 0xcc;
    *((char *)&v18 + 1) = 0xcc;
    *((char *)&v18 + 2) = 0xcc;
    *((char *)&v18 + 3) = 0xcc;
    *((char *)&v19) = 0xcc;
    *((char *)&(&v19)[1]) = 0xcc;
    *((char *)&(&v19)[2]) = 0xcc;
    *((char *)&(&v19)[3]) = 0xcc;
    *((char *)&(&v19)[4]) = 0xcc;
    *((char *)&(&v19)[5]) = 0xcc;
    *((char *)&(&v19)[6]) = 251;
    *((char *)&(&v19)[7]) = 63;
    v2 = 1;
    v3 = *((unsigned int *)(&v27 + 8)) & 0x8000;
    if ((unsigned short)v3)
        idx->field_2 = 45;
    else
        idx->field_2 = 32;
    v31 = *((unsigned int *)(&v27 + 4));
    v32 = *((unsigned int *)&v27);
    if (!v30 && !v31 && !v32)
    {
        idx->field_0 = 0;
        idx->field_2 = ((char)(((unsigned short)v3 != 0x8000) + 1) & 13) + 32;
        idx->field_3 = 1;
        idx->field_4 = 48;
        idx->field_5 = 0;
    }
    else
    {
        if (v30 == 0x7fff)
        {
            idx->field_0 = 1;
            if ((v31 != 0x80000000 || v32) && !(v31 & 0x40000000))
            {
                v0 = "1#SNAN";
                goto LABEL_48ef4c;
            }
            if ((unsigned short)v3 && v31 == 0xc0000000)
            {
                if (v32)
                    goto LABEL_48ef47;
                v0 = "1#IND";
                goto LABEL_48ef20;
            }
            else if (v31 == 0x80000000 && !v32)
            {
                v0 = "1#INF";
LABEL_48ef20:
                if (sub_47a2b9(&idx->field_4, 22, v0))
                    sub_470dc6(0, 0, 0, 0, 0);
                idx->field_3 = 5;
            }
            else
            {
LABEL_48ef47:
                v0 = "1#QNAN";
LABEL_48ef4c:
                if (sub_47a2b9(&idx->field_4, 22, v0))
                    sub_470dc6(0, 0, 0, 0, 0);
                idx->field_3 = 6;
            }
            return v33;
        }
        v34 = (unsigned short)((int)(((v30 >> 8) + (v31 >> 24) * 2) * 77 + v30 * 19728 - 323162868) >> 16);
        *((unsigned short *)&v20) = 0;
        i = -((short)v34);
        l = v34;
        *((unsigned short *)&(&v20)[10]) = v30;
        *((unsigned int *)&(&v20)[6]) = v31;
        *((unsigned int *)&(&v20)[2]) = v32;
        v6 = &g_4adf80;
        if (i)
        {
            if (i < 0)
            {
                i = -(i);
                v6 = &g_4ae0e0;
            }
            if (i)
            {
                do
                {
                    v6 += 84;
                    v36 = i & 7;
                    i >>= 3;
                    if (v36)
                    {
                        v37 = v36 * 12 + v6;
                        j = v37;
                        if (*((short *)v37) >= 0x8000)
                        {
                            v38 = v37;
                            *((int *)&v15) = *((int *)v38);
                            v39 = v38 + 4;
                            *((int *)&(&v15)[4]) = *((int *)v39);
                            v37 = &v15;
                            *((int *)&(&v15)[8]) = (int)v39[4];
                            *((unsigned int *)&(&v15)[2]) = *((unsigned int *)(&v15 + 2)) - 1;
                            j = &v15;
                        }
                        v40 = (short)v37[10];
                        n = 0;
                        *((unsigned int *)&v27) = 0;
                        *((unsigned int *)&(&v27)[4]) = 0;
                        (&v27)[2] = 0;
                        v41 = *((unsigned int *)(&v20 + 10));
                        v12 = (v40 ^ v41) & 0x8000;
                        v42 = (unsigned short)v41 & 0x7fff;
                        v43 = (unsigned short)v40 & 0x7fff;
                        iter = (unsigned short)(v43 + v42);
                        if (v42 >= 0x7fff || v43 >= 0x7fff || (unsigned short)iter > 0xbffd)
                        {
                            *((unsigned int *)&(&v20)[8]) = ((unsigned int)((!(unsigned short)v12) + 1) & 0x80000000) + 0x7fff8000;
LABEL_48f07f:
                            *((unsigned int *)&(&v20)[4]) = 0;
                            *((unsigned int *)&v20) = 0;
                            continue;
                        }
                        if ((unsigned short)iter > 16319)
                        {
                            if (!v42 && (iter += 1, !(*((unsigned int *)((void*)&v20 + 8)) & 0x7fffffff) && !*((unsigned int *)((void*)&v20 + 4)) && !*((unsigned int *)(void*)&v20)))
                            {
                                v25 = 0;
                                continue;
                            }
                            if (v43 || !(iter += 1, !((int)v37[8] & 0x7fffffff) && !(int)v37[4] && !*((int *)v37)))
                            {
                                v8 = 0;
                                node = &v27 + 1;
                                v14 = 5;
                                do
                                {
                                    i0 = v14;
                                    if (v14 > 0)
                                    {
                                        v4 = &v20 + v8 * 2;
                                        v5 = v37 + 8;
                                        do
                                        {
                                            v46 = *((short *)v5) * *(v4);
                                            v7 = 0;
                                            v47 = *((int *)((char *)node - 4)) + v46;
                                            if (v47 < *((int *)((char *)node - 4)) || v47 < v46)
                                                v7 = 1;
                                            *((unsigned int *)((char *)node - 4)) = v47;
                                            if (v7)
                                                *((unsigned short *)&node->field_0) = (short)node->field_0 + 1;
                                            v4 += 1;
                                            v5 -= 2;
                                            i0 -= 1;
                                        } while (i0 > 0);
                                        v37 = j;
                                    }
                                } while ((node += 2, v8 += 1, v14 = (int)(v14 - 1), v14 > 0));
                                iter1 = iter + 49154;
                                v49 = _ccall(14, 14, (unsigned short)iter1, 0, 0);
                                if (!(v49 & 1))
                                {
                                    do
                                    {
                                        if (*(&(&v27)[2]) & 0x80000000)
                                            break;
                                        v50 = *(&(&v27)[1]);
                                        *((unsigned int *)&v27) = v27 * 2;
                                        *((unsigned int *)&(&v27)[4]) = v50 * 2 | v27 >> 31;
                                        iter1 += 0xffff;
                                        (&v27)[2] = *((unsigned int *)(&v27 + 8)) * 2 | v50 >> 31;
                                        v51 = _ccall(14, 14, (unsigned short)iter1, 0, 0);
                                    } while (!(v51 & 1));
                                    v52 = _ccall(14, 14, (unsigned short)iter1, 0, 0);
                                    if (!(v52 & 1))
                                        goto LABEL_48f1da;
                                }
                                iter1 += 0xffff;
                                if ((unsigned short)iter1 < 0)
                                {
                                    v53 = (unsigned short)-(iter1);
                                    iter1 += v53;
                                    do
                                    {
                                        v54 = v53;
                                        if ((char)v27 & 1)
                                            n += 1;
                                    } while ((*((unsigned int *)&(&v27)[8]) = *((unsigned int *)((void*)&v27 + 8)) >> 1, v53 = v54 - 1, *((unsigned int *)&(&v27)[4]) = *((unsigned int *)((void*)&v27 + 4)) >> 1 | *((unsigned int *)((void*)&v27 + 8)) * 0x80000000, *((unsigned int *)&v27) = *((unsigned int *)(void*)&v27) >> 1 | *((unsigned int *)((void*)&v27 + 4)) * 0x80000000, v54 != 1));
                                    if (n != v53)
                                        v27 |= 1;
                                }
LABEL_48f1da:
                                if ((unsigned short)v27 > 0x8000 || (v27 = v27, ((unsigned int)v27 & 0x1ffff) == 0x18000))
                                {
                                    if (*((unsigned int *)((void*)&v27 + 2)) == 0xffffffff)
                                    {
                                        *((unsigned int *)&(&v27)[2]) = 0;
                                        if (*((unsigned int *)(&v27 + 6)) == 0xffffffff)
                                        {
                                            *((unsigned int *)&(&v27)[6]) = 0;
                                            if (*((unsigned short *)(&v27 + 10)) == 0xffff)
                                            {
                                                *((unsigned short *)((char *)&(&v27)[2] + 2)) = 0x8000;
                                                iter1 += 1;
                                            }
                                            else
                                            {
                                                *((unsigned short *)((char *)&(&v27)[2] + 2)) = *((unsigned short *)(&v27 + 10)) + 1;
                                            }
                                        }
                                        else
                                        {
                                            *((unsigned int *)((char *)&(&v27)[1] + 2)) = *((unsigned int *)(&v27 + 6)) + 1;
                                        }
                                    }
                                    else
                                    {
                                        *((unsigned int *)((char *)&v27 + 2)) = *((unsigned int *)((void*)&v27 + 2)) + 1;
                                    }
                                }
                                if ((unsigned short)iter1 >= 0x7fff)
                                {
                                    v22 = 0;
                                    v20 = 0;
                                    v24 = ((unsigned int)((!(unsigned short)v12) + 1) & 0x80000000) + 0x7fff8000;
                                    goto LABEL_48f25b;
                                }
                                else
                                {
                                    v20 = *((unsigned short *)((void*)&v27 + 2));
                                    v21 = *(&(&v27)[1]);
                                    v23 = *(&(&v27)[2]);
                                    v25 = (unsigned short)iter1 | (unsigned short)v12;
                                    goto LABEL_48f25b;
                                }
                            }
                        }
                        *((unsigned int *)&(&v20)[8]) = 0;
                        goto LABEL_48f07f;
                    }
                    else
                    {
LABEL_48f25b:
                    }
                } while (i);
            }
        }
        v55 = *((unsigned int *)(&v20 + 8)) >> 16;
        if (v55 >= 0x3fff)
        {
            l += 1;
            i0 = 0;
            *((unsigned int *)&v27) = 0;
            *((unsigned int *)&(&v27)[4]) = 0;
            *((unsigned int *)&(&v27)[8]) = 0;
            v56 = v55;
            v57 = (unsigned short)v56 & 0x7fff;
            v58 = (unsigned short)*((unsigned int *)(&v19 + 6)) & 0x7fff;
            v7 = (*((unsigned int *)(&v19 + 6)) ^ v56) & 0x8000;
            iter2 = (unsigned short)(v58 + v57);
            if (!(v57 < 0x7fff && v58 < 0x7fff && (unsigned short)iter2 <= 0xbffd))
            {
                *((unsigned int *)&(&v20)[4]) = 0;
                v60 = ((unsigned int)((!(unsigned short)v7) + 1) & 0x80000000) + 0x7fff8000;
                *((unsigned int *)&v20) = 0;
            }
            else if ((unsigned short)iter2 <= 16319)
            {
                v60 = 0;
LABEL_48f2da:
                *((unsigned int *)&(&v20)[4]) = 0;
                *((unsigned int *)&v20) = 0;
            }
            else
            {
                v60 = 0;
                if (!v57 && (iter2 += 1, !(*((unsigned int *)((void*)&v20 + 8)) & 0x7fffffff) && !*((unsigned int *)((void*)&v20 + 4)) && !*((unsigned int *)(void*)&v20)))
                {
                    *((unsigned short *)&(&v20)[10]) = 0;
                    goto LABEL_48f521;
                }
                else
                {
                    if (!v58 && !(iter2 += 1, *((unsigned int *)((void*)&v19 + 4)) & 0x7fffffff || *((unsigned int *)(void*)&v19) || v18))
                        goto LABEL_48f2da;
                    v8 = 0;
                    v61 = &v27 + 4;
                    v14 = 5;
                    do
                    {
                        n = v14;
                        if (v14 > 0)
                        {
                            v12 = &v19 + 4;
                            v62 = &v20 + v8 * 2;
                            do
                            {
                                j = 0;
                                v63 = *(v62) * *((short *)&v12->field_0);
                                v64 = *((int *)((char *)v61 - 4)) + v63;
                                if (v64 < *((int *)((char *)v61 - 4)) || v64 < v63)
                                    j = 1;
                                *((unsigned int *)((char *)v61 - 4)) = v64;
                                if (j)
                                    *((unsigned short *)&v61->field_0) = (short)v61->field_0 + 1;
                                v12 = (char *)v12 - 2;
                                v62 += 1;
                                n -= 1;
                            } while (n > 0);
                        }
                    } while ((v61 += 2, v8 += 1, v14 = (int)(v14 - 1), v14 > 0));
                    v65 = iter2 + 49154;
                    v66 = _ccall(14, 14, (unsigned short)v65, 0, 0);
                    if (!(v66 & 1))
                    {
                        do
                        {
                            v67 = *((unsigned int *)(&v27 + 8));
                            if (v67 < 0)
                                break;
                            v68 = *((unsigned int *)(&v27 + 4));
                            *((unsigned int *)&v27) = *((unsigned int *)&v27) * 2;
                            *((unsigned int *)&(&v27)[4]) = v68 * 2 | *((unsigned int *)&v27) >> 31;
                            v65 += 0xffff;
                            *((unsigned int *)&(&v27)[8]) = v67 * 2 | v68 >> 31;
                            v69 = _ccall(14, 14, (unsigned short)v65, 0, 0);
                        } while (!(v69 & 1));
                        v70 = _ccall(14, 14, (unsigned short)v65, 0, 0);
                        if (!(v70 & 1))
                            goto LABEL_48f467;
                    }
                    v65 += 0xffff;
                    if ((unsigned short)v65 < 0)
                    {
                        v71 = (unsigned short)-(v65);
                        v65 += v71;
                        do
                        {
                            v72 = v71;
                            if (*((char *)&v27) & 1)
                                i0 += 1;
                        } while ((*((unsigned int *)&(&v27)[8]) = *((unsigned int *)((void*)&v27 + 8)) >> 1, v71 = v72 - 1, *((unsigned int *)&(&v27)[4]) = *((unsigned int *)((void*)&v27 + 4)) >> 1 | *((unsigned int *)((void*)&v27 + 8)) * 0x80000000, *((unsigned int *)&v27) = *((unsigned int *)(void*)&v27) >> 1 | *((unsigned int *)((void*)&v27 + 4)) * 0x80000000, v72 != 1));
                        if (i0 != v71)
                            v27 |= 1;
                    }
LABEL_48f467:
                    if (*((unsigned short *)&v27) > 0x8000 || (*((unsigned int *)&v27) & 0x1ffff) == 0x18000)
                    {
                        if (*((unsigned int *)(&v27 + 2)) == 0xffffffff)
                        {
                            *((unsigned int *)&(&v27)[2]) = 0;
                            if (*((unsigned int *)(&v27 + 6)) == 0xffffffff)
                            {
                                *((unsigned int *)&(&v27)[6]) = 0;
                                if (*((unsigned short *)(&v27 + 10)) == 0xffff)
                                {
                                    *((unsigned short *)&(&v27)[10]) = 0x8000;
                                    v65 += 1;
                                }
                                else
                                {
                                    *((unsigned short *)&(&v27)[10]) = *((unsigned short *)(&v27 + 10)) + 1;
                                }
                            }
                            else
                            {
                                *((unsigned int *)&(&v27)[6]) = *((unsigned int *)(&v27 + 6)) + 1;
                            }
                        }
                        else
                        {
                            *((unsigned int *)&(&v27)[2]) = *((unsigned int *)(&v27 + 2)) + 1;
                        }
                    }
                    if ((unsigned short)v65 >= 0x7fff)
                    {
                        *((unsigned int *)&(&v20)[4]) = 0;
                        *((unsigned int *)&v20) = 0;
                        *((unsigned int *)&(&v20)[8]) = ((unsigned int)((!(unsigned short)v7) + 1) & 0x80000000) + 0x7fff8000;
                        goto LABEL_48f521;
                    }
                    else
                    {
                        *((unsigned short *)&v20) = *((unsigned short *)(&v27 + 2));
                        *((unsigned int *)&(&v20)[2]) = *((unsigned int *)(&v27 + 4));
                        *((unsigned int *)&(&v20)[6]) = *((unsigned int *)(&v27 + 8));
                        *((unsigned short *)&(&v20)[10]) = (unsigned short)v65 | (unsigned short)v7;
                        goto LABEL_48f521;
                    }
                }
            }
            *((unsigned int *)&(&v20)[8]) = v60;
        }
LABEL_48f521:
        idx1 = idx;
        v74 = l;
        v75 = a3;
        idx1->field_0 = v74;
        if (a4 & 1 && !(v75 += (int)(short)v74, v75 > 0))
        {
            idx1->field_0 = 0;
            idx1->field_3 = 1;
            idx1->field_2 = ((char)(((unsigned short)v3 != 0x8000) + 1) & 13) + 32;
            idx1->field_4 = 48;
            idx1->field_5 = 0;
        }
        else
        {
            if (v75 > 21)
            {
                v0 = 21;
                v75 = 21;
            }
            v76 = (*((unsigned int *)(&v20 + 8)) >> 16) - 0x3ffe;
            *((unsigned short *)&(&v20)[10]) = 0;
            j = 8;
            do
            {
                *((unsigned int *)&v20) = *((unsigned int *)&v20) * 2;
                j -= 1;
                *((unsigned int *)&(&v20)[4]) = *((unsigned int *)(&v20 + 4)) * 2 | *((unsigned int *)&v20) >> 31;
                *((unsigned int *)&(&v20)[8]) = *((unsigned int *)(&v20 + 8)) * 2 | *((unsigned int *)(&v20 + 4)) >> 31;
            } while (j != 1);
            if (v76 < 0)
            {
                v77 = -(v76);
                k = v77;
                if (v77 > 0)
                {
                    do
                    {
                        *((unsigned int *)&(&v20)[8]) = *((unsigned int *)(&v20 + 8)) >> 1;
                        k -= 1;
                        *((unsigned int *)&(&v20)[4]) = *((unsigned int *)(&v20 + 4)) >> 1 | *((unsigned int *)(&v20 + 8)) * 0x80000000;
                        *((unsigned int *)&v20) = *((unsigned int *)&v20) >> 1 | *((unsigned int *)(&v20 + 4)) * 0x80000000;
                    } while (k > 0);
                }
            }
            v79 = &idx1->field_4;
            v14 = v79;
            l = v75 + 1;
            if (l > 0)
            {
                do
                {
                    v80 = *((unsigned int *)(&v20 + 4));
                    v15 = *((unsigned int *)&v20);
                    v16 = *((unsigned int *)(&v20 + 4));
                    v17 = *((unsigned int *)(&v20 + 8));
                    v20 = *((unsigned int *)&v20) * 2;
                    v20 = (int)_INSERT(CONCAT(v20, 0), 0, v20 * 2);
                    v81 = v80 * 2 | *((unsigned int *)&v20) >> 31;
                    v82 = (*((unsigned int *)(&v20 + 8)) * 2 | v80 >> 31) * 2 | v81 >> 31;
                    v83 = v81 * 2 | v20 >> 31;
                    v84 = v15 + *((unsigned int *)&v20);
                    if (v84 < *((unsigned int *)&v20) || v84 < v15)
                    {
                        v85 = v83 + 1;
                        v86 = 0;
                        if (v85 < v83 || v85 < 1)
                            v86 = 1;
                        v83 = v85;
                        if (v86)
                        {
                            v82 += 1;
                            v83 = v85;
                        }
                    }
                    j = v16 + v83;
                    if (j < v83)
                    {
                        v82 += 1;
                    }
                    else if (j < v16)
                    {
                        v82 += 1;
                    }
                    *((unsigned int *)&v20) = v84 * 2;
                    v24 = (v82 + v17) * 2 | j >> 31;
                    *((char *)&v79->field_0) = (char)(v24 >> 24) + 48;
                    v79 = (char *)&v79->field_0 + 1;
                    l -= 1;
                    *((unsigned int *)&(&v20)[4]) = j * 2 | v84 >> 31;
                    v26 = 0;
                } while (l > 0);
            }
            if ((char)v87->field_0 < 53)
            {
                for (m = (char *)v87 - 1; m >= v14 && (char)m->field_0 == 48; m = (char *)m - 1);
                index = idx;
                if (m >= v14)
                {
                    index->field_3 = *((char *)&m) - *((char *)&index) - 3;
                    (&index->field_4)[*((char *)&m) - *((char *)&index) - 3] = 0;
                    return v91;
                }
                index->field_0 = 0;
                index->field_3 = 1;
                index->field_2 = ((char)(((unsigned short)v3 != 0x8000) + 1) & 13) + 32;
                *((char *)&v14->field_0) = 48;
                index->field_5 = 0;
            }
            else
            {
                for (v87 = (char *)v79 - 1; m >= v14 && (char)m->field_0 == 57; m = (char *)m - 1)
                {
                    *((char *)&m->field_0) = 48;
                }
                index = idx;
                if (m < v14)
                {
                    m = (char *)&m->field_0 + 1;
                    index->field_0 = index->field_0 + 1;
                    *((char *)&m->field_0) = (char)m->field_0 + 1;
                    index->field_3 = *((char *)&m) - *((char *)&index) - 3;
                    (&index->field_4)[*((char *)&m) - *((char *)&index) - 3] = 0;
                }
                else
                {
                    *((char *)&m->field_0) = (char)m->field_0 + 1;
                    index->field_3 = *((char *)&m) - *((char *)&index) - 3;
                    (&index->field_4)[*((char *)&m) - *((char *)&index) - 3] = 0;
                }
                return v91;
            }
        }
    }
    return v90;
}
