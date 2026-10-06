// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4692e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4692e0_3 {
    struct st_4692e0_0 *field_0;
    unsigned int field_4;
    int field_8;
    char padding_c[128];
    struct st_4692e0_4 *field_8c;
} st_4692e0_3;

typedef struct st_4692e0_7 {
    unsigned int field_0;
    unsigned short field_4;
    unsigned short field_6;
    char padding_8[8];
    unsigned short field_10;
} st_4692e0_7;

typedef struct st_4692e0_2 {
    char field_0;
    char field_1;
    char field_2;
} st_4692e0_2;

typedef struct st_4692e0_6 {
    char padding_0[36];
    unsigned int field_24;
} st_4692e0_6;

typedef struct st_4692e0_4 {
    struct st_4692e0_2 *field_0;
} st_4692e0_4;

typedef struct st_4692e0_1 {
    unsigned int field_0;
} st_4692e0_1;

typedef struct st_4692e0_0 {
    char padding_0[4];
    struct st_4692e0_1 *field_4;
} st_4692e0_0;

unsigned int sub_4692e0(st_4692e0_3 *idx, unsigned int a1, unsigned int a2)
{
    st_4692e0_7 *v2;  // ecx
    st_4692e0_7 *v3;  // eax
    int idx1;  // ecx
    char *v13;  // eax
    char *v14;  // eax
    char *v15;  // eax
    char *v16;  // eax
    unsigned int index;  // ebx
    st_4692e0_2 **node;  // ebp
    st_4692e0_2 *v19;  // ecx
    char *iter;  // eax
    char v21;  // 4101
    st_4692e0_2 **v4;  // ebx
    unsigned int v22;  // cc_dep1
    unsigned int v23;  // cc_dep2
    char v24;  // 4103
    int v25;  // eax
    unsigned int v26;  // 4111
    unsigned int v27;  // 4120
    unsigned int k;  // ecx
    void* v29;  // eax
    char *v30;  // eax
    char *v31;  // eax
    st_4692e0_6 *iter1;  // ebp
    char *v32;  // eax
    char *v33;  // eax
    unsigned int v34;  // edi
    unsigned int v6;  // eax
    char *v7;  // edi
    int v8;  // edi
    int v9;  // ebp
    int v10;  // ebx
    st_4692e0_2 **v11;  // eax
    int j;  // [bp-0x8]
    st_4692e0_6 *v1;  // [bp-0x4]

    *((unsigned int *)&idx->padding_c[4 * idx->field_4]) = a2;
    v2 = *((int *)&idx->padding_c[4 * idx->field_4]);
    if (v2->field_0 == 1414546259 && v2->field_4 == 1)
    {
        v3 = v2;
        v4 = idx->field_8c;
        iter1 = v3->field_6 + (char *)v3 + 36;
        v6 = v3->field_10;
        idx->field_8 = idx->field_8 + v6;
        v7 = &iter1->padding_0[4 * v6];
        v11 = sub_46d04a(idx->field_8 * 8, v8, v9, v10);
        idx->field_8c = v11;
        if (!v4)
        {
            idx1 = 0;
            if (idx->field_8 > NULL)
            {
                do
                {
                    idx->field_8c[1 + 2 * idx1].field_0 = *((int *)&idx->padding_c[4 * idx->field_4]) + *((int *)&iter1->padding_0[0]);
                    idx->field_8c[2 * idx1].field_0 = v7;
                    v13 = v7;
                    v14 = v13;
                    do
                    {
                        v15 = v14;
                        v16 = v15 + 1;
                        v14 = v16;
                    } while (*(v15));
                    idx1 += 1;
                    iter1 = &iter1->padding_0[4];
                    v7 = v7 + v16 - v13;
                } while (idx1 < idx->field_8);
            }
        }
        else
        {
            a2 = idx->field_8 - *((short *)(*((int *)&idx->padding_c[4 * idx->field_4]) + 16));
            sub_47a330(v11, v4, a2 * 8);
            if (v4)
                sub_46c941(v4);
            j = 0;
            if (0 < *((short *)(*((int *)&idx->padding_c[4 * idx->field_4]) + 16)))
            {
                do
                {
                    index = 0;
                    if (a2 > NULL)
                    {
                        node = idx->field_8c;
                        do
                        {
                            v19 = *(node);
                            iter = v7;
                            while (1)
                            {
                                v21 = *(iter);
                                v22 = v21;
                                v23 = v19->field_0;
                                if (v21 != v19->field_0)
                                    break;
                                if (v21)
                                {
                                    v24 = iter[1];
                                    v22 = v24;
                                    v23 = v19->field_1;
                                    if (v24 != v19->field_1)
                                        break;
                                    iter += 2;
                                    v19 = &v19->field_2;
                                    if (!v24)
                                        goto LABEL_469421;
                                }
                                else
                                {
LABEL_469421:
                                    v25 = 0;
                                    goto LABEL_46942a;
                                }
                            }
                            v26 = _ccall(4, v22, v23, 0);
                            v27 = _ccall(12, 0, v26 & 1, v26 & 1);
                            v25 = -(v26 & 1) + 1 - (v27 & 1);
LABEL_46942a:
                        } while (v25 > NULL && (index += 1, node += 8, index < a2));
                        iter1 = v1;
                    }
                    k = idx->field_8 - 1;
                    if (k > index)
                    {
                        do
                        {
                            v29 = &idx->field_8c[2 * k];
                            *((int *)v29) = *((int *)((char *)v29 - 8));
                            k -= 1;
                            *((int *)&v29[4]) = *((int *)((char *)v29 - 4));
                        } while (k > index);
                    }
                    idx->field_8c[1 + 2 * index].field_0 = *((int *)&idx->padding_c[4 * idx->field_4]) + *((int *)&iter1->padding_0[0]);
                    idx->field_8c[2 * index].field_0 = v7;
                    v30 = v7;
                    v31 = v30;
                    do
                    {
                        v32 = v31;
                        v33 = v32 + 1;
                        v31 = v33;
                    } while (*(v32));
                    a2 += 1;
                    v7 = v7 + v33 - v30;
                    iter1 = &iter1->padding_0[4];
                    j += 1;
                    v1 = iter1;
                } while (j < *((short *)(*((int *)&idx->padding_c[4 * idx->field_4]) + 16)));
            }
        }
        v34 = idx->field_4;
        idx->field_4 = v34 + 1;
        if (!*((short *)(*((int *)&idx->padding_c[4 * v34]) + 6)))
            return v34;
        idx->field_0->field_4(*((int *)&idx->padding_c[4 * v34]) + 36);
        return v34;
    }
    *((unsigned int *)&idx->padding_c[4 * idx->field_4]) = 0;
    return 0xffffffff;
}
