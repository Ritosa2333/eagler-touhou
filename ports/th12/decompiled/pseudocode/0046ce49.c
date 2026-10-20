// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ce49; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aaa18;

void sub_46ce49(void)
{
    void* idx;  // ebp

    sub_46fcec(&g_4aaa18, 16);
    *((unsigned int *)((char *)idx - 32)) = 0;
    *((unsigned int *)((char *)idx - 4)) = 0;
    for (*((int *)((char *)idx - 28)) = 0; *((int *)((char *)idx - 28)) < (int)idx[16]; *((unsigned int *)((char *)idx - 28)) = *((int *)((char *)idx - 28)) + 1)
    {
        (int)idx[20]();
        *((int *)&idx[8]) = (int)idx[8] + (int)idx[12];
    }
    *((unsigned int *)((char *)idx - 32)) = 1;
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_46ce96();
    sub_46fd31();
    return;
}
