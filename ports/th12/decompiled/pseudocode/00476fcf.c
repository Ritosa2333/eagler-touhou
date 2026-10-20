// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x476fcf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aadb8;

void sub_476fcf(unsigned int a0)
{
    void* v1;  // ebp
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aadb8, 8);
    sub_46ec9a(6);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    sub_47686b();
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    sub_476ffe();
    sub_46fd31(&g_4aadb8, 8, v0, a0);
    return;
}
