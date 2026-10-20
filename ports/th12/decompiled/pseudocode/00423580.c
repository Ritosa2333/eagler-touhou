// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423580; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423580_3 {
    struct st_423580_0 *field_0;
} st_423580_3;

typedef struct st_423580_1 {
    struct st_423580_2 *field_0;
    struct st_423580_1 *field_4;
} st_423580_1;

typedef struct st_423580_4 {
    unsigned int field_0;
    struct st_423580_4 *field_4;
} st_423580_4;

typedef struct st_423580_0 {
    char padding_0[1148];
    unsigned int field_47c;
} st_423580_0;

typedef struct st_423580_2 {
    char padding_0[20];
    struct st_423580_3 *field_14;
    char padding_18[1124];
    unsigned int field_47c;
} st_423580_2;

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

extern unsigned int g_4b44ec;
extern unsigned int g_4ce8cc;
extern char g_4cead2;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_423580(unsigned int *a0)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // esi
    unsigned int idx;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v5;  // edi
    unsigned int *iter;  // edx
    unsigned int v7;  // ebx
    unsigned int v8;  // ecx
    unsigned int v9;  // ebx
    st_423580_1 *iter1;  // eax
    unsigned int v11;  // ebp
    st_423580_4 *node;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v2 = v3;
    v1 = v4;
    v0 = v5;
    if (a0[2])
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
    if (a0[3])
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
    iter = a0 + 55;
    v7 = 10;
    do
    {
        v8 = *(iter);
        v9 = v7;
        if (v8)
        {
            iter1 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    v11 = iter1->field_0;
                    if (*((int *)v11) == v8)
                        goto LABEL_423680;
                } while ((iter1 = (st_423580_1 *)iter1->field_4, iter1));
            }
            node = *((int *)(g_4ce8cc + 8935104));
            v9 = v7;
            if (*((int *)(g_4ce8cc + 8935104)))
            {
                v7 = v7;
                while (1)
                {
                    v11 = node->field_0;
                    if (*((int *)v11) == v8)
                        break;
                    node = node->field_4;
                    v9 = v7;
                }
LABEL_423680:
                idx = v11;
                v9 = v7;
                if (idx)
                {
                    *((unsigned int *)(idx + 1148)) = *((int *)(idx + 1148)) | 0x10000000;
                    v9 = v7;
                    if (!*((int *)(idx + 24)))
                    {
                        v14 = *((int *)(idx + 20));
                        v9 = v7;
                        if (*((int *)(idx + 20)))
                        {
                            do
                            {
                                v15 = v14;
                                *((unsigned int *)(*((int *)v15) + 1148)) = *((int *)(*((int *)v15) + 1148)) | 0x10000000;
                                v14 = *((int *)(v15 + 4));
                                v9 = v7;
                            } while (*((int *)(v15 + 4)));
                        }
                    }
                }
            }
        }
    } while ((*(iter) = 0, iter += 4, v7 = v9 - 1, v9 != 1));
    if (g_4cead2 == 2)
        sub_424cc0(a0);
    sub_4236f0();
    g_4b44ec = 0;
    return;
}
