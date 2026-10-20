// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d0d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4a2458;
extern unsigned int g_4b0cf8;
extern unsigned int g_4b0cfc;
extern unsigned int g_4b0d00;
extern unsigned int g_4b0d04;
extern unsigned int g_4b0d08;

unsigned int * sub_44d0d0(void)
{
    g_4b0cfc = 0;
    g_4b0d00 = 0;
    g_4b0d04 = 0;
    g_4b0d08 = 0;
    g_4b0cf8 = &g_4a2458;
    return &g_4b0cf8;
}
