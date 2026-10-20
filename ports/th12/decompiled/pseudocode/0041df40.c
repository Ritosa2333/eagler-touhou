// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41df40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

int sub_41df40(void)
{
    unsigned long v4;  // ldt
    unsigned long v5;  // gdt
    unsigned long long v14;  // 4119
    unsigned long long v15;  // 4117
    unsigned short v6;  // fs
    unsigned long long v7;  // 4132
    unsigned int v8;  // eax
    unsigned long long v9;  // 4150
    int v10;  // esi
    int v11;  // ecx
    unsigned int v13;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v7 = _ccall(v4, v5, v6, 0);
    v8 = *((int *)(unsigned int)v7);
    v1 = *((int *)(unsigned int)v7);
    v9 = _ccall(v4, v5, v6, 0);
    *((unsigned int **)(unsigned int)v9) = &v1;
    v0 = sub_46c9ea(27984, g_4ad138 ^ &v10, v10, v11, v8, sub_4977db, -0x1);
    v2 = 0;
    v13 = (!sub_46c9ea(27984, g_4ad138 ^ &v10, v10, v11, v8, sub_4977db, -0x1) ? 0 : sub_41d150(sub_46c9ea(27984, g_4ad138 ^ &v10, v10, v11, v8, sub_4977db, -0x1)));
    v2 = 0xffffffff;
    if (!sub_41d250(v13))
    {
        v15 = _ccall(v4, v5, v6, 0);
        *((unsigned int *)(unsigned int)v15) = v8;
        return v13;
    }
    if (v13)
    {
        sub_41dc50(v13);
        sub_46ca4f(v13);
    }
    v14 = _ccall(v4, v5, v6, 0);
    *((unsigned int *)(unsigned int)v14) = sub_46c9ea(27984, g_4ad138 ^ &v10, v10, v11, v8, sub_4977db, -0x1);
    return 0;
}
