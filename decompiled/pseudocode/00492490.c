// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492490; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab438;

void sub_492490(void)
{
    void* v1;  // ebp

    sub_46fcec(&g_4ab438, 8);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    (int)v1[8]((int)v1[12]);
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    sub_46fd31();
    return;
}
