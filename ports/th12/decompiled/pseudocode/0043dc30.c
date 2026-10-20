// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43dc30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43dc30_0 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[1152];
    int field_490;
} st_43dc30_0;

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

extern unsigned int g_4ad138;
extern unsigned int g_4b4524;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_43dc30(st_43dc30_0 *idx)
{
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    unsigned short v7;  // fs
    unsigned long long v8;  // 4185
    unsigned long long v9;  // 4189
    int v10;  // eax
    unsigned long long v11;  // 4122
    unsigned int v0;  // [bp-0x20]
    char v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = 0xffffffff;
    v3 = sub_49700b;
    v8 = _ccall(v5, v6, v7, 0);
    v2 = *((int *)(unsigned int)v8);
    v0 = g_4ad138 ^ &v1;
    v9 = _ccall(v5, v6, v7, 0);
    *((unsigned int **)(unsigned int)v9) = &v2;
    v4 = 0;
    if (idx->field_8)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (idx->field_c)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    v10 = idx->field_490;
    g_4b4524 = 0;
    if (v10)
    {
        sub_46c941(v10);
        idx->field_490 = 0;
    }
    else
    {
        idx->field_490 = 0;
    }
    v11 = _ccall(v5, v6, v7, 0);
    *((unsigned int *)(unsigned int)v11) = v2;
    return;
}
