// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43e520; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43e520_0 {
    char padding_0[140];
    int field_8c;
} st_43e520_0;

typedef struct st_43e520_1 {
    char padding_0[20];
    struct st_43e520_2 *field_14;
} st_43e520_1;

typedef struct st_43e520_4 {
    struct st_43e520_1 *field_0;
} st_43e520_4;

typedef struct st_43e520_2 {
    unsigned int field_0;
} st_43e520_2;

typedef struct st_43e520_3 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[712];
    struct st_43e520_0 *field_2d8;
    struct st_43e520_4 *field_2dc;
    int field_2e0;
} st_43e520_3;

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

extern char g_49fc28;
extern char g_4a3738;
extern unsigned int g_4ad138;
extern unsigned int g_4b4528;
extern char g_4b50d4;
extern char g_4b50d8;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_43e520(st_43e520_3 *idx)
{
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    unsigned short v7;  // fs
    unsigned long long v8;  // 4139
    unsigned long long v9;  // 4161
    st_43e520_0 *index;  // edi
    int v11;  // eax
    unsigned int v12;  // ebx
    unsigned int v13;  // ebx
    unsigned long long v14;  // 4122
    unsigned int v0;  // [bp-0x20]
    char v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = 0xffffffff;
    v3 = sub_496f0b;
    v8 = _ccall(v5, v6, v7, 0);
    v2 = *((int *)(unsigned int)v8);
    v0 = g_4ad138 ^ &v1;
    v9 = _ccall(v5, v6, v7, 0);
    *((unsigned int **)(unsigned int)v9) = &v2;
    v4 = 0;
    sub_43e380();
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
    if (idx->field_2e0)
    {
        sub_46c941(idx->field_2e0);
        idx->field_2e0 = 0;
    }
    index = idx->field_2d8;
    if (index)
    {
        v11 = index->field_8c;
        *((char **)&index->padding_0[0]) = &g_49fc28;
        if (v11)
        {
            sub_46c941(v11);
            index->field_8c = 0;
        }
        sub_46ca4f(index);
        idx->field_2d8 = NULL;
    }
    if (idx->field_2dc)
    {
        idx->field_2dc->field_0->field_14(1);
        idx->field_2dc = 0;
    }
    v12 = &(&g_4b50d8)[g_4ce8cc];
    if (*((int *)&(&g_4b50d8)[g_4ce8cc]))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v12));
        *((unsigned int *)v12) = 0;
    }
    v13 = &(&g_4b50d4)[g_4ce8cc];
    if (*((int *)&(&g_4b50d4)[g_4ce8cc]))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v13));
        *((unsigned int *)v13) = 0;
    }
    g_4b4528 = 0;
    *((char **)&idx->padding_10[0]) = &g_4a3738;
    sub_464c40();
    v14 = _ccall(v5, v6, v7, 0);
    *((unsigned int *)(unsigned int)v14) = v2;
    return;
}
