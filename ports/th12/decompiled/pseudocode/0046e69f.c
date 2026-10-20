// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e69f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct STARTUPINFOA {
    unsigned int cb;
    PSTR lpReserved;
    PSTR lpDesktop;
    PSTR lpTitle;
    unsigned int dwX;
    unsigned int dwY;
    unsigned int dwXSize;
    unsigned int dwYSize;
    unsigned int dwXCountChars;
    unsigned int dwYCountChars;
    unsigned int dwFillAttribute;
    enum STARTUPINFOW_FLAGS dwFlags;
    unsigned short wShowWindow;
    unsigned short cbReserved2;
    Byte *lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA;

extern char g_400000;
extern char g_400018;
extern char g_40003c;
extern char g_400074;
extern char g_4000e8;
extern int g_4aaaf8;
extern unsigned int g_4b38d0;
extern unsigned int g_4d645c;

int sub_46e69f(void)
{
    STARTUPINFOA *v2;  // ebp
    int v3;  // eax
    int v4;  // eax
    unsigned int v5;  // ecx
    int v6;  // eax
    unsigned int v0;  // [bp-0xc]

    sub_46fcec(&g_4aaaf8, 88);
    *((unsigned int *)((char *)v2 - 4)) = 0;
    GetStartupInfoA((char *)v2 - 104);
    v0 = 0xfffffffe;
    *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
    if (*((short *)&g_400000) == 23117 && *((int *)&(&g_400000)[*((int *)&g_40003c)]) == 17744 && *((short *)&(&g_400018)[*((int *)&g_40003c)]) == 267 && *((int *)&(&g_400074)[*((int *)&g_40003c)]) > 14)
        *((unsigned int *)((char *)v2 - 28)) = *((int *)&(&g_4000e8)[*((int *)&g_40003c)]);
    else
        *((unsigned int *)((char *)v2 - 28)) = 0;
    if (!sub_46ea5c(1))
    {
        sub_46e62f(28); /* do not return */
    }
    else if (!sub_47562a())
    {
        sub_46e62f(16); /* do not return */
    }
    else
    {
        sub_47b31d();
        *((unsigned int *)((char *)v2 - 4)) = 1;
        if (sub_47b07b() < 0)
            sub_472c0d(27);
        g_4d645c = GetCommandLineA();
        g_4b38d0 = sub_47af44();
        if (sub_47ae89() < 0)
            sub_472c0d(8);
        if (sub_47ac02() < 0)
            sub_472c0d(9);
        v3 = sub_472d44(1);
        if (v3)
            sub_472c0d(v3);
        v4 = sub_47aba3();
        if (*((char *)v2 - 60) & 1)
        {
            v5 = *((short *)((char *)v2 - 56));
        }
        else
        {
            v0 = 10;
            v5 = 10;
        }
        v6 = sub_44f560(&g_400000, 0, v4, v5);
        *((int *)((char *)v2 - 32)) = v6;
        if (!*((int *)((char *)v2 - 28)))
            sub_472ef5(v6);
        sub_472f21();
        *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
        return sub_46fd31();
    }
}
