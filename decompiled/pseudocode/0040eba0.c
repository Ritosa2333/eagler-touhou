// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40eba0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

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

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

extern char g_4a3738;
extern unsigned int g_4ad138;
extern int g_4b43c4;
extern int g_4b43c8;
extern unsigned int g_4b43d0;
extern int g_4b43dc;
extern void g_4b43e4;
extern int g_4b44f0;
extern int g_4b4514;
extern int g_4b4524;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_40eba0(unsigned int *idx)
{
    unsigned long v7;  // ldt
    unsigned long v8;  // gdt
    unsigned short v9;  // fs
    unsigned long long v10;  // 4136
    unsigned int v11;  // ebx
    unsigned int v12;  // esi
    unsigned long long v13;  // 4158
    int v14;  // eax
    unsigned long long v15;  // 4122
    CRITICAL_SECTION *v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    unsigned int v6;  // [bp-0x4]

    v6 = 0xffffffff;
    v5 = sub_497779;
    v10 = _ccall(v7, v8, v9, 0);
    v4 = *((int *)(unsigned int)v10);
    v3 = v11;
    v2 = v12;
    v1 = g_4ad138 ^ &edi;
    v13 = _ccall(v7, v8, v9, 0);
    *((unsigned int **)(unsigned int)v13) = &v4;
    v6 = 1;
    sub_40e850();
    if (idx[2])
    {
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8.DebugInfo;
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8.DebugInfo;
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (idx[3])
    {
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8.DebugInfo;
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8.DebugInfo;
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (*((int *)&g_4b43e4))
    {
        v0 = *((int *)&g_4b43e4);
        sub_41dc50(*((int *)&g_4b43e4));
        sub_46ca4f(*((int *)&g_4b43e4));
    }
    if (g_4b4514)
    {
        sub_436270(g_4b4514);
        sub_46ca4f(g_4b4514);
    }
    if (g_4b43c8)
    {
        sub_4096d0(g_4b43c8);
        sub_46ca4f(g_4b43c8);
    }
    if (g_4b43dc)
    {
        sub_412f10();
        sub_46ca4f(g_4b43dc);
    }
    if (g_4b43c4)
    {
        sub_406930(g_4b43c4);
        sub_46ca4f(g_4b43c4);
    }
    if (g_4b44f0)
    {
        sub_425a00(g_4b44f0);
        sub_46ca4f(g_4b44f0);
    }
    if (g_4b4524)
    {
        sub_43dc30(g_4b4524);
        sub_46ca4f(g_4b4524);
    }
    v14 = idx[464];
    g_4b43d0 = 0;
    if (v14)
        sub_46c941(v14);
    idx[464] = 0;
    idx[4] = &g_4a3738;
    sub_464c40();
    v15 = _ccall(v7, v8, v9, 0);
    *((CRITICAL_SECTION **)(unsigned int)v15) = v0;
    return;
}
