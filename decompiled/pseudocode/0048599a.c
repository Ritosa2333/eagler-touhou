// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48599a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad9d4;
extern char g_4adab8;
extern unsigned int g_4adf98;

int sub_48599a(void)
{
    unsigned int *v1;  // eax

    v1 = sub_475467();
    if (v1[27] != *((int *)&g_4adab8) && !(g_4ad9d4 & v1[28]))
        sub_474181();
    return g_4adf98;
}
