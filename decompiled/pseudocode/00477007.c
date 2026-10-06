// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477007; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4aadd8;

int sub_477007(unsigned int a0)
{
    void* v0;  // ebp

    sub_46fcec(&g_4aadd8, 12);
    sub_46ec9a(6);
    *((unsigned int *)((char *)v0 - 4)) = 0;
    *((unsigned int *)((char *)v0 - 28)) = sub_476da8();
    *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
    sub_47703f();
    return sub_46fd31();
}
