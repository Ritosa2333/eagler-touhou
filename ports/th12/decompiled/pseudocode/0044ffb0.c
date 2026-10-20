// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ffb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern long long g_4cf408;
extern long long g_4cf410;
extern void g_4cf41c;
extern void g_4cf420;
extern void g_4cf424;

void sub_44ffb0(void)
{
    SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0, &g_4cf41c, 0);
    SystemParametersInfoA(SPI_GETLOWPOWERACTIVE, 0, &g_4cf420, 0);
    SystemParametersInfoA(SPI_GETPOWEROFFACTIVE, 0, &g_4cf424, 0);
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, 0, NULL, SPIF_SENDWININICHANGE);
    SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, 0, NULL, SPIF_SENDWININICHANGE);
    SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, 0, NULL, SPIF_SENDWININICHANGE);
    QueryPerformanceFrequency(&g_4cf408);
    QueryPerformanceCounter(&g_4cf410);
    return;
}
