// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x471154; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_471154_0 {
    char padding_0[12];
    char field_c;
} st_471154_0;

typedef struct st_471154_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[20];
    char field_24;
} st_471154_1;

typedef struct st_471154_2 {
    char padding_0[112];
    unsigned int field_70;
} st_471154_2;

extern char g_4723f4;
extern char g_49d620;
extern char g_49d640;
extern st_471154_1 g_4adbb0;
extern st_471154_1 g_4d6320;

int sub_471154(st_471154_0 *a0, char *a1, int a2, unsigned int a3)
{
    int v24;  // edi
    int v25;  // esi
    unsigned int *iter;  // eax
    unsigned int v35;  // ecx
    unsigned int v36;  // ecx
    unsigned int v37;  // ecx
    unsigned int v38;  // ecx
    unsigned int v39;  // ecx
    unsigned int v41;  // eax
    unsigned int v42;  // eax
    int v26;  // ebx
    int v27;  // eax
    st_471154_1 *v28;  // ecx
    st_471154_1 *v29;  // eax
    char *v30;  // eax
    char v31;  // 4131
    int v32;  // esi
    unsigned int node;  // edx
    char v0;  // [bp-0x8d4]
    unsigned int v2;  // [bp-0x280]
    unsigned int v3;  // [bp-0x27c]
    unsigned int v4;  // [bp-0x278]
    st_471154_0 *v5;  // [bp-0x274]
    unsigned int v6;  // [bp-0x26c]
    unsigned int v7;  // [bp-0x268]
    unsigned int v8;  // [bp-0x264]
    char *v9;  // [bp-0x260]
    unsigned int v10;  // [bp-0x25c]
    int v11;  // [bp-0x258]
    unsigned int v12;  // [bp-0x254]
    char *v13;  // [bp-0x248]
    int v14;  // [bp-0x244]
    st_471154_2 *idx;  // [bp-0x238]
    char v16;  // [bp-0x234]
    char v17;  // [bp-0x230]
    int v18;  // [bp-0x22c]
    unsigned int v19;  // [bp-0x228]
    unsigned int v20;  // [bp-0x224]
    int v21;  // [bp-0x21c], Other Possible Types: unsigned int
    unsigned int v22;  // [bp-0x218]
    unsigned int v23;  // [bp-0x214]

    v5 = a0;
    v22 = a3;
    v4 = 0;
    v23 = 0;
    v10 = 0;
    v3 = 0;
    v8 = 0;
    sub_46d3b4(a2, v24, v25, v26);
    v19 = 0xffffffff;
    v9 = NULL;
    if (a0)
    {
        if (!(a0->field_c & 64))
        {
            v27 = sub_47c1da(a0);
            if (v27 != 0xffffffff && v27 != -0x2)
                v28 = (v27 & 31) * 64 + (&g_4d6320.field_0)[v27 >> 5];
            else
                v28 = &g_4adbb0.field_0;
            if (!(v28->field_24 & 127))
            {
                if (v27 != 0xffffffff && v27 != -0x2)
                    v29 = (v27 & 31) * 64 + (&g_4d6320.field_0)[v27 >> 5];
                else
                    v29 = &g_4adbb0.field_0;
                if (!(v29->field_24 & 128))
                    goto LABEL_471257;
            }
        }
        else
        {
LABEL_471257:
            if (a1)
            {
                v14 = 0;
                v18 = 0;
                while (1)
                {
                    if (v18 == 1 && !v19)
                        goto LABEL_4723cb;
                    v30 = a1;
                    v11 = -0x1;
                    v21 = 0xffffffff;
                    v19 = 0xffffffff;
                    v6 = 0;
                    v31 = *(v30);
                    v7 = 0;
                    v2 = 0;
                    v12 = 0;
                    v20 = 0;
                    v17 = v31;
                    if (v31)
                    {
                        while (1)
                        {
                            v13 = v30 + 1;
                            if (v14 < NULL)
                                break;
                            v7 = (&g_49d640)[9 * (v31 - 32 <= 88 ? *(&(&g_49d620)[v31]) & 15 : 0) + v7] >> 4;
                            if (v7 == 1)
                            {
                                if (*(v13) == 37)
                                    goto LABEL_4713fa;
                                if (v19 == 0xffffffff)
                                {
                                    if (sub_4733c6(v13, &v9, 10) > NULL && *(v9) == 36)
                                    {
                                        if (!v18)
                                            sub_477420(&v0, 0, 1600);
                                        v19 = 1;
                                        goto LABEL_47139a;
                                    }
                                    else
                                    {
                                        v19 = 0;
                                    }
                                }
                                if (v19 != 1)
                                    goto LABEL_4713fa;
LABEL_47139a:
                                v21 = sub_4733c6(v13, &v9, 10) - 1;
                                v13 = v9 + 1;
                                if (!v18)
                                {
                                    if (v21 < 0 || *(v9) != 36 || v21 >= 100)
                                        goto LABEL_4711c0;
                                    if (v21 > v11)
                                        v11 = v21;
                                }
                                goto LABEL_4713fa;
                            }
                            else
                            {
                                if (v7 == 8)
                                    goto LABEL_4711c0;
                                if (v7 <= 7)
                                {
LABEL_4713fa:
                                    goto *((void *)(*((int *)&(&g_4723f4)[4 * v7])));
                                }
                                else
                                {
                                    v30 = v13;
                                    v17 = *(v30);
                                    if (*(v30))
                                        v31 = v17;
                                    else
                                        break;
                                }
                            }
                        }
                        if (v7 && v7 != 7)
                            break;
                        if (v19 == 1 && !v18)
                        {
                            v32 = 0;
                            if (v11 >= NULL)
                            {
                                node = v22;
                                iter = &v0 + 4;
                                do
                                {
                                    v35 = *((int *)((char *)iter - 4)) - 1;
                                    if (*((int *)((char *)iter - 4)) != 1 && !(v36 = v35 - 1, v35 == 1 || (v37 = v36 - 1, v36 == 1)))
                                    {
                                        v38 = v37 - 1;
                                        if (v37 != 1 && !(v39 = v38 - 1, v38 == 1))
                                        {
                                            if (v39 == 1)
                                                goto LABEL_47239e;
                                            if (v39 == 3)
                                                goto LABEL_472397;
                                            else
                                                goto LABEL_4711c0;
                                        }
LABEL_472397:
                                        *(iter) = node;
                                        node += 8;
                                    }
                                    else
                                    {
LABEL_47239e:
                                        *(iter) = node;
                                        node += 4;
                                    }
                                } while ((v32 = (int)(v32 + 1), iter += 16, v22 = node, v32 <= v11));
                            }
                        }
                    }
                    v18 += 1;
                    if (v18 >= 2)
                    {
LABEL_4723cb:
                        if (!v16)
                            return v42;
                        idx->field_70 = idx->field_70 & 0xfffffffd;
                        return v42;
                    }
                }
            }
        }
    }
LABEL_4711c0:
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    if (v16)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v41;
}
