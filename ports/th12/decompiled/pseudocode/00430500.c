// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x430500; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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
extern unsigned int g_4cf0dc;
extern int g_4cf0e0;
extern unsigned int g_4cf0e4;
extern unsigned int g_4cf0e8;
extern unsigned int g_4cf0f0;
extern CRITICAL_SECTION g_4cf188;
extern char g_4cf21e;

unsigned int sub_430500(void)
{
    int v1;  // esi
    unsigned int v2;  // eax
    unsigned int v3;  // 4102

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf188.DebugInfo);
        g_4cf21e = g_4cf21e + 1;
    }
    sub_464c40(v1);
    g_4cf0f0 = sub_422280;
    g_4cf0e8 = 1;
    g_4cf0e4 = 0;
    v2 = sub_46e54b(0, 0, sub_422280, 0, 0, &g_4cf0e0);
    v3 = g_4cee78 & 0x8000;
    g_4cf0dc = v2;
    if (v3)
    {
        LeaveCriticalSection(&g_4cf188.DebugInfo);
        g_4cf21e = g_4cf21e - 1;
    }
    return 0;
}
