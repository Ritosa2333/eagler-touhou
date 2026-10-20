// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42ec00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

typedef struct st_42ec00_0 {
    int field_0;
    int field_4;
} st_42ec00_0;

extern char g_4a3738;
extern unsigned int g_4ad138;
extern void g_4b43b8;
extern unsigned int g_4b44f8;
extern st_42ec00_0 *g_4b451c;
extern char g_4b50c0;
extern char g_4b50c4;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_42ec00(unsigned int *idx)
{
    unsigned long v7;  // ldt
    unsigned long v8;  // gdt
    int v17;  // eax
    unsigned long long v18;  // 4122
    unsigned short v9;  // fs
    unsigned long long v10;  // 4140
    unsigned int v11;  // eax
    unsigned int v12;  // ecx
    unsigned long long v13;  // 4164
    unsigned int v14;  // ebx
    unsigned int v15;  // ebx
    int *v16;  // edi
    char v0;  // [bp-0x20]
    int v1;  // [bp-0x1c]
    int v2;  // [bp-0x18]
    int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x4]

    v6 = 0xffffffff;
    v10 = _ccall(v7, v8, v9, 0);
    v11 = *((int *)(unsigned int)v10);
    v5 = *((int *)(unsigned int)v10);
    v4 = v12;
    v13 = _ccall(v7, v8, v9, 0);
    *((unsigned int **)(unsigned int)v13) = &v5;
    sub_464c40(g_4ad138 ^ &v0, v0, v1, v2, v3, idx + 4, v11, sub_497336, 1);
    if (idx[2])
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
    if (idx[3])
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
    sub_42e950();
    v14 = &(&g_4b50c4)[g_4ce8cc];
    if (*((int *)&(&g_4b50c4)[g_4ce8cc]))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v14));
        *((unsigned int *)v14) = 0;
    }
    g_4b44f8 = 0;
    if (*((int *)&g_4b43b8))
    {
        sub_401220(*((int *)&g_4b43b8));
        sub_46ca4f(*((int *)&g_4b43b8));
    }
    v15 = &(&g_4b50c0)[g_4ce8cc];
    if (*((int *)&(&g_4b50c0)[g_4ce8cc]))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v15));
        *((unsigned int *)v15) = 0;
    }
    sub_43d2e0();
    v16 = &g_4b451c->field_0;
    if (v16)
    {
        if (*(v16))
        {
            sub_46c941(*(v16));
            *(v16) = 0;
        }
        if (v16[1])
        {
            sub_46c941(v16[1]);
            v16[1] = 0;
        }
        sub_46ca4f(v16);
    }
    v17 = idx[298];
    g_4b451c = 0;
    if (v17)
        sub_46c941(v17);
    idx[298] = 0;
    idx[4] = &g_4a3738;
    sub_464c40();
    v18 = _ccall(v7, v8, v9, 0);
    *((unsigned int *)(unsigned int)v18) = v11;
    return;
}
