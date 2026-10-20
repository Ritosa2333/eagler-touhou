// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c7a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

typedef struct SECURITY_ATTRIBUTES {
    unsigned int nLength;
    void* lpSecurityDescriptor;
    int bInheritHandle;
} SECURITY_ATTRIBUTES;

int sub_46c7a0(PSTR a0, SECURITY_ATTRIBUTES *a1, SECURITY_ATTRIBUTES *a2, int a3, enum PROCESS_CREATION_FLAGS a4, void* a5, PSTR a6, STARTUPINFOA *a7, PROCESS_INFORMATION *a8, unsigned int a9)
{
    PSTR v0;  // [bp+0x0]

    return CreateProcessA(v0, a0, a1, a2, a3, a4, a5, a6, a7, a8);
}
