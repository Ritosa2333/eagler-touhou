// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402cc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_402cc0_0 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    int field_10;
    char padding_14[13760];
    unsigned int field_35d4;
    char padding_35d8[40];
    unsigned int field_3600;
    int field_3604;
} st_402cc0_0;

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
extern char g_4b0ce0;
extern unsigned int g_4b43bc;
extern st_402cc0_0 *g_4b43c0;
extern char g_4b50c0;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_402cc0(st_402cc0_0 *idx)
{
    unsigned long v7;  // ldt
    unsigned long v8;  // gdt
    unsigned int v17;  // 4111
    unsigned int v18;  // esi
    unsigned long long v19;  // 4122
    unsigned short v9;  // fs
    unsigned long long v10;  // 4185
    unsigned int v11;  // ebx
    unsigned int v12;  // esi
    unsigned long long v13;  // 4189
    int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    char v6;  // [bp-0x4], Other Possible Types: unsigned int

    v6 = 0xffffffff;
    v5 = sub_4972f6;
    v10 = _ccall(v7, v8, v9, 0);
    v4 = *((int *)(unsigned int)v10);
    v3 = v11;
    v1 = v12;
    v0 = g_4ad138 ^ &edi;
    v13 = _ccall(v7, v8, v9, 0);
    *((unsigned int **)(unsigned int)v13) = &v4;
    v6 = 1;
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
    if (idx->field_3600)
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
    if (idx->field_10)
    {
        sub_46c941(idx->field_10);
        idx->field_10 = 0;
    }
    if (idx->field_3604)
    {
        sub_46c941(idx->field_3604);
        idx->field_3604 = 0;
    }
    v14 = *((int *)&idx->padding_14[436]);
    idx->field_3604 = 0;
    if (v14)
    {
        sub_46c941(v14);
        *((unsigned int *)&idx->padding_14[436]) = 0;
    }
    if (*((int *)&idx->padding_35d8[4]))
    {
        sub_402870();
        sub_46ca4f(*((int *)&idx->padding_35d8[4]));
        *((unsigned int *)&idx->padding_35d8[4]) = 0;
    }
    if (!(g_4b0ce0 & 1))
    {
        v15 = idx->field_35d4 & 1;
        v16 = v15 + 3;
        v17 = _ccall(8, 3, v15, 3, 0);
        if (!(v17 & 1) && v16 < 32)
        {
            v18 = &(&g_4b50c0)[4 * v16 + g_4ce8cc];
            if (*((int *)&(&g_4b50c0)[4 * v16 + g_4ce8cc]))
            {
                sub_4604e0();
                sub_46ca4f(*((int *)v18));
                *((unsigned int *)v18) = 0;
            }
        }
    }
    if (g_4b43c0 == idx)
        g_4b43c0 = 0;
    if (g_4b43bc == idx)
        g_4b43bc = 0;
    v6 = 0;
    sub_46cf1e(&idx->padding_14[10112], 1204, 3, sub_4026e0);
    v2 = 0xffffffff;
    sub_46cf1e(&idx->padding_14[440], 1204, 8, sub_4026e0);
    v19 = _ccall(v7, v8, v9, 0);
    *((unsigned int *)(unsigned int)v19) = 1204;
    return;
}
