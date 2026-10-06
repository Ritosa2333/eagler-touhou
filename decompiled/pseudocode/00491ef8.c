// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491ef8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

char * sub_491ef8(unsigned int a0)
{
    unsigned long v2;  // ldt
    unsigned long v3;  // gdt
    unsigned short v4;  // fs
    unsigned long long v5;  // 4141
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    unsigned long long v9;  // 4178
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp+0x0]

    v5 = _ccall(v2, v3, v4, 0);
    v0 = *((int *)(unsigned int)v5);
    esp = (char *)&v0 - a0;
    esp = esp - 4;
    *((unsigned int *)(esp - 4)) = v6;
    esp = esp - 4;
    *((unsigned int *)(esp - 4)) = v7;
    esp = esp - 4;
    *((unsigned int *)(esp - 4)) = v8;
    *((unsigned long *)(esp - 4)) = g_4ad138 ^ &ebp;
    *((unsigned int *)(esp - 8)) = v1;
    v9 = _ccall(v2, v3, v4, 0);
    *((unsigned int **)(unsigned int)v9) = &v0;
    return &v0;
}
