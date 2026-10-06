// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45fa80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45fa80_4 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_45fa80_4;

typedef struct st_45fa80_2 {
    struct st_45fa80_3 *field_0;
    char padding_4[8];
    struct st_45fa80_4 *field_c;
    unsigned int field_10;
} st_45fa80_2;

typedef struct st_45fa80_9 {
    char padding_0[6];
    short field_6;
    short field_8;
    short field_a;
    char padding_c[4];
    struct st_45fa80_5 *field_10;
} st_45fa80_9;

typedef struct st_45fa80_3 {
    struct st_45fa80_0 *field_0;
} st_45fa80_3;

typedef struct st_45fa80_1 {
    unsigned int field_0;
} st_45fa80_1;

typedef struct st_45fa80_0 {
    char padding_0[72];
    struct st_45fa80_1 *field_48;
} st_45fa80_0;

typedef struct st_45fa80_5 {
    char padding_0[8];
    struct st_45fa80_1 *field_8;
} st_45fa80_5;

extern unsigned int g_4a2338[4];
extern unsigned int g_4a235c[4];
extern int g_4ce8f0;
extern char g_4ceae8;

int sub_45fa80(void)
{
    unsigned int v11;  // ebx
    unsigned int v12;  // esi
    unsigned int v13;  // eax
    unsigned int index;  // ebx
    unsigned int *v15;  // eax
    st_45fa80_2 *idx;  // edi
    st_45fa80_9 *v17;  // ecx
    unsigned int v18;  // eax
    st_45fa80_9 *v19;  // eax
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]
    unsigned int v9;  // [bp+0x4]
    int v10;  // [bp+0x8]

    v3 = v11;
    v2 = v12;
    index = v13;
    v4 = 0;
    if (g_4ceae8 & 1)
    {
        v15 = g_4a2338[index];
        if (!(v15 != 0x15 && v15))
        {
            index = 5;
        }
        else if (v15 == 0x14)
        {
            index = 3;
        }
    }
    idx->field_10 = idx->field_10 & 0xfffffffe;
    v1 = 1;
    v7 = v17->field_8;
    v8 = v17->field_a;
    v0 = v9;
    v5 = 0;
    v6 = 0;
    if (D3DXCreateTexture(g_4ce8f0, v9, v10, 1, 0, g_4a2338[index], 1, idx))
    {
        (*((int *)(*((int *)0x1) + 8)))(1);
        return;
    }
    idx->field_0->field_0->field_48(idx->field_0, 0, &v1);
    v18 = v17->field_6 * 2;
    D3DXLoadSurfaceFromMemory(g_4ce8f0, 0, &v0, &v17->field_10, *((int *)(2 * v18 + (char *)&g_4a2338[0])), *((int *)(2 * v18 + (char *)&g_4a235c[0])) * v17->field_8, 0, &v0, 1, 0);
    idx->field_c = g_4a235c[index];
    v19 = &v17->field_10;
    if (!v19)
        return;
    (*((int *)(*((int *)&v19->padding_0[0]) + 8)))(v19);
    return;
}
