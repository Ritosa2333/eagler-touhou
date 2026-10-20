// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47021f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47021f_0 {
    char padding_0[12];
    char field_c;
} st_47021f_0;

typedef struct st_47021f_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[20];
    char field_24;
} st_47021f_1;

typedef struct st_47021f_2 {
    char padding_0[112];
    unsigned int field_70;
} st_47021f_2;

extern unsigned int g_470d97[4];
extern char g_49cd88;
extern char g_49cda8;
extern st_47021f_1 g_4adbb0;
extern st_47021f_1 g_4d6320;

int sub_47021f(st_47021f_0 *a0, char *a1, int a2, unsigned int a3)
{
    char *v17;  // ebx
    int v18;  // edi
    unsigned int v27;  // eax
    int v19;  // esi
    int v20;  // ebx
    int v21;  // eax
    st_47021f_1 *v22;  // ecx
    st_47021f_1 *v23;  // eax
    int v24;  // ecx
    unsigned int v25;  // eax
    char v26;  // 4104
    unsigned int v0;  // [bp-0x290]
    st_47021f_2 *idx;  // [bp-0x258]
    char v2;  // [bp-0x254]
    st_47021f_0 *v3;  // [bp-0x250]
    unsigned int v4;  // [bp-0x24c]
    unsigned int v5;  // [bp-0x248]
    unsigned int v6;  // [bp-0x244]
    char *v7;  // [bp-0x240]
    unsigned int v8;  // [bp-0x23c]
    unsigned int v9;  // [bp-0x238]
    unsigned int v10;  // [bp-0x234]
    int v11;  // [bp-0x22c]
    unsigned int v12;  // [bp-0x228]
    unsigned int v13;  // [bp-0x224]
    unsigned int v14;  // [bp-0x21c]
    char v15;  // [bp-0x215]
    unsigned int v16;  // [bp-0x214]

    v17 = a1;
    v3 = a0;
    v12 = a3;
    v4 = 0;
    v16 = 0;
    v9 = 0;
    v14 = 0;
    v10 = 0;
    v6 = 0;
    v8 = 0;
    sub_46d3b4(a2, v18, v19, v20);
    if (a0)
    {
        if (!(a0->field_c & 64))
        {
            v21 = sub_47c1da(a0);
            if (v21 != -0x1 && v21 != -0x2)
                v22 = (v21 & 31) * 64 + (&g_4d6320.field_0)[v21 >> 5];
            else
                v22 = &g_4adbb0.field_0;
            if (!(v22->field_24 & 127))
            {
                if (v21 != -0x1 && v21 != -0x2)
                    v23 = (v21 & 31) * 64 + (&g_4d6320.field_0)[v21 >> 5];
                else
                    v23 = &g_4adbb0.field_0;
                if (!(v23->field_24 & 128))
                    goto LABEL_470323;
            }
        }
        else
        {
LABEL_470323:
            v24 = 0;
            if (v17)
            {
                v26 = *(v17);
                v11 = 0;
                v13 = 0;
                v5 = 0;
                v15 = v26;
                if (v26)
                {
                    while (1)
                    {
                        v7 = v17 + 1;
                        if (v11 < NULL)
                            break;
                        v0 = 7;
                        v24 = (char)((&g_49cda8)[8 * (v26 - 32 <= 88 ? *(&(&g_49cd88)[v26]) & 15 : 0) + v24]) >> 4;
                        if (v24 <= 7)
                            goto *((void *)(g_470d97[v24]));
                        v17 = v7;
                        v26 = *(v17);
                        v15 = v26;
                        if (!v26)
                            break;
                    }
                }
                if (!v2)
                    return v27;
                idx->field_70 = idx->field_70 & 0xfffffffd;
                return v27;
            }
        }
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v25;
}
