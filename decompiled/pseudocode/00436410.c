// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x436410; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

int sub_436410(void)
{
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    unsigned short v8;  // fs
    unsigned long long v9;  // 4133
    unsigned long long v10;  // 4149
    int v11;  // edi
    int v13;  // edi
    unsigned long long v14;  // 4119
    unsigned long long v15;  // 4117
    unsigned int v0;  // [bp-0x14]
    int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = 0xffffffff;
    v3 = sub_49773b;
    v9 = _ccall(v6, v7, v8, 0);
    v2 = *((int *)(unsigned int)v9);
    v10 = _ccall(v6, v7, v8, 0);
    *((unsigned int **)(unsigned int)v10) = &v2;
    v1 = sub_46c9ea(50588, g_4ad138 ^ &v11, v11);
    v4 = 0;
    v13 = (!v1 ? 0 : sub_4359a0(v1));
    v3 = 0xffffffff;
    if (!sub_435ae0())
    {
        v15 = _ccall(v6, v7, v8, 0);
        *((int *)(unsigned int)v15) = v1;
        return v13;
    }
    if (v13)
    {
        sub_436270(v13);
        sub_46ca4f(v13);
    }
    v14 = _ccall(v6, v7, v8, 0);
    *((unsigned int *)(unsigned int)v14) = v0;
    return 0;
}
