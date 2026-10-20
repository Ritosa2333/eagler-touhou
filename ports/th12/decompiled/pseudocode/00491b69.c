// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491b69; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_491c14;
extern unsigned int g_4ad138;

unsigned int sub_491b69(unsigned int *a0, unsigned int a1, int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6)
{
    unsigned long v15;  // ldt
    unsigned long v16;  // gdt
    unsigned short v17;  // fs
    unsigned long long v18;  // 4195
    unsigned long long v19;  // 4207
    unsigned long long v20;  // 4111
    unsigned long long v21;  // 4123
    unsigned long long v22;  // 4107
    char v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c]
    unsigned int *v2;  // [bp-0x38]
    unsigned int *v3;  // [bp-0x30]
    unsigned int *v4;  // [bp-0x2c], Other Possible Types: unsigned int
    unsigned int v5;  // [bp-0x28]
    unsigned int v6;  // [bp-0x24]
    unsigned int v7;  // [bp-0x20]
    unsigned int v8;  // [bp-0x1c]
    unsigned int v9;  // [bp-0x18]
    unsigned int v10;  // [bp-0x14]
    char *v11;  // [bp-0x10], Other Possible Types: unsigned int
    char *v12;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v13;  // [bp-0x8]
    char v14;  // [bp-0x4]

    if (a0 == 0x123)
    {
        *((char **)a1) = &g_491c14;
        return 1;
    }
    v4 = 0;
    v5 = sub_491c40;
    v6 = g_4ad138 ^ &v4;
    v7 = a4;
    v8 = a1;
    v9 = a5;
    v10 = a6;
    v11 = 0;
    v12 = 0;
    v13 = 0;
    v11 = &v0;
    v12 = &v14;
    v18 = _ccall(v15, v16, v17, 0);
    v4 = *((int *)(unsigned int)v18);
    v19 = _ccall(v15, v16, v17, 0);
    *((unsigned int ***)(unsigned int)v19) = &v4;
    v2 = a0;
    v3 = *((int *)(sub_475467(v0, 1, a0, a2) + 128));
    v3(*(a0), &v2);
    v1 = 0;
    if (v13)
    {
        v20 = _ccall(v15, v16, v17, 0);
        *(v4) = *((int *)*((int *)(unsigned int)v20));
        v21 = _ccall(v15, v16, v17, 0);
        *((unsigned int **)(unsigned int)v21) = v4;
    }
    else
    {
        v22 = _ccall(v15, v16, v17, 0);
        *((unsigned int **)(unsigned int)v22) = v4;
    }
    return 0;
}
