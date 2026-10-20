// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e44a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_49d57c;

void sub_46e44a(unsigned int a0)
{
    int v1;  // eax
    char v0;  // [bp+0x0]

    if (sub_477a40(&g_49d57c))
        sub_4759d5();
    v1 = sub_4753ee(&v0);
    if (v1)
        sub_4755b0(v1);
    ExitThread(a0);
    __debugbreak();
}
