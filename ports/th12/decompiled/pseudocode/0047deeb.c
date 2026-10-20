// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47deeb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49ddd4;
extern unsigned int g_4b4334;
extern unsigned int g_4b4338;
extern unsigned int g_4b433c;
extern unsigned int g_4b4340;
extern unsigned int g_4b4344;
extern unsigned int g_4b4348;
extern unsigned int g_4b434c;
extern unsigned int g_4b4350;
extern unsigned int g_4b4354;
extern unsigned int g_4b4358;
extern unsigned int g_4b435c;
extern unsigned int g_4b4360;
extern void g_4b4364;

unsigned int * sub_47deeb(unsigned int a0)
{
    if (!(g_4b4364 & 1))
    {
        *((unsigned int *)&g_4b4364) = *((int *)&g_4b4364) | 1;
        g_4b4334 = &g_49ddd4;
        g_4b4338 = 0;
        g_4b433c = 0;
        g_4b4340 = &g_49ddd4;
        g_4b4344 = 1;
        g_4b4348 = 4;
        g_4b434c = &g_49ddd4;
        g_4b4350 = 2;
        g_4b4354 = 0;
        g_4b4358 = &g_49ddd4;
        g_4b435c = 3;
        g_4b4360 = 0;
    }
    if (a0 > 3)
        return &g_4b4358;
    return &(&g_4b4334)[3 * a0];
}
