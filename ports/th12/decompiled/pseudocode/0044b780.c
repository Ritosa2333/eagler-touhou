// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b780; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44b780(void)
{
    void* v1;  // eax
    int v2;  // esi

    sub_46cf1e(v1, 16, *((int *)((char *)v1 - 4)), sub_44bc50, v2);
    sub_46ca4f(v1 - 4);
    return;
}
