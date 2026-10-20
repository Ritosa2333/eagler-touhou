// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453030; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern HANDLE g_4d4764;
extern HANDLE g_4d4768;
extern unsigned int g_4d4770;

unsigned int sub_453030(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    if (!g_4d4764)
        return 0;
    if (!g_4d4770)
        g_4d4770 = 1;
    v1 = v3;
    v0 = v4;
    if (WaitForSingleObject(g_4d4764, 100) == WAIT_TIMEOUT)
    {
        do
        {
            Sleep(1);
        } while (WaitForSingleObject(g_4d4764, 100) == WAIT_TIMEOUT);
    }
    if (WaitForSingleObject(g_4d4768, 100) == WAIT_TIMEOUT)
    {
        do
        {
            Sleep(1);
        } while (WaitForSingleObject(g_4d4768, 100) == WAIT_TIMEOUT);
    }
    CloseHandle(g_4d4764);
    CloseHandle(g_4d4768);
    g_4d4764 = 0;
    g_4d4768 = 0;
    return 0;
}
