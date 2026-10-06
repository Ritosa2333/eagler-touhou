// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4044f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4044f0_0 {
    char padding_0[16];
    void* field_10;
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[420];
    unsigned int field_1c4;
    unsigned int field_1c8;
    char padding_1cc[13368];
    int field_3604;
    int field_3608;
} st_4044f0_0;

typedef struct st_4044f0_1 {
    unsigned short field_0;
    unsigned short field_2;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[132];
    char field_90;
} st_4044f0_1;

extern int g_49f588;
extern void g_4d4f37;
extern char g_4d4f38;

int sub_4044f0(void)
{
    st_4044f0_0 *idx;  // ebp
    void* v6;  // eax
    void* v15;  // edi
    void* iter;  // edi
    unsigned int v17;  // ecx
    unsigned int v18;  // ecx
    unsigned int v19;  // eax
    int v20;  // ebx
    st_4044f0_1 *v21;  // eax
    int v22;  // edx
    unsigned int v23;  // eax
    void* node;  // ecx
    void* index;  // ecx
    unsigned int idx1;  // eax
    void* v8;  // eax
    void* v9;  // eax
    void* v10;  // eax
    unsigned int v11;  // esi
    unsigned int v12;  // edi
    unsigned int v13;  // eax
    void* v14;  // edi
    st_4044f0_0 *v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    st_4044f0_0 *v4;  // [bp+0x4]

    idx = v4;
    if (!idx->field_3604)
    {
        g_4d4f38 = 0;
        node = v6;
        do
        {
            v9 = v8;
            v10 = v9 + 1;
            v8 = v10;
        } while (*((char *)v9));
        v3 = v11;
        v2 = v12;
        v13 = v10 - node;
        v14 = &g_4d4f37;
        do
        {
            v15 = v14;
            iter = v15 + 1;
            v14 = iter;
        } while ((char)v15[1]);
        for (v17 = v13 >> 2; v17; node += 4)
        {
            v17 -= 1;
            *((int *)iter) = *((int *)node);
            iter += 4;
        }
        v1 = 0;
        v18 = v13 & 3;
        for (v0 = &idx->field_3608; v18; node += 1)
        {
            v18 -= 1;
            *((char *)iter) = *((char *)node);
            iter += 1;
        }
        v19 = sub_463c10();
        idx->field_3604 = v19;
        if (!v19)
            return;
    }
    v21 = sub_46d04a(idx->field_3608, v20);
    v22 = idx->field_3604;
    idx->field_10 = v21;
    sub_47a330(v21, v22, idx->field_3608);
    v23 = sub_45fe60();
    idx->field_1c4 = v23;
    if (!v23)
    {
        sub_464220(&g_49f588);
        return;
    }
    index = idx->field_10;
    idx->field_14 = index + 144;
    idx->field_18 = (int)index[4] + index;
    idx->field_1c = (int)index[8] + index;
    idx1 = 0;
    if (0 < *((short *)index))
    {
        do
        {
            *((void* *)(idx->field_14 + idx1 * 4)) = *((int *)(idx->field_14 + idx1 * 4)) + idx->field_10;
            idx1 += 1;
        } while (idx1 < *((short *)idx->field_10));
    }
    idx->field_1c8 = sub_46d04a((short)idx->field_10[2] * 1204);
    return;
}
