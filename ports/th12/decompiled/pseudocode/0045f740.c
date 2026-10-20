// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45f740; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45f740_4 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_45f740_4;

typedef struct st_45f740_2 {
    struct st_45f740_3 *field_0;
    char padding_4[4];
    unsigned int field_8;
    struct st_45f740_4 *field_c;
    unsigned int field_10;
} st_45f740_2;

typedef struct st_45f740_7 {
    char padding_0[6];
    short field_6;
    short field_8;
    short field_a;
    char padding_c[16];
    unsigned int field_1c;
} st_45f740_7;

typedef struct st_45f740_3 {
    struct st_45f740_0 *field_0;
} st_45f740_3;

typedef struct st_45f740_1 {
    unsigned int field_0;
} st_45f740_1;

typedef struct st_45f740_0 {
    char padding_0[72];
    struct st_45f740_1 *field_48;
} st_45f740_0;

typedef struct st_45f740_8 {
    char padding_0[8];
    struct st_45f740_1 *field_8;
} st_45f740_8;

extern unsigned int g_4a2338[4];
extern unsigned int g_4a235c[4];
extern char g_4ceae8;

unsigned int sub_45f740(unsigned int a0)
{
    unsigned int index;  // eax
    unsigned int *v16;  // eax
    unsigned int v21;  // edi
    st_45f740_2 *idx;  // esi
    st_45f740_7 *v18;  // ebx, Other Possible Types: int
    st_45f740_7 *v19;  // eax
    unsigned int v20;  // ecx
    st_45f740_8 **idx1;  // [bp-0x70]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x40]
    unsigned int v3;  // [bp-0x3c]
    unsigned int v4;  // [bp-0x38]
    unsigned int v5;  // [bp-0x34]
    unsigned int v6;  // [bp-0x30]
    char v7;  // [bp-0x2c], Other Possible Types: unsigned int
    unsigned int v8;  // [bp-0x28]
    unsigned int v9;  // [bp-0x24]
    unsigned int v10;  // [bp-0x20]
    unsigned int v11;  // [bp-0x1c]
    unsigned int v12;  // [bp-0x18]
    int v13;  // [bp-0x10]
    unsigned int v14;  // [bp-0x4]

    v5 = index;
    if (g_4ceae8 & 1)
    {
        v16 = g_4a2338[index];
        if (!(v16 != 0x15 && v16))
        {
            v5 = 5;
        }
        else if (v16 == 0x14)
        {
            v5 = 3;
        }
    }
    idx->field_10 = idx->field_10 & 0xfffffffe;
    idx->field_8 = a0;
    v4 = 0;
    v1 = 0;
    idx->field_0->field_0->field_48(idx->field_0, 0, &v4);
    if (!v14)
    {
        (*((int *)(*((int *)NULL) + 48)))(0, &v7);
        v3 = v11;
        v4 = v12;
        idx1 = NULL;
        v1 = 0;
        v2 = v21;
        D3DXLoadSurfaceFromFileInMemory(&v7, 0, &v1, v18, v13, 0, 1, 0, 0);
    }
    else
    {
        v19 = &v18->padding_0[v18->field_1c];
        v3 = 0;
        v4 = 0;
        v5 = v19->field_8;
        v6 = v19->field_a;
        v7 = 0;
        v8 = v21;
        v9 = v19->field_8;
        v10 = v19->field_a + v21;
        v20 = v19->field_6 * 2;
        idx1 = NULL;
        D3DXLoadSurfaceFromMemory(0, 0, &v7, &v19->padding_c[4], *((int *)(2 * v20 + (char *)&g_4a2338[0])), *((int *)(2 * v20 + (char *)&g_4a235c[0])) * v19->field_8, 0, &v3, 1, 0);
    }
    *(idx1)->field_8(idx1);
    sub_45ee00(idx1);
    idx->field_c = *((int *)(0x4 * idx1 + (char *)&g_4a235c[0]));
    return 0;
}
