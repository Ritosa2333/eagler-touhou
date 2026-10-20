// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46fcec; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

void sub_46fcec(unsigned int a0, unsigned int a1)
{
    unsigned long v3;  // ldt
    unsigned long v4;  // gdt
    unsigned short v5;  // fs
    unsigned long long v6;  // 4146
    unsigned int v7;  // ebx
    unsigned int v8;  // esi
    unsigned int v9;  // edi
    unsigned long long v10;  // 4190
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp+0x0]
    char *v2;  // [bp+0x8]

    v6 = _ccall(v3, v4, v5, 0);
    v0 = *((int *)(unsigned int)v6);
    v2 = &v1;
    esp = (char *)&v0 - a1;
    esp = esp - 4;
    *((unsigned int *)(esp - 4)) = v7;
    esp = esp - 4;
    *((unsigned int *)(esp - 4)) = v8;
    esp = esp - 4;
    *((unsigned int *)(esp - 4)) = v9;
    *((unsigned long *)(esp - 4)) = g_4ad138 ^ &v2;
    *((unsigned int *)(esp - 8)) = v1;
    v10 = _ccall(v3, v4, v5, 0);
    *((unsigned int **)(unsigned int)v10) = &v0;
    return;
}
