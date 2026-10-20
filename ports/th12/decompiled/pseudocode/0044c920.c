// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c920; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b4544;
extern char g_4cc550;

void sub_44c920(unsigned int a0, unsigned int a1)
{
    unsigned int i;  // eax
    int v0;  // [bp-0x4]

    sub_477420(&g_4cc550, 0, 0x2000, v0);
    i = &g_4b4544;
    do
    {
        memset(i - 4, 0, 12);
        i += 12;
    } while (i < &g_4cc550);
    return;
}
