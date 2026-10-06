// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x463e80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct CRITICAL_SECTION_DEBUG {
    unsigned short Type;
    unsigned short CreatorBackTraceIndex;
    struct CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    unsigned int EntryCount;
    unsigned int ContentionCount;
    unsigned int Flags;
    unsigned short CreatorBackTraceIndexHigh;
    unsigned short Identifier;
} CRITICAL_SECTION_DEBUG;

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf128;
extern char g_4cf21a;

unsigned int sub_463e80(void)
{
    unsigned int v2;  // edi
    HANDLE v3;  // esi
    unsigned int count;  // ebx
    unsigned int v0;  // [bp-0x4]
    Byte v1;  // [bp+0x4], Other Possible Types: unsigned int

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf128.DebugInfo);
        g_4cf21a = g_4cf21a + 1;
    }
    v3 = CreateFileA(v2, 0x40000000, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (v3 == 0xffffffff)
    {
        FormatMessageA(0x1300, NULL, GetLastError(), 0x400, &v1, 0, NULL);
        sub_4654b0("error : %s write error %s\r\n", v2, v1);
        LocalFree(v1);
        if (!(g_4cee78 & 0x8000))
            return 0xffffffff;
        LeaveCriticalSection(&g_4cf128.DebugInfo);
        g_4cf21a = g_4cf21a - 1;
        return 0xffffffff;
    }
    else
    {
        WriteFile(v3, v1, count, &v0, NULL);
        if (count != v0)
        {
            CloseHandle(v3);
            sub_4654b0("error : %s write error\r\n", v2);
            if (!(g_4cee78 & 0x8000))
                return 0xfffffffe;
            LeaveCriticalSection(&g_4cf128.DebugInfo);
            g_4cf21a = g_4cf21a - 1;
            return 0xfffffffe;
        }
        else
        {
            CloseHandle(v3);
            sub_4654b0("%s write ...\r\n", v2);
            if (!(g_4cee78 & 0x8000))
                return 0;
            LeaveCriticalSection(&g_4cf128.DebugInfo);
            g_4cf21a = g_4cf21a - 1;
            return 0;
        }
    }
}
