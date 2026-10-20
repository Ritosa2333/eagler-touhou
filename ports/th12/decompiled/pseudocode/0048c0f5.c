// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c0f5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ab038;

int sub_48c0f5(void)
{
    void* v1;  // ebp

    sub_46fcec(&g_4ab038, 12);
    if (!(int)v1[12])
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        sub_47c0fc((int)v1[12]);
        *((unsigned int *)((char *)v1 - 4)) = 0;
        *((unsigned int *)((char *)v1 - 28)) = sub_48c004((int)v1[8], (int)v1[12]);
        *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
        sub_48c15d();
    }
    return sub_46fd31();
}
