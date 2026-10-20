// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412f10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412f10_0 {
    char padding_0[140];
    int field_8c;
} st_412f10_0;

typedef struct st_412f10_1 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[84];
    struct st_412f10_0 *field_64;
} st_412f10_1;

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
extern unsigned int g_4b43dc;
extern unsigned int g_4b50e0;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

int sub_412f10(void)
{
    st_412f10_1 *index;  // eax
    int v4;  // edi
    unsigned int v13;  // ebx
    int *v14;  // esi
    int v5;  // esi
    int v6;  // ebp
    int v7;  // ebx
    int v8;  // ecx
    int v9;  // esi
    st_412f10_0 *idx;  // esi
    int v11;  // eax
    int v12;  // ebp
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x8]

    sub_40e940(index, v4, v5, v6, v7, v8);
    if (index->field_8)
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
    if (index->field_c)
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
    v9 = 12;
    do
    {
        if (*((int *)&index->field_64->padding_0[v9]))
            sub_46c941(*((int *)&index->field_64->padding_0[v9]));
    } while ((v9 = (int)(v9 + 4), v9 < 140));
    idx = index->field_64;
    if (idx)
    {
        v11 = idx->field_8c;
        *((char **)&idx->padding_0[0]) = &g_49fc28;
        if (v11)
        {
            sub_46c941(v11);
            idx->field_8c = 0;
        }
        sub_46ca4f(idx);
    }
    index->field_64 = NULL;
    v12 = 8;
    v0 = 4;
    v13 = &g_4b50e0;
    do
    {
        v1 = v0;
        if (v12 >= 0 && v12 < 32)
        {
            v14 = v13 + g_4ce8cc;
            if (*((int *)(v13 + g_4ce8cc)))
            {
                sub_4604e0();
                sub_46ca4f(*(v14));
                *(v14) = 0;
            }
        }
    } while ((v13 += 4, v12 = (int)(v12 + 1), v0 = v1 - 1, v1 != 1));
    g_4b43dc = 0;
    return;
}
