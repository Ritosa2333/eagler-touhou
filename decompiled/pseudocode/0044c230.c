// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c230; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bfb0_0 {
    char padding_0[8];
    struct st_44bfb0_1 *field_8;
    char padding_c[4];
    struct st_44bfb0_1 *field_10;
    struct st_44bfb0_1 *field_14;
    struct st_44bfb0_1 *field_18;
} st_44bfb0_0;

typedef struct st_44bfb0_1 {
    unsigned int field_0;
} st_44bfb0_1;

typedef struct st_44bfb0_5 {
    struct st_44bfb0_0 *field_0;
    char padding_4[4];
    unsigned int field_8;
} st_44bfb0_5;

extern unsigned int g_4a22dc;
extern unsigned int g_4ad138;

int sub_44c230(unsigned int a0)
{
    unsigned long v9;  // ldt
    unsigned long v10;  // gdt
    unsigned int v19;  // eax
    unsigned long long v20;  // 4118
    HANDLE v21;  // esi
    HANDLE v22;  // eax
    unsigned int size;  // eax
    unsigned int v24;  // edx
    unsigned int v25;  // eax
    HANDLE v26;  // eax
    unsigned long long v27;  // 4119
    unsigned short v11;  // fs
    unsigned long long v12;  // 4133
    st_44bfb0_5 v13;  // edi
    unsigned long long v14;  // 4151
    unsigned int v15;  // edi
    unsigned int v16;  // eax
    unsigned int v17;  // ecx
    unsigned int v18;  // edx
    unsigned int v0;  // [bp-0x24]
    st_44bfb0_5 v1;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]

    v7 = 0xffffffff;
    v6 = sub_496fd8;
    v12 = _ccall(v9, v10, v11, 0);
    v5 = *((int *)(unsigned int)v12);
    v1 = v13;
    v0 = g_4ad138 ^ &v1;
    v14 = _ccall(v9, v10, v11, 0);
    *((unsigned int **)(unsigned int)v14) = &v5;
    v15 = sub_44c130(a0);
    if (v15)
    {
        v16 = sub_46d1a0(a0, 47);
        v17 = v16 + 1;
        if (!v16)
            v17 = a0;
        v19 = sub_44b7b0(v17, v18, v15, NULL);
        v20 = _ccall(v9, v10, v11, 0);
        *((unsigned int *)(unsigned int)v20) = v5;
        return v19;
    }
    else
    {
        v2 = &g_4a22dc;
        v3 = 0xffffffff;
        v4 = 0;
        v7 = 0;
        sub_44bda0(a0, "r");
        v22 = v21;
        if (v22 == 0xffffffff)
            size = 0;
        else
            size = GetFileSize(v22, NULL);
        v25 = sub_44bfb0(&v1, v24, size);
        v26 = v21;
        v1 = &g_4a22dc;
        if (v26 != 0xffffffff)
            CloseHandle(v26);
        v27 = _ccall(v9, v10, v11, 0);
        *((unsigned int *)(unsigned int)v27) = v3;
        return v25;
    }
}
