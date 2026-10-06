// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431b00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ce8e8;
extern unsigned int g_4cee78;

int sub_431b00(void)
{
    sub_477420(&g_4ce8e8, 0, 2500);
    g_4cee78 = g_4cee78 | 16960;
    return &g_4ce8e8;
}
