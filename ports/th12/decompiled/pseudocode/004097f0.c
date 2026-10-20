// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4097f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b43c8;

int sub_4097f0(void)
{
    unsigned long v7;  // ldt
    unsigned long v8;  // gdt
    unsigned long long v17;  // 4121
    unsigned long long v18;  // 4119
    unsigned short v9;  // fs
    unsigned long long v10;  // 4134
    unsigned int v11;  // eax
    unsigned int v12;  // edi
    unsigned long long v13;  // 4154
    int v14;  // esi
    int v15;  // ecx
    int v16;  // esi
    int v0;  // [bp-0x24]
    int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x18]
    int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x4]

    v10 = _ccall(v7, v8, v9, 0);
    v11 = *((int *)(unsigned int)v10);
    v4 = *((int *)(unsigned int)v10);
    v2 = v12;
    v13 = _ccall(v7, v8, v9, 0);
    *((unsigned int **)(unsigned int)v13) = &v4;
    v1 = 5106656;
    v16 = sub_46c9ea(5106656, g_4ad138 ^ &v2, v12, v14, v15, v11, sub_49770b, -0x1);
    v3 = v16;
    v5 = 0;
    if (v16)
    {
        v1 = sub_4094f0;
        v0 = sub_4094b0;
        sub_46ce49(v16 + 100, 2552, 2001, sub_4094b0, sub_4094f0);
        sub_477420(v16, 0, 5106656);
        g_4b43c8 = v16;
    }
    else
    {
        v16 = 0;
    }
    v2 = 0xffffffff;
    if (!sub_409530())
    {
        v18 = _ccall(v7, v8, v9, 0);
        *((int *)(unsigned int)v18) = v1;
        return v16;
    }
    if (v16)
    {
        sub_4096d0(v16);
        sub_46ca4f(v16);
    }
    v17 = _ccall(v7, v8, v9, 0);
    *((int *)(unsigned int)v17) = v0;
    return 0;
}
