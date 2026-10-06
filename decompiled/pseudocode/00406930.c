// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406930; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406930_0 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[44];
    unsigned int field_3c;
    int field_40;
    int field_44;
    int field_48;
} st_406930_0;

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
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b43c4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_406930(st_406930_0 *idx)
{
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    unsigned int v16;  // edi
    unsigned short v8;  // fs
    unsigned long long v9;  // 4184
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned long long v12;  // 4188
    int v13;  // esi
    int v14;  // eax
    unsigned long long v15;  // 4126
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    v5 = 0xffffffff;
    v4 = sub_4975db;
    v9 = _ccall(v6, v7, v8, 0);
    v3 = *((int *)(unsigned int)v9);
    v2 = v10;
    v1 = v11;
    v0 = g_4ad138 ^ &v16;
    v12 = _ccall(v6, v7, v8, 0);
    *((unsigned int **)(unsigned int)v12) = &v3;
    v5 = 0;
    if (idx->field_3c && g_4b0c94 + g_4b0c90 * 2 == 5)
    {
        sub_461a70(idx->field_40);
        idx->field_40 = 0;
        sub_422d70(idx->field_40);
    }
    sub_461a70(idx->field_40);
    idx->field_40 = 0;
    sub_461a70(idx->field_44);
    idx->field_44 = 0;
    sub_461a70(idx->field_48);
    idx->field_48 = 0;
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
    if (*((int *)&idx[0x11].padding_10[4]))
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
            sub_406a80(idx->field_48);
        }
        else
        {
            sub_406a80();
        }
        return;
    }
    else
    {
        v13 = idx[16].field_44;
        g_4b43c4 = 0;
        if (v13)
        {
            sub_402870();
            sub_46ca4f(v13);
        }
        v14 = idx[16].field_40;
        idx[16].field_44 = 0;
        if (v14)
        {
            sub_46c941(v14);
            idx[16].field_40 = 0;
        }
        if (*((int *)&idx[16].padding_0[4]))
            sub_46c941(*((int *)&idx[16].padding_0[4]));
        *((unsigned int *)&idx[16].padding_0[4]) = 0;
        v15 = _ccall(v6, v7, v8, 0);
        *((unsigned int *)(unsigned int)v15) = v16;
        return;
    }
}
