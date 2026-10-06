// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f037; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47f037_0 {
    char field_0;
    char field_1;
    char field_2;
    char field_3;
} st_47f037_0;

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

extern st_47f037_0 *g_4b4318;

void* sub_47f037(void* a0, char *a1, char *a2, unsigned int a3)
{
    unsigned int v9;  // ebx
    unsigned int v10;  // edi
    st_47e7bf_0 *v20;  // eax
    unsigned int *v21;  // eax
    unsigned int v22;  // ecx
    unsigned int *v23;  // eax
    void* v24;  // eax
    void* idx;  // eax
    st_47f037_0 *v11;  // edi
    unsigned int v12;  // edx
    unsigned int v13;  // edx
    unsigned int v14;  // edx
    unsigned int v15;  // edx
    unsigned int v16;  // esi
    unsigned long long v17;  // esi
    unsigned int v18;  // edx
    int v0;  // [bp-0x38], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x34]
    unsigned int v2;  // [bp-0x30]
    unsigned int v3[2];  // [bp-0x2c]
    unsigned int v4[2];  // [bp-0x24]
    st_47e7bf_0 v5;  // [bp-0x1c]
    char v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]

    v8 &= 0xffff0000;
    v2 = v9;
    v1 = v10;
    v11 = &g_4b4318->field_1;
    g_4b4318 = v11;
    v12 = v11->field_0;
    v13 = v12 - 65;
    v7 = 0;
    if (v12 != 65)
    {
        v14 = v13 - 1;
        if (v13 != 1)
        {
            v15 = v14 - 1;
            if (v14 != 1)
            {
                if (v11->field_0 && v11->field_1)
                {
                    if (!a3)
                    {
                        v0 = v16;
                        v17 = (v12 - 48) * 16 + v11->field_1 - 48;
                        g_4b4318 = &v11->field_2;
                        if (v17 > 1)
                        {
                            sub_47e869(44);
                            v20 = sub_47e9ae(&v7, v18, &v5, sub_47e692(&v6, v18, v17, 0, 44));
                            v7 = v20->field_0;
                            v8 = *((int *)&v20->field_4);
                        }
                        v21 = sub_47ec6e(&v7, v15, v4, 62);
                        v7 = *(v21);
                        v22 = v21[1];
                        v8 = v22;
                        if (g_4b4318->field_0 == 36)
                        {
                            g_4b4318 = &g_4b4318->field_1;
                        }
                        else
                        {
                            v23 = sub_47ec6e(&v7, v18, v3, 94);
                            v7 = *(v23);
                            v22 = v23[1];
                            v8 = v22;
                        }
                        if (g_4b4318->field_0)
                        {
                            g_4b4318 = &g_4b4318->field_1;
                        }
                        else
                        {
                            sub_47e4af(1);
                            v22 = v8;
                        }
                        v24 = a0;
                        *((unsigned int *)v24) = v7;
                        *((unsigned int *)&v24[4]) = v22 | 0x4000;
                        return v24;
                    }
LABEL_47f162:
                    v0 = 2;
                }
                else
                {
                    v0 = 1;
                }
                sub_47e1ce(v0);
                return a0;
            }
            *(a1) = 37;
        }
        else
        {
            if (a3)
                goto LABEL_47f162;
            *(a2) = 1;
            sub_47e869(62);
        }
    }
    else if (!a3)
    {
        *(a1) = ((char)((*(a1) != 38) + 1) & 199) + 94;
    }
    idx = a0;
    g_4b4318 = &g_4b4318->field_1;
    *((char *)&idx[4]) = 0;
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    *((unsigned int *)idx) = 0;
    return idx;
}
