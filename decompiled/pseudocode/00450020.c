// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4cf41c;
extern unsigned int g_4cf420;
extern unsigned int g_4cf424;

int sub_450020(void)
{
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, g_4cf41c, NULL, SPIF_SENDWININICHANGE);
    SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, g_4cf420, NULL, SPIF_SENDWININICHANGE);
    SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, g_4cf424, NULL, SPIF_SENDWININICHANGE);
    return WINNLSEnableIME(NULL, 1);
}
