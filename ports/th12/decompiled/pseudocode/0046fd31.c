// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46fd31; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_46fd31(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned long v0;  // ldt
    unsigned long v1;  // gdt
    unsigned short v2;  // fs
    unsigned long long v3;  // 4123
    void* v4;  // ebp

    v3 = _ccall(v0, v1, v2, 0);
    *((int *)(unsigned int)v3) = *((int *)((char *)v4 - 16));
    return;
}
