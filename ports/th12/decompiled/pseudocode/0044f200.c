// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f200; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44f200_0 {
    char padding_0[72];
    unsigned int field_48;
} st_44f200_0;

extern unsigned int g_4ad138;

void sub_44f200(st_44f200_0 *a0)
{
    unsigned long v2;  // ldt
    unsigned long v3;  // gdt
    unsigned short v4;  // fs
    unsigned long long v5;  // 4132
    unsigned int v6;  // eax
    unsigned long long v7;  // 4150
    int v8;  // esi
    int v9;  // ebx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    v1 = 0xffffffff;
    v5 = _ccall(v2, v3, v4, 0);
    v6 = *((int *)(unsigned int)v5);
    v0 = *((int *)(unsigned int)v5);
    v7 = _ccall(v2, v3, v4, 0);
    *((unsigned int **)(unsigned int)v7) = &v0;
    sub_464c40(g_4ad138 ^ &v8, v8, v9, v6, sub_496e93, 1);
    a0->field_48 = 1;
    sub_4624c0(g_4ad138 ^ &v8, v8, v9, v6, sub_496e93, 1);
}
