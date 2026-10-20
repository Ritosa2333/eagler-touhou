// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x452fd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4cf4e8;
extern unsigned int g_4d4764;
extern unsigned int g_4d476c;
extern unsigned int g_4d4774;

unsigned int sub_452fd0(unsigned int a0)
{
    sub_477420(&g_4cf4e8, 0, 21152);
    g_4d4774 = a0;
    g_4d4764 = CreateThread(NULL, NULL, sub_4530e0, &g_4cf4e8, THREAD_CREATE_RUN_IMMEDIATELY, &g_4d476c);
    return 0;
}
