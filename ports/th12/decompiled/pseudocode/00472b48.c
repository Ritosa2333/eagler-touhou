// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472b48; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aabe0;

void sub_472b48(void)
{
    unsigned int *v1;  // eax
    void* v2;  // ebp

    sub_46fcec(&g_4aabe0, 8);
    v1 = *((int *)(sub_475467(&g_4aabe0, 8) + 120));
    if (v1)
    {
        *((unsigned int *)((char *)v2 - 4)) = 0;
        v1();
        *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
    }
    sub_479ff3(); /* do not return */
}
