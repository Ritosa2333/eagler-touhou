// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x481720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_481720_0 {
    char field_0;
    char field_1;
    char field_2;
} st_481720_0;

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

typedef struct st_481720_2 {
    char padding_0[4];
    unsigned int field_4;
} st_481720_2;

typedef struct st_481720_1 {
    st_47e7bf_0 field_0;
    unsigned int field_4;
} st_481720_1;

extern st_481720_0 *g_4b4318;
extern unsigned int g_4b4328;

int sub_481720(void)
{
    char v16;  // cl
    unsigned int v17;  // ebx
    unsigned int v26;  // edx
    st_47e7bf_0 *v27;  // eax
    st_481720_1 *v28;  // eax
    st_47e7bf_0 *v31;  // eax
    st_47e7bf_0 *v34;  // eax
    unsigned int v18;  // ebx
    st_481720_1 *v37;  // eax
    void* idx;  // eax
    void* v39;  // esi
    void* v41;  // eax
    st_481720_0 *v19;  // eax
    unsigned int v20;  // esi
    st_481720_2 *v21;  // esi
    unsigned int v22;  // edi
    unsigned int v23;  // edi
    st_481720_1 *v24;  // eax
    int v0;  // [bp-0x5c]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x40]
    unsigned int v3;  // [bp-0x3c]
    unsigned int v4;  // [bp-0x38]
    st_47e7bf_0 v5;  // [bp-0x34]
    char v6;  // [bp-0x2c]
    st_47e7bf_0 v7;  // [bp-0x24]
    char v8;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x18]
    st_47e7bf_0 v10;  // [bp-0x14]
    unsigned int v11;  // [bp-0x10]
    st_47e7bf_0 v12;  // [bp-0xc]
    unsigned int v13;  // [bp-0x8]
    unsigned int *v14;  // [bp+0x4]
    st_481720_2 *v15;  // [bp+0x8]

    v16 = g_4b4318->field_0;
    if (!g_4b4318->field_0)
    {
        sub_47ec26(v14, 1, v15);
        return;
    }
    if ((v16 < 54 || v16 > 57) && v16 != 95)
    {
        sub_47e1ce(2);
        return;
    }
    v4 = v17;
    v18 = v16 - 54;
    v19 = &g_4b4318->field_1;
    g_4b4318 = v19;
    if (v18 == 41)
    {
        if (v19->field_0)
        {
            v18 = v19->field_0 - 61;
            g_4b4318 = &v19->field_1;
            if (v18 < 4 || v18 > 7)
                goto LABEL_4817ac;
LABEL_4817af:
            if (v18 == 0xffffffff)
            {
                sub_47e1ce(2);
            }
            else
            {
                v10 = (st_47e7bf_0)0;
                v11 &= 0xffff0000;
                v3 = v20;
                v21 = v15;
                v2 = v22;
                v12 = (st_47e7bf_0)v21->padding_0;
                v23 = v18;
                v13 = v21->field_4;
                if (v23 & 2)
                {
                    v24 = sub_47ec4a(&v7, "::", &v12);
                    v13 = *((int *)&v24->field_0.field_4);
                    v12 = (st_47e7bf_0)v24->field_0.field_0;
                    if (g_4b4318->field_0)
                        v27 = sub_47e9ae(sub_47ec02(&v5, 32, sub_4814b0(&v6, &v7)), v26, &v7, &v12);
                    else
                        v27 = sub_47ec26(&v5, 1);
                    v13 = *((int *)&v27->field_4);
                    v12 = (st_47e7bf_0)v27->field_0;
                    if (!g_4b4318->field_0)
                    {
                        sub_47ec26(v14, 1, &v12);
                        goto LABEL_481a6c;
                    }
                    if (g_4b4318->field_0 == 64)
                    {
                        g_4b4318 = &g_4b4318->field_1;
                        if (((char)g_4b4328 & 96) != 96)
                        {
                            v28 = sub_47e0f7(&v5);
                            v10 = (st_47e7bf_0)v28->field_0.field_0;
                            v11 = *((int *)&v28->field_0.field_4);
                        }
                        else
                        {
                            sub_47ddc1(sub_47e0f7(&v5));
                        }
                    }
                    else
                    {
                        v1 = 2;
                        goto LABEL_481a64;
                    }
                }
                if ((char)v18 & 4)
                {
                    if ((char)~(g_4b4328 >> 1) & 1)
                    {
                        v31 = sub_47e9ae(sub_47ec02(&v7, 32, sub_480665(&v6, &v5, &v12)), v26, &v5, &v12);
                        v12 = (st_47e7bf_0)v31->field_0;
                        v13 = *((int *)&v31->field_4);
                    }
                    else
                    {
                        sub_47ddc1(sub_480665(&v5));
                    }
                }
                if ((char)~(g_4b4328 >> 1) & 1)
                {
                    v34 = sub_47e9ae(sub_47e8cb(&v6, &v5, &v12), v26, &v5, &v12);
                    v12 = (st_47e7bf_0)v34->field_0;
                    v13 = *((int *)&v34->field_4);
                }
                else
                {
                    sub_47ddc1(sub_47e8cb(&v5));
                }
                if (v21->padding_0)
                {
                    v37 = sub_47ec6e(sub_47ec02(&v6, 40, &v12, &v5, 41), v26, &v5, 41);
                    v12 = (st_47e7bf_0)v37->field_0.field_0;
                    v13 = *((int *)&v37->field_0.field_4);
                }
                idx = sub_47dc29(8, 0);
                if (idx)
                {
                    *((char *)&idx[4]) = 0;
                    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
                    *((struct st_47e7bf_0 *)idx) = 0;
                    v39 = idx;
                }
                else
                {
                    v39 = NULL;
                }
                sub_47e46b(&v8, v39);
                v41 = sub_47ec6e(sub_47ec02(&v7, 40, sub_47eedc(&v6, &v5, 41)), v26, &v5, 41);
                sub_47e7bf(&v12, v26, v41);
                if ((char)(g_4b4328 & 96) != 96 && v23 & 2)
                    sub_47e7bf(&v12, v26, &v10);
                if ((char)~(g_4b4328 >> 8) & 1)
                    sub_47e7bf(&v12, v26, sub_47efb8(&v5));
                else
                    sub_47ddc1(sub_47efb8(&v5));
                if (v39)
                {
                    *((struct st_47e7bf_0 *)v39) = v12;
                    *((unsigned int *)&v39[4]) = v13;
                    *(v14) = v8;
                    v14[1] = v9;
                }
                else
                {
                    v0 = 3;
LABEL_481a64:
                    sub_47e1ce(v0);
LABEL_481a6c:
                }
                return;
            }
        }
        else
        {
            sub_47ec26(v14, 1, v15);
        }
        return;
    }
    else if (v18 < 0 || v18 > 3)
    {
LABEL_4817ac:
        v18 = 0xffffffff;
        goto LABEL_4817af;
    }
}
