// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46cf81; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aaa78;
extern char g_4d6448;

int sub_46cf81(unsigned int a0)
{
    void* v0;  // ebp
    int v1;  // esi

    sub_46fcec(&g_4aaa78, 12);
    *((unsigned int *)((char *)v0 - 28)) = 0;
    v1 = (int)v0[8];
    if (v1 <= *((int *)&g_4d6448))
    {
        sub_46ec9a(4);
        *((unsigned int *)((char *)v0 - 4)) = 0;
        *((unsigned int *)((char *)v0 - 28)) = sub_46fa07(v1);
        *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
        sub_46cfc7();
    }
    return sub_46fd31();
}
