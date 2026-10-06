// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4610d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4610d0_0 {
    char padding_0[184];
    unsigned int field_b8;
} st_4610d0_0;

typedef struct st_4610d0_2 {
    unsigned int field_0;
} st_4610d0_2;

typedef struct st_4610d0_1 {
    char padding_0[1148];
    unsigned int field_47c;
    char padding_480[8];
    struct st_4610d0_2 *field_488;
} st_4610d0_1;

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

unsigned int sub_4610d0(unsigned int a0)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    st_4610d0_0 *idx;  // edi
    st_4610d0_1 **v8;  // eax
    st_4610d0_1 *index;  // esi
    st_4610d0_1 **v10;  // ebx
    char v0;  // [bp-0x88]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    st_4610d0_0 *v3;  // [bp-0xc]
    st_4610d0_0 *v4;  // [bp-0x8]

    v2 = v5;
    v1 = v6;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    *((unsigned int *)&idx[47719].padding_0[88]) = 0;
    *((unsigned int *)&idx[47725].padding_0[164]) = 0;
    v3 = &idx[47719].padding_0[60];
    v8 = *((int *)&idx[47527].padding_0[28]);
    v4 = &idx[47725].padding_0[0x88];
    idx->field_b8 = 0;
    if (v8)
    {
        do
        {
            index = *(v8);
            v10 = v8[1];
            if (index->field_47c & 0x10000000)
            {
                sub_461450(idx);
            }
            else
            {
                if (index->field_488)
                    index->field_488();
                if (sub_455630(index))
                {
                    sub_461450(idx);
                }
                else
                {
                    if (*((int *)&index->padding_0[32]) != 30 && *((int *)&index->padding_0[32]) != 31)
                        *((unsigned int *)&index->padding_0[32]) = 30;
                    *((st_4610d0_1 **)(*((int *)&(&v0)[4 * *((int *)&index->padding_0[32])]) + 28)) = index;
                    *((st_4610d0_1 **)&(&v0)[4 * *((int *)&index->padding_0[32])]) = index;
                    *((unsigned int *)&index->padding_0[28]) = 0;
                }
            }
        } while ((idx->field_b8 = idx->field_b8 + 1, v8 = v10, v10));
    }
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    return 1;
}
