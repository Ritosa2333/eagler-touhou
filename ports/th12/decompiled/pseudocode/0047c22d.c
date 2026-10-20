// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c22d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aaeb8;

int sub_47c22d(void)
{
    void* v2;  // ebp
    int v4;  // eax
    unsigned int v5;  // eax
    unsigned int v0;  // [bp-0xc]

    sub_46fcec(&g_4aaeb8, 12);
    if (!(int)v2[8])
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        v0 = 32;
        sub_47c13d(1, sub_47c025() + 32);
        *((unsigned int *)((char *)v2 - 4)) = 0;
        v4 = sub_48d319(sub_47c025() + 32);
        v5 = sub_47c025();
        *((int *)((char *)v2 - 28)) = sub_47021f(v5 + 32, (int)v2[8], 0, v2 + 12);
        sub_48d3b5(v4, sub_47c025() + 32);
        *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
        sub_47c2c9();
    }
    return sub_46fd31();
}
