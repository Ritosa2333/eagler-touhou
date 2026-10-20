// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f5d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47f5d0_1 {
    unsigned int field_0;
} st_47f5d0_1;

typedef struct st_47f5d0_0 {
    char field_0;
} st_47f5d0_0;

extern st_47f5d0_0 *g_4b4318;
extern unsigned int g_4b4328;
extern st_47f5d0_1 *g_4b432c;

int sub_47f5d0(void)
{
    unsigned int v24;  // ebx
    char v25;  // 4132
    unsigned int *v35;  // eax
    unsigned int v36;  // edx
    unsigned int v37;  // edx
    int v38;  // eax
    char *v39;  // eax
    unsigned int v40;  // edx
    unsigned int v42;  // edx
    unsigned int v43;  // esi
    char v26;  // bl
    unsigned int v44;  // esi
    unsigned int v45;  // esi
    unsigned int v27;  // esi
    unsigned int v28;  // esi
    unsigned int v29;  // edi
    unsigned int *v30;  // edi
    unsigned int v31;  // esi
    unsigned int v32;  // esi
    unsigned int v33;  // edx
    unsigned int v0;  // [bp-0xf0]
    char *v1;  // [bp-0xec], Other Possible Types: unsigned int
    unsigned int *v2;  // [bp-0xe8]
    unsigned int v3;  // [bp-0xe4], Other Possible Types: int
    unsigned int v4;  // [bp-0xe0]
    unsigned int v5;  // [bp-0xdc]
    unsigned int v6;  // [bp-0xd8]
    char v7;  // [bp-0xcc]
    char v8;  // [bp-0xc4]
    char v9;  // [bp-0xb4]
    char v10;  // [bp-0xac]
    char v11;  // [bp-0xa4]
    char v12;  // [bp-0x9c]
    char v13;  // [bp-0x94]
    char v14;  // [bp-0x8c]
    char v15;  // [bp-0x84]
    char v16;  // [bp-0x80]
    unsigned int v17;  // [bp-0x7c]
    unsigned int v18;  // [bp-0x7b], Other Possible Types: char
    char v19;  // [bp-0x7a]
    char v20[8];  // [bp-0x18]
    char v21;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v22;  // [bp-0xc]
    unsigned int *v23;  // [bp+0x4]

    v6 = v24;
    v25 = g_4b4318->field_0;
    v26 = g_4b4318->field_0;
    g_4b4318 = g_4b4318 + 1;
    v5 = v27;
    v28 = v25;
    v4 = v29;
    v30 = v23;
    if (v28 <= 0x44)
    {
        if (v28 == 0x44)
        {
LABEL_47f761:
            sub_47f57e(&v15);
            if (g_4b4328 & 0x4000)
            {
                sub_47e213(&v15, v37, v20, 16);
                v38 = sub_46d114(v20);
                v3 = g_4b432c(v38);
                if (v3)
                {
                    v3 = v3;
                    sub_47e59a(v3);
                    return;
                }
            }
            v3 = "'";
            v2 = v30;
            v1 = &v15;
            if (v26 == 0x44)
            {
                v0 = "`template-parameter";
                v39 = &v14;
            }
            else
            {
                v0 = "`non-type-template-parameter";
                v39 = &v7;
            }
            sub_47ec4a(v39);
            sub_47ec92();
            return;
        }
        v31 = v28;
        if (!v28)
        {
            g_4b4318 = (char *)g_4b4318 - 1;
LABEL_47f70b:
            v3 = 1;
            sub_47e1ce(v3);
            return;
        }
        v32 = v31 - 48;
        if (v31 == 48)
        {
            sub_47f57e(v30);
            return;
        }
        if (v32 == 1)
        {
            if (g_4b4318->field_0 == 64)
            {
                g_4b4318 = g_4b4318 + 1;
                v3 = "NULL";
                sub_47e59a(v3);
                return;
            }
            v1 = "&";
            v35 = sub_47e59a("&", v30, sub_481298(&v12));
LABEL_47f6ac:
            sub_47e9ae(v35, v36, v1, v30);
            return;
        }
        else if (v32 == 2)
        {
            sub_47f57e(&v15);
            sub_47f57e(&v21);
            if (v16 > 1 || (char)v22 > 1)
                goto LABEL_47f70b;
            if (sub_47e213(&v15, v33, &v18, 100))
            {
                v17 = v18;
                if (v18 == 45)
                {
                    v18 = v19;
                    v19 = 46;
                }
                else
                {
                    v18 = 46;
                }
                v1 = 101;
                v35 = sub_47ec6e(sub_47e59a(&v17, &v10, 101, v30, &v21), v33, &v17, &v10);
                goto LABEL_47f6ac;
            }
        }
    }
    else
    {
        if (v28 == 69)
        {
            sub_481298(v30);
            return;
        }
        if (v28 > 69)
        {
            if (v28 <= 74)
            {
                sub_47e56d(&v15, v40, 123);
                if (v26 >= 72 && v26 <= 74)
                {
                    sub_47e7bf(&v15, v40, sub_481298(&v13));
                    sub_47e9f6(44);
                }
                v43 = v28 - 70;
                if (v28 != 70)
                {
                    v44 = v43 - 1;
                    if (v43 != 1)
                    {
                        v45 = v44 - 1;
                        if (v44 == 1)
                            goto LABEL_47f862;
                        if (v45 != 1)
                        {
                            if (v45 != 2)
                            {
                                sub_47ec6e(&v15, v42, v30, 125);
                                return;
                            }
                            goto LABEL_47f822;
                        }
                    }
                    else
                    {
LABEL_47f822:
                        sub_47e7bf(&v15, v40, sub_47f57e(&v11));
                        sub_47e9f6(44);
                    }
                }
                sub_47e7bf(&v15, v40, sub_47f57e(&v9));
                sub_47e9f6(44);
LABEL_47f862:
                sub_47e7bf(&v15, v40, sub_47f57e(&v8));
                sub_47ec6e(&v15, v42, v30, 125);
                return;
            }
            if (v28 == 81)
                goto LABEL_47f761;
            if (v28 == 82)
            {
                sub_48023a(&v21, 0, 0);
                sub_47f57e(&v15);
                *(v30) = v21;
                v30[1] = v22;
                return;
            }
        }
    }
    v3 = 2;
    sub_47e1ce(v3);
    return;
}
