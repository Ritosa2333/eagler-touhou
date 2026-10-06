// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x420e70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_420e70_1 {
    char padding_0[27868];
    unsigned int field_6cdc;
    int field_6ce0;
    char padding_6ce4[52];
    char field_6d18;
} st_420e70_1;

typedef struct st_420e70_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_420e70_0;

extern unsigned int g_4aeeb4[4];
extern unsigned int g_4af0f8[4];
extern unsigned int g_4b0c40;
extern st_420e70_0 *g_4b0c44;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cc4;
extern unsigned int g_4b0cc8;
extern unsigned int g_4b0cd8;
extern unsigned int g_4b0ce0;
extern st_420e70_1 *g_4b43e4;

void sub_420e70(void)
{
    unsigned int v4;  // ebx
    unsigned int *v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // edi
    unsigned int v8;  // ecx
    int v9;  // eax
    int v10;  // eax
    int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v5 = &g_4b0c44->field_0;
    v1 = v6;
    v0 = v7;
    v8 = g_4b43e4->field_6cdc;
    if (g_4b0c44 != v8)
    {
        v9 = (char *)g_4b0c44 - v8;
        v10 = (int)(v9 + (v9 >> 31 & 31)) >> 5;
        if (v10 >= 578910)
        {
            v10 = 578910;
        }
        else if (!v10)
        {
            v10 = 1;
        }
        if (g_4b43e4->field_6ce0 < v10)
        {
            g_4b43e4->field_6ce0 = v10;
            v5 = &g_4b0c44->field_0;
        }
        v11 = (char *)v5 - v8;
        if (g_4b43e4->field_6ce0 > v11)
            g_4b43e4->field_6ce0 = v11;
        g_4b43e4->field_6cdc = g_4b43e4->field_6cdc + g_4b43e4->field_6ce0;
        if (g_4b43e4->field_6cdc >= g_4b0c44)
            g_4b43e4->field_6ce0 = 0;
    }
    if (g_4b0c40 < g_4b43e4->field_6cdc)
    {
        g_4b0c40 = g_4b43e4->field_6cdc;
        v12 = g_4b0ce0 | 4;
        g_4b0cc8 = g_4b0cc4;
        g_4b0ce0 = v12;
        if (!((char)g_4b0ce0 & 4))
            sub_420f90(0);
    }
    if (g_4b43e4->field_6d18 & 32)
        return;
    if (g_4b0ca8 != 4)
    {
        if (g_4b43e4->field_6cdc < g_4aeeb4[g_4b0cd8])
            return;
    }
    else
    {
        if (g_4b43e4->field_6cdc < g_4af0f8[g_4b0cd8])
            return;
    }
    sub_422ce0();
    g_4b0cd8 = g_4b0cd8 + 1;
    return;
}
