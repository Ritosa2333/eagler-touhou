// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d1da; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d1da_0 {
    char padding_0[12];
    unsigned int field_c;
} st_48d1da_0;

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

extern unsigned int g_4ab120;
extern unsigned int g_4d52e0;
extern char g_4d6300;

int sub_48d1da(unsigned int a0)
{
    void* v0;  // ebp
    int idx;  // esi
    CRITICAL_SECTION **v2;  // eax
    st_48d1da_0 *v3;  // eax

    sub_46fcec(&g_4ab120, 20);
    *((unsigned int *)((char *)v0 - 28)) = 0;
    *((unsigned int *)((char *)v0 - 36)) = 0;
    sub_46ec9a(1);
    *((unsigned int *)((char *)v0 - 4)) = 0;
    idx = 0;
    while (1)
    {
        *((int *)((char *)v0 - 32)) = idx;
        if (idx >= *((int *)&g_4d6300))
            break;
        v2 = g_4d52e0 + idx * 4;
        if (*(v2) && (char)*(v2)->OwningThread & 131)
        {
            sub_47c13d(idx, *(v2));
            *((unsigned int *)((char *)v0 - 4)) = 1;
            v3 = *((int *)(g_4d52e0 + idx * 4));
            if ((char)v3->field_c & 131)
            {
                if ((int)v0[8] == 1)
                {
                    if (sub_48d192(v3) != 0xffffffff)
                        *((unsigned int *)((char *)v0 - 28)) = *((int *)((char *)v0 - 28)) + 1;
                }
                else
                {
                    if (!(int)v0[8] && (char)v3->field_c & 2 && sub_48d192(v3) == 0xffffffff)
                        *((unsigned int *)((char *)v0 - 36)) = 0xffffffff;
                }
            }
            *((unsigned int *)((char *)v0 - 4)) = 0;
            sub_48d27c();
        }
        idx += 1;
    }
    *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
    sub_48d2ab();
    return sub_46fd31();
}
