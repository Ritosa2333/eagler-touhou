// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48eb34; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern HANDLE g_4ae420;
extern HANDLE g_4ae424;

HANDLE sub_48eb34(void)
{
    HANDLE v1;  // eax

    if (g_4ae424 != 0xffffffff && g_4ae424 != 0xfffffffe)
        CloseHandle(g_4ae424);
    v1 = g_4ae420;
    if (g_4ae420 != 0xffffffff && g_4ae420 != 0xfffffffe)
        v1 = CloseHandle(g_4ae420);
    return v1;
}
