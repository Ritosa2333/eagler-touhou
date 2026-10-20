// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4364f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4364f0_1 {
    unsigned int field_0;
} st_4364f0_1;

typedef struct st_4364f0_0 {
    char padding_0[2428];
    unsigned int field_97c;
    unsigned int field_980;
    char padding_984[12];
    unsigned int field_990;
    unsigned int field_994;
    unsigned int field_998;
    unsigned int field_99c;
    unsigned int field_9a0;
    unsigned int field_9a4;
    unsigned int field_9a8;
    unsigned int field_9ac;
    unsigned int field_9b0;
    unsigned int field_9b4;
    unsigned int field_9b8;
    unsigned int field_9bc;
    char padding_9c0[84];
    int field_a14;
    unsigned int field_a18;
    unsigned int field_a1c;
    char padding_a20[40];
    int field_a48;
    char padding_a4c[30824];
    int field_82b4;
    int field_82b8;
    int field_82bc;
    int field_82c0;
    char padding_82c4[8];
    unsigned int field_82cc;
    unsigned int field_82d0;
    char padding_82d4[104];
    struct st_4364f0_1 *field_833c;
    char padding_8340[16596];
    char field_c414;
    char padding_c415[3];
    unsigned int field_c418;
    char padding_c41c[380];
    unsigned int field_c598;
} st_4364f0_0;

typedef struct st_4364f0_6 {
    struct st_4364f0_7 *field_0;
    struct st_4364f0_6 *field_4;
} st_4364f0_6;

typedef struct st_4364f0_4 {
    char padding_0[1072];
    struct st_4364f0_0 *field_430;
    unsigned int field_434;
    unsigned int field_438;
} st_4364f0_4;

typedef struct st_4364f0_7 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
} st_4364f0_7;

typedef struct st_4364f0_5 {
    char padding_0[112];
    unsigned int field_70;
} st_4364f0_5;

extern unsigned int g_4a1148[4];
extern unsigned int g_4a114c[4];
extern unsigned int g_4b43c8;
extern st_4364f0_5 *g_4b43dc;
extern unsigned int g_4ce8cc;
extern unsigned int g_4d49d0;

unsigned int sub_4364f0(void)
{
    st_4364f0_0 *idx;  // edi
    unsigned int idx1;  // eax
    unsigned int *v14;  // esi
    unsigned int *v15;  // ebp
    unsigned int v16;  // eax
    int v17;  // 4098
    unsigned int v18;  // eax
    unsigned int v19;  // ecx
    unsigned int v20;  // esi
    unsigned int v21;  // eax
    int v22;  // ecx
    int v23;  // eax
    st_4364f0_4 *idx2;  // eax
    st_4364f0_0 *iter;  // esi
    st_4364f0_0 *v26;  // ebx
    int v27;  // edx
    int v28;  // edx
    unsigned int v29;  // edx
    unsigned int v30;  // ecx
    unsigned int v31;  // eax
    unsigned int i;  // ecx
    st_4364f0_6 *node;  // eax
    unsigned int *v34;  // eax
    st_4364f0_6 *iter1;  // eax
    unsigned int j;  // ecx
    st_4364f0_6 *iter2;  // eax
    unsigned int *v38;  // eax
    st_4364f0_6 *v39;  // eax
    st_4364f0_0 *v0;  // [bp-0x4c]
    int v1;  // [bp-0x48], Other Possible Types: unsigned int
    st_4364f0_0 *v2;  // [bp-0x44]
    unsigned int v3;  // [bp-0x40]
    unsigned int v4;  // [bp-0x40]
    unsigned int v5;  // [bp-0x3c]
    unsigned int v6;  // [bp-0x38]
    unsigned int *v7;  // [bp-0x2c], Other Possible Types: int
    unsigned int *v8;  // [bp-0x24]
    unsigned int *v9;  // [bp-0x20], Other Possible Types: unsigned int
    char v10;  // [bp-0x10]

    if ((char)(g_4d49d0 & 80) == 80)
    {
        idx->field_a1c = 5;
    }
    else if ((char)(g_4d49d0 & 96) == 96)
    {
        idx->field_a1c = 7;
    }
    else if ((char)(g_4d49d0 & 144) == 144)
    {
        idx->field_a1c = 6;
    }
    else if ((char)(g_4d49d0 & 160) == 160)
    {
        idx->field_a1c = 8;
    }
    else if ((char)g_4d49d0 & 32)
    {
        idx->field_a1c = 2;
    }
    else if ((char)g_4d49d0 & 16)
    {
        idx->field_a1c = 1;
    }
    else if ((char)g_4d49d0 & 64)
    {
        idx->field_a1c = 3;
    }
    else if ((char)g_4d49d0 < 0)
    {
        idx->field_a1c = 4;
    }
    else
    {
        idx->field_a1c = 0;
    }
    if (g_4b43dc && g_4b43dc->field_70 && idx->field_a48 >= 4)
    {
        idx->field_c598 = g_4d49d0 >> 3 & 1;
    }
    else
    {
        idx->field_c598 = 0;
        *((unsigned int *)&idx->padding_8340[16572]) = 30;
    }
    idx1 = idx->field_a1c * 4;
    v14 = *((int *)(2 * idx1 + (char *)&g_4a1148[0]));
    v15 = *((int *)(2 * idx1 + (char *)&g_4a114c[0]));
    if (idx->field_c598)
    {
        if (!*((int *)&idx->padding_a4c[30732]))
        {
            sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v10, 77, 0);
            *((unsigned int *)&idx->padding_a4c[30732]) = v9;
        }
        v7 = v14 * (idx->field_a1c < 5 ? idx->field_994 : idx->field_99c);
        v16 = (idx->field_a1c < 5 ? idx->field_994 : idx->field_99c);
    }
    else
    {
        if (!sub_461920(*((int *)&idx->padding_a4c[30732])))
        {
            *((unsigned int *)&idx->padding_a4c[30732]) = 0;
        }
        else
        {
            sub_461970(*((int *)&idx->padding_a4c[30732]));
            *((unsigned int *)&idx->padding_a4c[30732]) = 0;
        }
        v8 = v14 * (idx->field_a1c < 5 ? idx->field_990 : idx->field_998);
        v16 = (idx->field_a1c < 5 ? idx->field_990 : idx->field_998);
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = v15 * v16;
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = sub_4931e0();
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = sub_4931e0();
    if (v7 < 0)
    {
        if (idx->field_a14 >= 0)
        {
            v4 = 1;
            v2 = &idx->padding_0[20];
            sub_454d10(&idx->padding_0[20], 1);
        }
        else if (!v7)
        {
            goto LABEL_436737;
        }
LABEL_43671e:
        if (v7 > 0)
        {
            if (idx->field_a14 <= 0)
            {
                v1 = 3;
                v0 = &idx->padding_0[20];
                goto LABEL_436763;
            }
            if (!v7)
                goto LABEL_436754;
        }
        else if (!v7)
        {
            goto LABEL_436754;
        }
    }
    else
    {
        if (v7)
            goto LABEL_43671e;
LABEL_436737:
        if (idx->field_a14 < 0)
        {
            v4 = 2;
            v2 = &idx->padding_0[20];
            sub_454d10(&idx->padding_0[20], 2);
LABEL_436754:
            if (idx->field_a14 > 0)
                goto LABEL_43675d;
        }
        else if (idx->field_a14 > 0)
        {
LABEL_43675d:
            v1 = 4;
            v0 = &idx->padding_0[20];
LABEL_436763:
            sub_454d10(v0, v1);
        }
    }
    v17 = idx->field_a1c;
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_a14 = v7;
    idx->field_a18 = v9;
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_9a0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_9a4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (v17)
    {
        v18 = idx->field_9a4;
        v19 = idx->field_9a8;
        idx->field_9ac = idx->field_9a0;
        idx->field_9b0 = v18;
        idx->field_9b4 = v19;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v20 = sub_4931e0();
    idx->field_9b8 = v20;
    v21 = sub_4931e0();
    *((unsigned int *)&idx->padding_984[4]) = *((int *)&idx->padding_984[4]) + v20;
    v22 = *((int *)&idx->padding_984[4]);
    *((unsigned int *)&idx->padding_984[8]) = *((int *)&idx->padding_984[8]) + v21;
    idx->field_9bc = v21;
    v23 = *((int *)&idx->padding_984[8]);
    if (v22 < -0x5c00)
    {
        *((unsigned int *)&idx->padding_984[4]) = 0xffffa400;
    }
    else if (v22 > 0x5c00)
    {
        *((unsigned int *)&idx->padding_984[4]) = 0x5c00;
    }
    if (v23 < 0x1000)
    {
        *((unsigned int *)&idx->padding_984[8]) = 0x1000;
    }
    else if (v23 > 0xd800)
    {
        *((unsigned int *)&idx->padding_984[8]) = 0xd800;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_97c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_980 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (!sub_461920(*((int *)&idx->padding_a4c[30732])))
    {
        *((unsigned int *)&idx->padding_a4c[30732]) = 0;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx2 = sub_461920(*((int *)&idx->padding_a4c[30732]));
        if (idx2)
        {
            idx2->field_430 = v2;
            idx2->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            idx2->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        }
    }
    if (idx->field_c414 & 8)
        idx->field_c418 = idx->field_c418 + 1;
    iter = &idx->field_82c0;
    v6 = 8;
    do
    {
        v3 = v4;
        if (!*((int *)(&iter->padding_0[0] - 96)))
            continue;
        if (!(idx->field_c414 & 8))
        {
            v26 = &iter->padding_0[12];
            if (!idx->field_c598)
                v26 = &iter->padding_0[4];
            v27 = *((int *)&idx->padding_984[8]) + *((int *)&v26->padding_0[4]);
            *((int *)(&iter->padding_0[0] - 12)) = *((int *)&idx->padding_984[4]) + *((int *)&v26->padding_0[0]);
            *((int *)(&iter->padding_0[0] - 8)) = v27;
            if (*((int *)&iter->padding_0[124]))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                (*((int *)&iter->padding_0[124]))();
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_43695c;
            }
        }
        else
        {
            *((int *)(&iter->padding_0[0] - 12)) = *((int *)&idx->padding_984[4]);
            *((int *)(&iter->padding_0[0] - 8)) = *((int *)&idx->padding_984[8]);
            if (idx->field_c418 < 30)
            {
LABEL_43695c:
                if (!*((int *)&iter->padding_0[116]))
                {
                    v28 = *((int *)&idx->padding_8340[16572]);
                    if (v28 >= 30)
                    {
                        v29 = 1374389535 * (v28 * (*((int *)(&iter->padding_0[0] - 12)) - *((int *)(&iter->padding_0[0] - 4)))) >> 37;
                        v1 = v28 * (*((int *)(&iter->padding_0[0] - 8)) - *((int *)&iter->padding_0[0]));
                        v30 = (v29 >> 31) + v29;
                        v31 = ((unsigned int)(1374389535 * (v28 * (*((int *)(&iter->padding_0[0] - 8)) - *((int *)&iter->padding_0[0]))) >> 37) >> 31) + (1374389535 * (v28 * (*((int *)(&iter->padding_0[0] - 8)) - *((int *)&iter->padding_0[0]))) >> 37);
                        if (v30 || v31)
                        {
                            *((unsigned int *)(&iter->padding_0[0] - 4)) = *((int *)(&iter->padding_0[0] - 4)) + v30;
                            *((unsigned int *)&iter->padding_0[0]) = *((int *)&iter->padding_0[0]) + v31;
                        }
                        else
                        {
                            *((int *)(&iter->padding_0[0] - 4)) = *((int *)(&iter->padding_0[0] - 12));
                            *((int *)&iter->padding_0[0]) = *((int *)(&iter->padding_0[0] - 8));
                        }
                    }
                }
                else
                {
                    *((unsigned int *)&iter->padding_0[116]) = 0;
                    *((int *)(&iter->padding_0[0] - 4)) = *((int *)(&iter->padding_0[0] - 12));
                    *((int *)&iter->padding_0[0]) = *((int *)(&iter->padding_0[0] - 8));
                }
                /* unsupported instruction */
                /* unsupported instruction */
                i = *((int *)&iter->padding_0[80]);
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if (i)
                {
                    node = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            if (*((int *)&node->field_0->padding_0[0]) == i)
                            {
                                v34 = node->field_0;
                                goto LABEL_436a7b;
                            }
                        } while ((node = (st_4364f0_6 *)node->field_4, node));
                    }
                    iter1 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (*((int *)&iter1->field_0->padding_0[0]) != i)
                        {
                            if (!iter1->field_4)
                                goto LABEL_436aa9;
                        }
                        v34 = iter1->field_0;
LABEL_436a7b:
                        if (v34)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v34[268] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v34[269] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v34[270] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                        }
                    }
                }
LABEL_436aa9:
                j = *((int *)&iter->padding_0[84]);
                if (j)
                {
                    iter2 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            if (*((int *)&iter2->field_0->padding_0[0]) == j)
                            {
                                v38 = iter2->field_0;
                                goto LABEL_436b0a;
                            }
                        } while ((iter2 = (st_4364f0_6 *)iter2->field_4, iter2));
                    }
                    v39 = *((int *)(g_4ce8cc + 8935104));
                    if (*((int *)(g_4ce8cc + 8935104)))
                    {
                        while (*((int *)&v39->field_0->padding_0[0]) != j)
                        {
                            if (!v39->field_4)
                                goto LABEL_436ae4;
                        }
                        v38 = v39->field_0;
LABEL_436b0a:
                        if (v38)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v38[268] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v38[269] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v38[270] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            continue;
                        }
                    }
                }
LABEL_436ae4:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                *((unsigned int *)(&iter->padding_0[0] - 96)) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_461970(*((int *)&iter->padding_0[80]));
                sub_461970(*((int *)&iter->padding_0[84]));
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&idx->padding_c41c[0]) = 0;
            }
        }
    } while ((iter += 228, v4 = v3 - 1, v3 != 1));
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return 0;
}
