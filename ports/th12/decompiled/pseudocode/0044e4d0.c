// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e4d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

extern Byte g_4a245c;
extern Byte g_4a246c;
extern char g_4b0d48;
extern unsigned int g_4b453c;
extern unsigned int g_4cc54c;
extern unsigned int g_4ce550;
extern unsigned int g_4ce554;
extern void g_4ce568;
extern unsigned int g_4ce56c;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1e8;
extern char g_4cf222;

void sub_44e4d0(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v6;  // edi
    unsigned int iter;  // esi
    unsigned short v8;  // cx
    unsigned short v9;  // ax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0xc]

    if (!(char)sub_44d640(26))
        sub_44d640(21);
    v2 = v4;
    v1 = v5;
    v0 = v6;
    iter = 0;
    do
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf1e8.DebugInfo);
            g_4cf222 = g_4cf222 + 1;
        }
        g_4ce56c = g_4ce56c + 1;
        v8 = (*((int *)&g_4ce568) ^ 38448) - 25939;
        v9 = (v8 >> 14) + (unsigned short)(v8 * 4);
        *((unsigned short *)&g_4ce568) = v9;
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf1e8.DebugInfo);
            g_4cf222 = g_4cf222 - 1;
            v9 = *((short *)&g_4ce568);
        }
        (&g_4b0d48)[iter] = v9 >> 9;
        iter += 1;
    } while (iter < 0x100);
    g_4ce554 = CreateFontA(32, 0, 0, 0, 400, 0, 0, 0, 128, 0, 0, 4, 0x11, &g_4a245c);
    g_4ce550 = CreateFontA(32, 0, 0, 0, 600, 0, 0, 0, 128, 0, 0, 4, 0x11, &g_4a246c);
    g_4cc54c = CreateFontA(15, 0, 0, 0, 700, 0, 0, 0, 128, 0, 0, 4, 0x11, &g_4a245c);
    g_4b453c = CreateFontA(15, 0, 0, 0, 700, 0, 0, 0, 128, 0, 0, 4, 0x11, &g_4a246c);
    return;
}
