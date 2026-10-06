// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x481298; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_481298_0 {
    unsigned int field_0;
    unsigned int field_4;
} st_481298_0;

extern unsigned int g_4b4318;
extern unsigned int g_4b4328;
extern char g_4b4330;

st_481298_0 * sub_481298(st_481298_0 *a0)
{
    unsigned int v14;  // ebx
    unsigned int v15;  // esi
    st_481298_0 *v24;  // eax
    char *v25;  // eax
    unsigned int v26;  // edx
    st_481298_0 *v27;  // eax
    st_481298_0 *v28;  // eax
    unsigned int v16;  // edi
    char v17;  // cl
    unsigned int i;  // eax
    st_481298_0 *v19;  // eax
    unsigned int v20;  // esi
    unsigned int v21;  // ebx
    unsigned int v22;  // ebx
    st_481298_0 *v23;  // eax
    char *v0;  // [bp-0x44]
    char *v1;  // [bp-0x40], Other Possible Types: int
    char *v2;  // [bp-0x3c]
    char *v3;  // [bp-0x38], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x30]
    unsigned int v6;  // [bp-0x2c]
    char v7;  // [bp-0x28]
    char v8;  // [bp-0x20]
    char v9;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x14]
    char v11;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0xc]
    unsigned int v13;  // [bp-0x8]

    v6 = v14;
    v5 = v15;
    v4 = v16;
    if (g_4b4328 & 0x2000)
    {
        g_4b4328 = g_4b4328 & 0xffffdfff;
        sub_4827df(&v9, 0);
        g_4b4328 = g_4b4328 | 0x2000;
    }
    else
    {
        v17 = *((char *)g_4b4318);
        if (*((char *)g_4b4318) == 63)
        {
            g_4b4318 = g_4b4318 + 1;
            if (*((char *)g_4b4318) == v17 && *((char *)(g_4b4318 + 1)) == v17)
            {
                sub_481298(&v9);
                for (i = g_4b4318; *((char *)i); g_4b4318 = i + 1);
            }
            else
            {
                sub_48061b(&v11);
                v20 = v11;
                v21 = v12;
                if (v20 && v21 & 0x200)
                    v13 = 1;
                else
                    v13 = 0;
                if ((char)v21 <= 1)
                {
                    v22 = v21;
                    if (*((char *)g_4b4318))
                    {
                        v22 = v21;
                        if (*((char *)g_4b4318) != 64)
                        {
                            sub_4814b0(&v9);
                            v22 = v21;
                            if (v9)
                            {
                                if (g_4b4330)
                                {
                                    g_4b4330 = 0;
                                    v23 = sub_47e9ae(&v8, &v9);
                                    v20 = v23->field_0;
                                    v22 = v23->field_4;
                                    v11 = v20;
                                    v12 = v22;
                                    if (*((char *)g_4b4318) == 64)
                                        goto LABEL_48140a;
                                    v24 = sub_4814b0(&v8);
                                    v9 = v24->field_0;
                                    v10 = v24->field_4;
                                    v1 = &v11;
                                    v0 = &v8;
                                    v25 = &v7;
                                }
                                else
                                {
                                    v3 = &v11;
                                    v2 = &v7;
                                    v25 = &v8;
                                }
                                sub_47ec92(&v9, v26, v25, "::");
                                v27 = sub_47e9ae(v0, v1);
                                v22 = v27->field_4;
                                v20 = v27->field_0;
                                v12 = v22;
                                v11 = v20;
                            }
                        }
                    }
LABEL_48140a:
                    if (v13 && v20)
                    {
                        v22 |= 0x200;
                        v12 = v22;
                    }
                    if (v21 >> 15 & 1)
                    {
                        v22 |= 0x8000;
                        v12 = v22;
                    }
                    v21 = v22;
                    if (v20 && !(v21 & 0x1000))
                    {
                        if (*((char *)g_4b4318))
                        {
                            if (*((char *)g_4b4318) != 64)
                                goto LABEL_48144f;
                            g_4b4318 = g_4b4318 + 1;
                        }
                        if (!(g_4b4328 & 0x1000) || v13 || v21 & 0x8000)
                        {
                            sub_4806fd(a0, &v11);
                            return a0;
                        }
                        v9 = 0;
                        v10 &= 0xffff0000;
                        sub_4806fd(&v7, &v9);
                    }
                }
                v28 = a0;
                v28->field_0 = v20;
                v28->field_4 = v21;
                return v28;
            }
        }
        else
        {
            if (v17)
            {
LABEL_48144f:
                v1 = 2;
            }
            else
            {
                v3 = 1;
            }
            sub_47e1ce(v1);
            return a0;
        }
    }
    v19 = a0;
    v19->field_0 = v9;
    v19->field_4 = v10;
    return v19;
}
