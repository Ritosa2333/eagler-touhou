// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453fa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453fa0_5 {
    struct st_453fa0_3 *field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_453fa0_5;

typedef struct st_453fa0_3 {
    char padding_0[36];
    struct st_453fa0_1 *field_24;
} st_453fa0_3;

typedef struct st_453fa0_2 {
    struct st_453fa0_0 *field_0;
} st_453fa0_2;

typedef struct st_453fa0_1 {
    unsigned int field_0;
} st_453fa0_1;

typedef struct st_453fa0_0 {
    char padding_0[72];
    struct st_453fa0_1 *field_48;
} st_453fa0_0;

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

extern unsigned int g_45448c[4];
extern char g_4ceacc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf200;
extern char g_4cf223;
extern unsigned int g_4cf4f8;
extern unsigned int g_4cf538;
extern unsigned int g_4cf568;
extern st_453fa0_5 g_4d0e70;
extern unsigned int g_4d14d4;

unsigned int sub_453fa0(void)
{
    unsigned int v4;  // edi
    unsigned int *iter;  // edi
    int v6;  // esi
    int v7;  // ebx
    unsigned int index;  // esi
    st_453fa0_5 *v9;  // eax
    st_453fa0_2 **idx;  // esi
    int v11;  // edx
    int j;  // edx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x8]
    char v2;  // [bp-0x4], Other Possible Types: unsigned int

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf200.DebugInfo);
        g_4cf223 = g_4cf223 + 1;
    }
    if (!g_4cf4f8)
    {
        if (!(g_4cee78 & 0x8000))
            return 0;
        LeaveCriticalSection(&g_4cf200.DebugInfo);
        g_4cf223 = g_4cf223 - 1;
        return 0;
    }
    v0 = v4;
    v1 = 0;
    if (g_4d14d4 - 1 <= 7)
    {
        goto *((void *)(*((int *)((char *)&g_45448c[g_4d14d4] - 4))));
    }
    else
    {
        if (g_4ceacc)
        {
            iter = &g_4cf538;
            do
            {
                v6 = *((int *)((char *)iter - 48));
                if (v6 < 0)
                    break;
                v7 = *(iter);
                *((unsigned int *)((char *)iter - 48)) = 0xffffffff;
                if (v7 < 0)
                {
                    index = v6 * 3;
                    v9 = (&g_4d0e70.field_0)[2 * index];
                    idx = &(&g_4d0e70.field_0)[2 * index];
                    idx[5] = 0;
                    if (v9)
                    {
                        v9->field_0->field_24(v9, &v2);
                        idx[5] = v2 & 1;
                        *(idx)->field_0->field_48(*(idx));
                    }
                    *(iter) = 0;
                }
                else
                {
                    if (v7 > 0)
                    {
                        v11 = v7;
                        do
                        {
                            j = v11;
                            v11 = j - 1;
                        } while (j != 1);
                    }
                    *(iter) = 0;
                    sub_4544b0();
                }
                iter += 1;
            } while (iter < &g_4cf568);
        }
        if (!(g_4cee78 & 0x8000))
            return g_4d14d4;
        LeaveCriticalSection(&g_4cf200.DebugInfo);
        g_4cf223 = g_4cf223 - 1;
        return g_4d14d4;
    }
}
