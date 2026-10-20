// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43b600; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

int sub_43b600(unsigned int a0)
{
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    unsigned long long v16;  // 4117
    unsigned short v8;  // fs
    unsigned long long v9;  // 4132
    unsigned int v10;  // eax
    unsigned long long v11;  // 4150
    int v12;  // esi
    int v13;  // ecx
    int v14;  // esi
    unsigned long long v15;  // 4119
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x18]
    int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x4]

    v9 = _ccall(v6, v7, v8, 0);
    v10 = *((int *)(unsigned int)v9);
    v4 = *((int *)(unsigned int)v9);
    v11 = _ccall(v6, v7, v8, 0);
    *((unsigned int **)(unsigned int)v11) = &v4;
    v14 = sub_46c9ea(736, g_4ad138 ^ &v12, v12, v13, v10, sub_49763b, -0x1);
    v3 = v14;
    v5 = 0;
    if (v14)
    {
        v1 = 8;
        v0 = 36;
        sub_46ce49(v14 + 168, 36, 8, sub_43cd40, sub_43cd80);
        sub_477420(v14, 0, 736);
    }
    else
    {
        v14 = 0;
    }
    v2 = 0xffffffff;
    if (!sub_43ae80(v14))
    {
        v16 = _ccall(v6, v7, v8, 0);
        *((unsigned int *)(unsigned int)v16) = v1;
        return v14;
    }
    if (v14)
    {
        sub_43b450(v14);
        sub_46ca4f(v14);
    }
    v15 = _ccall(v6, v7, v8, 0);
    *((unsigned int *)(unsigned int)v15) = v0;
    return 0;
}
