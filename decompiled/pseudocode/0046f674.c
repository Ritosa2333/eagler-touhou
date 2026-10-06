// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46f674; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46f674_3 {
    unsigned int field_0;
    unsigned int field_4;
    int field_8;
    struct st_46f674_0 *field_c;
    struct st_46f674_2 *field_10;
    char field_14;
} st_46f674_3;

typedef struct st_46f674_1 {
    int field_0;
    struct st_46f674_0 *field_4;
} st_46f674_1;

typedef struct st_46f674_0 {
    unsigned int field_0;
    char padding_4[4];
    unsigned int field_8;
    int field_c;
    char padding_10[4076];
    unsigned int field_ffc;
    char padding_1000[4092];
    char field_1ffc;
} st_46f674_0;

typedef struct st_46f674_2 {
    char padding_0[68];
    unsigned int field_44;
    char padding_48[124];
    unsigned int field_c4;
    char field_c8;
    char padding_c9[123];
    unsigned int field_144;
    struct st_46f674_1 *field_148;
    char field_14c;
    char padding_14d[507];
    char field_348;
} st_46f674_2;

extern char g_4d6440;
extern st_46f674_3 *g_4d6444;

unsigned int sub_46f674(void)
{
    int l;  // ebx
    unsigned int v20;  // esi
    unsigned int v29;  // ecx
    int index;  // eax
    unsigned int v31;  // eax
    st_46f674_0 *v32;  // edi
    int m;  // esi
    st_46f674_0 *v34;  // ebx
    st_46f674_1 *iter;  // eax
    st_46f674_0 *iter1;  // ecx
    st_46f674_0 *v37;  // edx
    int v38;  // ecx
    st_46f674_3 *node;  // edx
    unsigned int v39;  // eax
    unsigned int v40;  // eax
    unsigned int v22;  // edi
    st_46f674_2 *v23;  // eax
    unsigned int v24;  // esi
    unsigned int *iter2;  // edi
    unsigned int v26;  // edx
    unsigned int n;  // esi
    int v28;  // ecx
    int v0;  // [bp-0x14c], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x148]
    unsigned int v2;  // [bp-0x144]
    unsigned int v3;  // [bp-0x13c]
    unsigned int v4;  // [bp-0x3c]
    st_46f674_3 *v5;  // [bp-0x38]
    st_46f674_0 *v6;  // [bp-0x34]
    st_46f674_0 *v7;  // [bp-0x30]
    unsigned int v8;  // [bp-0x2c]
    st_46f674_0 *v9;  // [bp-0x28]
    int v10;  // [bp-0x24]
    int i;  // [bp-0x20]
    unsigned int v12;  // [bp-0x1c]
    unsigned int v13;  // [bp-0x18]
    unsigned int v14;  // [bp-0x14]
    int j;  // [bp-0x10]
    st_46f674_0 *v16;  // [bp-0xc]
    unsigned int v17;  // [bp-0x8]

    l = 0;
    if (!g_4d6444)
        return 0xffffffff;
    v2 = v20;
    node = g_4d6444;
    v1 = v22;
    v5 = g_4d6444;
    i = 0;
    if (*((int *)&g_4d6440) <= NULL)
        return 0;
    do
    {
        v23 = node->field_10;
        if (!v23)
        {
            v0 = 0xfffffffe;
            return v0;
        }
        v16 = node->field_c;
        v9 = &v23->field_144;
        v24 = &v23->field_c4;
        v10 = node->field_8;
        v14 = 0;
        v13 = 0;
        j = 0;
        v4 = v24;
        do
        {
            v0 = 64;
            iter2 = &v3;
            v8 = 0;
            v12 = 0;
            for (v17 = 0; v0; iter2 += 1)
            {
                v0 -= 1;
                *(iter2) = 0;
            }
            if (v10 >= NULL)
            {
                if (v16)
                {
                    v26 = &v16->field_ffc;
                    do
                    {
                        n = v26 - 0xff0;
                        if (*((int *)(v26 - 0xff4)) != 0xffffffff || *((int *)v26) != 0xffffffff)
                        {
                            v0 = 0xfffffffb;
                            return v0;
                        }
                        do
                        {
                            v28 = *((int *)n);
                            if ((char)v28 & 1)
                            {
                                v29 = v28 - 1;
                                if (v29 > 0x400)
                                {
                                    v0 = 0xfffffffa;
                                    return v0;
                                }
                                v17 += 1;
                            }
                            else
                            {
                                index = (v28 >> 4) - 1;
                                if (index > 63)
                                {
                                    v0 = 63;
                                    index = v0;
                                }
                                (&v3)[index] = (&v3)[index] + 1;
                                v29 = v28;
                            }
                            if (v29 < 16 || (char)v29 & 15 || v29 > 0xff0)
                            {
                                v0 = 0xfffffff9;
                                return v0;
                            }
                            v31 = v29 + n;
                            if (*((int *)(v31 - 4)) != v28)
                            {
                                v0 = 0xfffffff8;
                                return v0;
                            }
                            n = v31;
                        } while (n < v26);
                        if (n != v26)
                        {
                            v0 = 0xfffffff8;
                            return v0;
                        }
                        v26 += 0x1000;
                        l += 1;
                    } while (l < 8);
                    v32 = v9;
                    if (v32->field_0 != v17)
                    {
                        v0 = 0xfffffff7;
                        return v0;
                    }
                    m = 0;
                    do
                    {
                        v17 = 0;
                        v34 = &v32->field_8;
                        iter = *((int *)((char *)v34 - 4));
                        v7 = v32;
                        v6 = v34;
                        if (*((int *)((char *)v34 - 4)) != v32)
                        {
                            do
                            {
                                if (v17 == (&v3)[m])
                                    break;
                                if (iter < v16 || iter >= &v16[4].field_c)
                                {
                                    v0 = 0xfffffff6;
                                    return v0;
                                }
                                iter1 = (iter & 0xfffff000) + 12;
                                v37 = &iter1->padding_10[4064];
                                if (iter1 == v37)
                                {
                                    v0 = 0xfffffff5;
                                    return v0;
                                }
                                do
                                {
                                } while (iter1 != iter && (iter1 += iter1->field_0 & 0xfffffffe, v34 = v6, iter1 != v37));
                                if (iter1 == v37)
                                {
                                    v0 = 0xfffffff5;
                                    return v0;
                                }
                                v38 = (iter->field_0 >> 4) - 1;
                                if (v38 > 63)
                                {
                                    v0 = 63;
                                    v38 = v0;
                                }
                                if (v38 != m)
                                {
                                    v0 = 0xfffffff4;
                                    return v0;
                                }
                                if (iter[1].field_0 != v7)
                                {
                                    v0 = 0xfffffff3;
                                    return v0;
                                }
                                v17 += 1;
                                v7 = iter;
                                iter = iter->field_4;
                            } while (iter != v32);
                            if (v17)
                            {
                                if (m < 32)
                                {
                                    v39 = 0x80000000 >> ((char)m & 31);
                                    v8 |= v39;
                                    v14 |= v39;
                                }
                                else
                                {
                                    v40 = 0x80000000 >> ((char)(m - 32) & 31);
                                    v12 |= v40;
                                    v13 |= v40;
                                }
                            }
                        }
                        if (v7->padding_4 != v32 || v17 != (&v3)[m])
                        {
                            v0 = 0xfffffff2;
                            return v0;
                        }
                        if (v34->field_0 != v7)
                        {
                            v0 = 0xfffffff1;
                            return v0;
                        }
                        m += 1;
                        v32 = v34;
                    } while (m < 64);
                    v24 = v4;
                    node = v5;
                    l = 0;
                }
                else
                {
                    v0 = 0xfffffffc;
                    return v0;
                }
            }
            if (v8 != *((int *)(v24 - 128)) || v12 != *((int *)v24))
            {
                v0 = 0xfffffff0;
                return v0;
            }
            v16 = &v16[4].field_c;
            v9 = &v9->padding_10[500];
            v10 *= 2;
            j += 1;
            v24 += 4;
            v4 = v24;
        } while (j < 32);
        if (v14 != node->field_0 || v13 != node->field_4)
        {
            v0 = 0xffffffef;
            return v0;
        }
        node = &node->field_14;
        i += 1;
        v5 = node;
    } while (i < *((int *)&g_4d6440));
    return 0;
}
