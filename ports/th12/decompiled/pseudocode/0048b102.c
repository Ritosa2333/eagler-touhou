// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b102; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4aaf78;

int sub_48b102(void)
{
    void* v1;  // ebp

    sub_46fcec(&g_4aaf78, 16);
    if (!(int)v1[8] || !~(sub_48e41a((int)v1[8], 0x7fff) < 0x7fff))
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        sub_46ec9a(7);
        *((unsigned int *)((char *)v1 - 4)) = 0;
        *((unsigned int *)((char *)v1 - 28)) = sub_48af42((int)v1[8]);
        *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
        sub_48b17c();
    }
    return sub_46fd31();
}
