// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x476f80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aad98;
extern unsigned int g_4b41cc;

void sub_476f80(void)
{
    void* v1;  // ebp

    sub_46fcec(&g_4aad98, 8);
    if (!g_4b41cc)
    {
        sub_46ec9a(6);
        *((unsigned int *)((char *)v1 - 4)) = 0;
        if (!g_4b41cc)
        {
            sub_47686b();
            g_4b41cc = g_4b41cc + 1;
        }
        *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
        sub_476fc6();
    }
    sub_46fd31();
    return;
}
