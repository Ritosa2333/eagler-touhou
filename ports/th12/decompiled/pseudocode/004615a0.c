// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4615a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4615a0_0 {
    char padding_0[304];
    unsigned int field_130;
} st_4615a0_0;

typedef struct st_4615a0_1 {
    char padding_0[32];
    int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4615a0_1;

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
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_4615a0(void)
{
    int v7;  // edi
    int v8;  // esi
    unsigned int *v17;  // eax
    int v9;  // ebp
    int v10;  // ebx
    st_4615a0_1 *idx;  // esi
    int v12;  // eax
    int v13;  // ebx
    unsigned int *index;  // ecx
    unsigned int v15;  // edx
    int v16;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]
    unsigned int *v3;  // [bp+0x0]
    st_4615a0_0 *idx1;  // [bp+0x4]
    int v5;  // [bp+0xc]
    int v6;  // [bp+0x10]

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx1->field_130 = idx1->field_130 + 1;
    idx = sub_4621c0(v7, v8, v9, v10);
    if (v12 >= 0)
        idx->field_20 = v12;
    idx->field_480 = idx->field_480 | 1;
    v13 = v6;
    if (!index && !((char)v13 & 8))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v15 = v2;
        idx->field_430 = v0;
        idx->field_434 = v1;
        goto LABEL_461663;
    }
    else if ((char)v13 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else
    {
        idx->field_430 = *(index);
        idx->field_434 = index[1];
        v15 = index[2];
LABEL_461663:
        idx->field_438 = v15;
    }
    if (!((char)v13 & 8))
    {
        sub_454d10(idx, v5);
    }
    else
    {
        sub_454df0(v5);
        v13 = v5;
    }
    v16 = v13;
    if (v16 & 4 && (char)v13 & 2)
    {
        v17 = sub_4613d0();
        goto LABEL_4616de;
    }
    else if (v16 & 4)
    {
        *(v3) = *((int *)sub_461350());
    }
    else if ((char)v13 & 2)
    {
        *(v3) = *((int *)sub_4612d0());
    }
    else
    {
        v17 = sub_461250();
LABEL_4616de:
        *(v3) = *(v17);
    }
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    return;
}
