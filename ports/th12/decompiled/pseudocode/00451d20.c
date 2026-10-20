// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451d20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ce908;
extern unsigned int g_4ce90c;
extern unsigned int g_4cee78;

void sub_451d20(void)
{
    unsigned int v1;  // eax

    g_4cee78 = g_4cee78 & 0xfffff3ff;
    sub_451a60();
    v1 = g_4cee78 ^ ((g_4ce908) * 0x400 ^ g_4cee78) & 0x400;
    g_4cee78 = v1 ^ ((g_4ce90c) * 0x800 ^ v1) & 0x800;
    return;
}
