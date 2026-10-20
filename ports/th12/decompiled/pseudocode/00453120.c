// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453120; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453120_0 {
    char padding_0[12];
    struct st_453120_1 *field_c;
} st_453120_0;

typedef struct st_453120_10 {
    struct st_453120_9 *field_0;
    char padding_4[4];
    struct st_453120_9 *field_8;
    HWND field_c;
    struct st_453120_9 *field_10;
    char padding_14[56];
    unsigned int field_4c;
    char padding_50[6460];
    unsigned int field_198c;
    unsigned int field_1990;
    int field_1994;
    char padding_1998[14588];
    unsigned int field_5294;
    unsigned int field_5298;
} st_453120_10;

typedef struct st_453120_6 {
    char padding_0[8];
    struct st_453120_1 *field_8;
    char padding_c[12];
    struct st_453120_1 *field_18;
} st_453120_6;

typedef struct st_453120_9 {
    struct st_453120_6 *field_0;
} st_453120_9;

typedef struct st_453120_1 {
    unsigned int field_0;
} st_453120_1;

extern int g_4a2e04;
extern int g_4a2e34;
extern int g_4a2e6c;
extern unsigned int g_4ad138;
extern unsigned int g_4ae5a0;
extern unsigned int g_4ae5a4[4];
extern unsigned int g_4aea50[4];
extern unsigned int g_4aea54[4];
extern unsigned int g_4d4770;

void sub_453120(st_453120_10 *idx, HWND a1)
{
    unsigned int v22;  // ebx
    unsigned int v23;  // esi
    st_453120_0 **v32;  // eax
    st_453120_10 *v33;  // edi
    unsigned int v34;  // ebx
    unsigned int *v35;  // edi
    st_453120_9 **v36;  // edi
    st_453120_6 **v37;  // eax
    unsigned int v24;  // edi
    int i;  // eax
    st_453120_10 *iter;  // ecx
    unsigned int v27;  // edx
    unsigned int v28;  // edx
    st_453120_9 **v29;  // eax
    st_453120_9 **v30;  // edi
    st_453120_6 **v31;  // eax
    unsigned int v0;  // [bp-0x7c]
    unsigned int v1;  // [bp-0x68], Other Possible Types: unsigned short
    unsigned short v2;  // [bp-0x66]
    unsigned int v3;  // [bp-0x64]
    unsigned int v4;  // [bp-0x60]
    int v5;  // [bp-0x5c], Other Possible Types: unsigned int, unsigned short
    unsigned short v6;  // [bp-0x5a]
    unsigned int v7;  // [bp-0x58], Other Possible Types: unsigned short
    unsigned int v8;  // [bp-0x54]
    unsigned int v9;  // [bp-0x50]
    char v10;  // [bp-0x4c]
    unsigned int v11;  // [bp-0x48]
    unsigned int v12;  // [bp-0x44]
    unsigned int v13;  // [bp-0x40]
    unsigned int v14;  // [bp-0x3c]
    char *v15;  // [bp-0x38], Other Possible Types: unsigned int
    unsigned int v16;  // [bp-0x34]
    unsigned int v17;  // [bp-0x30]
    unsigned int v18;  // [bp-0x2c]
    unsigned int v19;  // [bp-0x28]
    unsigned int v20;  // [bp-0x4]

    v20 = g_4ad138 ^ &v10;
    v9 = v22;
    v8 = v23;
    v7 = v24;
    i = 0;
    iter = &idx->padding_50[6456];
    do
    {
        *((unsigned int *)&iter->padding_4[0]) = 0xffffffff;
        v27 = &g_4ae5a0;
        do
        {
            v28 = v27;
            v27 = v28;
        } while (*((int *)v27) != i && (v27 = v28 + 20, v28 != 0xffffffec));
        iter->field_c = i;
        iter->field_8 = v27;
        i += 1;
        iter = &iter->padding_14[4];
    } while (i < 60);
    memset(&idx->padding_14[12], -0x1, 44);
    idx->field_4c = 0xffffffff;
    v29 = sub_46c9ea(v5);
    if (v29)
    {
        *(v29) = NULL;
        v30 = v29;
    }
    else
    {
        v30 = NULL;
    }
    idx->field_10 = v30;
    v31 = *(v30);
    if (v31)
    {
        *(v31)->field_8(v31);
        *(v30) = NULL;
    }
    if (ordinal.11.dsound.dll(0, v30, 0) >= 0 && *(v30)->field_0->field_18(*(v30), a1, 2) >= 0)
    {
        sub_465690(v30);
        v1 = 0;
        v5 = 0;
        idx->field_0 = idx->field_10->field_0;
        v11 = 0;
        v12 = 0;
        v13 = 0;
        v15 = 0;
        v3 = 0;
        v4 = 0;
        v7 = 0;
        v14 = 0;
        v16 = 0;
        v17 = 0;
        v18 = 0;
        v19 = 0;
        v1 = 1;
        v2 = 2;
        v7 = 0;
        v0 = 0;
        v6 = 16;
        v32 = idx->field_0;
        v15 = &v1;
        v33 = &idx->field_8;
        v5 = 4;
        *((unsigned int *)&idx->padding_14[4]) = 0;
        v11 = 36;
        v12 = 32776;
        v13 = 0x8000;
        v3 = 44100;
        v4 = 176400;
        if (*(v32)->field_c(v32, &v11, v33, 0) >= 0 && (*((int *)&v33->field_0->field_0[1].padding_c[4]))(v33->field_0, 0, 0x8000, &v0, &v5, &v3, &v4, 0) >= 0)
        {
            sub_477420(&v0, 0, 0x8000);
            (*((int *)&v33->field_0->field_0[2].padding_c[8]))(v33->field_0, &v0, v0, &v11, v33);
            (*((int *)&v33->field_0->field_0[1].padding_c[8]))(v33->field_0, 0, 0, 1);
            idx->field_5294 = 100;
            idx->field_5298 = 100;
            SetTimer(a1, NULL, 250, NULL);
            idx->field_c = a1;
            v34 = 0;
            v35 = &g_4ae5a4[0];
            while (g_4d4770 != 2)
            {
                if (!sub_454580(g_4aea50[*(v35)]))
                {
                    v34 += 1;
                    if (v35 + 5 >= &g_4aea54[0])
                    {
                        sub_464220(&g_4a2e6c);
                        _security_check_cookie();
                        return;
                    }
                }
                else
                {
                    sub_464220(&g_4a2e34, g_4aea50[g_4ae5a4[5 * v34]]);
                    _security_check_cookie();
                    return;
                }
            }
        }
    }
    else
    {
        sub_464220(&g_4a2e04);
        v36 = idx->field_10;
        if (v36)
        {
            v37 = *(v36);
            if (v37)
            {
                *(v37)->field_8(v37);
                *(v36) = NULL;
            }
            sub_46ca4f(v36);
            idx->field_10 = NULL;
        }
    }
    _security_check_cookie();
    return;
}
