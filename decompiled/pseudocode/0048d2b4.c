// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d2b4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ab148;

void sub_48d2b4(unsigned int a0)
{
    void* v1;  // ebp
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4ab148, 12);
    if (!(int)v1[8])
    {
        sub_48d1da(0);
    }
    else
    {
        sub_47c0fc((int)v1[8]);
        *((unsigned int *)((char *)v1 - 4)) = 0;
        *((unsigned int *)((char *)v1 - 28)) = sub_48d192((int)v1[8]);
        *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
        sub_48d2fd();
    }
    sub_46fd31(&g_4ab148, 12, v0, a0);
    return;
}
