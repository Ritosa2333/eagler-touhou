// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4530e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d4770;
extern unsigned int g_4d4778;

void sub_4530e0(void)
{
    unsigned int v2;  // esi
    unsigned int v0;  // [bp-0x4]

    sub_453120();
    if (!g_4d4770)
    {
        v0 = v2;
        do
        {
            Sleep(1);
        } while (!g_4d4770);
    }
    g_4d4778 = 1;
    return;
}
