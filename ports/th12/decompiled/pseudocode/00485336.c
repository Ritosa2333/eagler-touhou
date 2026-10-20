// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485336; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_485336_0 {
    char padding_0[152];
    struct st_485336_1 *field_98;
    struct st_485336_1 *field_9c;
    struct st_485336_1 *field_a0;
    unsigned int field_a4;
    unsigned int field_a8;
    unsigned int field_ac;
    unsigned int field_b0;
} st_485336_0;

typedef struct st_485336_1 {
    char field_0;
    char field_1;
} st_485336_1;

typedef struct st_485336_3 {
    char field_0;
} st_485336_3;

unsigned int sub_485336(int a0, unsigned int a1, unsigned short *idx, st_485336_3 **a3, unsigned int *a4, st_485336_0 *a5)
{
    unsigned int v17;  // edi
    char *iter;  // edi
    unsigned int *v27;  // ebx
    st_485336_3 **v28;  // esi
    unsigned int v29;  // edx
    char *iter1;  // ecx
    int v31;  // ecx
    unsigned int v32;  // eax
    unsigned int v33;  // eax
    unsigned int v34;  // eax
    unsigned int v35;  // eax
    unsigned int v36;  // eax
    unsigned int *v19;  // ecx
    char *v37;  // edi
    char v38;  // al
    char *v39;  // eax
    unsigned int v40;  // eax
    unsigned int v41;  // eax
    unsigned int v42;  // eax
    int v43;  // eax
    unsigned int v44;  // eax
    unsigned int v45;  // eax
    unsigned int v46;  // eax
    unsigned int v20;  // eax
    unsigned int v47;  // eax
    st_485336_1 *k;  // edi
    st_485336_1 *v49;  // eax
    st_485336_1 *v50;  // eax
    unsigned int v51;  // eax
    unsigned int v52;  // eax
    unsigned int v53;  // eax
    char *v54;  // eax
    void* node;  // eax
    void* iter2;  // ebx
    int i;  // eax
    unsigned int *v24;  // esi
    st_485336_3 **v25;  // ecx
    char j;  // al
    unsigned int v0;  // [bp-0x48]
    unsigned int v1;  // [bp-0x44]
    char *v2;  // [bp-0x40]
    char *v3;  // [bp-0x3c]
    unsigned int v4;  // [bp-0x38]
    unsigned int v5;  // [bp-0x34]
    int v6;  // [bp-0x30]
    unsigned short v7;  // [bp-0x24]
    unsigned short v8;  // [bp-0x22]
    unsigned short v9;  // [bp-0x1e]
    unsigned short v10;  // [bp-0x1c]
    unsigned short v11;  // [bp-0x1a]
    unsigned short v12;  // [bp-0x18]
    unsigned short v13;  // [bp-0x16]
    int v14;  // [bp-0x14], Other Possible Types: unsigned int
    char *v15;  // [bp-0x10], Other Possible Types: unsigned int *
    void* v16;  // [bp-0xc], Other Possible Types: int

    *((unsigned int *)&v6) = v17;
    iter = &(!a1 ? a5->field_a0 : (a1 == 1 ? a5->field_a4 : a5->field_a8))->field_0;
    if (a5->field_b0 != 1)
    {
        v19 = GetDateFormatA;
        if (a1 == 2)
            v19 = GetTimeFormatA;
        v5 = 0;
        v7 = 1900 + idx[10];
        v8 = idx[8] + 1;
        v9 = idx[6];
        v10 = idx[4];
        v12 = *(idx);
        v4 = 0;
        v3 = iter;
        v13 = 0;
        v2 = &v7;
        v1 = 0;
        v0 = a5->field_ac;
        v15 = v19;
        v11 = idx[2];
        v14 = v19();
        if (v14)
        {
            v20 = v14 + 8;
            if (v20 <= 0x400)
            {
                sub_48d830();
                node = &v6;
                if (!&v6)
                    goto LABEL_48541d;
                *((unsigned int *)&v6) = 0xcccc;
                goto LABEL_48541a;
            }
            else
            {
                node = sub_46d04a(v20);
                if (node)
                {
                    *((unsigned int *)node) = 0xdddd;
LABEL_48541a:
                    node += 8;
                }
            }
LABEL_48541d:
            v16 = node;
            if (node)
            {
                iter2 = node;
                i = v15(a5->field_ac, 0, &v7, iter, iter2, v14) - 1;
                if (i > 0)
                {
                    v24 = a4;
                    v25 = a3;
                    do
                    {
                        if (*(v24) <= 0)
                            break;
                        v14 = i;
                        *(v25)->field_0 = *((char *)iter2);
                        *(v25) = *(v25) + 1;
                        iter2 += 1;
                        *(v24) = *(v24) - 1;
                        i = v14 - 1;
                    } while (i > 0);
                }
                sub_48326a(v16);
                return v32;
            }
        }
    }
    j = *(iter);
    if (!*(iter))
        return v32;
    v27 = a4;
    v28 = a3;
    do
    {
        if (!*(v27))
            break;
        v29 = 0;
        v16 = 0;
        iter1 = iter;
        do
        {
            iter1 += 1;
            v29 += 1;
        } while (*(iter1) == j);
        v15 = iter1;
        v31 = j;
        if (v31 <= 100)
        {
            if (v43 == 100)
            {
                v40 = v29;
                v41 = v40 - 1;
                if (v40 != 1)
                {
                    v42 = v41 - 1;
                    if (v41 != 1)
                    {
                        if (v42 == 1)
                        {
                            goto LABEL_48576d;
                        }
                        else if (v42 == 2)
                        {
                            goto LABEL_48576d;
                        }
                    }
                }
                else
                {
                    v16 = 1;
                }
                goto LABEL_48576d;
            }
            if (v43 == 39)
            {
                iter = &iter[v29];
                if (!((char)v29 & 1))
                    continue;
                v38 = *(iter);
                if (!*(iter))
                    break;
                do
                {
                    if (!*(v27))
                        break;
                    if (v38 == 39)
                    {
                        iter += 1;
                        break;
                    }
                    if (sub_47c5a3(v38, a0) && *(v27) > 1)
                    {
                        v39 = iter + 1;
                        if (!*(v39))
                            return v32;
                        *(v28)->field_0 = *(iter);
                        *(v28) = *(v28) + 1;
                        *(v27) = *(v27) - 1;
                        iter = v39;
                    }
                    *(v28)->field_0 = *(iter);
                    *(v28) = *(v28) + 1;
                    iter += 1;
                    *(v27) = *(v27) - 1;
                    v38 = *(iter);
                } while (*(iter));
            }
            if (v43 != 65)
            {
                if (v43 == 72)
                {
                    v36 = v29;
                    if (v36 == 1)
                    {
                        v16 = 1;
                    }
                    else if (!(v36 == 2))
                    {
                        goto LABEL_4854d6;
                    }
                    goto LABEL_48576d;
                }
                if (v43 == 77)
                {
                    v33 = v29;
                    v34 = v33 - 1;
                    if (v33 != 1)
                    {
                        v35 = v34 - 1;
                        if (v34 != 1)
                        {
                            if (v35 != 1)
                            {
                                if (v35 == 2)
                                    goto LABEL_48576d;
                            }
                            else
                            {
                                goto LABEL_48576d;
                            }
                        }
                    }
                    else
                    {
                        v16 = 1;
                    }
                    goto LABEL_48576d;
                }
                else if (!(v43 == 97))
                {
                    goto LABEL_4854d6;
                }
            }
            if (!sub_46e2ea(iter, "am/pm"))
            {
                v37 = iter + 5;
            }
            else
            {
                if (sub_46e2ea(iter, "a/p"))
                    goto LABEL_48558b;
                v37 = iter + 3;
            }
            v15 = v37;
LABEL_48558b:
            goto LABEL_48576d;
        }
        else
        {
            v43 = v31;
            v44 = v43 - 104;
            if (v43 != 104)
            {
                v45 = v44 - 5;
                if (v44 != 5)
                {
                    v46 = v45 - 6;
                    if (v45 != 6)
                    {
                        if (v46 == 1)
                        {
                            if (v29 == 1 && *(v27) > 0)
                            {
                                if (sub_47c5a3(k->field_0, a0) && *(v27) > 1)
                                {
                                    v49 = &k->field_1;
                                    if (!v49->field_0)
                                        return v32;
                                    *(v28)->field_0 = k->field_0;
                                    *(v28) = *(v28) + 1;
                                    *(v27) = *(v27) - 1;
                                    k = v49;
                                }
                                *(v28)->field_0 = k->field_0;
                                *(v28) = *(v28) + 1;
                                *(v27) = *(v27) - 1;
                                goto LABEL_485788;
                            }
                            else
                            {
                                for (k = (*((int *)&idx[4]) <= 11 ? a5->field_98 : a5->field_9c); k->field_0 && *(v27) > 0; *(v27) = *(v27) - 1)
                                {
                                    if (sub_47c5a3(k->field_0, a0) && *(v27) > 1)
                                    {
                                        v50 = &k->field_1;
                                        if (!v50->field_0)
                                            return v32;
                                        *(v28)->field_0 = k->field_0;
                                        *(v28) = *(v28) + 1;
                                        *(v27) = *(v27) - 1;
                                        k = v50;
                                    }
                                    *(v28)->field_0 = k->field_0;
                                    *(v28) = *(v28) + 1;
                                    k = &k->field_1;
                                }
                            }
                        }
                        if (v46 == 6)
                        {
                            v47 = v29 - 1;
                            if (v47 == 1)
                            {
                                goto LABEL_48576d;
                            }
                            else if (v47 == 3)
                            {
                                goto LABEL_48576d;
                            }
                        }
                    }
                    else
                    {
                        v51 = v29;
                        if (v51 == 1)
                        {
                            v16 = 1;
                        }
                        else if (!(v51 == 2))
                        {
                            goto LABEL_4854d6;
                        }
                        goto LABEL_48576d;
                    }
                }
                else
                {
                    v52 = v29;
                    if (v52 == 1)
                    {
                        v16 = 1;
                    }
                    else if (!(v52 == 2))
                    {
                        goto LABEL_4854d6;
                    }
                    goto LABEL_48576d;
                }
            }
            else
            {
                v53 = v29;
                if (v53 != 1)
                {
                    if (v53 == 2)
                        goto LABEL_48576b;
LABEL_4854d6:
                    if (sub_47c5a3(v43, a0) && *(v27) > 1)
                    {
                        v54 = iter + 1;
                        if (!*(v54))
                            return v32;
                        *(v28)->field_0 = *(iter);
                        *(v28) = *(v28) + 1;
                        *(v27) = *(v27) - 1;
                        iter = v54;
                    }
                    *(v28)->field_0 = *(iter);
                    *(v28) = *(v28) + 1;
                    iter += 1;
                    *(v27) = *(v27) - 1;
                }
                else
                {
                    v16 = 1;
LABEL_48576b:
LABEL_48576d:
                    if (!sub_484f4e(a0, v27, a5, v16))
                        return v32;
LABEL_485788:
                    iter = v15;
                }
            }
        }
    } while ((j = *(iter), *(iter)));
    return v32;
}
