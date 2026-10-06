// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d050; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b451c;

int sub_43d050(void)
{
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    unsigned short v7;  // fs
    unsigned long long v8;  // 4132
    unsigned int v9;  // eax
    unsigned long long v10;  // 4150
    int v11;  // ecx
    unsigned long long v13;  // 4118
    char v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x4]

    v8 = _ccall(v5, v6, v7, 0);
    v9 = *((int *)(unsigned int)v8);
    v2 = *((int *)(unsigned int)v8);
    v10 = _ccall(v5, v6, v7, 0);
    *((unsigned int **)(unsigned int)v10) = &v2;
    v1 = sub_46c9ea(126460, g_4ad138 ^ &v0, v0, v11, v9, sub_49703b, -0x1);
    v3 = 0;
    g_4b451c = (!sub_46c9ea(126460, g_4ad138 ^ &v0, v0, v11, v9, sub_49703b, -0x1) ? NULL : sub_43cf90());
    v13 = _ccall(v5, v6, v7, 0);
    *((unsigned int *)(unsigned int)v13) = v9;
    return g_4b451c;
}
