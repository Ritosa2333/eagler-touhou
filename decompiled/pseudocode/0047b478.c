// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b478; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47b478_0 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
} st_47b478_0;

extern unsigned int g_4ad138;

void sub_47b478(st_47b478_0 *idx, unsigned int a1, unsigned int a2)
{
    unsigned int v11;  // ebx
    unsigned int v12;  // esi
    unsigned long long v21;  // 4117
    unsigned int v13;  // edi
    unsigned long v14;  // ldt
    unsigned long v15;  // gdt
    unsigned short v16;  // fs
    unsigned long long v17;  // 4163
    unsigned long long v18;  // 4175
    unsigned int v19;  // esi
    unsigned int *v20;  // ebx
    unsigned int v0;  // [bp-0x2c]
    unsigned int v1;  // [bp-0x28]
    unsigned int v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    st_47b478_0 *v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]
    unsigned int v9;  // [bp-0x4]
    unsigned int *v10;  // [bp+0x0]

    v9 = v11;
    v8 = v12;
    v7 = v13;
    v6 = idx;
    v5 = a1;
    v4 = a2;
    v3 = a2;
    v2 = sub_47b508;
    v17 = _ccall(v14, v15, v16, 0);
    v1 = *((int *)(unsigned int)v17);
    v3 = g_4ad138 ^ &v1;
    v18 = _ccall(v14, v15, v16, 0);
    *((unsigned int **)(unsigned int)v18) = &v1;
    while (1)
    {
        v19 = idx->field_c;
        if (v19 == 0xfffffffe || a1 != 0xfffffffe && v19 <= a1)
            break;
        v20 = (idx->field_8 ^ *(v10)) + v19 * 12 + 16;
        idx->field_c = *(v20);
        if (v20[1])
            continue;
        sub_48c99d(0x101);
        sub_48c9bc(0x101);
    }
    v21 = _ccall(v14, v15, v16, 0);
    *((unsigned int *)(unsigned int)v21) = v0;
    return;
}
