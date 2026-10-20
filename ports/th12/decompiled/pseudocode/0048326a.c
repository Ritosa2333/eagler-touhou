// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48326a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_48326a(void* a0)
{
    void* v0;  // eax

    if (!a0)
        return;
    v0 = a0 - 8;
    if (*((int *)v0) != 0xdddd)
        return;
    sub_46c941(v0);
    return;
}
