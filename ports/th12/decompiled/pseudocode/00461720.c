// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461720_6 {
    char padding_0[304];
    unsigned int field_130;
} st_461720_6;

typedef struct st_461720_1 {
    char padding_0[20];
    struct st_461720_0 *field_14;
    char padding_18[8];
    unsigned int field_20;
} st_461720_1;

typedef struct st_461720_2 {
    unsigned int field_0;
} st_461720_2;

typedef struct st_461720_0 {
    char padding_0[8];
    struct st_461720_0 *field_8;
    char padding_c[8];
    struct st_461720_0 *field_14;
    struct st_461720_1 *field_18;
    char padding_1c[4];
    unsigned int field_20;
    char padding_24[24];
    struct st_461720_1 *field_3c;
    char padding_40[1008];
    unsigned int field_430;
    unsigned int field_434;
    struct st_461720_2 *field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_461720_0;

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

int sub_461720(void)
{
    st_461720_1 *idx;  // ebp
    st_461720_6 *idx1;  // edi
    st_461720_0 *idx2;  // ecx
    int v8;  // esi
    int v9;  // ebp
    int v10;  // ebx
    st_461720_0 *v11;  // esi
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    unsigned int *v14;  // eax
    st_461720_1 *index;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int *v2;  // [bp-0x4]
    int v4;  // [bp+0x8]
    st_461720_1 *v5;  // [bp+0xc]

    idx = v5;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx1->field_130 = idx1->field_130 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = sub_4621c0(v8, v9, v10);
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v12 = idx->field_20;
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v11->field_480 = v11->field_480 | 1;
    v11->field_430 = v0;
    v11->field_20 = v12;
    v11->field_434 = v1;
    v11->field_438 = v2;
    sub_454d10(v11, v4);
    v11->field_3c = idx;
    if (v13 & 4 && (char)v13 & 2)
    {
        v14 = sub_4613d0();
        goto LABEL_4617ec;
    }
    else if (v13 & 4)
    {
        *(v2) = *((int *)sub_461350());
    }
    else if ((char)v13 & 2)
    {
        *(v2) = *((int *)sub_4612d0());
    }
    else
    {
        v14 = sub_461250();
LABEL_4617ec:
        *(v2) = *(v14);
    }
    index = &idx->padding_0[16];
    idx2 = &v11->padding_c[4];
    if (idx->field_14)
    {
        *((struct st_461720_0 **)&idx2->padding_0[4]) = idx->field_14;
        *((st_461720_0 **)(*((int *)&index->padding_0[4]) + 8)) = idx2;
    }
    *((st_461720_0 **)&index->padding_0[4]) = idx2;
    idx2->field_8 = index;
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    return;
}
