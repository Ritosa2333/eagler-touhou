// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492d00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492d00_2 {
    char padding_0[12];
    struct st_492d00_1 *field_c;
} st_492d00_2;

typedef struct st_492d00_4 {
    char padding_0[12];
    int field_c;
    unsigned int field_10;
} st_492d00_4;

typedef struct st_492d00_3 {
    char padding_0[28];
    struct st_492d00_2 *field_1c;
} st_492d00_3;

typedef struct st_492d00_0 {
    unsigned int field_0;
    int field_4;
    char padding_8[4];
    unsigned int field_c;
    char padding_10[12];
    int field_1c;
} st_492d00_0;

typedef struct st_492d00_1 {
    unsigned int field_0;
    unsigned int field_4;
    char field_8;
} st_492d00_1;

extern int g_4ab554;
extern int g_4ae4d8;

void sub_492d00(st_492d00_3 *a0, void* a1, unsigned int a2, int a3, st_492d00_0 *a4, char a5, int a6, void* v2)
{
    void* v18;  // ecx
    unsigned int v19;  // ebx
    unsigned int v28;  // esi
    unsigned int v29;  // eax
    st_492d00_0 *v30;  // edi
    st_492d00_4 *iter;  // eax
    unsigned int node;  // ebx
    unsigned int v34;  // edx
    st_492d00_0 *v20;  // ebx
    unsigned int v21;  // esi
    unsigned int v22;  // edi
    st_492d00_3 *idx;  // esi
    unsigned int v24;  // ebx
    unsigned int *v27;  // edi
    void* v0;  // [bp-0x60]
    int v1;  // [bp-0x5c]
    unsigned int v3;  // [bp-0x54]
    st_492d00_3 *v4;  // [bp-0x4c]
    unsigned int v5;  // [bp-0x48]
    unsigned int v6;  // [bp-0x3c]
    unsigned int v7;  // [bp-0x38]
    unsigned int v8;  // [bp-0x34]
    char v9;  // [bp-0x30]
    unsigned int v10;  // [bp-0x24]
    char v11;  // [bp-0x20], Other Possible Types: unsigned int
    int v12;  // [bp-0x1c]
    unsigned int v13;  // [bp-0x18]
    char v14;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v15;  // [bp-0x10]
    int v16;  // [bp-0xc]
    char v17;  // [bp-0x5]

    v18 = a1;
    v8 = v19;
    v20 = a4;
    v7 = v21;
    v6 = v22;
    v17 = 0;
    v16 = (v20->field_4 <= 128 ? (char)v18[8] : (int)v18[8]);
    if (v16 >= -0x1 && v16 < v20->field_4)
    {
        idx = a0;
        if (*((int *)&idx->padding_0[0]) == 3765269347)
        {
            v24 = 429065504;
            if (*((int *)&idx->padding_0[16]) == 3 && (*((int *)&idx->padding_0[20]) == 429065504 || *((int *)&idx->padding_0[20]) == 429065505 || *((int *)&idx->padding_0[20]) == 429065506) && !idx->field_1c)
            {
                if (*((int *)(sub_475467() + 0x88)))
                {
                    idx = *((int *)(sub_475467() + 0x88));
                    a0 = idx;
                    a2 = *((int *)(sub_475467() + 140));
                    if (!sub_49319c(idx, 1))
                        sub_472b94(); /* do not return */
                    if (*((int *)&idx->padding_0[0]) == 3765269347 && *((int *)&idx->padding_0[16]) == 3 && (*((int *)&idx->padding_0[20]) == 429065504 || *((int *)&idx->padding_0[20]) == 429065505 || *((int *)&idx->padding_0[20]) == 429065506) && !idx->field_1c)
                        sub_472b94(); /* do not return */
                    a0 = a0;
                    if (*((int *)(sub_475467() + 148)))
                    {
                        v27 = *((int *)(sub_475467() + 148));
                        v28 = 0;
                        *((unsigned int *)(sub_475467() + 148)) = 0;
                        if (!sub_492530(a0))
                        {
                            v24 = 0;
                            if (*(v27) <= NULL)
                                sub_472b48(); /* do not return */
                            while (!(char)sub_46cdd0(&g_4ae4d8))
                            {
                                v28 += 1;
                                v24 += 16;
                                if (v28 >= *(v27))
                                    sub_472b48(); /* do not return */
                            }
                            v5 = 1;
                            v4 = a0;
                            sub_492181();
                            sub_491f63("bad exception");
                            sub_46ff8f(&v9, &g_4ab554);
                        }
                        idx = a0;
                    }
                }
                else
                {
                    return;
                }
            }
            if (*((int *)&idx->padding_0[0]) == 3765269347 && *((int *)&idx->padding_0[16]) == 3 && !(v29 = (unsigned int)*((int *)&idx->padding_0[20]), *((int *)&idx->padding_0[20]) != v24 && *((int *)&idx->padding_0[20]) != 429065505 && *((int *)&idx->padding_0[20]) != 429065506))
            {
                v30 = a4;
                if (v30->field_c > NULL)
                {
                    for (iter = sub_491cdf(v30, a6, v16, &v14, &v11); v14 < v11; iter += 1)
                    {
                        if (*((int *)&iter->padding_0[0]) <= v16 && v16 <= *((int *)&iter->padding_0[4]))
                        {
                            v15 = iter->field_10;
                            v12 = iter->field_c;
                            if (iter->field_c > NULL)
                            {
                                do
                                {
                                    node = &idx->field_1c->field_c->field_4;
                                    v13 = idx->field_1c->field_c->field_0;
                                    if (idx->field_1c->field_c->field_0 > NULL)
                                    {
                                        do
                                        {
                                            v10 = *((int *)node);
                                            if (sub_491fb3(v15, *((int *)node), idx->field_1c))
                                            {
                                                v17 = 1;
                                                sub_492b9e(idx, a2, a3, a4, v10, a6, v2);
                                                idx = a0;
                                                goto LABEL_492f59;
                                            }
                                        } while ((v13 -= 1, node += 4, v13 > 0));
                                    }
                                } while ((v12 = (int)(v12 - 1), v15 += 16, v12 > 0));
                            }
                        }
LABEL_492f59:
                        v14 += 1;
                    }
                    v30 = a4;
                }
                if (a5)
                {
                    v3 = 1;
                    v2 = idx;
                    sub_492181();
                }
                if (!v17 && (v30->field_0 & 0x1fffffff) >= 429065505 && v30->field_1c && !sub_492530(idx))
                {
                    sub_475467();
                    sub_475467();
                    *((st_492d00_3 **)(sub_475467() + 0x88)) = idx;
                    *((unsigned int *)(sub_475467() + 140)) = a2;
                    if (!v2)
                    {
                        v2 = a1;
                        v2 = a1;
                    }
                    else
                    {
                        v2 = v2;
                    }
                    sub_491a21(a2, v34, v2, idx);
                    v3 = 0xffffffff;
                    v2 = a4;
                    v1 = a3;
                    v0 = a1;
                    sub_49205b();
                    sub_4925ab(a4->field_1c); /* do not return */
                }
                goto LABEL_49302f;
            }
            else
            {
                v20 = a4;
            }
        }
        if (v20->field_c > NULL)
        {
            if (a5)
                sub_472b48(); /* do not return */
            sub_492c0c(idx, a1, a2, a3, v20, v16, a6, v2);
        }
LABEL_49302f:
        if (!*((int *)(sub_475467() + 148)))
            return;
        sub_472b94(); /* do not return */
    }
    sub_472b94(); /* do not return */
}
