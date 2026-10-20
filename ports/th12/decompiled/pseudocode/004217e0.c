// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4217e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aebf0;
extern int g_4b0cb0;
extern char g_4b452c;

void sub_4217e0(void)
{
    int v1;  // eax

    v1 = g_4b0cb0;
    if (v1 < 7)
    {
        v1 += 1;
        g_4b0cb0 = v1;
    }
    *((char **)&g_4b452c) = &(&g_4aebf0)[64 * v1];
    return;
}
