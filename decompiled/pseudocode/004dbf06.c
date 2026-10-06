// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbf06; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dbf06_2 {
    char padding_0[81];
    struct st_4dbf06_3 *field_51;
    char padding_55[9];
    struct st_4dbf06_3 *field_5e;
    char padding_62[8];
    struct st_4dbf06_3 *field_6a;
    char padding_6e[214];
    unsigned int field_144;
    unsigned int field_148;
    char padding_14c[828];
    struct st_4dbf06_0 *field_488;
    unsigned int field_48c;
    unsigned int field_490;
    char padding_494[257];
    struct st_4dbf06_3 *field_595;
    unsigned int field_599;
    unsigned int field_59d;
    char padding_5a1[4];
    unsigned int field_5a5;
    struct st_4dbf06_0 *field_5a9;
    unsigned int field_5ad;
    struct st_4dbf06_3 *field_5b1;
    struct st_4dbf06_3 *field_5b5;
    char padding_5b9[12];
    unsigned int field_5c5;
    unsigned int field_5c9;
    char field_5cd[4];
    char field_5d0;
    char padding_5d1[2516];
    struct st_4dbf06_3 *field_fa5;
    struct st_4dbf06_3 *field_fa9;
    struct st_4dbf06_3 *field_fad;
} st_4dbf06_2;

typedef struct st_4dbf06_3 {
    unsigned int field_0;
} st_4dbf06_3;

typedef struct st_4dbf06_14 {
    unsigned int field_0;
    unsigned short field_4;
} st_4dbf06_14;

typedef struct st_4dbf06_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_4dbf06_0;

typedef struct st_4dbf06_13 {
    unsigned short field_0;
    char padding_2[2];
    unsigned int field_4;
} st_4dbf06_13;

typedef struct st_4dbf06_16 {
    char padding_0[6];
    unsigned short field_6;
    char padding_8[12];
    unsigned short field_14;
    char padding_16[62];
    unsigned int field_54;
} st_4dbf06_16;


int sub_4dbf06(unsigned int a0)
{
    unsigned int v11;  // cc_op
    unsigned int v12;  // cc_dep1
    unsigned int v21;  // ebx
    st_4dbf06_13 *node;  // esi
    unsigned int v23;  // edi
    unsigned int v24;  // ecx
    unsigned int v25;  // ebx
    st_4dbf06_14 *v26;  // esi
    st_4dbf06_14 *v27;  // esi
    st_4dbf06_14 *v28;  // esi
    st_4dbf06_0 *v29;  // edx
    st_4dbf06_3 **iter;  // esi
    unsigned int v13;  // cc_dep2
    unsigned int v31;  // eax
    st_4dbf06_0 *v32;  // eax
    st_4dbf06_0 *v33;  // edx
    unsigned int *v34;  // eax
    unsigned int *v35;  // eax
    unsigned int v36;  // ebx
    unsigned int *v37;  // edi
    unsigned int v38;  // eax
    st_4dbf06_0 *v39;  // ebx
    unsigned int v40;  // ebx
    unsigned int v14;  // cc_ndep
    st_4dbf06_0 *v41;  // ebx
    unsigned int v42;  // eax
    unsigned int *v43;  // eax
    char *v44;  // eax
    st_4dbf06_0 *v45;  // esi
    st_4dbf06_16 *v46;  // edi
    unsigned int v47;  // ecx
    void* iter1;  // edi
    st_4dbf06_2 *v49;  // esi
    unsigned int v50;  // eax
    unsigned int v15;  // 4101
    st_4dbf06_2 *v51;  // esi
    unsigned int v52;  // ecx
    unsigned int v53;  // ecx
    unsigned int i;  // ecx
    st_4dbf06_2 *v55;  // esi
    st_4dbf06_2 *idx;  // ebp
    unsigned int *v17;  // ebx
    st_4dbf06_2 *iter2;  // esi
    unsigned int v19;  // eax
    unsigned int v20;  // edx
    unsigned int v0;  // [bp-0x40]
    st_4dbf06_2 *v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x38]
    unsigned int *v3;  // [bp-0x2c]
    st_4dbf06_2 *v4;  // [bp-0x28]
    st_4dbf06_0 *v5;  // [bp-0x24], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x20]
    unsigned int *v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0xc]
    unsigned int v9;  // [bp-0x4]

    v15 = _ccall(4, v11, v12, v13, v14);
    if (!(v15 & 1))
    {
        InterlockedExchange(&idx->padding_494[253], *(v17));
        *(v17) = *((int *)&idx->padding_494[253]);
    }
    iter2 = &idx->padding_5b9[1];
    if (*((int *)&iter2->padding_0[0]))
    {
        *((unsigned int *)&idx->padding_6e[212]) = (*((int *)&(idx->padding_0)[1]))(0, 0x1800, 0x1000, 4);
        do
        {
            v19 = *((int *)&iter2->padding_0[4]);
            if (*((int *)&iter2->padding_0[4]) != 0xfffffef2)
            {
                *((unsigned int *)&idx->padding_6e[208]) = (*((int *)&(idx->padding_0)[1]))(0, v19 + 270, 0x1000, 4);
                return sub_4dbf59(0, v19 + 270, 0x1000, 4);
            }
        } while ((iter2 += 12, *((int *)&iter2->padding_0[0])));
        (*((int *)&idx->padding_55[6]))(*((int *)&idx->padding_6e[212]), 0, 0x8000);
        if (*((int *)&idx->padding_494[249]))
            InterlockedExchange(&idx->padding_494[253], *((int *)*((int *)&idx->padding_494[249])));
    }
    v20 = *((int *)&idx->padding_14c[820]) - *((int *)&idx->padding_494[245]);
    if (*((int *)&idx->padding_14c[820]) != *((int *)&idx->padding_494[245]))
    {
        v21 = 0;
        node = *((int *)&(idx->padding_494)[1]) + *((int *)&idx->padding_14c[820]);
        while (*((int *)&node->field_0))
        {
            a0 = node->field_4 - 8 >> 1;
            v23 = *((int *)&node->field_0) + *((int *)&idx->padding_14c[820]);
            node += 1;
            do
            {
                v24 = a0;
                v25 = _INSERT(v21, 0, node->field_0);
                v21 = v25 >> 12;
                if (v21 == 1)
                {
                    v21 = node->field_0 & 0xfff;
                    *((unsigned short *)(v23 + v21)) = *((short *)(v23 + v21)) + (unsigned short)(v20 >> 16);
                }
                else if (v21 == 2)
                {
                    v21 = node->field_0 & 0xfff;
                    *((unsigned short *)(v23 + v21)) = *((short *)(v23 + v21)) + (unsigned short)v20;
                }
                else if (v21 == 3)
                {
                    v21 = node->field_0 & 0xfff;
                    *((unsigned int *)(v23 + v21)) = *((int *)(v23 + v21)) + v20;
                }
            } while ((node->field_0 = 0xffff, node += 2, a0 = v24 - 1, v24 != 1));
        }
    }
    if (*((int *)((char *)&idx->field_599 + 1)))
    {
        v26 = *((int *)((char *)&idx->field_599 + 1)) + *((int *)&idx->padding_14c[820]);
        while (1)
        {
            v27 = &v26->field_4;
            if (!v26->field_0)
                break;
            v28 = (char *)&v27->field_0 + 2;
            *((short *)(v26->field_0 + *((int *)&idx->padding_14c[820]))) = v27->field_0;
            v26 = v28;
        }
    }
    iter = 12988 + *((int *)&idx->padding_14c[820]);
    while (iter[3])
    {
        v31 = iter[3] + v29;
        v32 = (*((int *)&idx->padding_5d1[2508]))(v31);
        if (!v32)
            v32 = (*((int *)&idx->padding_5d1[2512]))(v31);
        *((st_4dbf06_0 **)((char *)&idx->field_59d + 1)) = v32;
        *((unsigned int *)&idx->padding_5a1[1]) = 0;
        while (1)
        {
            v33 = *((int *)&idx->padding_14c[820]);
            v34 = &*(iter)->field_0;
            if (!*(iter))
                v34 = &iter[4]->field_0;
            v35 = v34 + v33 + *((int *)&idx->padding_5a1[1]);
            v36 = *(v35);
            v37 = iter[4] + v33 + *((int *)&idx->padding_5a1[1]);
            if (!v36)
            {
                *(iter) = v35;
                iter[3] = v35;
                iter[4] = v35;
                iter += 5;
                v29 = *((int *)&idx->padding_14c[820]);
                break;
            }
            if (!(v36 & 0x80000000))
                v36 = &v33->padding_0[v36 + 2];
            v38 = (*((int *)&idx->padding_5d1[2504]))(*((int *)((char *)&idx->field_59d + 1)), v36 & 0x7fffffff, v36);
            v39 = *((int *)((char *)&idx->field_59d + 1));
            if (!v38)
            {
                if (!(v39 & 0x80000000))
                {
                    v7 = v37;
                    v6 = (char *)iter[3] + *((int *)&idx->padding_14c[820]);
                    v5 = v39;
                    v4 = &idx->padding_494[63];
                    v3 = v37;
                }
                else
                {
                    v40 = v39 & 0x7fffffff;
                    if (*((int *)((char *)&idx->field_59d + 1)) == *((int *)&idx->padding_14c[824]))
                    {
                        v7 = v37;
                        v41 = *((int *)((char *)&idx->field_59d + 1));
                        v38 = *((int *)(&v41->padding_0[*((int *)&v41->padding_0[28 + *((int *)&v41[1].padding_0[56 + v41->field_3c])]) + 4 * v40] - 4)) + *((int *)((char *)&idx->field_59d + 1));
                        goto LABEL_4dc1d6;
                    }
                    else
                    {
                        v7 = v37;
                        v6 = (char *)iter[3] + *((int *)&idx->padding_14c[820]);
                        v5 = v40;
                        v4 = &idx->padding_494[144];
                        v3 = v37;
                    }
                }
                *((unsigned int *)((char *)&idx->field_5a5 + 1)) = (*((int *)&idx->padding_5d1[2504]))(*((int *)&idx->padding_14c[824]), &idx->padding_494[5]);
                v42 = (*((int *)&idx->padding_5d1[2512]))(&idx->padding_494[0x11]);
                idx->field_488 = v42;
                *((unsigned int *)((char *)&idx->field_5a9 + 1)) = (*((int *)&idx->padding_5d1[2504]))(v42, &idx->padding_494[28]);
                v43 = (*((int *)&idx->padding_5d1[2504]))(idx->field_488, &idx->padding_494[40]);
                v43(idx->field_488, &idx->padding_494[40]);
                (*((int *)((char *)&idx->field_5a9 + 1)))(0, &idx->padding_494[0x11], &idx->padding_494[50], 48);
                v44 = (*((int *)((char *)&idx->field_5a5 + 1)))(0xffffffff);
                *(v44) = *(v44) + *((char *)&v44);
                *(v44) = *(v44) + *((char *)&v44);
                *(v44) = *(v44) + *((char *)&v44);
                *(v44) = *(v44) + *((char *)&v44);
                *(v44) = *(v44) + *((char *)&v44);
                *(v44) = *(v44) + *((char *)&v44);
            }
            else
            {
LABEL_4dc1d6:
                *(v37) = v38;
                *((int *)&idx->padding_5a1[1]) = *((int *)&idx->padding_5a1[1]) + 4;
            }
        }
    }
    v45 = *((int *)&idx->padding_14c[820]);
    v46 = &v45->padding_0[v45->field_3c];
    v9 = a0;
    v8 = a0;
    (*((int *)&idx->padding_62[2]))(v45, v46->field_54, 4, &v8, a0, &v9, a0);
    v6 = v46->field_54;
    v5 = v45;
    v47 = v46->field_6;
    iter1 = &v46->padding_0[v46->field_14] - 16;
    v49 = &idx->padding_5b9[1];
    do
    {
        v50 = *((int *)&v49->padding_0[0]);
        v51 = &v49->padding_0[4];
        if (!v50)
            break;
        do
        {
            v52 = v47;
            iter1 += 40;
            v53 = v52 - 1;
            v47 = v53;
        } while (v52 != 1 & v50 != (int)iter1[12]);
        if (v50 != (int)iter1[12])
            break;
        v2 = v53 + 1;
        v1 = v51;
        v0 = 1;
        if (v51->padding_0[7] & 224)
            v0 = 2;
        if (v51->padding_0[7] & 128)
            v0 *= 2;
        if (v51->padding_0[7] & 32)
            v0 *= 16;
        (*((int *)&idx->padding_62[2]))(v50 + *((int *)&idx->padding_14c[820]), (int)iter1[8]);
        i = v50 + *((int *)&idx->padding_14c[820]);
        v55 = &v51->padding_0[4];
        v49 = &v55->padding_0[4];
        *((int *)&iter1[36]) = *((int *)&v55->padding_0[0]);
        v47 = i - 1;
    } while (i != 1);
    (*((int *)&idx->padding_62[2]))();
}
