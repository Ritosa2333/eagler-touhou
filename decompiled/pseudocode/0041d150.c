// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41d150; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b43e4;

unsigned int * sub_41d150(unsigned int *idx)
{
    unsigned long v4;  // ldt
    unsigned long v5;  // gdt
    unsigned short v6;  // fs
    unsigned long long v7;  // 4140
    unsigned int v8;  // eax
    unsigned long long v9;  // 4156
    int v10;  // esi
    unsigned long long v11;  // 4126
    char v0;  // [bp-0x40]
    char v1;  // [bp-0x2c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0xc]

    v7 = _ccall(v4, v5, v6, 0);
    v8 = *((int *)(unsigned int)v7);
    v3 = *((int *)(unsigned int)v7);
    v9 = _ccall(v4, v5, v6, 0);
    *((unsigned int **)(unsigned int)v9) = &v3;
    sub_46ce49(idx + 4, 1204, 8, sub_4027e0, sub_4026e0, g_4ad138 ^ &v10, v10, v8, sub_49716f, -0x1);
    v2 = 0;
    sub_46ce49(idx + 2412, 1204, 8, sub_4027e0, sub_4026e0);
    v1 = 1;
    sub_46ce49(idx + 4820, 1204, 2, sub_4027e0, sub_4026e0);
    v0 = 2;
    sub_46ce49(idx + 5422, 1204, 4, sub_4027e0, sub_4026e0);
    sub_4027e0(idx + 5422, 1204, 4, sub_4027e0, sub_4026e0);
    idx[6964] = idx[6964] & 0xfffffffe;
    idx[6987] = idx[6987] & 0xfffffffe;
    sub_477420(idx, 0, 27984);
    *(idx) = *(idx) | 2;
    g_4b43e4 = idx;
    v11 = _ccall(v4, v5, v6, 0);
    *((unsigned int *)(unsigned int)v11) = 4;
    return idx;
}
