// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474181; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aac80;
extern unsigned int g_4ad9d4;

int sub_474181(unsigned int a0)
{
    unsigned int *v1;  // esi
    void* v2;  // ebp
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aac80, 12);
    v1 = sub_475467(&g_4aac80, 12);
    if (g_4ad9d4 & v1[28] && v1[27])
    {
        v1 = *((int *)(sub_475467() + 108));
    }
    else
    {
        sub_46ec9a(12);
        *((unsigned int *)((char *)v2 - 4)) = 0;
        *((unsigned int *)((char *)v2 - 28)) = sub_474143();
        *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
        sub_4741eb();
    }
    if (!v1)
        sub_472c0d(32);
    return (unsigned int)sub_46fd31(&g_4aac80, 12, v0, a0);
}
