// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ef10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44ef10_0 {
    char padding_0[4];
    struct st_44ef10_0 *field_4;
    char padding_8[12];
    int field_14;
    unsigned int field_18;
} st_44ef10_0;

extern unsigned int g_4ad138;

st_44ef10_0 * sub_44ef10(st_44ef10_0 *idx, unsigned int a1, unsigned int a2, int a3)
{
    unsigned long v10;  // ldt
    unsigned long v11;  // gdt
    unsigned int v20;  // ecx
    st_44ef10_0 *v21;  // eax
    unsigned long long v22;  // 4124
    unsigned short v12;  // fs
    unsigned long long v13;  // 4191
    unsigned int v14;  // ebx
    unsigned int v15;  // esi
    unsigned int v16;  // edi
    unsigned long long v17;  // 4195
    unsigned int v18;  // esi
    unsigned int v19;  // ebx
    unsigned int v0;  // [bp-0x2c]
    unsigned int v1;  // [bp-0x28]
    unsigned int v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x20]
    st_44ef10_0 *v4;  // [bp-0x18]
    char *v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]
    char v9;  // [bp-0x4]

    v8 = 0xffffffff;
    v7 = sub_496eb0;
    v13 = _ccall(v10, v11, v12, 0);
    v6 = *((int *)(unsigned int)v13);
    v3 = v14;
    v2 = v15;
    v1 = v16;
    v0 = g_4ad138 ^ &v9;
    v17 = _ccall(v10, v11, v12, 0);
    *((unsigned int **)(unsigned int)v17) = &v6;
    v5 = &v0;
    v4 = idx;
    v18 = a2 | 15;
    if (v18 > 0xfffffffe)
    {
        v18 = a2;
    }
    else
    {
        v19 = idx->field_18;
        v20 = v19 >> 1;
        if (v18 / 3 < v20 && v19 <= 0xfffffffe - v20)
            v18 = v20 + v19;
    }
    v8 = 0;
    a2 = sub_44f0a0(v18 + 1);
    v8 = 0xffffffff;
    if (a3)
        sub_46e210(a2, v18 + 1, (idx->field_18 < 16 ? (struct st_44ef10_0 *)&idx->field_4 : idx->field_4), a3);
    if (idx->field_18 >= 16)
        sub_46ca4f(idx->field_4);
    v21 = &idx->field_4;
    v21->padding_0[0] = 0;
    *((st_44ef10_0 **)&v21->padding_0[0]) = a2;
    idx->field_18 = v18;
    idx->field_14 = a3;
    if (v18 >= 16)
        v21 = a2;
    v21->padding_0[a3] = 0;
    v22 = _ccall(v10, v11, v12, 0);
    *((unsigned int *)(unsigned int)v22) = v6;
    return v21;
}
