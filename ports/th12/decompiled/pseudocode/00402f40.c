// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402f40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

int sub_402f40(unsigned int a0)
{
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    int v14;  // eax
    unsigned long long v15;  // 4119
    unsigned long long v16;  // 4117
    unsigned short v7;  // fs
    unsigned long long v8;  // 4132
    unsigned int v9;  // eax
    unsigned long long v10;  // 4150
    int v11;  // esi
    int v12;  // ecx
    int v13;  // esi
    int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]
    int v4;  // [bp+0x0]

    v8 = _ccall(v5, v6, v7, 0);
    v9 = *((int *)(unsigned int)v8);
    v1 = *((int *)(unsigned int)v8);
    v10 = _ccall(v5, v6, v7, 0);
    *((unsigned int **)(unsigned int)v10) = &v1;
    v14 = 14116;
    v14 = sub_46c9ea(14116, g_4ad138 ^ &v11, v11, v12, v9, sub_49766b, -0x1);
    v0 = v14;
    v3 = 0;
    if (v14)
        v13 = sub_402970(v14);
    else
        v13 = 0;
    v2 = 0xffffffff;
    if (!sub_402a40(v13, v4))
    {
        v16 = _ccall(v5, v6, v7, 0);
        *((unsigned long *)(unsigned int)v16) = g_4ad138 ^ &v11;
        return v13;
    }
    if (v13)
    {
        sub_402cc0(v13);
        sub_46ca4f(v13);
    }
    v15 = _ccall(v5, v6, v7, 0);
    *((int *)(unsigned int)v15) = v14;
    return 0;
}
