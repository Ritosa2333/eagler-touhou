// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c9ea; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab5d0;
extern int g_4b38c0;
extern void g_4b38cc;

unsigned int sub_46c9ea(int a0)
{
    unsigned int v1;  // eax
    char v0;  // [bp-0x10]

    do
    {
        v1 = sub_46d04a(a0);
        if (v1)
            return v1;
    } while (sub_46ff67(a0));
    if (!(g_4b38cc & 1))
    {
        *((unsigned int *)&g_4b38cc) = *((int *)&g_4b38cc) | 1;
        sub_46c9cf();
        sub_46da32(sub_497ac1);
    }
    sub_44f180(&g_4b38c0);
    sub_46ff8f(&v0, &g_4ab5d0);
    __debugbreak();
}
