// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b416; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aae58;

int sub_47b416(unsigned int a0, unsigned int a1)
{
    void* v1;  // ebp
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aae58, 16);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    *((int *)((char *)v1 - 28)) = InitializeCriticalSectionAndSpinCount((int)v1[8], (int)v1[12]);
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    return (unsigned int)sub_46fd31(&g_4aae58, 16, v0, a0);
}
