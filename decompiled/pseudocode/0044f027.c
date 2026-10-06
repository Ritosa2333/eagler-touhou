// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f027; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_44f027(void)
{
    void* v1;  // ebp
    void* idx;  // esi

    idx = *((int *)((char *)v1 - 20));
    if ((int)idx[24] >= 16)
        sub_46ca4f((int)idx[4]);
    *((unsigned int *)&idx[24]) = 15;
    *((unsigned int *)&idx[20]) = 0;
    *((char *)&idx[4]) = 0;
    sub_46ff8f(0, 0);
    __debugbreak();
}
