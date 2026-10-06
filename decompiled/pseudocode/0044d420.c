// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d420; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44d420_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[4];
    char field_10;
    char padding_11[15];
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
} st_44d420_0;

extern int g_4a245c;
extern unsigned int g_4ad138;

st_44d420_0 * sub_44d420(st_44d420_0 *index)
{
    unsigned long v2;  // ldt
    unsigned long v3;  // gdt
    unsigned short v4;  // fs
    unsigned long long v5;  // 4145
    unsigned int v6;  // eax
    unsigned long long v7;  // 4161
    st_44d420_0 *idx;  // ecx
    int v9;  // esi
    unsigned long long v10;  // 4117
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    v1 = 0xffffffff;
    v5 = _ccall(v2, v3, v4, 0);
    v6 = *((int *)(unsigned int)v5);
    v0 = *((int *)(unsigned int)v5);
    v7 = _ccall(v2, v3, v4, 0);
    *((unsigned int **)(unsigned int)v7) = &v0;
    idx = index->padding_c;
    *((unsigned int *)&idx->padding_11[7]) = 15;
    *((unsigned int *)&idx->padding_11[3]) = 0;
    *((char *)&idx->field_4) = 0;
    index->field_0 = 0;
    index->field_28 = 600;
    index->field_8 = 16;
    index->field_4 = 0;
    sub_44ec80(&g_4a245c, 13, g_4ad138 ^ &v9, v9, v6, sub_496f7b, 0);
    v10 = _ccall(v2, v3, v4, 0);
    *((unsigned long *)(unsigned int)v10) = g_4ad138 ^ &v9;
    return index;
}
