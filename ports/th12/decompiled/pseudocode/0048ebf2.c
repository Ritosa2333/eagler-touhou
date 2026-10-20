// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ebf2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab1a8;

int sub_48ebf2(unsigned int a0)
{
    void* v0;  // ebp
    void* v1;  // esi

    sub_46fcec(&g_4ab1a8, 12);
    *((unsigned int *)((char *)v0 - 28)) = 0xffffffff;
    v1 = (int)v0[8];
    if (!v1)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else if ((char)v1[12] & 64)
    {
        *((unsigned int *)&v1[12]) = 0;
    }
    else
    {
        sub_47c0fc(v1);
        *((unsigned int *)((char *)v0 - 4)) = 0;
        *((unsigned int *)((char *)v0 - 28)) = sub_48eb7b(v1);
        *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
        sub_48ec66();
    }
    return sub_46fd31();
}
