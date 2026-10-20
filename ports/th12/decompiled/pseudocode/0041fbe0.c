// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41fbe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41fbe0_1 {
    unsigned int field_0;
} st_41fbe0_1;

typedef struct st_41fbe0_0 {
    char padding_0[27952];
    struct st_41fbe0_1 *field_6d30;
    unsigned int field_6d34;
} st_41fbe0_0;

extern unsigned int g_4ad138;
extern unsigned int g_4b0cb8;
extern unsigned int g_4b0cc0;
extern st_41fbe0_0 *g_4b43e4;

void sub_41fbe0(void)
{
    unsigned long v4;  // ldt
    unsigned long v5;  // gdt
    unsigned int *v15;  // eax
    unsigned int v16;  // 4101
    unsigned long long v17;  // 4118
    unsigned short v6;  // fs
    unsigned long long v7;  // 4172
    unsigned int v8;  // eax
    unsigned long long v9;  // 4176
    int v10;  // edi
    int v11;  // esi
    int v12;  // ecx
    unsigned int v13;  // ebx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v7 = _ccall(v4, v5, v6, 0);
    v8 = *((int *)(unsigned int)v7);
    v1 = *((int *)(unsigned int)v7);
    v9 = _ccall(v4, v5, v6, 0);
    *((unsigned int **)(unsigned int)v9) = &v1;
    if (g_4b43e4->field_6d30)
    {
        sub_41cd90(g_4ad138 ^ &v10, v10, v11, v12, v8, sub_49769b, -0x1);
        sub_46ca4f(g_4b43e4->field_6d30);
        g_4b43e4->field_6d30 = NULL;
    }
    v0 = sub_46c9ea(172);
    v2 = 0;
    v15 = (!sub_46c9ea(172) ? NULL : sub_41f900(*((int *)(g_4b43e4->field_6d34 + v13 * 8 + 4)) + g_4b43e4->field_6d34));
    g_4b43e4->field_6d30 = v15;
    *(v15) = v13;
    v16 = g_4b0cb8;
    g_4b0cb8 = v13 + 1;
    if (v16 != g_4b0cb8)
        g_4b0cc0 = 0;
    v17 = _ccall(v4, v5, v6, 0);
    *((unsigned int *)(unsigned int)v17) = sub_46c9ea(172);
    return;
}
