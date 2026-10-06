// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454960; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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
extern CRITICAL_SECTION g_4cf200;
extern char g_4cf223;

int sub_454960(void)
{
    unsigned int v3;  // esi
    int v4;  // eax
    unsigned int *v5;  // eax
    unsigned int *v6;  // ecx
    unsigned int *idx;  // edx
    char *v8;  // edi
    char *iter;  // eax
    char i;  // 4102
    unsigned int v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]
    unsigned int v2;  // [bp+0x8]

    v0 = v3;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf200.DebugInfo);
        g_4cf223 = g_4cf223 + 1;
    }
    v4 = 0;
    v6 = v5 + 0x7fb;
    do
    {
        if (!*(v6))
        {
            idx = &v5[67 * v4];
            idx[0x7fb] = v1;
            idx[0x7fc] = v2;
            iter = v8;
            do
            {
                i = *(iter);
                *(idx - v8 + iter + 0x1ff8) = *(iter);
                iter += 1;
            } while (i);
            idx[0x7fd] = 0;
            break;
        }
    } while ((v4 = (int)(v4 + 1), v6 += 268, v4 < 31));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf200.DebugInfo);
        g_4cf223 = g_4cf223 - 1;
    }
    return;
}
