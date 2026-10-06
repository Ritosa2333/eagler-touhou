// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f244; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_44f244(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v0;  // ebx
    unsigned long v1;  // ldt
    unsigned long v2;  // gdt
    unsigned short v3;  // fs
    unsigned long long v4;  // 4145

    sub_462740(v0);
    sub_462740(v0);
    memset(v0 + 44, 0, 12);
    memset(v0 + 8, 0, 12);
    v4 = _ccall(v1, v2, v3, 0);
    *((unsigned int *)(unsigned int)v4) = a0;
    return 0;
}
