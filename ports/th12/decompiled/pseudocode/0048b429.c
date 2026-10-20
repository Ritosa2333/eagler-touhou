// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b429; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4aafd8;

int sub_48b429(void)
{
    void* v1;  // ebp

    sub_46fcec(&g_4aafd8, 12);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    *((unsigned int *)((char *)v1 - 28)) = 1;
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    return sub_46fd31();
}
