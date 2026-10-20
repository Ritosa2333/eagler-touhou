// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x459a60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_459a60_1 {
    char padding_0[168];
    unsigned int field_a8;
} st_459a60_1;

typedef struct st_459a60_0 {
    char padding_0[956];
    unsigned int field_3bc;
    unsigned int field_3c0;
    char padding_3c4[184];
    unsigned int field_47c;
    char field_47e;
    char padding_47f[1];
    unsigned int field_480;
} st_459a60_0;

typedef struct st_459a60_3 {
    unsigned int field_0;
} st_459a60_3;

typedef struct st_459a60_2 {
    char padding_0[228];
    struct st_459a60_3 *field_e4;
    char padding_e8[44];
    struct st_459a60_3 *field_114;
} st_459a60_2;

typedef struct st_459a60_5 {
    struct st_459a60_2 *field_0;
} st_459a60_5;

extern char g_4b5638;
extern char g_4b5640;
extern char g_4b5646;
extern st_459a60_5 *g_4ce8f0;

int sub_459a60(void)
{
    st_459a60_0 *v7;  // ebp
    st_459a60_1 *idx;  // eax
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    char v19;  // al
    int v9;  // esi
    int v10;  // ebp
    int v11;  // ebx
    char v12;  // al
    unsigned int v13;  // edi
    unsigned int v14;  // edi
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v0;  // [bp-0x50]
    unsigned int v1;  // [bp-0x34]
    unsigned int v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x10]
    st_459a60_0 *v6;  // [bp+0x4]

    v7 = v6;
    if (*((char *)(idx + &g_4b5640)) != (char)(v7->field_47c >> 5 & 7))
    {
        sub_45a3c0(v9, v10, v11);
        v12 = v7->field_47c >> 5;
        *((char *)(idx + &g_4b5640)) = v12 & 7;
        switch (v12 & 7)
        {
        case 0:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 5);
            v4 = 6;
            goto LABEL_459b57;
        case 1:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 5);
            v4 = 2;
            goto LABEL_459b57;
        case 2:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 5);
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 20, 2);
            v2 = 3;
            goto LABEL_459b6a;
        case 3:
            v5 = 2;
            goto LABEL_459b44;
        case 4:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 10);
            v4 = 6;
            goto LABEL_459b57;
        case 5:
            v5 = 9;
LABEL_459b44:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19);
            v4 = 1;
LABEL_459b57:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 20);
            v2 = 1;
LABEL_459b6a:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 171);
        case 6:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 4);
            v4 = 6;
            goto LABEL_459b57;
        }
    }
    v1 = v13;
    v14 = (!(1 & *((char *)&v7->field_47c + 2)) ? v7->field_3bc : v7->field_3c0);
    v3 = v14;
    if (*((int *)&idx[52172].padding_0[64]))
    {
        v15 = idx[52172].padding_0[62] * (char)(v14 >> 16) >> 7;
        if (v15 >= 0x100)
            v15 = 0xff;
        v3 = _INSERT(v14, 2, v15);
        v16 = *((char *)((void*)&v14 + 1)) * idx[52172].padding_0[61] >> 7;
        if (v16 >= 0x100)
            v16 = 0xff;
        *((char *)&v3 + 1) = v16;
        v17 = (char)v3 * idx[52172].padding_0[60] >> 7;
        if (v17 >= 0x100)
            v17 = 0xff;
        *((char *)&v3) = v17;
        v18 = *((char *)((void*)&v3 + 3)) * idx[52172].padding_0[63] >> 7;
        if (v18 >= 0x100)
            v18 = 0xff;
        *((char *)&v3 + 3) = v18;
        v14 = v3;
    }
    if (*((int *)(idx + &g_4b5638)) != v14)
    {
        sub_45a3c0();
        *((unsigned int *)(idx + &g_4b5638)) = v14;
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 60, v14);
    }
    if (*((char *)(idx + &g_4b5646)) != (char)((unsigned int)(*((int *)&v7->field_47e)) >> 1 & 1))
    {
        sub_45a3c0();
        v19 = (unsigned int)(*((int *)&v7->field_47e)) >> 1;
        *((char *)(idx + &g_4b5646)) = v19 & 1;
        if (!(v19 & 1))
        {
            g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 5, 2);
            v0 = 2;
        }
        else
        {
            g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 5, 1);
            v0 = 1;
        }
        g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 6);
    }
    idx->field_a8 = idx->field_a8 + 1;
    return;
}
