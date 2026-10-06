// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453b8e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4cf4f8;
extern unsigned int g_4d0d68[4];
extern unsigned int g_4d0e28[4];
extern unsigned int g_4d0e68;

unsigned int sub_453b8e(int a0, unsigned int a1)
{
    unsigned int v1;  // edi
    unsigned int v2;  // esi

    if (sub_465aa0(g_4cf4f8, g_4d0e28[v1], g_4d0d68[v1], 0, 0, 0, 0, v2 - a1, a0) < 0)
        goto LABEL_0x453aeb;
    g_4d0e68 = v1;
    return 0;
}
