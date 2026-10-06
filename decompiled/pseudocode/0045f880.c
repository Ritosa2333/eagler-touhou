// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45f880; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45f880_5 {
    struct st_45f880_6 *field_0;
    unsigned int field_4;
    struct st_45f880_3 *field_8;
    char padding_c[4];
    unsigned int field_10;
} st_45f880_5;

typedef struct st_45f880_6 {
    struct st_45f880_0 *field_0;
} st_45f880_6;

typedef struct st_45f880_1 {
    unsigned int field_0;
} st_45f880_1;

typedef struct st_45f880_3 {
    char padding_0[48];
    struct st_45f880_1 *field_30;
} st_45f880_3;

typedef struct st_45f880_0 {
    char padding_0[48];
    struct st_45f880_1 *field_30;
    char padding_34[20];
    struct st_45f880_1 *field_48;
} st_45f880_0;

extern unsigned int g_4a2338[4];
extern char g_4ce8f0;
extern char g_4ceae8;

int sub_45f880(unsigned int a0, unsigned int a1, int a2)
{
    unsigned int v13;  // ebx
    int v14;  // ebp
    unsigned int index;  // eax
    unsigned int *v16;  // eax
    st_45f880_5 *idx;  // esi
    unsigned int v18;  // ecx
    int v19;  // edi
    int v20;  // ecx
    int v21;  // edx
    unsigned int v2;  // [bp-0xb0]
    unsigned int v3;  // [bp-0xac]
    unsigned int v4;  // [bp-0xa8]
    unsigned int v5;  // [bp-0xa8]
    int v6;  // [bp-0xa4]
    st_45f880_3 *v8;  // [bp-0x80]
    unsigned int *v9;  // [bp-0x6c]
    unsigned int v10;  // [bp-0x48]
    unsigned int idx1;  // [bp-0x40]
    char v12;  // [bp-0x3c]

    v10 = v13;
    v14 = a2;
    idx1 = index;
    if (g_4ceae8 & 1)
    {
        v16 = g_4a2338[index];
        if (!(v16 != 0x15 && v16))
        {
            idx1 = 5;
        }
        else if (v16 == 0x14)
        {
            idx1 = 3;
        }
    }
    idx->field_10 = idx->field_10 & 0xfffffffe;
    v18 = idx->field_4;
    v9 = g_4a2338[idx1];
    v8 = idx->field_8;
    if (D3DXCreateTextureFromFileInMemoryEx(*((int *)&g_4ce8f0), v18, idx->field_8, 0, 0, 0, 0, g_4a2338[idx1], 1, 1, -0x1, 0, 0, 0, &v12))
        return -0x1;
    (*((int *)(*((int *)0) + 72)))(0, 0, &v8);
    v8->field_30(&v8, &v9);
    if (!v19 && !v14)
    {
        idx->field_0 = &v8;
    }
    else
    {
        v5 = 0;
        D3DXCreateTexture(*((int *)&g_4ce8f0), v19, v14, 0, 0, 0, 1, idx);
        idx->field_0->field_0->field_48(idx->field_0, 0, &v5);
        v20 = v14;
        v21 = v19;
        if (v21 + a0 > *((int *)&g_4ce8f0))
            v21 = *((int *)&g_4ce8f0) - a0;
        if (v20 > v18)
            v20 = v18 - 0;
        v2 = a0;
        v6 = v20;
        v4 = v21 + a0;
        v3 = 0;
        if (D3DXLoadSurfaceFromSurface(v14, 0, 0, &v4, 0, &v2, 1, 0))
            sub_4622e0("D3DXLoadSurfaceFromSurface error\n", *((int *)(0x4 * &v4 + (char *)&g_4a2338[0])));
        v14 = v20;
    }
    sub_45ee00();
    *((unsigned int *)&idx->padding_c[0]) = 4;
    return v14 * v19 * 4;
}
