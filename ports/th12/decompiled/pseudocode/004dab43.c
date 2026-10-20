// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dab43; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4dab43(void)
{
    unsigned long v1;  // ldt
    unsigned long v2;  // gdt
    unsigned long long v3;  // 4102

    v3 = _ccall(v1, v2, 11327, 1519377799);
    goto *((void *)((unsigned int)v3));
}
