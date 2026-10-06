// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e658; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_400000;
extern char g_400018;
extern char g_40003c;
extern char g_400074;
extern char g_4000e8;

unsigned int sub_46e658(void)
{
    if (*((short *)&g_400000) != 23117)
    {
        return 0;
    }
    else if (*((int *)&(&g_400000)[*((int *)&g_40003c)]) != 17744)
    {
        return 0;
    }
    else if (*((short *)&(&g_400018)[*((int *)&g_40003c)]) != 267)
    {
        return 0;
    }
    else if (*((int *)&(&g_400074)[*((int *)&g_40003c)]) > 14)
    {
        return *((int *)&(&g_4000e8)[*((int *)&g_40003c)]);
    }
    else
    {
        return 0;
    }
}
