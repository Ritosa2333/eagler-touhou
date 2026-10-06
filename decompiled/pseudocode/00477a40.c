// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477a40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477a40_0 {
    char padding_0[36];
    unsigned int field_24;
} st_477a40_0;

extern char g_400000;
extern unsigned int g_4aae18;
extern unsigned int g_4ad138;

unsigned int sub_477a40(int a0)
{
    unsigned long v8;  // ldt
    unsigned long v9;  // gdt
    unsigned short v10;  // fs
    unsigned long long v11;  // 4153
    unsigned long long v12;  // 4175
    int v13;  // edi
    st_477a40_0 *v14;  // eax
    unsigned int v15;  // eax
    unsigned long long v16;  // 4150
    unsigned long long v17;  // 4125
    unsigned int v0;  // [bp-0x2c]
    int v1;  // [bp-0x24]
    int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    char v7;  // [bp-0x4]

    v6 = 0xfffffffe;
    v5 = &g_4aae18;
    v4 = sub_46fd80;
    v11 = _ccall(v8, v9, v10, 0);
    v3 = *((int *)(unsigned int)v11);
    v5 ^= g_4ad138;
    v0 = g_4ad138 ^ &v7;
    v12 = _ccall(v8, v9, v10, 0);
    *((unsigned int **)(unsigned int)v12) = &v3;
    v6 = 0;
    if (sub_4779b0(&g_400000, g_4ad138 ^ &v7, v13, v1, v2, &v0))
    {
        v14 = sub_4779f0(&g_400000, a0 - &g_400000);
        if (v14)
        {
            v15 = v14->field_24;
            v16 = _ccall(v8, v9, v10, 0);
            *((unsigned int *)(unsigned int)v16) = v3;
            return ~(v15 >> 31) & 1;
        }
    }
    v17 = _ccall(v8, v9, v10, 0);
    *((unsigned int *)(unsigned int)v17) = v3;
    return 0;
}
