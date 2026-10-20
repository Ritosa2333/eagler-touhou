// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c8ed; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

void sub_48c8ed(unsigned int a0, unsigned int a1)
{
    unsigned int v9;  // ebx
    unsigned int v10;  // esi
    unsigned int v19;  // esi
    unsigned int index;  // esi
    unsigned long long v21;  // 4118
    unsigned int v11;  // edi
    unsigned long v12;  // ldt
    unsigned long v13;  // gdt
    unsigned short v14;  // fs
    unsigned long long v15;  // 4149
    unsigned long long v16;  // 4162
    unsigned int *idx;  // eax
    unsigned int v18;  // ebx
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x18]
    unsigned int v4;  // [bp-0x14]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]
    unsigned int *v8;  // [bp+0x0]

    v7 = v9;
    v6 = v10;
    v5 = v11;
    v4 = a0;
    v3 = 0xfffffffe;
    v2 = sub_48c8a8;
    v15 = _ccall(v12, v13, v14, 0);
    v1 = *((int *)(unsigned int)v15);
    v0 = g_4ad138 ^ &v1;
    v16 = _ccall(v12, v13, v14, 0);
    *((unsigned int **)(unsigned int)v16) = &v1;
    while (1)
    {
        idx = v8;
        v18 = idx[2];
        v19 = idx[3];
        if (v19 == 0xffffffff || a0 != 0xffffffff && v19 <= a0)
            break;
        index = v19 * 3;
        v2 = *((int *)(v18 + index * 4));
        idx[3] = *((int *)(v18 + index * 4));
        if (!*((int *)(v18 + index * 4 + 4)))
        {
            sub_48c99d(0x101);
            sub_48c9bc(0x101);
        }
    }
    v21 = _ccall(v12, v13, v14, 0);
    *((unsigned int *)(unsigned int)v21) = v0;
    return;
}
