// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c310; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bda0_1 {
    unsigned int field_0;
} st_44bda0_1;

typedef struct st_44bda0_0 {
    char padding_0[4];
    struct st_44bda0_1 *field_4;
} st_44bda0_0;

typedef struct st_44bda0_2 {
    struct st_44bda0_0 *field_0;
    HANDLE field_4;
    unsigned int field_8;
} st_44bda0_2;

extern st_44bda0_2 g_4a22dc;
extern unsigned int g_4ad138;

int sub_44c310(unsigned int a0, unsigned int a1)
{
    unsigned long v11;  // ldt
    unsigned long v12;  // gdt
    HANDLE v21;  // eax
    unsigned long long v22;  // 4123
    unsigned short v13;  // fs
    unsigned long long v14;  // 4137
    unsigned long long v15;  // 4159
    unsigned int size;  // edi
    unsigned int v18;  // eax
    unsigned long long v19;  // 4122
    unsigned int v20;  // edx
    char v0;  // [bp-0x28]
    int v1;  // [bp-0x24]
    int v2;  // [bp-0x20]
    int v3;  // [bp-0x1c]
    st_44bda0_2 v4;  // [bp-0x18]
    HANDLE v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]
    unsigned int v9;  // [bp-0x4]

    v9 = 0xffffffff;
    v8 = sub_496fa8;
    v14 = _ccall(v11, v12, v13, 0);
    v7 = *((int *)(unsigned int)v14);
    v15 = _ccall(v11, v12, v13, 0);
    *((unsigned int **)(unsigned int)v15) = &v7;
    size = 0;
    if (sub_44c130(g_4ad138 ^ &v0, v0, v1, v2, v3))
    {
        v18 = sub_44b8e0();
        v19 = _ccall(v11, v12, v13, 0);
        *((unsigned int *)(unsigned int)v19) = v7;
        return v18;
    }
    v4 = (st_44bda0_2)&g_4a22dc.field_0;
    v5 = 0xffffffff;
    v6 = 0;
    v9 = 0;
    sub_44bda0(&v4, v20, a0, "r");
    v21 = v5;
    if (v21 != 0xffffffff)
    {
        size = GetFileSize(v21, NULL);
        v21 = v5;
    }
    v4 = (st_44bda0_2)&g_4a22dc.field_0;
    if (v21 != 0xffffffff)
        CloseHandle(v21);
    v22 = _ccall(v11, v12, v13, 0);
    *((unsigned int *)(unsigned int)v22) = v7;
    return size;
}
