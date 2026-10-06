// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ce90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    unsigned int dwProcessId;
    unsigned int dwThreadId;
} PROCESS_INFORMATION;

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

int sub_44ce90(PSTR a0, PSTR i)
{
    PROCESS_INFORMATION v0;  // [bp-0x54]
    STARTUPINFOA v1;  // [bp-0x44], Other Possible Types: int, char
    unsigned int v2;  // [bp+0xc]

    v1 = (int)_INSERT(CONCAT(v1, 0), 16, 0x80000000);
    *((unsigned int *)&(&v1)[20]) = 0x80000000;
    *((unsigned int *)&(&v1)[24]) = 0x80000000;
    *((unsigned int *)&(&v1)[28]) = 0x80000000;
    v1.wShowWindow = 10;
    *((unsigned short *)&(&v1)[50]) = 0;
    *((unsigned int *)&v1) = 0x44;
    *((unsigned int *)&(&v1)[4]) = 0;
    *((unsigned int *)&(&v1)[8]) = 0;
    *((unsigned int *)&(&v1)[12]) = 0;
    *((unsigned int *)&(&v1)[32]) = 80;
    *((unsigned int *)&(&v1)[36]) = 25;
    *((unsigned int *)&(&v1)[40]) = 0;
    *((unsigned int *)&(&v1)[44]) = 0;
    *((unsigned int *)&(&v1)[52]) = 0;
    *((unsigned int *)&(&v1)[56]) = 0;
    *((unsigned int *)&(&v1)[60]) = 0;
    v1.hStdError = 0;
    if (!CreateProcessA(a0, i, NULL, NULL, 0, NORMAL_PRIORITY_CLASS, NULL, NULL, &v1, &v0))
    {
        return 0xffffffff;
    }
    else if (v2)
    {
        i = 259;
        do
        {
            GetExitCodeProcess(v0.hProcess, &i);
        } while (i == 259);
        return i;
    }
    else
    {
        return 0;
    }
}
