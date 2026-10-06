// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e437; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48e437_0 {
    char padding_0[4];
    char field_4;
} st_48e437_0;

extern char g_4ae320;
extern char g_4d6308;
extern unsigned int g_4d6320[4];

void* sub_48e437(void* a0, void* a1, unsigned int iter)
{
    unsigned int *v10;  // edi
    unsigned int v11;  // esi
    char *v20;  // ecx
    st_48e437_0 *v21;  // eax
    void* iter1;  // ebx
    void* v23;  // ecx
    void* v24;  // eax
    st_48e437_0 *v25;  // eax
    void* iter2;  // ebx
    unsigned int j;  // eax
    unsigned int v28;  // ecx
    unsigned int v29;  // ecx
    char *v12;  // eax
    void* v30;  // ecx
    void* v31;  // ebx
    void* v32;  // ebx
    unsigned int v33;  // eax
    void* node;  // ebx
    void* v35;  // eax
    unsigned short v36;  // cx
    void* v37;  // ecx
    st_48e437_0 *v38;  // esi
    void* v39;  // eax
    int v13;  // edi
    int v40;  // ebx
    unsigned int v14;  // eax
    void* v15;  // eax
    unsigned int v16;  // eax
    int v17;  // eax
    unsigned int v18;  // ecx
    unsigned int v19;  // edx
    unsigned int v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x28]
    int v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x1c]
    void* v4;  // [bp-0x18]
    void* v5;  // [bp-0x14], Other Possible Types: unsigned int
    void* v6;  // [bp-0x10]
    Byte v7[6];  // [bp-0xc]
    char v8;  // [bp-0x6]
    Byte v9;  // [bp-0x5]

    v1 = 0xfffffffe;
    v4 = 0xfffffffe;
    if (a0 == v4)
    {
        *((unsigned int *)sub_46e982(v2, iter)) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        return 0xffffffff;
    }
    if (a0 < NULL || a0 >= *((int *)&g_4d6308))
    {
        *((unsigned int *)sub_46e982(v40)) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        sub_470f2d(0, 0, 0, 0, 0);
        v39 = 0xffffffff;
    }
    else
    {
        v10 = &g_4d6320[a0 >> 5];
        v11 = (a0 & 31) * 64;
        v12 = *(v10) + v11;
        if (!(v12[4] & 1))
        {
            *((unsigned int *)sub_46e982(v13)) = 0;
            *((unsigned int *)sub_46e96f()) = 9;
            goto LABEL_48e541;
        }
        if (iter <= 0x7fffffff)
        {
            v5 = NULL;
            if (!(iter && !(v12[4] & 2)))
            {
                v39 = NULL;
            }
            else if (a1)
            {
                v8 = (char)(v12[36] * 2) >> 1;
                v14 = (char)(v12[36] * 2) >> 1;
                v0 = 4;
                if (v14 != 1)
                {
                    iter = iter;
                    if (v14 == 2)
                    {
                        if ((char)~(iter) & 1)
                        {
                            iter &= 0xfffffffe;
                            goto LABEL_48e51c;
                        }
                    }
                    else
                    {
LABEL_48e51c:
                        v15 = a1;
                        v6 = v15;
                        goto LABEL_48e5a8;
                    }
                }
                else if ((char)~(iter) & 1)
                {
                    v16 = iter >> 1;
                    iter = 4;
                    if (v16 >= 4)
                        iter = v16;
                    v6 = sub_4777ce(iter);
                    if (!v6)
                    {
                        *((unsigned int *)sub_46e96f()) = 12;
                        *((unsigned int *)sub_46e982()) = 8;
LABEL_48e584:
                        v39 = 0xffffffff;
                    }
                    else
                    {
                        v17 = sub_47b5cb(a0, 0, 0, 1);
                        v18 = *(v10);
                        *((int *)(v11 + v18 + 40)) = v17;
                        v15 = v6;
                        *((unsigned int *)(v11 + v18 + 44)) = v19;
LABEL_48e5a8:
                        v20 = *(v10) + v11;
                        if (v20[4] & 72 && v20[5] != 10 && iter)
                        {
                            *((char *)v15) = v20[5];
                            v15 += 1;
                            iter -= 1;
                            v5 = 0x1;
                            *((char *)(v11 + *(v10) + 5)) = 10;
                            if (v8 && *((char *)(v11 + *(v10) + 37)) != 10 && iter)
                            {
                                *((char *)v15) = *((char *)(v11 + *(v10) + 37));
                                v15 += 1;
                                iter -= 1;
                                v5 = 0x2;
                                *((char *)(v11 + *(v10) + 37)) = 10;
                                if (v8 == 1 && *((char *)(v11 + *(v10) + 38)) != 10 && iter)
                                {
                                    *((char *)v15) = *((char *)(v11 + *(v10) + 38));
                                    v15 += 1;
                                    iter -= 1;
                                    v5 = 0x3;
                                    *((char *)(v11 + *(v10) + 38)) = 10;
                                }
                            }
                        }
                        if (ReadFile(*((int *)(v11 + *(v10))), v15, iter, &v3, NULL) && v3 >= NULL && v3 <= iter)
                        {
                            v5 += v3;
                            v21 = v11 + *(v10) + 4;
                            if (!(v21->padding_0[0] & 128))
                                goto LABEL_48e84e;
                            if (v8 != 2)
                            {
                                if (v3 && *((char *)v6) == 10)
                                    v21->padding_0[0] = v21->padding_0[0] | 4;
                                else
                                    v21->padding_0[0] = v21->padding_0[0] & 251;
                                iter1 = v6;
                                iter = iter1;
                                v5 += iter1;
                                if (iter1 < v5)
                                {
                                    do
                                    {
                                        v23 = iter;
                                        if (*((char *)v23) == 26)
                                        {
                                            v25 = v11 + *(v10) + 4;
                                            if (!(v25->padding_0[0] & 64))
                                            {
                                                v25->padding_0[0] = v25->padding_0[0] | 2;
                                                break;
                                            }
                                            else
                                            {
                                                *((char *)iter1) = *((char *)v23);
                                                iter1 += 1;
                                                break;
                                            }
                                        }
                                        if (*((char *)v23) != 13)
                                        {
                                            *((char *)iter1) = *((char *)v23);
                                            iter1 += 1;
                                            iter = v23 + 1;
                                            continue;
                                        }
                                        if (v23 < v5 - 1)
                                        {
                                            v24 = v23 + 1;
                                            if (*((char *)v24) == 10)
                                            {
                                                iter = v23 + 2;
                                                goto LABEL_48e6ce;
                                            }
                                            else
                                            {
                                                iter = v24;
                                                goto LABEL_48e745;
                                            }
                                        }
                                        else
                                        {
                                            iter += 1;
                                            if ((ReadFile(*((int *)(v11 + *(v10))), &v9, 1, &v3, NULL) || !GetLastError()) && v3)
                                            {
                                                if (*((char *)(v11 + *(v10) + 4)) & 72)
                                                {
                                                    if (v9 != 10)
                                                    {
                                                        *((char *)iter1) = 13;
                                                        *((Byte *)(v11 + *(v10) + 5)) = v9;
                                                        iter1 += 1;
                                                        continue;
                                                    }
                                                }
                                                else
                                                {
                                                    if (iter1 != v6 || v9 != 10)
                                                    {
                                                        sub_47b5cb(a0, -0x1, -0x1, 1);
                                                        if (v9 == 10)
                                                            continue;
                                                        goto LABEL_48e745;
                                                    }
                                                }
LABEL_48e6ce:
                                                *((char *)iter1) = 10;
                                                iter1 += 1;
                                            }
                                            else
                                            {
LABEL_48e745:
                                                *((char *)iter1) = 13;
                                                iter1 += 1;
                                            }
                                        }
                                    } while (iter < v5);
                                }
                                v5 = iter1 - v6;
                                if (v8 != 1 || !v5)
                                    goto LABEL_48e84e;
                                iter2 = iter1 - 1;
                                if (*((char *)iter2) >= 0)
                                {
                                    iter2 += 1;
                                    goto LABEL_48e819;
                                }
                                j = 1;
                                for (v28 = *((char *)iter2); !(&g_4ae320)[v28] && j <= 4 && iter2 >= v6; j += 1)
                                {
                                    iter2 -= 1;
                                    v28 = *((char *)iter2);
                                }
                                v29 = *(&(&g_4ae320)[*((char *)iter2)]);
                                if (!v29)
                                {
                                    *((unsigned int *)sub_46e96f()) = 42;
LABEL_48e84a:
                                    v4 = 0xffffffff;
                                }
                                else
                                {
                                    if (v29 + 1 == j)
                                    {
                                        iter2 += j;
                                    }
                                    else
                                    {
                                        v30 = *(v10) + v11;
                                        if ((char)v30[4] & 72)
                                        {
                                            v31 = iter2 + 1;
                                            *((char *)&v30[5]) = *((char *)iter2);
                                            if (j >= 2)
                                            {
                                                *((char *)(v11 + *(v10) + 37)) = *((char *)v31);
                                                v31 += 1;
                                            }
                                            if (j == 3)
                                            {
                                                *((char *)(v11 + *(v10) + 38)) = *((char *)v31);
                                                v31 += 1;
                                            }
                                            iter2 = v31 - j;
                                        }
                                        else
                                        {
                                            sub_47b5cb(a0, -(j), (int)(-(j)) >> 31, 1);
                                        }
                                    }
LABEL_48e819:
                                    v32 = iter2 - v6;
                                    v5 = MultiByteToWideChar(65001, 0, v6, v32, a1, iter >> 1);
                                    if (!v5)
                                    {
                                        v33 = (unsigned int)GetLastError();
                                        goto LABEL_48e843;
                                    }
                                    else
                                    {
                                        v5 *= 2;
                                        *((unsigned int *)(v11 + *(v10) + 48)) = v5 != v32;
                                    }
                                }
                            }
                            else
                            {
                                if (v3 && *((short *)v6) == 10)
                                    v21->padding_0[0] = v21->padding_0[0] | 4;
                                else
                                    v21->padding_0[0] = v21->padding_0[0] & 251;
                                node = v6;
                                iter = node;
                                v5 += node;
                                if (node < v5)
                                {
                                    do
                                    {
                                        v35 = iter;
                                        v36 = *((short *)v35);
                                        if (v36 == 26)
                                        {
                                            v38 = v11 + *(v10) + 4;
                                            if (!(v38->padding_0[0] & 64))
                                            {
                                                v38->padding_0[0] = v38->padding_0[0] | 2;
                                                break;
                                            }
                                            else
                                            {
                                                *((short *)node) = *((short *)v35);
                                                node += 2;
                                                break;
                                            }
                                        }
                                        if (v36 != 13)
                                        {
                                            *((unsigned short *)node) = v36;
                                            node += 2;
                                            iter = v35 + 2;
                                            continue;
                                        }
                                        if (v35 < v5 - 2)
                                        {
                                            v37 = v35 + 2;
                                            if (*((short *)v37) == 10)
                                            {
                                                iter = v35 + 4;
                                                goto LABEL_48e8f1;
                                            }
                                            else
                                            {
                                                iter = v37;
                                                goto LABEL_48e984;
                                            }
                                        }
                                        else
                                        {
                                            iter += 2;
                                            if ((ReadFile(*((int *)(v11 + *(v10))), v7, 2, &v3, NULL) || !GetLastError()) && v3)
                                            {
                                                if (*((char *)(v11 + *(v10) + 4)) & 72)
                                                {
                                                    if (v7 != 10)
                                                    {
                                                        v0 = 13;
                                                        *((unsigned short *)node) = 13;
                                                        *((char *)(v11 + *(v10) + 5)) = *(&v7[0]);
                                                        *((char *)(v11 + *(v10) + 37)) = *(&v7[1]);
                                                        *((char *)(v11 + *(v10) + 38)) = 10;
                                                        goto LABEL_48e98a;
                                                    }
                                                }
                                                else
                                                {
                                                    if (node != v6 || v7 != 10)
                                                    {
                                                        sub_47b5cb(a0, -0x2, -0x1, 1);
                                                        if (v7 == 10)
                                                            continue;
                                                        goto LABEL_48e984;
                                                    }
                                                }
LABEL_48e8f1:
                                                v0 = 10;
                                                goto LABEL_48e986;
                                            }
                                            else
                                            {
LABEL_48e984:
                                                v0 = 13;
LABEL_48e986:
                                                *((unsigned short *)node) = v0;
LABEL_48e98a:
                                                node += 2;
                                            }
                                        }
                                    } while (iter < v5);
                                }
                                v5 = node - v6;
                            }
                        }
                        else
                        {
                            v33 = (unsigned int)GetLastError();
                            v0 = 5;
                            if (v33 == 5)
                            {
                                *((unsigned int *)sub_46e96f()) = 9;
                                *((unsigned int *)sub_46e982()) = 5;
                                goto LABEL_48e84a;
                            }
                            if (v33 != 109)
                            {
LABEL_48e843:
                                sub_46e995(v33);
                                goto LABEL_48e84a;
                            }
                            else
                            {
                                v4 = NULL;
                            }
                        }
LABEL_48e84e:
                        if (v6 != a1)
                            sub_46c941(v6);
                        v39 = v4;
                        if (v39 == 0xfffffffe)
                            v39 = v5;
                    }
                }
            }
        }
        else
        {
            *((unsigned int *)sub_46e982()) = 0;
            *((unsigned int *)sub_46e96f()) = 22;
LABEL_48e541:
            sub_470f2d(0, 0, 0, 0, 0);
            goto LABEL_48e584;
        }
    }
    return v39;
}
