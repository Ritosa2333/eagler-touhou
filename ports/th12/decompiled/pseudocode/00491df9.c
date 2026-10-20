// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491df9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

unsigned int sub_491df9(int a0, int a1, int a2, int a3, int a4)
{
    unsigned long v3;  // ldt
    unsigned long v4;  // gdt
    unsigned short v5;  // fs
    unsigned long long v6;  // 4211
    unsigned int v7;  // eax
    unsigned long long v8;  // 4215
    unsigned int v9;  // eax
    unsigned long long v10;  // 4112
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x1c]
    char v2;  // [bp+0x0]

    v0 = 0;
    v6 = _ccall(v3, v4, v5, 0);
    v7 = *((int *)(unsigned int)v6);
    v1 = *((int *)(unsigned int)v6);
    v8 = _ccall(v3, v4, v5, 0);
    *((unsigned int **)(unsigned int)v8) = &v1;
    v9 = sub_493150(a2, a0, a4, v7, sub_491b36, g_4ad138 ^ &v1, a1, a0, a3 + 1, &v2);
    v10 = _ccall(v3, v4, v5, 0);
    *((unsigned int *)(unsigned int)v10) = v7;
    return v9;
}
