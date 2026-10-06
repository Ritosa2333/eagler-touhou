// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ecb6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ecb6_0 {
    char field_0;
} st_47ecb6_0;

extern st_47ecb6_0 *g_4b4318;

int sub_47ecb6(void)
{
    unsigned int v10;  // esi
    char *iter;  // esi
    unsigned int v20;  // edi
    int v21;  // edi
    unsigned int v22;  // edx
    unsigned int v23;  // ebx
    unsigned int v24;  // eax
    char v25;  // 4102
    int v26;  // eax
    unsigned int *v27;  // eax
    unsigned int *v28;  // ecx
    int i;  // ecx
    char v13;  // al
    unsigned int v14;  // edx
    unsigned int v15;  // eax
    unsigned int *v17;  // eax
    unsigned int *v18;  // ecx
    unsigned int v19;  // ebx
    int v0;  // [bp-0x44]
    int v1;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x2c]
    unsigned int v3;  // [bp-0x28]
    unsigned int v4;  // [bp-0x24]
    char v5;  // [bp-0x18]
    int v6;  // [bp-0xc]
    int v7;  // [bp-0x8]
    unsigned int *v8;  // [bp+0x4]
    char v9;  // [bp+0x8]

    v4 = v10;
    iter = &g_4b4318->field_0;
    i = 0;
    v7 = 0;
    if (*(iter) == 81)
    {
        iter += 1;
        v7 = "`non-type-template-parameter";
        g_4b4318 = iter;
    }
    v13 = *(iter);
    if (!*(iter))
    {
        sub_47e1ce(v8, v14, 1);
        return;
    }
    if (v13 >= 48 && v13 <= 57)
    {
        v15 = *(iter) - 47;
        g_4b4318 = iter + 1;
        v17 = (!v7 ? sub_47e692(v15, (int)(v15) >> 31) : sub_47ec4a(&v5, v7, sub_47e692(v15, (int)(v15) >> 31)));
        v18 = v8;
        *(v18) = *(v17);
        v18[1] = v17[1];
        return;
    }
    v3 = v19;
    v2 = v20;
    for (v21 = 0; v13 != 64; i = v23 + v24)
    {
        if (!v13)
        {
            v1 = 1;
            sub_47e1ce(v8, v22, v1);
            return;
        }
        else if (v13 < 65)
        {
            v1 = 2;
            sub_47e1ce(v8, v22, v1);
            return;
        }
        else if (v13 <= 80)
        {
            v23 = v13 - 65;
            v6 = (int)(v23) >> 31;
            v24 = sub_483220(i, v21, 16, 0);
            iter += 1;
            v21 = v6 + v22 + (v23 + v24 < v23);
            g_4b4318 = iter;
            v13 = *(iter);
        }
        else
        {
            v1 = 2;
            sub_47e1ce(v8, v22, v1);
            return;
        }
    }
    v25 = *(iter);
    g_4b4318 = iter + 1;
    if (v25 != 64)
    {
        v1 = 2;
        sub_47e1ce(v8, v22, v1);
        return;
    }
    v1 = v21;
    v0 = i;
    if (v9)
    {
        if (v7)
        {
            v26 = sub_47e700();
        }
        else
        {
            v27 = sub_47e700();
            v28 = v8;
            *(v28) = *(v27);
            v28[1] = v27[1];
            return;
        }
    }
    else
    {
        if (v7)
        {
            v26 = sub_47e692();
        }
        else
        {
            v27 = sub_47e692();
            v28 = v8;
            *(v28) = *(v27);
            v28[1] = v27[1];
            return;
        }
    }
    v27 = sub_47ec4a(&v5, v7, v26);
    v28 = v8;
    *(v28) = *(v27);
    v28[1] = v27[1];
    return;
}
