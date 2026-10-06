// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

unsigned int * sub_402970(unsigned int *idx)
{
    unsigned long v2;  // ldt
    unsigned long v3;  // gdt
    unsigned short v4;  // fs
    unsigned long long v5;  // 4166
    unsigned int v6;  // eax
    unsigned long long v7;  // 4184
    int v8;  // edi
    int v9;  // esi
    unsigned long long v10;  // 4126
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0xc]

    v5 = _ccall(v2, v3, v4, 0);
    v6 = *((int *)(unsigned int)v5);
    v1 = *((int *)(unsigned int)v5);
    v7 = _ccall(v2, v3, v4, 0);
    *((unsigned int **)(unsigned int)v7) = &v1;
    idx[13] = idx[13] & 0xfffffffe;
    idx[18] = idx[18] & 0xfffffffe;
    idx[36] = idx[36] & 0xfffffffe;
    idx[55] = idx[55] & 0xfffffffe;
    idx[74] = idx[74] & 0xfffffffe;
    idx[109] = idx[109] & 0xfffffffe;
    sub_46ce49(idx + 115, 1204, 8, sub_4027e0, sub_4026e0, g_4ad138 ^ &v8, v8, v9, v6, sub_4971ab, -0x1);
    v0 = 0;
    sub_46ce49(idx + 2533, 1204, 3, sub_4027e0, sub_4026e0);
    idx[3444] = idx[3444] & 0xfffffffe;
    sub_477420(idx, 0, 14116);
    *(idx) = *(idx) | 2;
    v10 = _ccall(v2, v3, v4, 0);
    *((void* *)(unsigned int)v10) = sub_4027e0;
    return idx;
}
