// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x447b20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_447b20_3 {
    char padding_0[1648];
    unsigned int field_670;
} st_447b20_3;

typedef struct st_447b20_0 {
    char padding_0[40];
    unsigned int field_28;
    char padding_2c[212];
    unsigned int field_100;
    char padding_104[212];
    unsigned int field_1d8;
    char padding_1dc[212];
    unsigned int field_2b0;
} st_447b20_0;

typedef struct st_447b20_1 {
    unsigned int field_0;
    struct st_447b20_1 *field_4;
} st_447b20_1;

extern int g_4a1fd8;
extern unsigned int g_4ad138;
extern char g_4af208;
extern unsigned int g_4b451c;
extern unsigned int g_4ce8cc;

void sub_447b20(unsigned int *index)
{
    int *v8;  // ebx
    unsigned int v9;  // esi
    char i;  // 4102
    st_447b20_0 **v19;  // esi
    st_447b20_0 **v20;  // esi
    st_447b20_0 **v21;  // esi
    int v22;  // esi
    int v23;  // ebp
    unsigned int v24;  // ecx
    int j;  // edx
    st_447b20_1 *iter;  // eax
    unsigned int v27;  // eax
    unsigned int v10;  // edi
    st_447b20_1 *node;  // eax
    unsigned int v29;  // esi
    unsigned int *v30;  // ebp
    st_447b20_3 *iter1;  // edi
    unsigned int v32;  // ebx
    unsigned int v33;  // ebx
    unsigned int k;  // ecx
    st_447b20_1 *iter2;  // eax
    unsigned int v36;  // eax
    st_447b20_1 *v37;  // eax
    int v11;  // edi
    int v12;  // edx
    unsigned int *v13;  // edx
    unsigned int v14;  // ebp
    unsigned int v15;  // ebx
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int v0;  // [bp-0xc8]
    unsigned int v1;  // [bp-0xc4]
    int *v2;  // [bp-0xbc]
    int <0x447b20[is_1]|Stack bp-0xb8, 1 B>;  // [bp-0xb8]
    unsigned int *v3;  // [bp-0xb8]
    unsigned int *idx;  // [bp-0xb4]
    unsigned int *v5;  // [bp-0xb0]
    unsigned int v6;  // [bp-0x4]

    v6 = g_4ad138 ^ &<0x447b20[is_1]|Stack bp-0xb8, 1 B>;
    v2 = v8;
    v1 = v9;
    v0 = v10;
    v11 = 0;
    v12 = 0;
    v5 = index;
    if (index[118] * 10 - 10 > 0)
    {
        do
        {
            if ((&g_4af208)[v11] == index[64])
                v12 += 1;
        } while ((v11 = (int)(v11 + 1), v12 < index[118] * 10 - 10));
    }
    v13 = NULL;
    index[172] = 0;
    idx = NULL;
    v3 = index + 412;
    while (1)
    {
        if (v11 < 113)
        {
            while ((&g_4af208)[v11] != index[64])
            {
                if (v11 + 1 >= 113)
                    goto LABEL_447bb1;
            }
            if (v11 >= 113)
                goto LABEL_447bb1;
            v14 = g_4b451c;
            v15 = v11 * 144;
            if (*((int *)(v15 + g_4b451c + 109224)))
            {
                v16 = v15 + g_4b451c + 109092;
                v17 = v16;
                do
                {
                    i = *((char *)v17);
                    *((char *)&v5 - v16 + v17) = *((char *)v17);
                    v17 += 1;
                } while (i);
                v19 = &v5;
                do
                {
                    v20 = v19;
                    v21 = (char *)v20 + 1;
                    v19 = v21;
                } while (*((char *)v20));
                v22 = v21 - ((char *)&v5 + 1);
                if (v22 < 42)
                {
                    v23 = 42 - v22;
                    sub_477420((char *)&v5 + v22, 32, v23);
                    v22 += v23;
                    v14 = g_4b451c;
                }
                v24 = idx[10] * 17908 + v14 + 8;
                j = *(v2);
                *((char *)&v5 + v22) = 0;
                if (!j)
                {
LABEL_447c6e:
                }
                else
                {
                    iter = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            if (*((int *)iter->field_0) == j)
                            {
                                v27 = iter->field_0;
                                goto LABEL_447cb5;
                            }
                        } while ((iter = (st_447b20_1 *)iter->field_4, iter));
                    }
                    node = *((int *)(g_4ce8cc + 8935104));
                    if (!*((int *)(g_4ce8cc + 8935104)))
                        goto LABEL_447c6e;
                    v15 = v15;
                    while (*((int *)node->field_0) != j)
                    {
                        if (!node->field_4)
                            goto LABEL_447cb9;
                    }
                    v27 = node->field_0;
LABEL_447cb5:
                    if (v27)
                        goto LABEL_447cc3;
                }
LABEL_447cb9:
                *(v2) = 0;
LABEL_447cc3:
                v11 += 1;
                sub_4608f0((-(0 < *((int *)(v15 + v24 + 1764))) & 1052561) + 0xefefef, 0, 0, 0, "No.%3d %s %4d/%4d", v11, &v5, *((int *)(v15 + v24 + 1764)), *((int *)(v15 + v24 + 1768)));
            }
            else
            {
                v29 = index[10] * 17908 + g_4b451c + 8;
                if (!sub_461920(*(v2)))
                    *(v30) = 0;
                v11 += 1;
                sub_4608f0(0x808080, 0, 0, 0, &g_4a1fd8, v11, *((int *)(v15 + v29 + 1764)), *((int *)(v15 + v29 + 1768)));
            }
            idx[172] = idx[172] + 1;
            v2 += 1;
            v3 = (char *)v3 + 1;
            if (v3 >= 0xa)
                break;
            index = idx;
            v13 = v3;
        }
        else
        {
LABEL_447bb1:
            if (v13 < 0xa)
            {
                iter1 = (char *)index + 0x4 * v13 + 1648;
                v32 = -0x1 * v13 + 10;
                do
                {
                    v33 = v32;
                    k = *((int *)&iter1->padding_0[0]);
                    if (!k)
                    {
LABEL_447bda:
                        goto LABEL_447dca;
                    }
                    else
                    {
                        iter2 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)))
                        {
                            do
                            {
                                if (*((int *)iter2->field_0) == k)
                                {
                                    v36 = iter2->field_0;
                                    goto LABEL_447dc6;
                                }
                            } while ((iter2 = (st_447b20_1 *)iter2->field_4, iter2));
                        }
                        v37 = *((int *)(g_4ce8cc + 8935104));
                        if (!*((int *)(g_4ce8cc + 8935104)))
                            goto LABEL_447bda;
                        while (*((int *)v37->field_0) != k)
                        {
                            if (!v37->field_4)
                                goto LABEL_447dca;
                        }
                        v36 = v37->field_0;
LABEL_447dc6:
                        if (v36)
                            continue;
LABEL_447dca:
                        *((unsigned int *)&iter1->padding_0[0]) = 0;
                    }
                } while ((sub_4608f0(-0x1, 0, 0, 0, " "), iter1 += 4, v32 = v33 - 1, v33 != 1));
            }
        }
    }
    _security_check_cookie();
    return;
}
