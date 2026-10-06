// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42fbb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

extern HANDLE g_4ae590;
extern unsigned int g_4cee78;
extern unsigned int g_4cefb8;
extern Byte g_4cefbc;
extern int g_4cefcc;
extern int g_4cefd0;
extern CRITICAL_SECTION g_4cf128;
extern char g_4cf21a;

int sub_42fbb0(int a0)
{
    int v5;  // eax
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    sub_463fb0(v0, v1, v2, a0);
    if (g_4ae590 != 0xffffffff)
    {
        WriteFile(g_4ae590, &g_4cefbc, 14, &v3, NULL);
        if (v3 != 14)
        {
            CloseHandle(g_4ae590);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf128.DebugInfo);
                g_4cf21a = g_4cf21a - 1;
            }
        }
    }
    sub_464130();
    sub_464130();
    if (g_4ae590 != 0xffffffff)
    {
        CloseHandle(g_4ae590);
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf128.DebugInfo);
            g_4cf21a = g_4cf21a - 1;
        }
    }
    if (g_4cefcc)
    {
        sub_46c941(g_4cefcc);
        g_4cefcc = 0;
    }
    v5 = g_4cefd0;
    if (v5)
    {
        v5 = sub_46c941(v5);
        g_4cefd0 = 0;
    }
    g_4cefb8 = 0;
    return v5;
}
