// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425b10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b44f0;

unsigned int * sub_425b10(void)
{
    unsigned long v7;  // ldt
    unsigned long v8;  // gdt
    unsigned long long v17;  // 4117
    unsigned short v9;  // fs
    unsigned long long v10;  // 4132
    unsigned int v11;  // eax
    unsigned long long v12;  // 4150
    int v13;  // esi
    int v14;  // ecx
    unsigned int *v15;  // esi
    unsigned long long v16;  // 4119
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x18]
    unsigned int *v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x4]

    v10 = _ccall(v7, v8, v9, 0);
    v11 = *((int *)(unsigned int)v10);
    v4 = *((int *)(unsigned int)v10);
    v12 = _ccall(v7, v8, v9, 0);
    *((unsigned int **)(unsigned int)v12) = &v4;
    v15 = sub_46c9ea(6713316, g_4ad138 ^ &v13, v13, v14, v11, sub_4977ab, -0x1);
    v3 = v15;
    v5 = 0;
    if (v15)
    {
        v1 = 2664;
        v0 = 2520;
        sub_46ce49(v15 + 5, 2520, 2664, sub_4258d0, sub_425900);
        sub_477420(v15, 0, 6713316);
        *(v15) = *(v15) | 2;
        g_4b44f0 = v15;
    }
    else
    {
        v15 = NULL;
    }
    v2 = 0xffffffff;
    if (!sub_425940(v15))
    {
        v17 = _ccall(v7, v8, v9, 0);
        *((unsigned int *)(unsigned int)v17) = v1;
        return v15;
    }
    if (v15)
    {
        sub_425a00(v15);
        sub_46ca4f(v15);
    }
    v16 = _ccall(v7, v8, v9, 0);
    *((unsigned int *)(unsigned int)v16) = v0;
    return NULL;
}
