// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43b6f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43b6f0_0 {
    char padding_0[16];
    unsigned int field_10;
} st_43b6f0_0;

extern unsigned int g_4ad138;

st_43b6f0_0 * sub_43b6f0(unsigned int a0)
{
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    unsigned long long v15;  // 4122
    unsigned long long v16;  // 4117
    unsigned short v7;  // fs
    unsigned long long v8;  // 4132
    unsigned int v9;  // eax
    unsigned long long v10;  // 4150
    int v11;  // esi
    int v12;  // ecx
    st_43b6f0_0 *v13;  // esi
    st_43b6f0_0 *v14;  // esi
    int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x18]
    st_43b6f0_0 *v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x4]

    v8 = _ccall(v5, v6, v7, 0);
    v9 = *((int *)(unsigned int)v8);
    v3 = *((int *)(unsigned int)v8);
    v10 = _ccall(v5, v6, v7, 0);
    *((unsigned int **)(unsigned int)v10) = &v3;
    v13 = sub_46c9ea(736, g_4ad138 ^ &v11, v11, v12, v9, sub_49760b, -0x1);
    v2 = v13;
    v4 = 0;
    if (v13)
    {
        v0 = sub_43cd40;
        sub_46ce49(&v13[8].padding_0[8], 36, 8, sub_43cd40, sub_43cd80);
        sub_477420(v13, 0, 736);
        v14 = v13;
    }
    else
    {
        v14 = NULL;
    }
    v1 = 0xffffffff;
    v14->field_10 = 2;
    if (!sub_43c350(v14, v13))
    {
        v16 = _ccall(v5, v6, v7, 0);
        *((int *)(unsigned int)v16) = v0;
        return v14;
    }
    sub_43b450(v14);
    sub_46ca4f(v14);
    v15 = _ccall(v5, v6, v7, 0);
    *((int *)(unsigned int)v15) = v0;
    return NULL;
}
