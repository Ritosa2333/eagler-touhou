// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453460; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4a2e34;
extern unsigned int g_4aea50[4];
extern unsigned int g_4d1410;
extern unsigned int g_4d4770;

int sub_453460(void)
{
    unsigned int v2;  // esi
    int idx;  // esi
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    idx = 0;
    do
    {
        if (g_4d4770 == 2)
            return v4;
        v4 = sub_463c10(0, 0);
        (&g_4d1410)[idx] = v4;
        if (!v4)
            return sub_464220(&g_4a2e34, g_4aea50[idx]);
        idx += 1;
    } while (idx < 49);
    if (g_4d4770)
        return v4;
    do
    {
        v5 = (unsigned int)Sleep(1);
    } while (!g_4d4770);
    return v5;
}
