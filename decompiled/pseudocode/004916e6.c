// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4916e6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab2d4;

void sub_4916e6(void)
{
    void* v1;  // ebp

    sub_491e59(0x44);
    sub_4916bf("invalid string position");
    *((unsigned int *)((char *)v1 - 4)) = 0;
    sub_491638(v1 - 40);
    sub_46ff8f(v1 - 80, &g_4ab2d4);
    __debugbreak();
}
