// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4919da; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4919da(unsigned int a0, unsigned int a1)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // ebx
    unsigned long v4;  // ldt
    unsigned long v5;  // gdt
    unsigned short v6;  // fs
    unsigned long long v7;  // 4139
    unsigned long long v8;  // 4147
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    v0 = v3;
    v1 = a1 + 12;
    v7 = _ccall(v4, v5, v6, 0);
    v8 = _ccall(v4, v5, v6, 0);
    *((int *)(unsigned int)v8) = *((int *)*((int *)(unsigned int)v7));
    goto *((void *)(a0));
}
