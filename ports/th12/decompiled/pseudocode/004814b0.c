// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4814b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4814b0_0 {
    char padding_0[1];
    char field_1;
} st_4814b0_0;

typedef struct st_47e4f1_1 {
    struct st_47e4f1_0 *field_0;
    char field_4;
} st_47e4f1_1;

typedef struct st_47e4f1_0 {
    char padding_0[4];
    char field_4;
} st_47e4f1_0;

typedef struct st_4814b0_1 {
    unsigned int field_0;
} st_4814b0_1;

extern st_4814b0_1 *g_4b4310;
extern st_4814b0_0 *g_4b4318;
extern char g_4b4330;
extern char g_4b4331;

int sub_4814b0(void)
{
    unsigned int v27;  // ebx
    unsigned int v28;  // esi
    unsigned int v37;  // eax
    unsigned int v38;  // eax
    unsigned int v39;  // eax
    st_4814b0_0 *v41;  // eax
    char *v45;  // eax
    void* idx;  // esi
    unsigned int v30;  // edi
    unsigned int v31;  // edx
    unsigned int v32;  // edx
    st_4814b0_0 *v35;  // ecx
    unsigned int v36;  // eax
    char *v0;  // [bp-0xc0]
    void* v1;  // [bp-0xbc], Other Possible Types: int
    int v2;  // [bp-0xb8]
    unsigned int v3;  // [bp-0xa8]
    unsigned int v4;  // [bp-0xa4]
    unsigned int v5;  // [bp-0xa0]
    char v6;  // [bp-0x9c]
    char v7;  // [bp-0x94]
    char v8;  // [bp-0x8c]
    char v9;  // [bp-0x84]
    char v10;  // [bp-0x7c]
    char v11;  // [bp-0x74]
    char v12;  // [bp-0x6c]
    char v13;  // [bp-0x64]
    char v14;  // [bp-0x5c]
    char v15;  // [bp-0x54]
    char v16;  // [bp-0x4c]
    char v17;  // [bp-0x44]
    char v18;  // [bp-0x3c]
    char v19;  // [bp-0x34]
    char v20;  // [bp-0x2c]
    char v21;  // [bp-0x24]
    char v22;  // [bp-0x1c]
    char v23;  // [bp-0x14]
    st_47e4f1_1 v24;  // [bp-0xc]
    void* v25;  // [bp+0x4]
    char v26;  // [bp+0x7]

    v5 = v27;
    v4 = v28;
    idx = v25;
    v3 = v30;
    *((char *)&idx[4]) = 0;
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    *((unsigned int *)idx) = 0;
    v26 = 0;
    while (!(char)idx[4] && g_4b4318->padding_0 && g_4b4318->padding_0 != 64)
    {
        if (g_4b4330 && !g_4b4331)
            return;
        if (*((int *)idx))
        {
            sub_47dde0(idx, v31, sub_47ec4a(&v7, "::", idx));
            if (v26)
            {
                sub_47dde0(idx, v31, sub_47ec02(&v10, 91, idx));
                v26 = 0;
            }
        }
        if (g_4b4318->padding_0 == 63)
        {
            v35 = &g_4b4318->field_1;
            g_4b4318 = v35;
            v36 = (char)v35->padding_0;
            v37 = v36 - 36;
            if (v36 != 36)
            {
                v38 = v37 - 1;
                if (v37 != 1)
                {
                    if (v38 != 26)
                    {
                        v39 = v38 - 27;
                        if (v39 != 1)
                        {
                            v1 = idx;
                            if (v39 != 9)
                            {
                                v0 = &v8;
                                sub_47f32e(&v6);
                                goto LABEL_4816b5;
                            }
                            else
                            {
                                g_4b4318 = &v35->field_1;
                                sub_48023a(&v11, 1, 0, &v17, 93, &v9);
                                sub_47ec6e(&v17, 93, &v9);
                                sub_47dde0(idx, v31, sub_47e9ae());
                                v26 = 1;
                                continue;
                            }
                        }
                    }
                    else
                    {
                        v41 = &v35->field_1;
                        if (v41->padding_0 == 95 && v35[1].padding_0 == 63)
                        {
                            g_4b4318 = v41;
                            sub_47fb56(&v20, 0, 0, &v15, idx);
                            sub_47dde0(idx, v31, sub_47e9ae(&v15, idx));
                            if (g_4b4318->padding_0 == 64)
                            {
                                g_4b4318 = &g_4b4318->field_1;
                                continue;
                            }
                        }
                        else
                        {
                            sub_47ec02(&v16, 96, sub_481298(&v18, &v19, 39, &v13, idx));
                            sub_47ec6e();
                            goto LABEL_4816b5;
                        }
                    }
                }
                sub_47e5d3(&v24, v32, &g_4b4318, 64);
                sub_47dde0(idx, v31, sub_47ec4a(&v14, "`anonymous namespace'", idx));
                if (g_4b4310->field_0 != 9)
                {
                    sub_47e32e(&v24);
                    continue;
                }
            }
            else
            {
                v1 = idx;
                v0 = &v12;
                g_4b4318 = (char *)v35 - 1;
                v45 = &v23;
            }
        }
        else
        {
            v1 = idx;
            v0 = &v21;
            v45 = &v22;
        }
        sub_48023a(v45, 1, 0);
LABEL_4816b5:
        sub_47dde0(idx, v31, sub_47e9ae());
    }
    if (g_4b4318->padding_0)
    {
        if (g_4b4318->padding_0 != 64)
            v1 = 2;
        else
            return;
    }
    else
    {
        if (!*((int *)idx))
        {
            v1 = 1;
        }
        else
        {
            sub_47e1ce(&v23, v32, 1);
            sub_47ec92(&v21, "::", &v22, idx, v2);
            sub_47dde0(idx, v31, sub_47e9ae());
            return;
        }
    }
    sub_47e2f7(v1);
    return;
}
