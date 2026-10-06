// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491687; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab250;

void sub_491687(unsigned int a0, unsigned int a1)
{
    void* v1;  // ebp
    unsigned int v2;  // edx

    sub_491e59(0x44);
    sub_4916bf("string too long");
    *((unsigned int *)((char *)v1 - 4)) = 0;
    sub_4915e9(v1 - 80, v2, v1 - 40);
    sub_46ff8f(v1 - 80, &g_4ab250);
    __debugbreak();
}
