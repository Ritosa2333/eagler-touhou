// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ceb4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ae314;
extern HANDLE g_4ae424;

int sub_48ceb4(char a0)
{
    unsigned int v3;  // eax
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    unsigned int v0;  // [bp-0x14]
    Byte v1[8];  // [bp-0x10]

    if (g_4ae314)
    {
        if (g_4ae424 == 0xfffffffe)
            sub_48eb15();
        if (g_4ae424 != 0xffffffff)
        {
            if (WriteConsoleW(g_4ae424, &/* unsupported instruction */, 1, &v0, NULL))
            {
                g_4ae314 = 1;
LABEL_48cf5c:
                return v5;
            }
            if (g_4ae314 == 2 && GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
            {
                g_4ae314 = 0;
                goto LABEL_48cf20;
            }
        }
    }
    else
    {
LABEL_48cf20:
        v3 = WideCharToMultiByte(GetConsoleOutputCP(), 0, &/* unsupported instruction */, 1, v1, 5, NULL, NULL);
        if (g_4ae424 != 0xffffffff && WriteConsoleA(g_4ae424, v1, v3, &v0, NULL))
            goto LABEL_48cf5c;
    }
    return v4;
}
