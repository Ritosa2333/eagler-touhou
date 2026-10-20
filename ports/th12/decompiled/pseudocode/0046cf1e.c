// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46cf1e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aaa58;

void sub_46cf1e(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    void* idx;  // ebp
    unsigned int v1;  // esi
    int v2;  // 4096

    sub_46fcec(&g_4aaa58, 12);
    *((unsigned int *)((char *)idx - 28)) = 0;
    v1 = (int)idx[12];
    *((unsigned int *)&idx[8]) = (int)idx[8] + (int)idx[16] * v1;
    *((unsigned int *)((char *)idx - 4)) = 0;
    while (1)
    {
        v2 = (int)idx[16];
        *((unsigned int *)&idx[16]) = (int)idx[16] - 1;
        if (v2 - 1 < 0)
            break;
        *((unsigned int *)&idx[8]) = (int)idx[8] - v1;
        (int)idx[20]();
    }
    *((unsigned int *)((char *)idx - 28)) = 1;
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_46cf69();
    sub_46fd31();
    return;
}
