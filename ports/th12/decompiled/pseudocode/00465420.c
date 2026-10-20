// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465420; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b0ec8;
extern unsigned int g_4b2ec8;
extern char g_4b2ecc;

char sub_465420(void)
{
    g_4b0ec8 = 0;
    g_4b2ecc = 0;
    g_4b2ec8 = &g_4b0ec8;
    return 200;
}
