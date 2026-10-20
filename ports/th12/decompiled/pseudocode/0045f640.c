// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45f640; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45f640_5 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_45f640_5;

typedef struct st_45f640_3 {
    struct st_45f640_4 *field_0;
    char padding_4[4];
    int field_8;
    struct st_45f640_5 *field_c;
    unsigned int field_10;
} st_45f640_3;

typedef struct st_45f640_6 {
    char padding_0[6];
    short field_6;
    short field_8;
    short field_a;
    char padding_c[16];
    unsigned int field_1c;
} st_45f640_6;

typedef struct st_45f640_4 {
    struct st_45f640_0 *field_0;
} st_45f640_4;

typedef struct st_45f640_1 {
    unsigned int field_0;
} st_45f640_1;

typedef struct st_45f640_0 {
    char padding_0[8];
    struct st_45f640_1 *field_8;
    char padding_c[60];
    struct st_45f640_1 *field_48;
} st_45f640_0;

extern unsigned int g_4a2338[4];
extern unsigned int g_4a235c[4];
extern char g_4ceae8;

unsigned int sub_45f640(void)
{
    unsigned int index;  // eax
    unsigned int *v8;  // eax
    st_45f640_6 *v15;  // edi, Other Possible Types: int
    st_45f640_3 *idx;  // esi
    st_45f640_0 **v10;  // eax
    int v11;  // ebx
    st_45f640_6 *v12;  // eax
    unsigned int v13;  // ecx
    st_45f640_0 **idx1;  // edx
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x1c]
    char v2;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0x8]

    v3 = index;
    if (g_4ceae8 & 1)
    {
        v8 = g_4a2338[index];
        if (!(v8 != 0x15 && v8))
        {
            v3 = 5;
        }
        else if (v8 == 0x14)
        {
            v3 = 3;
        }
    }
    v10 = idx->field_0;
    idx->field_10 = idx->field_10 & 0xfffffffe;
    idx->field_8 = v11;
    *(v10)->field_48(v10, 0, &v2, 0);
    v0 = 0;
    if (!v5)
    {
        D3DXLoadSurfaceFromFileInMemory(v10, 0, 0, v15, v11, 0, 1, 0);
    }
    else
    {
        v12 = &v15->padding_0[v15->field_1c];
        v1 = 0;
        v2 = 0;
        v3 = v12->field_8;
        v4 = v12->field_a;
        v13 = v12->field_6 * 2;
        idx1 = v10;
        D3DXLoadSurfaceFromMemory(idx1, 0, 0, &v12->padding_c[4], *((int *)(2 * v13 + (char *)&g_4a2338[0])), *((int *)(2 * v13 + (char *)&g_4a235c[0])) * v12->field_8, 0, &v1, 1);
    }
    *(idx1)->field_8(idx1);
    sub_45ee00(idx1);
    idx->field_c = *((int *)(0x4 * idx1 + (char *)&g_4a235c[0]));
    return 0;
}
