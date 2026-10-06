// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4848e2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad9d4;
extern char g_4adab8;

int sub_4848e2(void)
{
    unsigned int *v1;  // ecx
    unsigned int v2;  // eax

    v1 = sub_475467();
    v2 = v1[27];
    if (v2 != *((int *)&g_4adab8) && !(g_4ad9d4 & v1[28]))
        v2 = sub_474181();
    return v2 + 12;
}
